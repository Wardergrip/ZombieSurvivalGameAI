#include "stdafx.h"
#include "InventoryManager.h"


#include "HelperFuncts.h"

InventoryManager::InventoryManager(IExamInterface* pInterface)
	:m_pInterface{ pInterface } 
{
	m_Inventory.reserve(m_pInterface->Inventory_GetCapacity());
	for (UINT i{ 0 }; i < m_Inventory.capacity(); ++i)
	{
		m_Inventory.push_back(eItemType::RANDOM_DROP);
	}
}

void InventoryManager::DEBUG_PrintInv() const
{
	UINT i{ 0 };
	for (auto item : m_Inventory)
	{
		std::cout << "SLOT [" << i << "]: ";
		++i;
		switch (item)
		{
		case eItemType::FOOD:
			std::cout << "FOOD";
			break;
		case eItemType::GARBAGE:
			std::cout << "GARBAGE";
			break;
		case eItemType::MEDKIT:
			std::cout << "MEDKIT";
			break;
		case eItemType::PISTOL:
			std::cout << "PISTOL";
			break;
		case eItemType::RANDOM_DROP:
			std::cout << "RANDOM_DROP (Empty)";
			break;
		case eItemType::RANDOM_DROP_WITH_CHANCE:
			std::cout << "RANDOM_DROP_WITH_CHANCE (Wtf)";
			break;
		case eItemType::SHOTGUN:
			std::cout << "SHOTGUN";
			break;
		default:
			break;
		}
		std::cout << '\n';
	}
}

bool InventoryManager::HaveGun() const
{
	ItemInfo itemInfo;
	for (UINT i{ 0 }; i < m_pInterface->Inventory_GetCapacity(); ++i)
	{
		if (m_pInterface->Inventory_GetItem(i, itemInfo))
		{
			if (itemInfo.Type == eItemType::PISTOL || itemInfo.Type == eItemType::SHOTGUN)
			{
				return true;
			}
		}
	}
	return false;
}

bool InventoryManager::HaveItem(eItemType itemType) const
{
	ItemInfo itemInfo;
	for (UINT i{ 0 }; i < m_pInterface->Inventory_GetCapacity(); ++i)
	{
		if (m_Inventory.at(i) == eItemType::RANDOM_DROP) continue;

		if (m_pInterface->Inventory_GetItem(i, itemInfo))
		{
			if (itemInfo.Type == itemType)
			{
				return true;
			}
		}
	}
	return false;
}

bool InventoryManager::IsInventoryFull() const
{
	return GetFreeItemSlot() == invalid_inventory_slot;
}

UINT InventoryManager::GetFreeItemSlot() const
{
	ItemInfo itemInfo;
	for (UINT i{ 0 }; i < m_pInterface->Inventory_GetCapacity(); ++i)
	{
		if (!m_pInterface->Inventory_GetItem(i, itemInfo))
		{
			return i;
		}
	}

	return invalid_inventory_slot;
}

bool InventoryManager::GrabAndAddItem(EntityInfo entityInfo)
{
	if (entityInfo.Type != eEntityType::ITEM)
	{
		return false;
	}
	ItemInfo itemInfo;
	m_pInterface->Item_GetInfo(entityInfo, itemInfo);
	if (itemInfo.Type == eItemType::GARBAGE)
	{
		m_pInterface->Item_Destroy(entityInfo);
		return true;
	}
	if (IsInventoryFull())
	{
		return false;
	}

	if (!m_pInterface->Item_Grab(entityInfo, itemInfo)) return false;
	UINT slot = GetFreeItemSlot();
	m_pInterface->Inventory_AddItem(slot, itemInfo);
	m_Inventory[slot] = itemInfo.Type;
	return true;
}

bool InventoryManager::UseMedkit()
{
	if (!HaveItem(eItemType::MEDKIT))
	{
		return false;
	}

	const float agentHP{ m_pInterface->Agent_GetInfo().Health };
	const constexpr float thresholdHp{ 9 }; // Will not consider using medkits if above this
	const constexpr float maxHp{ 10 };
	if (agentHP > thresholdHp) return false;

	UINT idx = std::distance(m_Inventory.begin(), std::find(m_Inventory.begin(), m_Inventory.end(), eItemType::MEDKIT));
	ItemInfo itemInfo{};
	if (m_pInterface->Inventory_GetItem(idx, itemInfo))
	{
		const int medkitHp{ m_pInterface->Medkit_GetHealth(itemInfo) };
		// Is the difference in max HP and current HP bigger or equal to medkitHP?
		// This is to circumvent overhealing.
		if (maxHp - agentHP >= medkitHp)
		{
			m_pInterface->Inventory_UseItem(idx);
		}
		if (medkitHp <= 0)
		{
			m_pInterface->Inventory_RemoveItem(idx);
			m_Inventory.at(idx) = eItemType::RANDOM_DROP;
		}
		return true;
	}

	return false;
}

void InventoryManager::DeleteGarbage()
{
	ItemInfo itemInfo;
	for (UINT i{ 0 }; i < m_pInterface->Inventory_GetCapacity(); ++i)
	{
		if (m_pInterface->Inventory_GetItem(i, itemInfo))
		{
			if (itemInfo.Type == eItemType::GARBAGE)
			{
				m_pInterface->Inventory_RemoveItem(i);
			}
		}
	}
}
