#include "Client_pch.h"
#include "Client_Defines.h"
#include "Inventory.h"

// Singleton
#include "GameInstance.h"
#include "Item_Manager.h"
#include "Quest_Manager.h"

#include "Client_Functions.h"

IMPLEMENT_SINGLETON(CInventory)

HRESULT CInventory::Initialize()
{
	m_pItem_Manager = CItem_Manager::Get_Instance();
	Safe_AddRef(m_pItem_Manager);

	for (_uint i = 0; i < (_uint)CATEGORY::CATEGORY_END; ++i)
	{
		array<vector<INVENTORY_ITEM*>, MAX_PAGE_SIZE> inventory;
		for (_uint i = 0; i < MAX_PAGE_SIZE; ++i)
		{
			vector<INVENTORY_ITEM*> vecInven;
			vecInven.resize(MAX_INVENTORY_SIZE);
			inventory[i] = vecInven;
		}

		m_inventory.emplace(i, inventory);
	}

	m_dummy.resize(MAX_INVENTORY_SIZE);

	return S_OK;
}

void CInventory::Tick()
{
	_Organize_Vector();
	_Organize_Array();
}

HRESULT CInventory::Add_Item(_uint _iItemID, _uint _iCount)
{
	/* �Ű������� �Ѿ�� _iItemID�� �����Ǵ� ������ ������ ������ �Ŵ��� ��ü���� ������ */
	auto& tInfo = m_pItem_Manager->Get_ItemInfo(_iItemID);

	/* ���࿡ ������ ������ ������ �Ŵ������� ã�� �������� return E_FAIL */
	if (0 == tInfo.iID)
		return E_FAIL;

	auto iter = m_inventory.find(tInfo.iType);

#pragma region ������ �ڵ�
	/* m_inventory���� �ϴ� �Ű������� �Ѿ�� �������� iType���� �ش��ϴ� ī�װ��� �迭�� ã������,
	�� �迭�� ó������ ������ ��ȸ�ϸ鼭 �� �ȿ� ����ִ� _pItem�� ���ڷ� ���� �Լ��� ������.
	find_if�Լ��� ���� �Լ��� ����־��� �� true�� ��� _pItem�� ����ִ� iterator(=�迭 ���� �ϳ�)�� ��ȯ */
	//auto findIter = find_if(m_inventory[tInfo.iType].begin(), m_inventory[tInfo.iType].end(),
		//[tInfo, _iCount](INVENTORY_ITEM* _pItem)
		//{
			/* ���࿡ �� ĭ�� �������� ���� ��� */
			//if (nullptr == _pItem)
				//return false;

			/* ������ ����ִ� �������� ������ �Ű������� �Ѿ�� �������� ������ ������ �� 
			�Ű� ������ �Ѿ�� �������� �ִ� ������ �Ѿ ��� */
			//if (tInfo.iMaximum <= _pItem->iCount + _iCount)
				//return false;

			/* ���࿡ Add_Item�Ϸ��� �Ű������� ���޵� �������� id�� ���� �κ��丮 �迭�� ����ִ� ���̵� �ٸ� ��� */
			//if (tInfo.iID != _pItem->iItemID)
				//return false;

			/* 1. �� ĭ�� �������� �ְ�
			   2. �� ĭ�� ����ִ� �������� ������ �Ű������� �Ѿ�� �������� ������ ������ ��, �Ű������� �Ѿ�� �������� �ִ� ������ �Ѿ�� �ʰ�
			   3. Add_Item�Ϸ��� �Ű������� ���޵� �������� id�� ���� �κ��丮 �迭�� �� �ִ� ���̵� ���� ��� */
			//return true;
		//});

	/* ���࿡ findIter�� end()�� �ƴϸ�(== Add_Item �Ϸ��� �������� �̹� �κ��� �ְ�, ���� ������ ����, ���� 2�����ǿ� ������ ��)*/
	//if (findIter != m_inventory[tInfo.iType].end())
	//{
		/* �κ��丮�� �ִ� �������� ������ �Ű������� �Ѿ�� �������� ������ŭ ���� */
		//(*findIter)->iCount += _iCount;
	//}

	/* findIter�� end()�� ��(==������ Add_Item�Ϸ��� �������� �κ��丮�� ���� �� */
	//else
	//{
		/* m_inventory���� �ϴ� �Ű������� �Ѿ�� �������� iType���� �ش��ϴ� ī�װ��� �迭�� ã������,
		�� �迭���� ����ִ� ĭ�� ã�´�. */
		//auto findNullptrIter = find_if(m_inventory[tInfo.iType].begin(), m_inventory[tInfo.iType].end(),
			//[](INVENTORY_ITEM* _pItem)
			//{
				//if (nullptr != _pItem)
					//return false;

				//return true;
			//});

		/* ���࿡ ����ִ� ĭ�� ã������ */
		//if (findNullptrIter != m_inventory[tInfo.iType].end())
			/* �� ĭ�� Add_Item�Ϸ��� �������� ������ �߰� */
			//(*findNullptrIter) = new INVENTORY_ITEM(_iItemID, _iCount);

		/* ���࿡ ����ִ� ĭ�� ã�� �������� Add_Item�Լ� ���� */
		//else
			//return E_FAIL;
	//}
// return S_OK;
#pragma endregion
#pragma region INHO
	/* �ش� ī�װ��� array�� ù ���������� ������ ���������� ���鼭 ��ȸ */
	for (auto arrIter = m_inventory[tInfo.iType].begin(); arrIter != m_inventory[tInfo.iType].end(); ++arrIter)
	{
		auto findIter = find_if((*arrIter).begin(), (*arrIter).end(),
			[tInfo, _iCount](INVENTORY_ITEM* _pItem)
			{
				/* ���࿡ �� ĭ�� �������� ���� ��� */
				if (nullptr == _pItem)
					return false;

				/* ������ ����ִ� �������� ������ �Ű������� �Ѿ�� �������� ������ ������ ��
				�Ű� ������ �Ѿ�� �������� �ִ� ������ �Ѿ ��� */
				if (tInfo.iMaximum <= _pItem->iCount + _iCount)
					return false;

				/* ���࿡ Add_Item�Ϸ��� �Ű������� ���޵� �������� id�� ���� �κ��丮�� ����ִ� ���̵� �ٸ� ��� */
				if (tInfo.iID != _pItem->iItemID)
					return false;

				/* 1. �� ĭ�� �������� �ְ�
				2. �� ĭ�� ����ִ� �������� ������ �Ű������� �Ѿ�� �������� ������ ������ ��, �Ű������� �Ѿ�� �������� �ִ� ������ �Ѿ�� �ʰ�
				3. Add_Item�Ϸ��� �Ű������� ���޵� �������� id�� ���� �κ��丮�� �� �ִ� ���̵� ���� ��� */
				return true;
			});

			/* ���࿡ findIter�� end()�� �ƴϸ�(== Add_Item �Ϸ��� �������� �̹� �κ��� �ְ�, ���� ������ ����, ���� 2�����ǿ� ������ ��)*/
			if (findIter != (*arrIter).end())
			{
				/* �κ��丮�� �ִ� �������� ������ �Ű������� �Ѿ�� �������� ������ŭ ���� */
				(*findIter)->iCount += _iCount;

				CQuest_Manager::Get_Instance()->Notify(_iItemID, _iCount);

				return S_OK;
			}

			/* findIter�� end()�� ��(==������ Add_Item�Ϸ��� �������� �ش� array�� ���� �� */
			//else
			//{
			//	/* vector �����̳��� �� ĭ�� ã�´�. */
			//	auto findNullptrIter = find_if((*arrIter).begin(), (*arrIter).end(),
			//		[](INVENTORY_ITEM* _pItem)
			//		{
			//			if (nullptr != _pItem)
			//				return false;
			//	
			//			return true;
			//		});

			//	/* ���࿡ ����ִ� ĭ�� ã������ */
			//	if (findNullptrIter != (*arrIter).end())
			//	{
			//		/* �� ĭ�� Add_Item�Ϸ��� �������� ������ �߰� */
			//		(*findNullptrIter) = new INVENTORY_ITEM(_iItemID, _iCount);

			//		CQuest_Manager::Get_Instance()->Notify(_iItemID, _iCount);

			//		return S_OK;
			//	}
			//		

			//	/* ���࿡ ����ִ� ĭ�� ã�� �������� Add_Item�Լ� ���� */
			//	//else
			//		//return E_FAIL;
			//}
	}

	/* �ݺ��� ���� ���� ������ �߰� ��������, �ٽ� �ݺ��� ���鼭 �� ĭ �ִ��� ã��, �� ĭ ������ �߰� */
	for (auto arrIter = m_inventory[tInfo.iType].begin(); arrIter != m_inventory[tInfo.iType].end(); ++arrIter)
	{
		auto& vec = (*arrIter);

		for (size_t i = 0; i < vec.size(); ++i)
		{
			/* �� ĭ �ƴ� �� */
			if (nullptr != vec[i])
			{
				/* �� ĭ�� ����ִ� �������� ID�� �߰��Ϸ��� �������� ID�� ������ */
				if (vec[i]->iItemID == _iItemID)
				{
					/* �Ѿ�� ������� �߰��ϸ� ���� �ִ� ������ �ʰ��ϴ� ��� */
					// 99 <= 97 + 10
					if(tInfo.iMaximum <= vec[i]->iCount + _iCount)
					{
						// �ϴ� tInfo.iMaximum - vec[i]->iCount��ŭ�� �߰��Ѵ�.
						_uint iCanAddCount = tInfo.iMaximum - vec[i]->iCount;
						vec[i]->iCount += iCanAddCount;
						// �׸��� _iCount���� iCanAddCount��ŭ ����, ���� _iCount�� �� ���� �߰��ؾ���
						_iCount -= iCanAddCount;

						CQuest_Manager::Get_Instance()->Notify(_iItemID, iCanAddCount);
						continue;
					}
				}
			}
			/* �� ĭ ã���� */
			else
			{
				const auto& tItemInfo = CItem_Manager::Get_Instance()->Get_ItemInfo(_iItemID);

				/* �߰��Ϸ��� ������ ������ �� �������� maximum���� ũ�� */
				if(_iCount > tItemInfo.iMaximum)
				{
					/* �ϴ� �� ĭ �����ϱ� iMaximum��ŭ�� �߰� */
					vec[i] = new INVENTORY_ITEM(_iItemID, tItemInfo.iMaximum);

					CQuest_Manager::Get_Instance()->Notify(_iItemID, tItemInfo.iMaximum);
					
					/* ��� */
					if (SUCCEEDED(Add_Item(_iItemID, _iCount - tItemInfo.iMaximum)))
						return S_OK;
				}

				/* �߰��Ϸ��� ������ ������ �� �������� maximum���� �۰ų� ������ */
				else
				{
					/* ������ ���� �߰� */
					vec[i] = new INVENTORY_ITEM(_iItemID, _iCount);

					CQuest_Manager::Get_Instance()->Notify(_iItemID, _iCount);

					return S_OK;
				}
			}
		}
	}

	/* �߰� �������� */
	MSG_BOX("Failed to Add_Item!!");
	return E_FAIL;

#pragma endregion

}

HRESULT CInventory::Remove_Item(_uint _iItemID, _uint _iCount)
{

#pragma region ������ �ڵ�
	//for (auto& pairItems : m_inventory)
	//{
	//	auto& items = pairItems.second;

	//	auto iter = find_if(items.begin(), items.end(), [_iItemID](INVENTORY_ITEM* _pItem)
	//		{
	//			if (nullptr == _pItem)
	//				return false;

	//			if (_iItemID != _pItem->iItemID)
	//				return false;

	//			return true;
	//		});

	//	// Ž�� ����
	//	if (iter != items.end())
	//	{
	//		auto& pItem = *iter;

	//		if (pItem->iCount >= _iCount)
	//			pItem->iCount -= _iCount;

	//		if (0 >= pItem->iCount)
	//		{
	//			Safe_Delete(pItem);
	//			/* vector �����̳� ������ �߰��� */
	//			items.erase(iter);
	//		}
	//			

	//		return S_OK;
	//	}
	//}

	//return E_FAIL;
#pragma endregion
#pragma region INHO
	/* ��� ī�װ� ��ȸ */
	for (auto pairIter = m_inventory.begin(); pairIter != m_inventory.end(); ++pairIter)
	{
		auto& arr = pairIter->second;

		/* array�� iter ��ȸ */
		for (auto arrIter = arr.begin(); arrIter != arr.end(); ++arrIter)
		{
			auto& vec = (*arrIter);

			/* vector�� iter ��ȸ */
			auto iter = find_if(vec.begin(), vec.end(), [_iItemID](INVENTORY_ITEM* _pItem)
						{
							/* ������ �κ��丮 ĭ�� ������� �� */
							if (nullptr == _pItem)
								return false;

							/* ������ �κ��丮�� �ִ� �����۰� �����Ϸ��� �������� ID�� �ٸ� �� */
							if (_iItemID != _pItem->iItemID)
								return false;

							/* 1. ������ �κ��丮 ĭ�� ������� �ʰ� 
							   2. �κ��丮 ĭ�� �ִ� �����۰� �����Ϸ��� �������� ID�� ���� �� */
							return true;
						});

			// Ž�� ����
			if (iter != vec.end())
			{
				auto& pItem = *iter;

				/* ex) ������ �κ��丮 ĭ�� �ִ� �������� ���� 10��, �����Ϸ��� ������ ���� 9����
				10 - 9 = 1, 1�� �����. */
				if (pItem->iCount >= _iCount)
					pItem->iCount -= _iCount;

				/* �����Ϸ��� �������� ������ 0�� ������ ��, ������ ����*/
				if (0 >= pItem->iCount)
				{
					Safe_Delete(pItem);
				}

				return S_OK;
			}
		}
	}

	return E_FAIL;
#pragma endregion
}

/*
HRESULT CInventory::Pop_Equipment(_uint _iCategory, _uint _iPage, _uint _iIndex, INVENTORY_ITEM** _ppItem)
{
	if (m_inventory.size() <= _iCategory)
		return E_FAIL;

	auto& arr = m_inventory[_iCategory];

	if (arr.size() <= _iPage)
		return E_FAIL;

	auto& vec = arr[_iPage];

	if (vec.size() <= _iIndex)
		return E_FAIL;

	// ���� ������ ���
	INVENTORY_ITEM* pItemTemp = vec[_iIndex];
	if (nullptr == pItemTemp)
		return E_FAIL;

	// �κ��丮���� pop
	vec[_iIndex] = nullptr;

	// ����س��� �ּ� ��ȯ
	*_ppItem = pItemTemp;

	return S_OK;
}

HRESULT CInventory::Push_Equipment(INVENTORY_ITEM* _pItem)
{
	// ���ϱ� ���� �ȴþ
	// -> �׳� �� �� ������ �� ĭ ã�Ƽ� �ű�ٰ� ������ ��
	// -> �ٵ� ���ϱ� ��� ī�װ��� ����
	auto& arr = m_inventory[(size_t)CATEGORY::CATEGORY_EQUIPMENT];

	for (auto arrIter = arr.begin(); arrIter != arr.end(); ++arrIter)
	{
		auto& vec = (*arrIter);

		for (auto vecIter = vec.begin(); vecIter != vec.end(); ++vecIter)
		{
			// �� ĭ �߰��ϸ� �ű�ٰ� ����ְ� return;
			if (nullptr == *vecIter)
			{
				*vecIter = _pItem;
				return S_OK;
			}
		}
	}

	return E_FAIL;
}
*/

const ITEM_INFO_DESC& CInventory::Get_ItemInfo(_uint _iIndex, _uint _iCategory, _uint _iPage) const
{
#pragma region ������ �ڵ�
	/*auto iter = m_inventory.find(_iCategory);

	if (iter == m_inventory.end())
		return m_pItem_Manager->Get_ItemInfo(0);

	if (nullptr != iter->second[_iIndex])
		return m_pItem_Manager->Get_ItemInfo(iter->second[_iIndex]->iItemID);

	else
		return m_pItem_Manager->Get_ItemInfo(0);*/
#pragma endregion

#pragma region INHO
	auto iter = m_inventory.find(_iCategory);

	/* _iCategory�� �ش��ϴ� ī�װ��� ã�� ������ �� */
	if (iter == m_inventory.end())
		return m_pItem_Manager->Get_ItemInfo(0);

	/* _iCategory�� �ش��ϴ� ī�װ��� ã���� ��, �� ī�װ��� array�� ������� */
	auto& arr = iter->second;

	/* _iPage�� �ش��ϴ� �������� ã����, �� �������� vector�� ������� */
	auto& vec = arr[_iPage];

	/* �ش� �κ��丮 ĭ�� �������� ������ */
	if (nullptr != vec[_iIndex])
		return m_pItem_Manager->Get_ItemInfo(vec[_iIndex]->iItemID);

	/* �ش� �κ��丮 ĭ�� �������� ������ */
	else
		return m_pItem_Manager->Get_ItemInfo(0);
#pragma endregion
}

_uint CInventory::Get_ItemID(_uint _iIndex, _uint _iCategory) const
{
	_uint iID = 0;

	return iID;
}

const vector<INVENTORY_ITEM*>& CInventory::Get_Items(_uint _iCategory, _uint _iPage)
{
	auto iter = m_inventory.find(_iCategory);

	if (iter == m_inventory.end())
		return m_dummy;

	auto& arr = iter->second;
	return arr[_iPage];
}

/*
const array<vector<INVENTORY_ITEM*>, CInventory::MAX_PAGE_SIZE>& CInventory::Get_Items_Arr(_uint _iCategory)
{
	auto iter = m_inventory.find(_iCategory);

	if (iter == m_inventory.end())
		return m_arrDummy;

	return iter->second;
}

_uint CInventory::Get_CookedCount(_uint _iID)
{
	_uint iCount = 0;

	auto& arr = m_inventory[(size_t)CATEGORY::CATEGORY_COOK];

	for (auto arrIter = arr.begin(); arrIter != arr.end(); ++arrIter)
	{
		auto& vec = (*arrIter);

		for (auto vecIter = vec.begin(); vecIter != vec.end(); ++vecIter)
		{
			// ������ ����ִ� ĭ �߰��ϸ�
			if (nullptr != *vecIter)
			{
				auto tItemInfo = (*vecIter);
				// ã������ �丮 ID�� ��ġ�ϸ�
				if (_iID == tItemInfo->iItemID)
				{
					iCount += tItemInfo->iCount;
				}
			}
		}
	}

	return iCount;
}

_uint CInventory::Get_CookedCount(_uint _iID, _uint _iInvenCellIndex)
{
	_uint iIndex = 0;

	auto& arr = m_inventory[(size_t)CATEGORY::CATEGORY_COOK];

	for (auto arrIter = arr.begin(); arrIter != arr.end(); ++arrIter)
	{
		auto& vec = (*arrIter);

		for (auto vecIter = vec.begin(); vecIter != vec.end(); ++vecIter)
		{
			// ������ ����ִ� ĭ �߰��ϸ�
			if (nullptr != *vecIter)
			{
				auto tItemInfo = (*vecIter);
				// ã������ �丮 ID�� ��ġ�ϰ�, �κ��丮 �ε����� ��ġ�ϸ�
				if (_iID == tItemInfo->iItemID &&
					iIndex == _iInvenCellIndex)
				{
					return tItemInfo->iCount;
				}
			}

			++iIndex;
		}
	}

	// �ش� �丮 �߰� ���ϸ� 0 ��ȯ
	return 0;
}

_uint CInventory::Get_MirrorCityCoinCount()
{
	auto& arr = m_inventory[(size_t)CATEGORY::CATEGORY_INGRIDIENT];

	for (auto arrIter = arr.begin(); arrIter != arr.end(); ++arrIter)
	{
		auto& vec = (*arrIter);

		for (auto vecIter = vec.begin(); vecIter != vec.end(); ++vecIter)
		{
			// ������ ����ִ� ĭ �߰��ϸ�
			if (nullptr != *vecIter)
			{
				auto tItemInfo = (*vecIter);
				// ã������ �̷���Ƽ ���� ID�� ��ġ�ϸ�
				if (100001 == tItemInfo->iItemID)
				{
					return tItemInfo->iCount;
				}
			}
		}
	}

	// �� �����µ� �̷���Ƽ ������ ������
	return 0;
}

void CInventory::_Organize_Vector()
{
	// �κ��丮 �߰��� ����ִ� �κ� ������ �ڿ� �ִ°� ���ܾ���
	for (auto mapIter = m_inventory.begin(); mapIter != m_inventory.end(); ++mapIter)
	{
		auto& arr = mapIter->second;
		for (auto arrIter = arr.begin(); arrIter != arr.end(); ++arrIter)
		{
			auto& vec = *arrIter;

			// ��� ���Ұ� nullptr�̸� continue
			if (all_of(vec.begin(), vec.end(), [](INVENTORY_ITEM* item) { return item == nullptr; }))
				continue;

			// ��� ���Ұ� ������������ continue;
			if (all_of(vec.begin(), vec.end(), [](INVENTORY_ITEM* item) { return item != nullptr; }))
				continue;

			for (auto vecIter = vec.begin(); vecIter != vec.end();)
			{
				if (nullptr == *vecIter)
				{
					// ��ĭ�� �߰����� �� �� ĭ �ڿ� �� nullptr�϶��� break;
					if (all_of(vecIter + 1, vec.end(), [](INVENTORY_ITEM* item) { return item == nullptr; }))
						break;

					// ���� ���� ������ ��� �׸��� �� ĭ�� ������ �̵�
					auto resultIter = std::rotate(vecIter, vecIter + 1, vec.end());

					// ������ �׸��� nullptr�� �ʱ�ȭ
					*(vec.end() - 1) = nullptr;

					vecIter = resultIter;
				}
				else
				{
					++vecIter;
				}
			}
		}
	}
}

void CInventory::_Organize_Array()
{
	// ������ �� ���� ���� ��
	for (auto mapIter = m_inventory.begin(); mapIter != m_inventory.end(); ++mapIter)
	{
		auto& arr = mapIter->second;

		for (auto arrIter = arr.begin(); arrIter != arr.end(); ++arrIter)
		{
			auto& vec = *arrIter;

			// ���� vector�� �����ְų�, ���� vector�� end() �ų�, ������ vector�� �� ��������� ���� �ʿ� ����
			if(all_of(vec.begin(), vec.end(), [](INVENTORY_ITEM* item) { return item != nullptr; }) ||
				arr.end() == (arrIter + 1) || all_of((*(arrIter + 1)).begin(), (*(arrIter + 1)).end(), [](INVENTORY_ITEM* item) { return item == nullptr; }))
				continue;

			for (auto vecIter = vec.begin(); vecIter != vec.end();)
			{
				if (nullptr == *vecIter)
				{
					// vector �߰��� �� ĭ �߰����� �� �� �� ĭ ���� ������ �� ĭ����
					size_t distanceToEnd = distance(vecIter, vec.end());
					// ex) distanceToEnd = 5;

					auto arrIterTemp = arrIter;
					++arrIterTemp;
					while (arrIterTemp != arr.end() &&
						!(all_of(vec.begin(), vec.end(), [](INVENTORY_ITEM* item) { return item == nullptr; })))
					{
						auto& vecItem = (*arrIterTemp);

						auto beginIter = vecItem.begin();
						auto endIter = vecItem.begin() + distanceToEnd;

						vector<INVENTORY_ITEM*> tempVec;

						// ���� vector���� �� vector�� �� ��ŭ ������ �����ͼ�
						for (auto vecItemIter = vecItem.begin(); vecItemIter != endIter; ++vecItemIter)
						{
							tempVec.push_back(*vecItemIter);
							*vecItemIter = nullptr;
						}

						// �� vector���ٰ� �� ���������� �������
						auto arrIterFront = arrIterTemp - 1;
						auto& vecFront = (*arrIterFront);

						for (auto iterVecFront = vecFront.begin(); iterVecFront != vecFront.end(); ++iterVecFront)
						{
							if (nullptr == *iterVecFront)
							{
								for (auto iterVecFrontTemp = iterVecFront; iterVecFrontTemp != vecFront.end(); ++iterVecFrontTemp)
								{
									int iIndex = 0;
									(*iterVecFrontTemp) = tempVec[iIndex];
									++iIndex;
								}
							}
						}

						// ���� ������ ���� ���� vector�� �� ĭ�� ��� ����
						//for (auto vecIteratorTemp = (*arrIterTemp).begin(); vecIteratorTemp != (*arrIterTemp).end(); ++vecIteratorTemp)
						//{
						//	if (nullptr == *vecIteratorTemp)
						//		distanceToEnd = distance(vecIteratorTemp, (*arrIterTemp).end());
						//}

						//++arrIterTemp;
						return;
					}
				}
				else
				{
					++vecIter;
				}
			}
		}
	}
}
*/

_uint CInventory::Get_ItemCount(_uint _iID)
{
	_uint iCount = 0;

	for (auto& pairCategory : m_inventory)
	{
		for (auto& page : pairCategory.second)
		{
			if (page.empty())
				continue;

			for_each(page.begin(), page.end(),
				[&iCount, iID = _iID](const INVENTORY_ITEM* _pItem)
				{
					if (nullptr == _pItem) return;
					if (_pItem->iItemID == iID)	iCount += _pItem->iCount;
				});
		}
	}

	return iCount;
}

void CInventory::Free()
{
	Safe_Release(m_pItem_Manager);

	for (auto& pairInventory : m_inventory)
	{
		auto& arr = pairInventory.second;
		for (auto& vec : arr)
		{
			for (auto& pItem : vec)
			{
				Safe_Delete(pItem);
			}

			vec.clear();
		}
	}

	m_inventory.clear();
}
