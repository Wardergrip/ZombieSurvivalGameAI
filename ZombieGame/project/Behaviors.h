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

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------

namespace BT_Actions
{
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



		

		float wanderAngle{35.f};
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
		//auto nextTargetPos = pInterface->NavMesh_GetClosestPathPoint(target);
		auto nextTargetPos = pInterface->NavMesh_GetClosestPathPoint(Elite::Vector2(-100,0));

		pSteering->LinearVelocity = nextTargetPos - agentInfo.Position; //Desired Velocity
		pSteering->LinearVelocity.Normalize();						  //Normalize Desired Velocity
		pSteering->LinearVelocity *= agentInfo.MaxLinearSpeed;		  //Rescale to Max Speed

		return Elite::BehaviorState::Success;
	}
	/*
	Elite::BehaviorState ChangeToSeekFood(Elite::Blackboard* pBlackboard)
	{
		AgarioAgent* pAgent;

		if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		Elite::Vector2 targetPos;
		if (pBlackboard->GetData("Target", targetPos) == false)
		{
			return Elite::BehaviorState::Failure;
		}

		DEBUGRENDERER2D->DrawCircle(targetPos, 2, {0,1,0}, DEBUGRENDERER2D->NextDepthSlice());
		pAgent->SetToSeek(targetPos);

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState ChangeToFleeAgent(Elite::Blackboard* pBlackboard)
	{
		AgarioAgent* pAgent;

		if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		AgarioAgent* closestEnemy;
		if (pBlackboard->GetData("AgentFleeTarget", closestEnemy) == false || closestEnemy == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		pAgent->SetToFlee(closestEnemy->GetPosition());
		DEBUGRENDERER2D->DrawCircle(closestEnemy->GetPosition(), closestEnemy->GetRadius() * 1.1f + 2, {1,0,0}, DEBUGRENDERER2D->NextDepthSlice());

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState ChangeToSeekSmallAgent(Elite::Blackboard* pBlackboard)
	{
		AgarioAgent* pAgent;

		if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
		{
			return Elite::BehaviorState::Failure;
		}

		Elite::Vector2 targetPos;
		if (pBlackboard->GetData("Target", targetPos) == false)
		{
			return Elite::BehaviorState::Failure;
		}

		pAgent->SetToSeek(targetPos);

		DEBUGRENDERER2D->DrawCircle(targetPos, pAgent->GetRadius() + 2, {1,1,0}, DEBUGRENDERER2D->NextDepthSlice());

		return Elite::BehaviorState::Success;
	}*/
}

//-----------------------------------------------------------------
// Conditions
//-----------------------------------------------------------------

namespace BT_Conditions
{
	/*bool IsFoodNearby(Elite::Blackboard* pBlackboard)
	{
		AgarioAgent* pAgent{ nullptr };
		std::vector<AgarioFood*>* pFoodVec{ nullptr };

		if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
		{
			return false;
		}

		if (pBlackboard->GetData("FoodVec", pFoodVec) == false || pFoodVec == nullptr)
		{
			return false;
		}

		if (pFoodVec->empty())
			return false;

		const float searchRadius{ pAgent->GetRadius() + 20.f };
		float closestDistSqr{ searchRadius * searchRadius };
		AgarioFood* pClosestFood{ nullptr };
		const Elite::Vector2 agentPos{ pAgent->GetPosition() };
		DEBUGRENDERER2D->DrawCircle(agentPos, searchRadius, { 0,1,0 }, DEBUGRENDERER2D->NextDepthSlice());
		for (const auto& pFood : *pFoodVec)
		{
			float distSqr = pFood->GetPosition().DistanceSquared(agentPos);

			if (distSqr < closestDistSqr)
			{
				closestDistSqr = distSqr;
				pClosestFood = pFood;
			}
		}

		if (pClosestFood != nullptr)
		{
			pBlackboard->ChangeData("Target", pClosestFood->GetPosition());
			return true;
		}
		return false;
	}

	bool IsLargeEnemyNearby(Elite::Blackboard* pBlackboard)
	{
		AgarioAgent* pAgent{ nullptr };
		std::vector<AgarioAgent*>* pAgentsVec{ nullptr };

		if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
		{
			return false;
		}

		if (pBlackboard->GetData("AgentsVec", pAgentsVec) == false || pAgentsVec == nullptr)
		{
			return false;
		}

		if (pAgentsVec->empty())
			return false;

		const float searchRadius{ pAgent->GetRadius() + 22.f };

		float closestDistSqr{ searchRadius * searchRadius };
		AgarioAgent* pClosestAgent{ nullptr };
		const Elite::Vector2 agentPos{ pAgent->GetPosition() };
		const float agentRadius{ pAgent->GetRadius() };
		DEBUGRENDERER2D->DrawCircle(agentPos, searchRadius, { 1,0,0 }, DEBUGRENDERER2D->NextDepthSlice());
		for (const auto& pEnemyAgent : *pAgentsVec)
		{
			if (pEnemyAgent == pAgent) continue;
			const float distSqr = pEnemyAgent->GetPosition().DistanceSquared(agentPos);
			if (distSqr < closestDistSqr && agentRadius < pEnemyAgent->GetRadius())
			{
				closestDistSqr = distSqr;
				pClosestAgent = pEnemyAgent;
			}
		}

		if (pClosestAgent != nullptr)
		{
			pBlackboard->ChangeData("AgentFleeTarget", pClosestAgent);
			return true;
		}
		return false;
	}

	bool IsSmallEnemyNearby(Elite::Blackboard* pBlackboard)
	{
		AgarioAgent* pAgent{ nullptr };
		std::vector<AgarioAgent*>* pAgentsVec{ nullptr };

		if (pBlackboard->GetData("Agent", pAgent) == false || pAgent == nullptr)
		{
			return false;
		}

		if (pBlackboard->GetData("AgentsVec", pAgentsVec) == false || pAgentsVec == nullptr)
		{
			return false;
		}

		if (pAgentsVec->empty())
			return false;

		const float searchRadius{ pAgent->GetRadius() + 21.f };

		float closestDistSqr{ searchRadius * searchRadius };
		AgarioAgent* pClosestAgent{ nullptr };
		const Elite::Vector2 agentPos{ pAgent->GetPosition() };
		const float agentRadius{ pAgent->GetRadius() };
		const float radiusMargin{ 1 };
		DEBUGRENDERER2D->DrawCircle(agentPos, searchRadius, { 1,1,0 }, DEBUGRENDERER2D->NextDepthSlice());
		for (const auto& pEnemyAgent : *pAgentsVec)
		{
			if (pEnemyAgent == pEnemyAgent) continue;
			const float distSqr = pEnemyAgent->GetPosition().DistanceSquared(agentPos);
			if (distSqr < closestDistSqr && agentRadius > (pEnemyAgent->GetRadius() + radiusMargin))
			{
				closestDistSqr = distSqr;
				pClosestAgent = pEnemyAgent;
			}
		}

		if (pClosestAgent != nullptr)
		{
			pBlackboard->ChangeData("Target", pClosestAgent->GetPosition());
			return true;
		}
		return false;
	}*/
}








#endif