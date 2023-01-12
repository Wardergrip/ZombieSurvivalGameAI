#include "stdafx.h"
#include "ExplorationManager.h"
#include "HelperFuncts.h"

ExplorationManager::ExplorationManager(IExamInterface* pInterface, int divisions)
	:m_pInterface{ pInterface }
	,m_Divisions{divisions}
{
	auto worldInfo = pInterface->World_GetInfo();
	m_GridWidth = worldInfo.Dimensions.x / divisions;
	m_GridHeight = worldInfo.Dimensions.y / divisions;
	const Elite::Vector2 topLeftCenter{ Elite::Vector2{worldInfo.Center.x - worldInfo.Dimensions.x / 2, worldInfo.Center.y - worldInfo.Dimensions.y / 2} - Elite::Vector2{m_GridWidth / 2.f, m_GridHeight / 2.f}};

	for (int i{ 0 }; i < (divisions * divisions); ++i)
	{
		GridInfo grid{};
		grid.Center.x = topLeftCenter.x + (i % divisions) * m_GridWidth;
		grid.Center.y = topLeftCenter.y + (i / divisions) * m_GridHeight;
		grid.hasHouse = false;
		grid.visited = false;
		m_Grid.emplace_back(grid);
	}

	m_OriginalLocation = pInterface->Agent_GetInfo().Position;
}

void ExplorationManager::DEBUG_DrawGrid() const
{
	const Elite::Vector3 defaultCol{ 0.45f,0.45f,0.45f };
	const Elite::Vector3 visitedCol{0.8f,0.8f,0.8f};
	for (const auto& grid : m_Grid)
	{
		Elite::Vector2 p0{ grid.Center + Elite::Vector2{-m_GridWidth / 2,m_GridHeight / 2} };
		Elite::Vector2 p1{ grid.Center + Elite::Vector2{m_GridWidth / 2,m_GridHeight / 2} };
		Elite::Vector2 p2{ grid.Center + Elite::Vector2{m_GridWidth / 2,-m_GridHeight / 2} };
		Elite::Vector2 p3{ grid.Center + Elite::Vector2{-m_GridWidth / 2,-m_GridHeight / 2} };

		Elite::Vector3 col{ grid.visited ? visitedCol : defaultCol };
		float depth{ grid.visited ? -1.f : 0.f };
		m_pInterface->Draw_Segment(p0, p1, col,depth);
		m_pInterface->Draw_Segment(p1, p2, col,depth);
		m_pInterface->Draw_Segment(p2, p3, col,depth);
		m_pInterface->Draw_Segment(p3, p0, col,depth);

		// Draw corners
		/*if (grid.visited)
		{
			m_pInterface->Draw_Point(p0, 10.f, Elite::Vector3{1,1,0});
			m_pInterface->Draw_Point(p1, 10.f, Elite::Vector3{1,0,0});
			m_pInterface->Draw_Point(p2, 10.f, Elite::Vector3{1,0,0});
			m_pInterface->Draw_Point(p3, 10.f, Elite::Vector3{1,0,0});
		}*/
	}
}

void ExplorationManager::Update(float dt)
{
	auto agentInfo = m_pInterface->Agent_GetInfo();
	int idx = PosToIdx(agentInfo.Position + Elite::Vector2{0,m_GridHeight});
	constexpr const float visitedRange{ 5 };
	if (idx != -1 && m_Grid.at(idx).visited == false)
	{
		if ((m_Grid.at(idx).Center - agentInfo.Position).MagnitudeSquared() <= (visitedRange * visitedRange))
		{
			m_Grid.at(idx).visited = true;
		}
	}
}

Elite::Vector2 ExplorationManager::GetNextLocation()
{
	auto rowCol = HF::GetRowColFromIndex(PosToIdx({ m_pInterface->Agent_GetInfo().Position + Elite::Vector2{ 0,m_GridHeight } }), m_Divisions);
	auto row = rowCol.first;
	auto col = rowCol.second;

	float closestDistance{ FLT_MAX };
	Elite::Vector2 agentPosition = m_pInterface->Agent_GetInfo().Position;
	Elite::Vector2 target{0,0};
	for (const auto& grid : m_Grid)
	{
		if (grid.visited) continue;
		auto distance = grid.Center.DistanceSquared(m_OriginalLocation) + grid.Center.DistanceSquared(agentPosition) /*+ grid.Center.DistanceSquared(m_LastLocation)*/;
		if (distance < closestDistance)
		{
			target = grid.Center;
			closestDistance = distance;
		}
	}

	return target;
}

int ExplorationManager::PosToIdx(const Elite::Vector2& p) const
{
	for (size_t i{0}; i < m_Grid.size(); ++i)
	{
		Elite::Vector2 topLeft{ m_Grid.at(i).Center + Elite::Vector2{-m_GridWidth / 2,m_GridHeight / 2} };
		if (HF::IsPointInRect(topLeft, m_GridWidth, m_GridHeight, p))
		{
			// Draw corners
			/*m_pInterface->Draw_Circle(topLeft, 4.f, Elite::Vector3{ 1.f,1,0 });
			m_pInterface->Draw_Circle(topLeft + Elite::Vector2{m_GridWidth,0}, 4.f, Elite::Vector3{ 1.f,0,0 });
			m_pInterface->Draw_Circle(topLeft + Elite::Vector2{m_GridWidth, -m_GridHeight}, 4.f, Elite::Vector3{ 1.f,0,0 });
			m_pInterface->Draw_Circle(topLeft + Elite::Vector2{0,-m_GridHeight}, 4.f, Elite::Vector3{ 1.f,0,0 });*/
			return i;
		}
	}

	return -1;
}
