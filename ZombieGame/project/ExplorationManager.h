#ifndef EXPLORATIONMANAGER
#define EXPLORATIONMANAGER
#include "Exam_HelperStructs.h"
#include "IExamInterface.h"
#include <vector>
#include <queue>

class ExplorationManager final
{
public:
	ExplorationManager(IExamInterface* pInterface, int divisions = 10);
	~ExplorationManager() = default;

	ExplorationManager(const ExplorationManager& explorationManager) = delete;
	ExplorationManager(ExplorationManager&& explorationManager) = delete;
	ExplorationManager& operator=(const ExplorationManager& explorationManager) = delete;
	ExplorationManager& operator=(ExplorationManager&& explorationManager) = delete;

	void DEBUG_DrawGrid() const;

	void Update(float dt);
	Elite::Vector2 GetNextLocation() const;

private:
	struct GridInfo
	{
		bool visited{ false };
		bool hasHouse{ false };
		Elite::Vector2 Center{};
	};

	IExamInterface* m_pInterface = nullptr;
	std::vector<GridInfo> m_Grid{};
	Elite::Vector2 m_OriginalLocation;
	int m_Divisions;
	float m_GridWidth;
	float m_GridHeight;

	int PosToIdx(const Elite::Vector2& p) const;
};

#endif