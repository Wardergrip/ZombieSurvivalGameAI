#include "stdafx.h"
#include "SteeringManager.h"
#include "HelperFuncts.h"

SteeringManager::SteeringManager(IExamInterface* pInterface, SteeringPlugin_Output* pSteering)
	:m_pInterface{ pInterface }
	,m_pSteering{ pSteering }
{
}

void SteeringManager::AutoOrient(bool state)
{
	m_pSteering->AutoOrient = state;
}

void SteeringManager::Seek(const Elite::Vector2& target)
{
	auto agentInfo = m_pInterface->Agent_GetInfo();

	auto nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(target);

	m_pSteering->LinearVelocity = nextTargetPos - agentInfo.Position; //Desired Velocity
	m_pSteering->LinearVelocity.Normalize();						  //Normalize Desired Velocity
	m_pSteering->LinearVelocity *= agentInfo.MaxLinearSpeed;		  //Rescale to Max Speed

	m_pInterface->Draw_Circle(target, 2.f, Elite::Vector3{ 0,1.f,1.f });
}

void SteeringManager::Flee(const Elite::Vector2& target, float factor)
{
	auto agentInfo = m_pInterface->Agent_GetInfo();

	Elite::Vector2 desiredDirection = (target - agentInfo.Position);
	auto newTarget = agentInfo.Position - (desiredDirection.GetNormalized() * factor);
	Seek(newTarget);

	m_pInterface->Draw_Circle(target, 2.f, Elite::Vector3{ 1.f,0.8f,0.f });
}

void SteeringManager::Wander()
{
	AutoOrient(true);

	auto agentInfo = m_pInterface->Agent_GetInfo();

	float wanderAngle{ agentInfo.Orientation };
	const constexpr float maxAngleChange{ 10.f };
	const constexpr float offsetDistance{ 10.f };
	const constexpr float radius{ 5.f };
	const float randomAngle{ HF::ToRadians(static_cast<float>(rand() % static_cast<int>(HF::ToDegrees(maxAngleChange)))) };
	const float sign{ (rand() % 2) ? 1.f : -1.f };
	const float nextAngle{ wanderAngle + (sign * randomAngle) };
	wanderAngle = nextAngle;
	Elite::Vector2 circleCenter{ agentInfo.Position + (HF::AngleToLookDirection(agentInfo.Orientation) * offsetDistance) };
	Elite::Vector2 target{ circleCenter.x + (std::cosf(wanderAngle) * radius),circleCenter.y + (std::sinf(wanderAngle) * radius) };
	Elite::Vector2 targetVector{ target - agentInfo.Position };

	Seek(target);

	m_pInterface->Draw_Circle(circleCenter, radius, Elite::Vector3{ 0,1,0 });
	m_pInterface->Draw_Direction(agentInfo.Position, targetVector, offsetDistance, Elite::Vector3{ 0,1,0 });
}

void SteeringManager::SpinAround()
{
	AutoOrient(false);

	auto agentInfo = m_pInterface->Agent_GetInfo();

	m_pSteering->AngularVelocity = agentInfo.MaxAngularSpeed;
}

void SteeringManager::Face(const Elite::Vector2& target)
{
	AutoOrient(false);

	auto agentInfo = m_pInterface->Agent_GetInfo();

	Elite::Vector2 desiredDirection = (target - agentInfo.Position);
	desiredDirection.Normalize();
	const float agentRot{ agentInfo.Orientation + 0.5f * static_cast<float>(M_PI) };
	Elite::Vector2 agentDirection{ std::cosf(agentRot),std::sinf(agentRot) };
	m_pSteering->AngularVelocity = (desiredDirection.Dot(agentDirection)) * agentInfo.MaxAngularSpeed;

	auto dir = agentInfo.Position - target;
	auto dist = dir.Normalize();
	m_pInterface->Draw_Direction(agentInfo.Position, dir, dist, Elite::Vector3{ 1.f,0,1.0f });
}

void SteeringManager::StopMovement()
{
	m_pSteering->LinearVelocity = Elite::ZeroVector2;
}
