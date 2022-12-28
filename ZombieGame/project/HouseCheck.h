#pragma once
#ifndef HOUSECHECK
#define HOUSECHECK
struct HouseCheck : public HouseInfo
{
	HouseCheck()
		:checkedCorner{ false,false,false,false },
		isAllDone{false}
	{

	}
	HouseCheck(const HouseInfo& hi)
		:HouseCheck()
	{
		this->Center = hi.Center;
		this->Size = hi.Size;
	}
	std::vector<bool> checkedCorner;
	bool IsDone() const 
	{ 
		return isAllDone; 
	}
	Elite::Vector2 GetNextCorner() const
	{
		if (isAllDone) return Elite::Vector2{};
		for (size_t i{ 0 }; i < checkedCorner.size(); ++i)
		{
			if (checkedCorner.at(i)) continue;

			const constexpr float wallThickness{ 3.f };
			switch (i)
			{
			case 0:
				return Center + Elite::Vector2{ Size.x / 2.f - wallThickness,Size.y / 2.f  - wallThickness};
			case 1:
				return Center + Elite::Vector2{ -Size.x / 2.f + wallThickness,Size.y / 2.f  - wallThickness};
			case 2:
				return Center + Elite::Vector2{ -Size.x / 2.f + wallThickness,-Size.y / 2.f + wallThickness };
			case 3:
				return Center + Elite::Vector2{ Size.x / 2.f - wallThickness,-Size.y / 2.f + wallThickness };
			}
		}
		return Elite::Vector2{};
	}
	void UpdateCheckedCorners(const Elite::Vector2& currentAgentPos)
	{
		if (isAllDone) return;
		auto nextCorner = GetNextCorner();
		const constexpr float minimumDistance{ 2.f };
		if (currentAgentPos.DistanceSquared(nextCorner) <= minimumDistance)
		{
			for (size_t i{ 0 }; i < checkedCorner.size(); ++i)
			{
				if (checkedCorner[i]) continue;
				checkedCorner[i] = true;
				return;
			}
		}
		isAllDone = std::all_of(checkedCorner.begin(), checkedCorner.end(), [](bool v) { return v; });
	}
private:
	bool isAllDone;
};
#endif