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
	/* 매개변수로 넘어온 _iItemID에 대응되는 아이템 정보를 아이템 매니저 객체에서 가져옴 */
	auto& tInfo = m_pItem_Manager->Get_ItemInfo(_iItemID);

	/* 만약에 아이템 정보를 아이템 매니저에서 찾지 못했으면 return E_FAIL */
	if (0 == tInfo.iID)
		return E_FAIL;

	auto iter = m_inventory.find(tInfo.iType);

#pragma region 진서형 코드
	/* m_inventory에서 일단 매개변수로 넘어온 아이템의 iType값에 해당하는 카테고리의 배열을 찾은다음,
	그 배열을 처음부터 끝까지 순회하면서 그 안에 들어있던 _pItem을 인자로 람다 함수에 전달함.
	find_if함수는 람다 함수에 집어넣었을 때 true를 뱉는 _pItem이 들어있던 iterator(=배열 공간 하나)를 반환 */
	//auto findIter = find_if(m_inventory[tInfo.iType].begin(), m_inventory[tInfo.iType].end(),
		//[tInfo, _iCount](INVENTORY_ITEM* _pItem)
		//{
			/* 만약에 그 칸에 아이템이 없을 경우 */
			//if (nullptr == _pItem)
				//return false;

			/* 기존에 들어있던 아이템의 개수에 매개변수로 넘어온 아이템의 개수를 더했을 때 
			매개 변수로 넘어온 아이템의 최대 개수를 넘어갈 경우 */
			//if (tInfo.iMaximum <= _pItem->iCount + _iCount)
				//return false;

			/* 만약에 Add_Item하려고 매개변수로 전달된 아이템의 id와 기존 인벤토리 배열에 들어있던 아이디가 다를 경우 */
			//if (tInfo.iID != _pItem->iItemID)
				//return false;

			/* 1. 그 칸에 아이템이 있고
			   2. 그 칸에 들어있던 아이템의 개수에 매개변수로 넘어온 아이템의 개수를 더했을 때, 매개변수로 넘어온 아이템의 최대 개수를 넘어가지 않고
			   3. Add_Item하려고 매개변수로 전달된 아이템의 id와 기존 인벤토리 배열에 들어가 있던 아이디가 같을 경우 */
			//return true;
		//});

	/* 만약에 findIter가 end()가 아니면(== Add_Item 하려는 아이템이 이미 인벤에 있고, 개수 더했을 때도, 위의 2번조건에 만족할 때)*/
	//if (findIter != m_inventory[tInfo.iType].end())
	//{
		/* 인벤토리에 있던 아이템의 개수를 매개변수로 넘어온 아이템의 개수만큼 증가 */
		//(*findIter)->iCount += _iCount;
	//}

	/* findIter가 end()일 때(==기존에 Add_Item하려는 아이템이 인벤토리에 없을 때 */
	//else
	//{
		/* m_inventory에서 일단 매개변수로 넘어온 아이템의 iType값에 해당하는 카테고리의 배열을 찾은다음,
		그 배열에서 비어있는 칸을 찾는다. */
		//auto findNullptrIter = find_if(m_inventory[tInfo.iType].begin(), m_inventory[tInfo.iType].end(),
			//[](INVENTORY_ITEM* _pItem)
			//{
				//if (nullptr != _pItem)
					//return false;

				//return true;
			//});

		/* 만약에 비어있는 칸을 찾았으면 */
		//if (findNullptrIter != m_inventory[tInfo.iType].end())
			/* 그 칸에 Add_Item하려는 아이템의 정보를 추가 */
			//(*findNullptrIter) = new INVENTORY_ITEM(_iItemID, _iCount);

		/* 만약에 비어있는 칸을 찾지 못했으면 Add_Item함수 실패 */
		//else
			//return E_FAIL;
	//}
// return S_OK;
#pragma endregion
#pragma region INHO
	/* 해당 카테고리의 array의 첫 페이지부터 마지막 페이지까지 돌면서 순회 */
	for (auto arrIter = m_inventory[tInfo.iType].begin(); arrIter != m_inventory[tInfo.iType].end(); ++arrIter)
	{
		auto findIter = find_if((*arrIter).begin(), (*arrIter).end(),
			[tInfo, _iCount](INVENTORY_ITEM* _pItem)
			{
				/* 만약에 그 칸에 아이템이 없을 경우 */
				if (nullptr == _pItem)
					return false;

				/* 기존에 들어있던 아이템의 개수에 매개변수로 넘어온 아이템의 개수를 더했을 때
				매개 변수로 넘어온 아이템의 최대 개수를 넘어갈 경우 */
				if (tInfo.iMaximum <= _pItem->iCount + _iCount)
					return false;

				/* 만약에 Add_Item하려고 매개변수로 전달된 아이템의 id와 기존 인벤토리에 들어있던 아이디가 다를 경우 */
				if (tInfo.iID != _pItem->iItemID)
					return false;

				/* 1. 그 칸에 아이템이 있고
				2. 그 칸에 들어있던 아이템의 개수에 매개변수로 넘어온 아이템의 개수를 더했을 때, 매개변수로 넘어온 아이템의 최대 개수를 넘어가지 않고
				3. Add_Item하려고 매개변수로 전달된 아이템의 id와 기존 인벤토리에 들어가 있던 아이디가 같을 경우 */
				return true;
			});

			/* 만약에 findIter가 end()가 아니면(== Add_Item 하려는 아이템이 이미 인벤에 있고, 개수 더했을 때도, 위의 2번조건에 만족할 때)*/
			if (findIter != (*arrIter).end())
			{
				/* 인벤토리에 있던 아이템의 개수를 매개변수로 넘어온 아이템의 개수만큼 증가 */
				(*findIter)->iCount += _iCount;

				CQuest_Manager::Get_Instance()->Notify(_iItemID, _iCount);

				return S_OK;
			}

			/* findIter가 end()일 때(==기존에 Add_Item하려는 아이템이 해당 array에 없을 때 */
			//else
			//{
			//	/* vector 컨테이너의 빈 칸을 찾는다. */
			//	auto findNullptrIter = find_if((*arrIter).begin(), (*arrIter).end(),
			//		[](INVENTORY_ITEM* _pItem)
			//		{
			//			if (nullptr != _pItem)
			//				return false;
			//	
			//			return true;
			//		});

			//	/* 만약에 비어있는 칸을 찾았으면 */
			//	if (findNullptrIter != (*arrIter).end())
			//	{
			//		/* 그 칸에 Add_Item하려는 아이템의 정보를 추가 */
			//		(*findNullptrIter) = new INVENTORY_ITEM(_iItemID, _iCount);

			//		CQuest_Manager::Get_Instance()->Notify(_iItemID, _iCount);

			//		return S_OK;
			//	}
			//		

			//	/* 만약에 비어있는 칸을 찾지 못했으면 Add_Item함수 실패 */
			//	//else
			//		//return E_FAIL;
			//}
	}

	/* 반복문 도는 동안 아이템 추가 못했으면, 다시 반복문 돌면서 빈 칸 있는지 찾고, 빈 칸 있으면 추가 */
	for (auto arrIter = m_inventory[tInfo.iType].begin(); arrIter != m_inventory[tInfo.iType].end(); ++arrIter)
	{
		auto& vec = (*arrIter);

		for (size_t i = 0; i < vec.size(); ++i)
		{
			/* 빈 칸 아닐 때 */
			if (nullptr != vec[i])
			{
				/* 그 칸에 들어있는 아이템의 ID와 추가하려는 아이템의 ID가 같은데 */
				if (vec[i]->iItemID == _iItemID)
				{
					/* 넘어온 개수대로 추가하면 스택 최대 개수를 초과하는 경우 */
					// 99 <= 97 + 10
					if(tInfo.iMaximum <= vec[i]->iCount + _iCount)
					{
						// 일단 tInfo.iMaximum - vec[i]->iCount만큼은 추가한다.
						_uint iCanAddCount = tInfo.iMaximum - vec[i]->iCount;
						vec[i]->iCount += iCanAddCount;
						// 그리고 _iCount에서 iCanAddCount만큼 빼줌, 남은 _iCount개 빈 곳에 추가해야함
						_iCount -= iCanAddCount;

						CQuest_Manager::Get_Instance()->Notify(_iItemID, iCanAddCount);
						continue;
					}
				}
			}
			/* 빈 칸 찾으면 */
			else
			{
				const auto& tItemInfo = CItem_Manager::Get_Instance()->Get_ItemInfo(_iItemID);

				/* 추가하려는 아이템 개수가 그 아이템의 maximum보다 크면 */
				if(_iCount > tItemInfo.iMaximum)
				{
					/* 일단 빈 칸 있으니까 iMaximum만큼은 추가 */
					vec[i] = new INVENTORY_ITEM(_iItemID, tItemInfo.iMaximum);

					CQuest_Manager::Get_Instance()->Notify(_iItemID, tItemInfo.iMaximum);
					
					/* 재귀 */
					if (SUCCEEDED(Add_Item(_iItemID, _iCount - tItemInfo.iMaximum)))
						return S_OK;
				}

				/* 추가하려는 아이템 개수가 그 아이템의 maximum보다 작거나 같으면 */
				else
				{
					/* 아이템 새로 추가 */
					vec[i] = new INVENTORY_ITEM(_iItemID, _iCount);

					CQuest_Manager::Get_Instance()->Notify(_iItemID, _iCount);

					return S_OK;
				}
			}
		}
	}

	/* 추가 못했으면 */
	MSG_BOX("Failed to Add_Item!!");
	return E_FAIL;

#pragma endregion

}

HRESULT CInventory::Remove_Item(_uint _iItemID, _uint _iCount)
{

#pragma region 진서형 코드
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

	//	// 탐색 성공
	//	if (iter != items.end())
	//	{
	//		auto& pItem = *iter;

	//		if (pItem->iCount >= _iCount)
	//			pItem->iCount -= _iCount;

	//		if (0 >= pItem->iCount)
	//		{
	//			Safe_Delete(pItem);
	//			/* vector 컨테이너 쓰려고 추가함 */
	//			items.erase(iter);
	//		}
	//			

	//		return S_OK;
	//	}
	//}

	//return E_FAIL;
#pragma endregion
#pragma region INHO
	/* 모든 카테고리 순회 */
	for (auto pairIter = m_inventory.begin(); pairIter != m_inventory.end(); ++pairIter)
	{
		auto& arr = pairIter->second;

		/* array의 iter 순회 */
		for (auto arrIter = arr.begin(); arrIter != arr.end(); ++arrIter)
		{
			auto& vec = (*arrIter);

			/* vector의 iter 순회 */
			auto iter = find_if(vec.begin(), vec.end(), [_iItemID](INVENTORY_ITEM* _pItem)
						{
							/* 기존의 인벤토리 칸이 비어있을 때 */
							if (nullptr == _pItem)
								return false;

							/* 기존에 인벤토리에 있던 아이템과 삭제하려는 아이템의 ID가 다를 때 */
							if (_iItemID != _pItem->iItemID)
								return false;

							/* 1. 기존의 인벤토리 칸이 비어있지 않고 
							   2. 인벤토리 칸에 있던 아이템과 삭제하려는 아이템의 ID가 같을 때 */
							return true;
						});

			// 탐색 성공
			if (iter != vec.end())
			{
				auto& pItem = *iter;

				/* ex) 기존에 인벤토리 칸에 있던 아이템의 개수 10개, 삭제하려는 아이템 개수 9개면
				10 - 9 = 1, 1개 남긴다. */
				if (pItem->iCount >= _iCount)
					pItem->iCount -= _iCount;

				/* 삭제하려는 아이템의 개수가 0개 이하일 때, 아이템 삭제*/
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

	// 기존 데이터 백업
	INVENTORY_ITEM* pItemTemp = vec[_iIndex];
	if (nullptr == pItemTemp)
		return E_FAIL;

	// 인벤토리에서 pop
	vec[_iIndex] = nullptr;

	// 백업해놓은 주소 반환
	*_ppItem = pItemTemp;

	return S_OK;
}

HRESULT CInventory::Push_Equipment(INVENTORY_ITEM* _pItem)
{
	// 장비니까 개수 안늘어남
	// -> 그냥 싹 다 뒤져서 빈 칸 찾아서 거기다가 넣으면 됨
	// -> 근데 장비니까 장비 카테고리는 고정
	auto& arr = m_inventory[(size_t)CATEGORY::CATEGORY_EQUIPMENT];

	for (auto arrIter = arr.begin(); arrIter != arr.end(); ++arrIter)
	{
		auto& vec = (*arrIter);

		for (auto vecIter = vec.begin(); vecIter != vec.end(); ++vecIter)
		{
			// 빈 칸 발견하면 거기다가 집어넣고 return;
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
#pragma region 진서형 코드
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

	/* _iCategory에 해당하는 카테고리를 찾지 못했을 때 */
	if (iter == m_inventory.end())
		return m_pItem_Manager->Get_ItemInfo(0);

	/* _iCategory에 해당하는 카테고리를 찾았을 때, 그 카테고리의 array를 가지고옴 */
	auto& arr = iter->second;

	/* _iPage에 해당하는 페이지를 찾으면, 그 페이지의 vector를 가지고옴 */
	auto& vec = arr[_iPage];

	/* 해당 인벤토리 칸에 아이템이 있으면 */
	if (nullptr != vec[_iIndex])
		return m_pItem_Manager->Get_ItemInfo(vec[_iIndex]->iItemID);

	/* 해당 인벤토리 칸에 아이템이 없으면 */
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
			// 아이템 들어있는 칸 발견하면
			if (nullptr != *vecIter)
			{
				auto tItemInfo = (*vecIter);
				// 찾으려는 요리 ID랑 일치하면
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
			// 아이템 들어있는 칸 발견하면
			if (nullptr != *vecIter)
			{
				auto tItemInfo = (*vecIter);
				// 찾으려는 요리 ID랑 일치하고, 인벤토리 인덱스도 일치하면
				if (_iID == tItemInfo->iItemID &&
					iIndex == _iInvenCellIndex)
				{
					return tItemInfo->iCount;
				}
			}

			++iIndex;
		}
	}

	// 해당 요리 발견 못하면 0 반환
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
			// 아이템 들어있는 칸 발견하면
			if (nullptr != *vecIter)
			{
				auto tItemInfo = (*vecIter);
				// 찾으려는 미러시티 코인 ID랑 일치하면
				if (100001 == tItemInfo->iItemID)
				{
					return tItemInfo->iCount;
				}
			}
		}
	}

	// 다 뒤졌는데 미러시티 코인이 없으면
	return 0;
}

void CInventory::_Organize_Vector()
{
	// 인벤토리 중간에 비어있는 부분 있으면 뒤에 있는거 땡겨야함
	for (auto mapIter = m_inventory.begin(); mapIter != m_inventory.end(); ++mapIter)
	{
		auto& arr = mapIter->second;
		for (auto arrIter = arr.begin(); arrIter != arr.end(); ++arrIter)
		{
			auto& vec = *arrIter;

			// 모든 원소가 nullptr이면 continue
			if (all_of(vec.begin(), vec.end(), [](INVENTORY_ITEM* item) { return item == nullptr; }))
				continue;

			// 모든 원소가 가득차있으면 continue;
			if (all_of(vec.begin(), vec.end(), [](INVENTORY_ITEM* item) { return item != nullptr; }))
				continue;

			for (auto vecIter = vec.begin(); vecIter != vec.end();)
			{
				if (nullptr == *vecIter)
				{
					// 빈칸을 발견했을 때 빈 칸 뒤에 다 nullptr일때는 break;
					if (all_of(vecIter + 1, vec.end(), [](INVENTORY_ITEM* item) { return item == nullptr; }))
						break;

					// 현재 슬롯 이후의 모든 항목을 한 칸씩 앞으로 이동
					auto resultIter = std::rotate(vecIter, vecIter + 1, vec.end());

					// 마지막 항목은 nullptr로 초기화
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
	// 이전에 각 벡터 정렬 후
	for (auto mapIter = m_inventory.begin(); mapIter != m_inventory.end(); ++mapIter)
	{
		auto& arr = mapIter->second;

		for (auto arrIter = arr.begin(); arrIter != arr.end(); ++arrIter)
		{
			auto& vec = *arrIter;

			// 현재 vector가 꽉차있거나, 다음 vector가 end() 거나, 다음꺼 vector가 텅 비어있으면 땡길 필요 없음
			if(all_of(vec.begin(), vec.end(), [](INVENTORY_ITEM* item) { return item != nullptr; }) ||
				arr.end() == (arrIter + 1) || all_of((*(arrIter + 1)).begin(), (*(arrIter + 1)).end(), [](INVENTORY_ITEM* item) { return item == nullptr; }))
				continue;

			for (auto vecIter = vec.begin(); vecIter != vec.end();)
			{
				if (nullptr == *vecIter)
				{
					// vector 중간에 빈 칸 발견했을 때 그 빈 칸 부터 끝까지 몇 칸인지
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

						// 다음 vector에서 앞 vector의 빈 만큼 데이터 가져와서
						for (auto vecItemIter = vecItem.begin(); vecItemIter != endIter; ++vecItemIter)
						{
							tempVec.push_back(*vecItemIter);
							*vecItemIter = nullptr;
						}

						// 앞 vector에다가 빈 곳에서부터 집어넣음
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

						// 다음 루프를 위해 지금 vector의 빈 칸이 몇갠지 센다
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
