#ifndef INVENTORYMANAGER
#define INVENTORYMANAGER
#include "Exam_HelperStructs.h"
#include "IExamInterface.h"
#include <vector>

#define invalid_inventory_slot 4294967295U

class InventoryManager final
{
public:
public:
	InventoryManager(IExamInterface* pInterface);
	~InventoryManager() = default;

	InventoryManager(const InventoryManager& inventoryManager) = delete;
	InventoryManager(InventoryManager&& inventoryManager) = delete;
	InventoryManager& operator=(const InventoryManager& inventoryManager) = delete;
	InventoryManager& operator=(InventoryManager&& inventoryManager) = delete;

	void DEBUG_PrintInv() const;

	bool HaveGun() const;
	bool HaveItem(eItemType itemType) const;
	bool IsInventoryFull() const;

	UINT GetFreeItemSlot() const;

	bool GrabAndAddItem(EntityInfo entityInfo);
	bool UseMedkit();
	bool UseFood();
	void DeleteGarbage();
private:
	IExamInterface* m_pInterface = nullptr;
	std::vector<eItemType> m_Inventory;
};

#endif 