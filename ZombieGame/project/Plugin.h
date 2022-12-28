#pragma once
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"

#include "EDecisionMaking.h"
#include "EBehaviorTree.h"
#include "EBlackboard.h"

#include "HouseCheck.h"

class IBaseInterface;
class IExamInterface;
class InventoryManager;
struct Timer;


class Plugin :public IExamPlugin
{
public:
	Plugin() {};
	virtual ~Plugin() {};

	void Initialize(IBaseInterface* pInterface, PluginInfo& info) override;
	void DllInit() override;
	void DllShutdown() override;

	void InitGameDebugParams(GameDebugParams& params) override;
	void Update(float dt) override;

	SteeringPlugin_Output UpdateSteering(float dt) override;
	void Render(float dt) const override;

private:
	//Interface, used to request data from/perform actions with the AI Framework
	IExamInterface* m_pInterface = nullptr;
	std::vector<EntityInfo> GetEntitiesInFOV() const;
	std::vector<HouseInfo> GetHousesInFOV() const;

	bool UpdateHousesInFOV();
	bool UpdateEntitiesInFOV();

	Elite::Vector2 m_Target = {};
	bool m_CanRun = false; //Demo purpose
	bool m_GrabItem = false; //Demo purpose
	bool m_UseItem = false; //Demo purpose
	bool m_RemoveItem = false; //Demo purpose
	float m_AngSpeed = 0.f; //Demo purpose

	UINT m_InventorySlot = 0;

	Elite::BehaviorTree* m_pBehaviorTree{ nullptr };
	SteeringPlugin_Output* m_pSteeringOutputData;
	InventoryManager* m_pInventoryManager;
	std::vector<EntityInfo>* m_pEntitiesInFOV;
	std::vector<HouseInfo>* m_pHousesInFOV;
	std::vector<HouseCheck>* m_pHousesChecked;
	Timer* m_pLastDangerTimer;
};

//ENTRY
//This is the first function that is called by the host program
//The plugin returned by this function is also the plugin used by the host program
extern "C"
{
	__declspec (dllexport) IPluginBase* Register()
	{
		return new Plugin();
	}
}