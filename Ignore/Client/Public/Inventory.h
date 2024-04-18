#pragma once

#include "Base.h"

BEGIN(Client)

class CItem_Manager;

class CInventory final : public CBase
{
	DECLARE_SINGLETON(CInventory)

public:
	constexpr static inline _uint MAX_INVENTORY_SIZE = 25;
	constexpr static inline _uint MAX_PAGE_SIZE = 5;

private:
	CInventory() = default;
	virtual ~CInventory() = default;

public:
	HRESULT Initialize();
	void	Tick();
	/* 매개변수로 넘어온 _iItemID에 대응되는 아이템을, _iCount개 만큼 배열에 추가 */
	HRESULT Add_Item(_uint _iItemID, _uint _iCount = 1);
	HRESULT Remove_Item(_uint _iItemID, _uint _iCount = 1);
	HRESULT Pop_Equipment(_uint _iCategory, _uint _iPage, _uint _iIndex, INVENTORY_ITEM** _ppItem);
	HRESULT Push_Equipment(INVENTORY_ITEM* _pItem);

	const ITEM_INFO_DESC& Get_ItemInfo(_uint _iIndex, _uint _iCategory, _uint _iPage) const;
	_uint Get_ItemID(_uint _iIndex, _uint _iCategory = 0) const;

	/* 매개변수로 넘어온 카테고리(페이지)의 아이템들이 들어있는 배열을 가져옴 */
	//const array<INVENTORY_ITEM*, MAX_INVENTORY_SIZE>& Get_Items(_uint _iCategory = 0);
	//const vector<INVENTORY_ITEM*>& Get_Items(_uint _iCategory = 0);
	const vector<INVENTORY_ITEM*>& Get_Items(_uint _iCategory = 0, _uint _iPage = 0);
	const array<vector<INVENTORY_ITEM*>, MAX_PAGE_SIZE>& Get_Items_Arr(_uint _iCategory);
	/* 보유하고 있는 요리 개수 or, 요리 재료 개수가 몇개인지*/
	_uint Get_CookedCount(_uint _iID);
	_uint Get_CookedCount(_uint _iID, _uint _iInvenCellIndex);
	_uint Get_MirrorCityCoinCount();

	void _Organize_Vector();
	void _Organize_Array();

	_uint Get_ItemCount(_uint _iID);

public:
	virtual void Free() override;

private:
	CItem_Manager* m_pItem_Manager = { nullptr };

	map<_uint, array<vector<INVENTORY_ITEM*>, MAX_PAGE_SIZE>> m_inventory;
	vector<INVENTORY_ITEM*> m_dummy;
	array<vector<INVENTORY_ITEM*>, MAX_PAGE_SIZE> m_arrDummy;
};

END