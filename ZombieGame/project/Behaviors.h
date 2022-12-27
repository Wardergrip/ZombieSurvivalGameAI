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
#include "AgentProps.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------

namespace BT_Actions
{
	Elite::BehaviorState CheeseStrategyV1(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringPlugin_Output* pSteering{};

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("steering", pSteering) == false || pSteering == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		auto agentInfo = pInterface->Agent_GetInfo();

		auto target = Elite::Vector2{ 0,-100 };

		auto nextTargetPos = pInterface->NavMesh_GetClosestPathPoint(target);
		//auto nextTargetPos = pInterface->NavMesh_GetClosestPathPoint(Elite::Vector2(-100,0));

		pSteering->LinearVelocity = nextTargetPos - agentInfo.Position; //Desired Velocity
		pSteering->LinearVelocity.Normalize();						  //Normalize Desired Velocity
		pSteering->LinearVelocity *= agentInfo.MaxLinearSpeed;		  //Rescale to Max Speed

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState ChangeToWander(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringPlugin_Output* pSteering{};

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("steering", pSteering) == false || pSteering == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		auto agentInfo = pInterface->Agent_GetInfo();
#pragma region Calcs
		float wanderAngle{ agentInfo.Orientation};
		const constexpr float maxAngleChange{ 10.f };
		const constexpr float offsetDistance{10.f};
		const constexpr float radius{ 5.f };
		const float randomAngle{ HF::ToRadians(static_cast<float>(rand() % static_cast<int>(HF::ToDegrees(maxAngleChange)))) };
		const float sign{ (rand() % 2) ? 1.f : -1.f };
		const float nextAngle{ wanderAngle +(sign * randomAngle)};
		wanderAngle = nextAngle;
		Elite::Vector2 circleCenter{ agentInfo.Position + (HF::AngleToLookDirection(agentInfo.Orientation) * offsetDistance)};
		Elite::Vector2 target{ circleCenter.x + (std::cosf(wanderAngle) * radius),circleCenter.y + (std::sinf(wanderAngle) * radius) };
		Elite::Vector2 targetVector{ target - agentInfo.Position };
#pragma endregion
		auto nextTargetPos = pInterface->NavMesh_GetClosestPathPoint(target);
		//auto nextTargetPos = pInterface->NavMesh_GetClosestPathPoint(Elite::Vector2(-100,0));

		pSteering->LinearVelocity = nextTargetPos - agentInfo.Position; //Desired Velocity
		pSteering->LinearVelocity.Normalize();						  //Normalize Desired Velocity
		pSteering->LinearVelocity *= agentInfo.MaxLinearSpeed;		  //Rescale to Max Speed

		pInterface->Draw_Circle(circleCenter, radius, Elite::Vector3{0,1,0});
		pInterface->Draw_Direction(agentInfo.Position, targetVector, offsetDistance, Elite::Vector3{0,1,0});

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState GoToFirstHouse(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringPlugin_Output* pSteering{};
		std::vector<HouseInfo>* pHousesInFOV{ nullptr };

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("steering", pSteering) == false || pSteering == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("housesInFOV", pHousesInFOV) == false || pHousesInFOV == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		auto agentInfo = pInterface->Agent_GetInfo();

		auto target = pHousesInFOV->begin()->Center;

		auto nextTargetPos = pInterface->NavMesh_GetClosestPathPoint(target);

		pSteering->LinearVelocity = nextTargetPos - agentInfo.Position; //Desired Velocity
		pSteering->LinearVelocity.Normalize();						  //Normalize Desired Velocity
		pSteering->LinearVelocity *= agentInfo.MaxLinearSpeed;		  //Rescale to Max Speed

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState LootFOV(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringPlugin_Output* pSteering{};
		std::vector<EntityInfo>* pEntitiesInFOV{ nullptr };

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("steering", pSteering) == false || pSteering == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("entitiesInFOV", pEntitiesInFOV) == false || pEntitiesInFOV == nullptr)
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
			ItemInfo itemInfo;
			pInterface->Item_Grab(pEntitiesInFOV->at(0), itemInfo);
			pInterface->Inventory_AddItem(0, itemInfo);
			return Elite::BehaviorState::Running;
		}

		auto nextTargetPos = pInterface->NavMesh_GetClosestPathPoint(target);

		pSteering->LinearVelocity = nextTargetPos - agentInfo.Position; //Desired Velocity
		pSteering->LinearVelocity.Normalize();						  //Normalize Desired Velocity
		pSteering->LinearVelocity *= agentInfo.MaxLinearSpeed;		  //Rescale to Max Speed

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState SpinAround(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };
		SteeringPlugin_Output* pSteering{};

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}
		if (pBlackboard->GetData("steering", pSteering) == false || pSteering == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		pSteering->AutoOrient = false;
		pSteering->LinearVelocity = Elite::ZeroVector2;
		pSteering->AngularVelocity = pInterface->Agent_GetInfo().MaxAngularSpeed;

		return Elite::BehaviorState::Success;
	}
}

//-----------------------------------------------------------------
// Conditions
//-----------------------------------------------------------------

namespace BT_Conditions
{
	bool IsEnemyInPOV(Elite::Blackboard* pBlackboard)
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

		for (const auto& entity : *pEntitiesInFOV)
		{
			if (entity.Type == eEntityType::ENEMY)
			{
				return true;
			}
		}
		return false;
	}

	bool IsHouseInPOV(Elite::Blackboard* pBlackboard)
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

		if (pHousesInFOV->size() > 0)
		{
			return true;
		}
		return false;
	}

	bool DoIHaveGun(Elite::Blackboard* pBlackboard)
	{
		IExamInterface* pInterface{ nullptr };

		if (pBlackboard->GetData("interface", pInterface) == false || pInterface == nullptr)
		{
			return false;
		}

		ItemInfo itemInfo;
		for (UINT i{ 0 }; i < pInterface->Inventory_GetCapacity(); ++i)
		{
			if (pInterface->Inventory_GetItem(i, itemInfo))
			{
				if (itemInfo.Type == eItemType::PISTOL || itemInfo.Type == eItemType::SHOTGUN)
				{
					return true;
				}
			}
		}
		return false;
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
}








#endif