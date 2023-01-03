/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "IExamPlugin.h"
#include "IExamInterface.h"
#include "Exam_HelperStructs.h"
#include "EBehaviorTree.h"
#include "..\inc\EliteMath\EVector2.h"
#include "HelperFuncts.h"
#include "InventoryManager.h"
#include "SteeringManager.h"
#include "HouseCheck.h"
#include "Timer.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------

namespace BT_Actions
{
	Elite::BehaviorState CheeseStrategyV1(Elite::Blackboard* pBlackboard)
	{
		SteeringManager* pSteeringManager{};

		if (pBlackboard->GetData("steeringManager", pSteeringManager) == false || pSteeringManager == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		auto target = Elite::Vector2{ 0,-100 };

		pSteeringManager->Seek(target);

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState ChangeToWander(Elite::Blackboard* pBlackboard)
	{
		SteeringManager* pSteeringManager{};

		if (pBlackboard->GetData("steeringManager", pSteeringManager) == false || pSteeringManager == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		pSteeringManager->Wander();

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState GoToFirstHouse(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringManager* pSteeringManager{};
		std::vector<HouseInfo>* pHousesInFOV{ nullptr };

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("steeringManager", pSteeringManager) == false || pSteeringManager == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("housesInFOV", pHousesInFOV) == false || pHousesInFOV == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pHousesInFOV->empty())
		{
			return Elite::BehaviorState::Failure;
		}

		auto agentInfo = pInterface->Agent_GetInfo();

		bool houseLeaveLocationValid{ false };
		if (pBlackboard->GetData("houseLeaveLocationValid", houseLeaveLocationValid))
		{
			if (!houseLeaveLocationValid)
			{
				pBlackboard->ChangeData("houseLeaveLocation", agentInfo.Position);
				pBlackboard->ChangeData("houseLeaveLocationValid", true);
			}
		}

		auto target = pHousesInFOV->begin()->Center;

		pSteeringManager->Seek(target);

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState LootFOV(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringManager* pSteeringManager{};
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };
		InventoryManager* pInventoryManager{ nullptr };
		
		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("steeringManager", pSteeringManager) == false || pSteeringManager == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("entitiesInFOV", pEntitiesInFOV) == false || pEntitiesInFOV == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("inventoryManager", pInventoryManager) == false || pInventoryManager == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		
		if (pEntitiesInFOV->empty())
		{
			return Elite::BehaviorState::Failure;
		}

		auto agentInfo = pInterface->Agent_GetInfo();

		if (pEntitiesInFOV->at(0).Type != eEntityType::ITEM)
		{
			return Elite::BehaviorState::Failure;
		}
		auto target = pEntitiesInFOV->at(0).Location;

		if ((target - pInterface->Agent_GetInfo().Position).MagnitudeSquared() < agentInfo.GrabRange * agentInfo.GrabRange)
		{
			if (pInventoryManager->GrabAndAddItem(pEntitiesInFOV->at(0)))
			{
				return Elite::BehaviorState::Running;
			}
			return Elite::BehaviorState::Failure;
		}

		pSteeringManager->Seek(target);

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState SpinAround(Elite::Blackboard* pBlackboard)
	{
		SteeringManager* pSteeringManager{};

		if (pBlackboard->GetData("steeringManager", pSteeringManager) == false || pSteeringManager == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		pSteeringManager->SpinAround();

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState GoOutsideOfHouse(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringManager* pSteeringManager{};

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("steeringManager", pSteeringManager) == false || pSteeringManager == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		auto agentInfo = pInterface->Agent_GetInfo();

		bool houseLeaveLocationValid{ false };
		if (pBlackboard->GetData("houseLeaveLocationValid", houseLeaveLocationValid))
		{
			if (!houseLeaveLocationValid)
			{
				return Elite::BehaviorState::Failure;
			}
		}
		else
		{
			return Elite::BehaviorState::Failure;
		}

		Elite::Vector2 target{};
		// If data validation fails return failure
		if (!pBlackboard->GetData("houseLeaveLocation", target))
		{
			return Elite::BehaviorState::Failure;
		}

		auto nextTargetPos = pInterface->NavMesh_GetClosestPathPoint(target);

		pSteeringManager->Seek(target);

		if (Elite::Distance(nextTargetPos, agentInfo.Position) < 2.f)
		{
			pSteeringManager->StopMovement();
			pBlackboard->ChangeData("houseLeaveLocationValid", false);
		}

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState UseMedkit(Elite::Blackboard* pBlackboard)
	{
		InventoryManager* pInventoryManager{ nullptr };

		if (pBlackboard->GetData("inventoryManager", pInventoryManager) == false || pInventoryManager == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		if (pInventoryManager->UseMedkit())
		{
			return Elite::BehaviorState::Success;
		}
		return Elite::BehaviorState::Failure;
	}

	Elite::BehaviorState UseFood(Elite::Blackboard* pBlackboard)
	{
		InventoryManager* pInventoryManager{ nullptr };

		if (pBlackboard->GetData("inventoryManager", pInventoryManager) == false || pInventoryManager == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		if (pInventoryManager->UseFood())
		{
			return Elite::BehaviorState::Success;
		}
		return Elite::BehaviorState::Failure;
	}

	Elite::BehaviorState SearchHouse(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringManager* pSteeringManager{ nullptr };
		std::vector<HouseCheck>* pHousesChecked{ nullptr };

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("steeringManager", pSteeringManager) == false || pSteeringManager == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("housesChecked", pHousesChecked) == false || pHousesChecked == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		auto agentInfo = pInterface->Agent_GetInfo();

		Elite::Vector2 target;
		bool found{ false };
		for (auto& house : *pHousesChecked)
		{
			if (house.IsDone()) continue;
			house.UpdateCheckedCorners(agentInfo.Position);
			if (house.IsDone()) continue;
			target = house.GetNextCorner();
			found = true;
			break;
		}
		if (found == false)
		{
			return Elite::BehaviorState::Failure;
		}

		pSteeringManager->Seek(target);

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState KiteAndShoot(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringManager* pSteeringManager{ nullptr };
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };
		InventoryManager* pInventoryManager{ nullptr };

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("steeringManager", pSteeringManager) == false || pSteeringManager == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("entitiesInFOV", pEntitiesInFOV) == false || pEntitiesInFOV == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("inventoryManager", pInventoryManager) == false || pInventoryManager == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		auto agentInfo = pInterface->Agent_GetInfo();

		pSteeringManager->AutoOrient(false);

		bool atleastOneEnemyinFOV{ false };

		std::vector<int> enemyIdxs{};
		for (int i{0}; i < pEntitiesInFOV->size(); ++i)
		{
			if (pEntitiesInFOV->at(i).Type == eEntityType::ENEMY)
			{
				atleastOneEnemyinFOV = true;
				enemyIdxs.push_back(i);
			}
		}
		if (!atleastOneEnemyinFOV)
		{
			pSteeringManager->SpinAround();
			return Elite::BehaviorState::Running;
		}

		EntityInfo closestEnemy{};
		closestEnemy.Location.x = 10000.f;
		for (int enemyIdx : enemyIdxs)
		{
			if (agentInfo.Position.Distance(pEntitiesInFOV->at(enemyIdx).Location) < agentInfo.Position.Distance(closestEnemy.Location))
			{
				closestEnemy = pEntitiesInFOV->at(enemyIdx);
			}
		}

		const constexpr float angleEps{ 0.05f };
		Elite::Vector2 desiredDirection = (closestEnemy.Location - agentInfo.Position);
		// Check if we're oriented to the closest enemy
		if (std::abs(agentInfo.Orientation - std::atan2(desiredDirection.y, desiredDirection.x)) < angleEps)
		{
			// If we're oriented to the closest enemy, shoot it
			if (pInventoryManager->UseGun())
			{
				return Elite::BehaviorState::Success;
			}
			return Elite::BehaviorState::Failure;
		}

		// Else, face towards closest enemy
		pSteeringManager->Face(closestEnemy.Location);

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState RunFromEnemy(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringManager* pSteeringManager{ nullptr };
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("steeringManager", pSteeringManager) == false || pSteeringManager == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("entitiesInFOV", pEntitiesInFOV) == false || pEntitiesInFOV == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		auto agentInfo = pInterface->Agent_GetInfo();

		pSteeringManager->AutoOrient(false);

		bool atleastOneEnemyinFOV{ false };

		std::vector<int> enemyIdxs{};
		for (int i{ 0 }; i < pEntitiesInFOV->size(); ++i)
		{
			if (pEntitiesInFOV->at(i).Type == eEntityType::ENEMY)
			{
				atleastOneEnemyinFOV = true;
				enemyIdxs.push_back(i);
			}
		}
		if (!atleastOneEnemyinFOV)
		{
			pSteeringManager->SpinAround();
			return Elite::BehaviorState::Running;
		}

		EntityInfo closestEnemy{};
		closestEnemy.Location.x = 10000.f;
		for (int enemyIdx : enemyIdxs)
		{
			if (agentInfo.Position.Distance(pEntitiesInFOV->at(enemyIdx).Location) < agentInfo.Position.Distance(closestEnemy.Location))
			{
				closestEnemy = pEntitiesInFOV->at(enemyIdx);
			}
		}

		const constexpr float angleEps{ 0.05f };
		Elite::Vector2 desiredDirection = (closestEnemy.Location - agentInfo.Position);
		// Check if we're oriented to the closest enemy
		if (std::abs(agentInfo.Orientation - std::atan2(desiredDirection.y, desiredDirection.x)) < angleEps)
		{
			// If so, just move backwards
			pSteeringManager->Flee(closestEnemy.Location);
			return Elite::BehaviorState::Success;
		}

		// Else, face towards closest enemy
		pSteeringManager->Face(closestEnemy.Location);

		// And move backwards
		pSteeringManager->Flee(closestEnemy.Location);

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState FleeFromPurgeZone(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringManager* pSteeringManager{ nullptr };
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("steeringManager", pSteeringManager) == false || pSteeringManager == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("entitiesInFOV", pEntitiesInFOV) == false || pEntitiesInFOV == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		// Find all purgezones
		std::vector<int> purgeZoneIndices;
		{
			int i{};
			for (const auto& entity : *pEntitiesInFOV)
			{
				if (entity.Type == eEntityType::PURGEZONE)
				{
					purgeZoneIndices.push_back(i);
				}
				++i;
			}
		}

		// No purgezones found
		if (purgeZoneIndices.empty())
		{
			return Elite::BehaviorState::Failure;
		}

		auto agentInfo = pInterface->Agent_GetInfo();

		// Find closest purgezone
		EntityInfo closestPurgeZone;
		closestPurgeZone.Location.x = 1000.f;
		for (int purgeZoneIdx : purgeZoneIndices)
		{
			if (agentInfo.Position.Distance(pEntitiesInFOV->at(purgeZoneIdx).Location) < agentInfo.Position.Distance(closestPurgeZone.Location))
			{
				closestPurgeZone = pEntitiesInFOV->at(purgeZoneIdx);
			}
		}

		// Face the purgezone
		pSteeringManager->Face(closestPurgeZone.Location);

		// Run away from it 
		const constexpr float fleeFactor{ 10.f };
		pSteeringManager->Flee(closestPurgeZone.Location, fleeFactor);

		return Elite::BehaviorState::Success;
	}
	
}

//-----------------------------------------------------------------
// Conditions
//-----------------------------------------------------------------

namespace BT_Conditions
{
	bool IsEnemyInFOV(Elite::Blackboard* pBlackboard)
	{
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };
		Timer* pLastDangerTimer{ nullptr };

		if (pBlackboard->GetData("entitiesInFOV", pEntitiesInFOV) == false || pEntitiesInFOV == nullptr)
		{
			return false;
		}
		if (pBlackboard->GetData("lastDangerTimer", pLastDangerTimer) == false || pLastDangerTimer == nullptr)
		{
			return false;
		}

		for (int i{0}; i < pEntitiesInFOV->size(); ++i)
		{
			if (pEntitiesInFOV->at(i).Type == eEntityType::ENEMY)
			{
				pLastDangerTimer->ResetTimer();
				pLastDangerTimer->Enable();
				return true;
			}
		}

		return false;
	}

	bool IsHouseInFOV(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		std::vector<HouseInfo>* pHousesInFOV{ nullptr };

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return false;
		}
		if (pBlackboard->GetData("housesInFOV", pHousesInFOV) == false || pHousesInFOV == nullptr)
		{
			return false;
		}

		return pHousesInFOV->size() > 0;
	}

	bool IsHouseInFOVUnlooted(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		std::vector<HouseInfo>* pHousesInFOV{ nullptr };
		std::vector<HouseCheck>* pHousesChecked{ nullptr };

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return false;
		}
		if (pBlackboard->GetData("housesInFOV", pHousesInFOV) == false || pHousesInFOV == nullptr)
		{
			return false;
		}
		if (pBlackboard->GetData("housesChecked", pHousesChecked) == false || pHousesChecked == nullptr)
		{
			return false;
		}
		if (!IsHouseInFOV(pBlackboard))
		{
			return false;
		}

		for (const auto& houseFOV : *pHousesInFOV)
		{
			for (const auto& houseCheck : *pHousesChecked)
			{
				if (houseFOV.Center == houseCheck.Center)
				{
					return !houseCheck.IsDone();
				}
			}
		}
	}

	bool IsLootInFOV(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return false;
		}
		if (pBlackboard->GetData("entitiesInFOV", pEntitiesInFOV) == false || pEntitiesInFOV == nullptr)
		{
			return false;
		}

		return pEntitiesInFOV->size() > 0;
	}

	bool IsInDanger(Elite::Blackboard* pBlackboard)
	{
		Timer* pLastDangerTimer{ nullptr };

		if (pBlackboard->GetData("lastDangerTimer", pLastDangerTimer) == false || pLastDangerTimer == nullptr)
		{
			return false;
		}

		if (IsEnemyInFOV(pBlackboard))
		{
			return true;
		}

		if (pLastDangerTimer->IsEnabled() && !pLastDangerTimer->IsDone())
		{
			return true;
		}

		return false;
	}

	bool IsInventoryFull(Elite::Blackboard* pBlackboard)
	{
		InventoryManager* pInventoryManager{ nullptr };

		if (pBlackboard->GetData("inventoryManager", pInventoryManager) == false || pInventoryManager == nullptr)
		{
			return false;
		}

		return pInventoryManager->IsInventoryFull();
	}

	bool DoIHaveGun(Elite::Blackboard* pBlackboard)
	{
		InventoryManager* pInventoryManager{ nullptr };

		if (pBlackboard->GetData("inventoryManager", pInventoryManager) == false || pInventoryManager == nullptr)
		{
			return false;
		}
		
		return pInventoryManager->HaveGun();
	}

	bool DoIHaveMedKit(Elite::Blackboard* pBlackboard)
	{
		InventoryManager* pInventoryManager{ nullptr };

		if (pBlackboard->GetData("inventoryManager", pInventoryManager) == false || pInventoryManager == nullptr)
		{
			return false;
		}

		return pInventoryManager->HaveItem(eItemType::MEDKIT);
	}

	bool DoIHaveFood(Elite::Blackboard* pBlackboard)
	{
		InventoryManager* pInventoryManager{ nullptr };

		if (pBlackboard->GetData("inventoryManager", pInventoryManager) == false || pInventoryManager == nullptr)
		{
			return false;
		}

		return pInventoryManager->HaveItem(eItemType::FOOD);
	}

	bool AgentInsideHouse(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return false;
		}

		return pInterface->Agent_GetInfo().IsInHouse;
	}

	bool IsPurgeZoneInFOV(Elite::Blackboard* pBlackboard)
	{
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };

		if (pBlackboard->GetData("entitiesInFOV", pEntitiesInFOV) == false || pEntitiesInFOV == nullptr)
		{
			return false;
		}

		for (const auto& entity : *pEntitiesInFOV)
		{
			if (entity.Type == eEntityType::PURGEZONE)
			{
				return true;
			}
		}
	
		return false;
	}
}








#endif