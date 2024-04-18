#include "Client_pch.h"
#include "Client_Defines.h"
#include "Dialog_Manager.h"

// Singleton
#include "GameInstance.h"
#include "Inventory.h"
#include "Quest_Manager.h"

// GameObject
#include "Character_NPC.h"
#include "InteractionObject.h"

#include "Client_Functions.h"
#include "Engine_Functions.h"

constexpr static _float TEXT_SPEED = 0.03f;

IMPLEMENT_SINGLETON(CDialog_Manager)

CDialog_Manager::CDialog_Manager()
{
}

HRESULT CDialog_Manager::Initialize(const string& _strFilePath)
{
	// ���������� csv�� ó���ϱ�
	// �ѱ��� ��¿�� ���� �����Է�����..

	/*
	�ϳ��� ��ȭ ������ ��ȭ�� ����
	�ϳ��� ��ȭ �� �ȿ��� ��ȭ�� ������ ����������.
	
	DialogSceneID�� ��ȭâ�� �����ȴٸ� ����Ʈ, NPC��ȭ ��� �����̵� �� �� ����
	*/

	// DialogSceneID : ��ȭ ��ġ
	// GameObjectID : NPC_ID

	//m_mapDescriptions[0].push_back({ 1, TEXT("{color:#ff0000}ù��° �׽�Ʈ ��ȭ\n{color:#00ff00}�ٹٲ� �׽�Ʈ") });
	//m_mapDescriptions[0].push_back({ 2, TEXT("{color:#0000ff}�ι�° �׽�Ʈ ��ȭ\n\n{color:#ffffff}�ٹٲ� �׽�Ʈ") });
	////m_mapDescriptions[0].push_back({ 2, TEXT("����° �׽�Ʈ ��ȭ[sound:Sickle_Impact_1] �۵� Ȯ��") });
	////m_mapDescriptions[0].push_back({ 2, TEXT("�׹�° �׽�Ʈ ��ȭ[sound:Sickle_Impact_2][sound:Sickle_Impact_3] �۵�[sound:Sickle_Impact_4] Ȯ��[sound:Sickle_Impact_5]") });
	////m_mapDescriptions[0].push_back({ 2, TEXT("�ټ���° �׽�Ʈ ��ȭ[questbegin:123]") });
	////m_mapDescriptions[0].push_back({ 2, TEXT("��������° �׽�Ʈ ��ȭ[questclear:123]") });
	//m_mapDescriptions[0].push_back({ 2, TEXT("������ ���� {color:#00ff00}�׽�Ʈ ��ȭ[reward:200001/1]\n{color:#00ff00}�ٹٲ� �׽�Ʈ\n{color:#00ff00}�ٹٲ� �׽�Ʈ") });
	//m_mapDescriptions[0].push_back({ 2, TEXT("����Ʈ{color:#ff0f0f} ���� �׽�Ʈ ��ȭ[questbegin:1]") });

	// 0~3 : Hit , 4 : Dialog

	// �� ��ȭ
	m_mapDescriptions[10].push_back({ ID_NPC_LIN, TEXT("[camera:1][fsm:Interaction/4]�̾��ؿ�. ������ �ٺ���.. {color:#f7e600}���{color:#ffffff}���� ã�ư�������.") });
	m_mapDescriptions[11].push_back({ ID_NONE, TEXT("[camera:1](���� �ٺ����δ�.)") });
	m_mapDescriptions[12].push_back({ ID_NONE, TEXT("[camera:1](�ɰ��� ǥ���� �����ִ�.)") });
	m_mapDescriptions[13].push_back({ ID_NONE, TEXT("[camera:1](�ɰ��� ǥ���� �����ִ�.)") });

	// ��� ��ȭ
	m_mapDescriptions[20].push_back({ ID_NONE, TEXT("[camera:1](���� ��մ��� �ǽ� �����ִ�.)") });

	m_mapDescriptions[21].push_back({ ID_NPC_RUBY, TEXT("[camera:1][fsm:Interaction/4]{color:#f7e600}�� {color:#ffffff}��ϴ� ������ ���Ḧ �Ⱦ�������~") });
	m_mapDescriptions[21].push_back({ ID_PLAYER, TEXT("�� ��ű���...") });
	m_mapDescriptions[21].push_back({ ID_NPC_RUBY, TEXT("����!") });

	// �ø� ��ȭ
	m_mapDescriptions[30].push_back({ ID_NONE, TEXT("[camera:1]({color:#f7e600}��{color:#ffffff}�� ��ȭ���ΰ� ����.)") });
	m_mapDescriptions[31].push_back({ ID_NONE, TEXT("[camera:1](������ ���ƺ��δ�.)") });
	m_mapDescriptions[32].push_back({ ID_NONE, TEXT("[camera:1](ǥ���� ���� �ʾƺ��δ�.)") });

	// ����Ʈ ��ȭ
	m_mapDescriptions[101].push_back({ ID_NPC_LIN, TEXT("[camera:1][fsm:Interaction/4]�̷���Ƽ�� �°��� ȯ���մϴ�. ������ �ݰ�����.") });
	m_mapDescriptions[101].push_back({ ID_NPC_LIN, TEXT("���� �̷���Ƽ�� ��å���� ������ �ð� �ִ� {color:#f7e600}��{color:#ffffff}�Դϴ�.\n�Ƿʰ� �ȵȴٸ� �ڱ�Ұ��� ���� �� �������?") });
	m_mapDescriptions[101].push_back({ ID_PLAYER, TEXT("{color:#f7e600}����ī�׽�{color:#ffffff}��. ������ �ݰ���.") });
	m_mapDescriptions[101].push_back({ ID_NPC_LIN, TEXT("[animation:npc_pe_dialogue1]�������� ã�ƿ��� ���� �幮 ���ε�, Ȥ�� ��Ʈ������ ������\nã�ƿԳ���?") });
	m_mapDescriptions[101].push_back({ ID_PLAYER, TEXT("...[wait:1.0] ��� �˾Ҿ�?") });
	m_mapDescriptions[101].push_back({ ID_NPC_LIN, TEXT("[animation:npc_pe_dialogue2]������ �ƽ�Ʈ�� ���Ǽҿ��� ������ �޾Ҿ��.\n�׷��� ���� ������ ã�ƿö��� �Ǿ��� �;���.") });
	m_mapDescriptions[101].push_back({ ID_NPC_LIN, TEXT("�׷���, ����Ʈ�� �ʿ��Ѱ���?") });
	m_mapDescriptions[101].push_back({ ID_PLAYER, TEXT("�¾�.") });
	m_mapDescriptions[101].push_back({ ID_NPC_LIN, TEXT("[animation:npc_pe_dialogue1]�׷� ���ʰ� ������ ��Ź�� �ҰԿ�.\n��ħ ���� �ϼ��� ���� �����ϴ� ���̰ŵ��.") });
	m_mapDescriptions[101].push_back({ ID_NPC_LIN, TEXT("�켱�� {color:#f7e600}���{color:#ffffff}���� ã�ư�������.\n���� ����Ʈ�� �ȳ����ٰſ���.") });
	m_mapDescriptions[101].push_back({ ID_NONE, TEXT("[nextdialog:10][questclear:1][autoend:1]") });

	m_mapDescriptions[102].push_back({ ID_NPC_RUBY, TEXT("[camera:1][fsm:Interaction/4]{color:#f7e600}�� {color:#ffffff}��Ͽ��� ��� ������.{color:#f7e600}����ī�׽� {color:#ffffff}����?") });
	m_mapDescriptions[102].push_back({ ID_PLAYER, TEXT("��. �ݰ���.") });
	m_mapDescriptions[102].push_back({ ID_NPC_RUBY, TEXT("{color:#f7e600}��{color:#ffffff} ��Ͽ��� {color:#b8f8fb}������ ����{color:#ffffff}�� ���� ������ �� �� �ֳ���?\n���� ���� �帱�Կ�.") });
	m_mapDescriptions[102].push_back({ ID_PLAYER, TEXT("���� ������ �൵ ���� �ʾ�?") });
	m_mapDescriptions[102].push_back({ ID_NPC_RUBY, TEXT("��..[wait:0.5] �װ�..[wait:0.5] �ƹ�ư �׷��� �־��!") });
	m_mapDescriptions[102].push_back({ ID_NPC_RUBY, TEXT("��, �׸��� ���� �������ֶ�� �ߴٰ� ������� ������!") });
	m_mapDescriptions[102].push_back({ ID_PLAYER, TEXT("..? �˰ھ�.") });
	m_mapDescriptions[102].push_back({ ID_NONE, TEXT("[nextdialog:20][questclear:2][reward:300001/2][autoend:1]") });

	m_mapDescriptions[104].push_back({ ID_NPC_LIN, TEXT("[camera:1][fsm:Interaction/4]�ݹ� ���ƿԱ���. �տ� ��� Ȥ��..") });
	m_mapDescriptions[104].push_back({ ID_PLAYER, TEXT("{color:#b8f8fb}������ ����{color:#ffffff}��.\n{color:#f7e600}���{color:#ffffff}���� ��Ź.. �ƴ� �� �ַ��� �����Ծ�.") });
	m_mapDescriptions[104].push_back({ ID_NPC_LIN, TEXT("�̾������� ����ҰԿ�.\n���� �������� �ʴ� ������󼭿�.") });
	m_mapDescriptions[104].push_back({ ID_PLAYER, TEXT("��.. �׷�..") });
	m_mapDescriptions[104].push_back({ ID_NPC_LIN, TEXT("��ħ �� �Ծ��. �縷���뿡�� ������ �Դµ� ��Ż�ڵ� ������ Ȱ���� ������ �ֳ�����.\n��Ż�� Ÿ�� ������ {color:#f7e600}�ø�{color:#ffffff}�� ã�ư����� �� �ſ���.") });
	m_mapDescriptions[104].push_back({ ID_NONE, TEXT("[nextdialog:11][questclear:4][autoend:1]") });

	m_mapDescriptions[105].push_back({ ID_PLAYER, TEXT("[camera:1][fsm:Interaction/4]Ȥ�� �����..") });
	m_mapDescriptions[105].push_back({ ID_NPC_SIRI, TEXT("��! ������ ���� �߿Ծ��!") });
	m_mapDescriptions[105].push_back({ ID_NPC_SIRI, TEXT("�����������ϰ��ִµ���Ż�ڹ��������̴�ġ���ϰ��ڱⷻ�����ڱ�����������?\n�ڱع�����������л��°��Ǿ���ڱ⳯�ٱ�����ϴµ�����糳����������...") });
	m_mapDescriptions[105].push_back({ ID_PLAYER, TEXT("�̾������� õõ�� ������.\n���� ��⸦ �ϴ��� �ϳ��� �𸣰ھ�.") });
	m_mapDescriptions[105].push_back({ ID_NPC_SIRI, TEXT("��! �̾��ؿ�. ����� �̷���Ƽ���� �����شٴ� �� �������ΰ���?") });
	m_mapDescriptions[105].push_back({ ID_PLAYER, TEXT("�Ƹ��� �����ž�.") });
	m_mapDescriptions[105].push_back({ ID_NPC_SIRI, TEXT("���� �縷������ ���°踦 ���� �� �����ϴ� {color:#f7e600}�ø�{color:#ffffff}��� �ؿ�.\n�̹��� ��ó�� {color:#ff6f61}����{color:#ffffff}�� �����ϰ� �ִµ� ��Ż�� �������� ���ڱ� ���̴�ġ����\n{color:#ff6f61}����{color:#ffffff}�� �ڱ��ϱ� �������� ������?") });
	m_mapDescriptions[105].push_back({ ID_NPC_SIRI, TEXT("{color:#ff6f61}����{color:#ffffff}�� �¼��� �������� �˷��� �ִµ�, ������ ������ �� �׸�ŭ �����Ҽ��� �����.\n�׷��� ��¿ �� ���� ������ �����ϰ� ���ƿԴµ� �����Ҽ��� ���޴��󱸿�.\n�׷���...") });
	m_mapDescriptions[105].push_back({ ID_PLAYER, TEXT("�׷���?") });
	m_mapDescriptions[105].push_back({ ID_NPC_SIRI, TEXT("�������� �� �׷����� ����� �ȵǴϱ� �� ó�����ּ����� �ؿ�~") });
	m_mapDescriptions[105].push_back({ ID_PLAYER, TEXT("����.. �˰ھ�.") });
	m_mapDescriptions[105].push_back({ ID_NPC_SIRI, TEXT("���ñ濡 ����� {color:#b8f8fb}������ ��{color:#ffffff}�� ä�����ٷ���? ������!") });
	m_mapDescriptions[105].push_back({ ID_NONE, TEXT("[nextdialog:30][questclear:5][autoend:1]") });

	m_mapDescriptions[109].push_back({ ID_NONE, TEXT("[camera:1][fsm:Interaction/4]���� �Ծ��? ����߾��.") });
	m_mapDescriptions[109].push_back({ ID_NONE, TEXT("[questclear:9][autoend:1]") });

	m_mapDescriptions[110].push_back({ ID_NPC_SIRI, TEXT("[camera:1][fsm:Interaction/4]{color:#ff6f61}����{color:#ffffff}�� ���°� �̻��ؿ�. ������ �޴��� �ð��� ������ �����Ǿ �ٽ� �¼������� �ϴµ�\n������ �� ���������� ���߾��.") });
	m_mapDescriptions[110].push_back({ ID_NPC_SIRI, TEXT("�Ƹ� �� ��Ż�ڳ���� ������ �� �� ������...\n�̾������� {color:#ff6f61}����{color:#ffffff}�� ���¸� �ٽ� Ȯ���ϰ� ���ٷ���?") });
	m_mapDescriptions[110].push_back({ ID_NPC_SIRI, TEXT("���� ������ ��̰� ������ �ʴ´ٸ� {color:#ff6f61}����{color:#ffffff}�� óġ���ּ���.\n�� �̻� ������ ���ϴ°� �ƽ����� �׳� �α⿡�� �ʹ� �����ϰŵ��.") });
	m_mapDescriptions[110].push_back({ ID_NONE, TEXT("[nextdialog:31][questclear:10][autoend:1]") });

	m_mapDescriptions[111].push_back({ ID_NPC_SIRI, TEXT("[camera:1][fsm:Interaction/4]{color:#ff6f61}����{color:#ffffff}�� ��� �Ƴ���?!") });
	m_mapDescriptions[111].push_back({ ID_PLAYER, TEXT("óġ�߾�.") });
	m_mapDescriptions[111].push_back({ ID_NPC_SIRI, TEXT("...�׷�����.") });
	m_mapDescriptions[111].push_back({ ID_NPC_SIRI, TEXT("�̷���Ƽ�� ���ư��� {color:#f7e600}��{color:#ffffff}���� �������ٷ���? ���� ������ �� �����־��.\n��Ź�ҰԿ�.") });
	m_mapDescriptions[111].push_back({ ID_PLAYER, TEXT("�˰ھ�.") });
	m_mapDescriptions[111].push_back({ ID_NONE, TEXT("[nextdialog:33][questclear:11][autoend:1]") });

	m_mapDescriptions[112].push_back({ ID_NPC_LIN, TEXT("[camera:1][fsm:Interaction/4]�縷������ ��Ȳ�� �����?") });
	m_mapDescriptions[112].push_back({ ID_PLAYER, TEXT("(��Ȳ�� �����Ѵ�.)") });
	m_mapDescriptions[112].push_back({ ID_NPC_LIN, TEXT("�׷�����.. ����ϼ̾��. �켱�� �̴ϰ��� �����Ŷ� �ϸ鼭 ���� �輼��.") });
	m_mapDescriptions[112].push_back({ ID_NPC_LIN, TEXT("�������� ���� {color:#b8f8fb}�̷���Ƽ ����{color:#ffffff}�� ����ؼ� ����Ǵ� ������ ����� ��ȯ�� �� �����ſ���.\n�����.. 100������ ��ƿ��ø� ���ο� ����Ʈ�� �帱�Կ�.") });
	m_mapDescriptions[112].push_back({ ID_NPC_LIN, TEXT("[nextdialog:12][questclear:12][autoend:1]") });

	m_mapDescriptions[113].push_back({ ID_NPC_LIN, TEXT("[camera:1][fsm:Interaction/4]��հ� �� ���� �Գ���?") });
	m_mapDescriptions[113].push_back({ ID_NPC_LIN, TEXT("�̾������� ����� ���� ������.\n{color:#f7e600}�ø�{color:#ffffff}�κ��� �ɿ��� ���°� �ɻ�ġ �ʴٴ� ������ �Ծ��.\n���� �ٷ� �ɿ����� �̵����ٷ���?") });
	m_mapDescriptions[113].push_back({ ID_NPC_LIN, TEXT("����, ���� ����{color:#b8f8fb} �̷���Ƽ ����{color:#ffffff}�� ������� �ʾҴٸ� ��� ��ȯ�ؼ� ������.") });
	m_mapDescriptions[113].push_back({ ID_NONE, TEXT("[nextdialog:13][questclear:13][autoend:1]") });

	m_mapDescriptions[116].push_back({ ID_NPC_LIN, TEXT("[camera:1][fsm:Interaction/4]����߾��.") });
	m_mapDescriptions[116].push_back({ ID_NPC_LIN, TEXT("���⼭�� ���� ��������... ����� ���� �����̰ڳ׿�.") });
	m_mapDescriptions[116].push_back({ ID_NPC_LIN, TEXT("������ �����.") });
	m_mapDescriptions[116].push_back({ ID_NPC_LIN, TEXT("[nextdialog:99999][questclear:16][questbegin:17][autoend:1]") });



	m_mapDescriptions[99999].push_back({ ID_NONE, TEXT("[autoend:1]") });
	return S_OK;
}

void CDialog_Manager::Tick(_float _fTimeDelta)
{
	// test code
	if (CFunc::Is_KeyDown(DIK_P, false))
		Begin_Dialog(0);

	if (!m_funcTick(this, _fTimeDelta))
		End_Dialog();
}

HRESULT CDialog_Manager::Begin_Dialog(DialogSceneID _iID, CGameObject* _pCaller, _uint _eDialogType)
{
	auto iter = m_mapDescriptions.find(_iID);

	if (iter == m_mapDescriptions.end())
		return E_FAIL;

	m_pCurrentDescriptions = &iter->second;
	m_removeEventDescriptions.clear();

	m_funcTick = mem_fn(&CDialog_Manager::_Calculate_Dialog);

	m_pCaller = _pCaller;

	auto eCurrentLevel = CFunc::Get_CurLevel();
	CGameInstance::Get_Instance()->Enable_UI(eCurrentLevel, TEXT("Layer_UI_Dialogue"), 0);

	CFunc::Disable_MainUI();

	m_fX = 230.f * (CGlobal::g_fWinSizeX / 1920.f);
	m_fY = 890.f * (CGlobal::g_fWinSizeY / 1080.f);

	m_iCurrentTalker = iter->second[0].first;

	return S_OK;
}

HRESULT CDialog_Manager::End_Dialog()
{
	m_pCurrentDescriptions = nullptr;
	m_iCurrentReadIndex = 0;
	m_iCurrentReadCharBeginIndex = 0;
	m_iCurrentReadCharEndIndex = 0;
	m_fProgressTime = 0.f;

	m_wstrDescription.clear();
	m_removeEventDescriptions.clear();

	m_funcTick = mem_fn(&CDialog_Manager::_Dummy);

	m_pCaller = nullptr;

	Level eCurLevel = CGameInstance::Get_Instance()->Get_CurLevel();
	CGameInstance::Get_Instance()->Disable_UI((_uint)eCurLevel, TEXT("Layer_UI_Dialogue"), 0);

	CFunc::Enable_MainUI();

	m_funcEndOfDialog();
	m_funcEndOfDialog = []() {};

	m_iCurrentTalker = 0;

	return S_OK;
}

HRESULT CDialog_Manager::Next_Dialog()
{
	if (m_iCurrentReadIndex + 1 >= m_pCurrentDescriptions->size())
		return E_FAIL;

	++m_iCurrentReadIndex;

	m_iCurrentReadCharBeginIndex = 0;
	m_iCurrentReadCharEndIndex = 0;
	m_wstrDescription.clear();
	m_removeEventDescriptions.clear();

	m_iCurrentTalker = (*m_pCurrentDescriptions)[m_iCurrentReadIndex].first;

	return S_OK;
}

void CDialog_Manager::Bind_Callback_EndOfDialog(function<void()> _func)
{
	m_funcEndOfDialog = _func;
}

HRESULT CDialog_Manager::_ExecuteEvent(const wstring& _wstrDescription)
{
	/*
	[anim:� ��ü, � �ִϸ��̼�]
	{color:#ffffff} color ó���� ���� ��
	[sound:~~]
	[camera:~~]
	[wait:1.0]
	[reward:id, num / id, num ....]
	[questbegin: questID]
	[questclear : questID]
	[nextdialog : dialogID]
	[autoend : ��ȭ �ڵ�����]
	*/

	auto iterBegin = _wstrDescription.begin() + m_iCurrentReadCharEndIndex;
	auto iterEnd = find(iterBegin, _wstrDescription.end(), L':');

	wstring wstrEvent(iterBegin + 1, iterEnd);

	iterBegin = iterEnd + 1;
	iterEnd = find(iterBegin, _wstrDescription.end(), L']');

	wstring wstrContent(iterBegin, iterEnd);

	m_removeEventDescriptions.push_back(wstring(_wstrDescription.begin() + m_iCurrentReadCharBeginIndex, _wstrDescription.begin() + m_iCurrentReadCharEndIndex));

	m_iCurrentReadCharBeginIndex = distance(_wstrDescription.begin(), iterEnd) + 1;
	m_iCurrentReadCharEndIndex = m_iCurrentReadCharBeginIndex;

	if (TEXT("color") == wstrEvent)
	{

	}

	else if (TEXT("sound") == wstrEvent)
	{
		string strAudioTag(wstrContent.begin(), wstrContent.end());
		CGameInstance::Get_Instance()->Play_Audio(strAudioTag);
	}

	else if (TEXT("animation") == wstrEvent)
	{
		if (nullptr == m_pCaller)
			return S_OK;

		string strAnimTag(wstrContent.begin(), wstrContent.end());

		if (OBJECT_TYPE::NPC == m_pCaller->Get_ObjectType())
		{
			auto pObject = static_cast<CCharacter_NPC*>(m_pCaller);

			pObject->Change_Animation_For_Dialog(strAnimTag);
		}
	}

	else if (TEXT("fsm") == wstrEvent)
	{
		if (nullptr == m_pCaller)
			return S_OK;

		auto it = find(wstrContent.begin(), wstrContent.end(), L'/');
		string szTagFSM = string(wstrContent.begin(), it);
		_int eInteractionType = stoi(wstring(it+1, wstrContent.end()));


		static_cast<CCharacter_NPC*>(m_pCaller)->Change_FSM_State(szTagFSM, eInteractionType);
	}

	else if (TEXT("camera") == wstrEvent)
	{
		if (nullptr == m_pCaller)
			return S_OK;

		auto iIndex = stoi(wstrContent);

		auto pTransform = static_cast<CTransform*>(m_pCaller->Find_Component(TEXT("Com_Transform")));

		if (nullptr != pTransform)
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			EventMsg tEventMsg;
			tEventMsg.eEventType = EVENT_TYPE::GAME_MODE_CAMERA;
			tEventMsg.wParam = iIndex;

			Vector4 vAt = pTransform->Get_State(CTransform::STATE_POSITION);
			tEventMsg.cParam = vAt;

			pGameInstance->Handling_Event_GameMode(tEventMsg);
		}
	}

	else if (TEXT("wait") == wstrEvent)
	{
		_float fTime = stof(wstrContent);
		m_fWaitTime = fTime;
	}

	else if (TEXT("reward") == wstrEvent)
	{
		auto it = find(wstrContent.begin(), wstrContent.end(), L'/');
		_uint iItemID = stoi(wstring(wstrContent.begin(), it));
		_uint iCount = stoi(wstring(it + 1, wstrContent.end()));

		if (SUCCEEDED(CInventory::Get_Instance()->Add_Item(iItemID, iCount)))
		{
			auto pGameInstance = CGameInstance::Get_Instance();
			auto iCurrentLevel = static_cast<_uint>(pGameInstance->Get_CurLevel());

			SYSTEMINFO_GAIN_ITEM(iItemID, iCount)
		}
	}

	else if (TEXT("questbegin") == wstrEvent)
	{
		_uint iQuestID = stoi(wstrContent);
		CQuest_Manager::Get_Instance()->Begin_Quest(iQuestID);
	}

	else if (TEXT("questclear") == wstrEvent)
	{
		_uint iQuestID = stoi(wstrContent);
		CQuest_Manager::Get_Instance()->Clear_Quest(iQuestID);
	}

	else if (TEXT("nextdialog") == wstrEvent)
	{
		if (nullptr == m_pCaller)
			return S_OK;

		_uint iDialogID = stoi(wstrContent);

		if (m_pCaller->Get_ObjectType() == OBJECT_TYPE::NPC)
			static_cast<CCharacter_NPC*>(m_pCaller)->Set_DialogID(iDialogID);
	}

	else if (TEXT("autoend") == wstrEvent)
	{
		return E_FAIL;
	}

	else
	{
		MSG_BOX("Not found dialog event.");
	}

	return S_OK;
}

_bool CDialog_Manager::_Calculate_Dialog(_float _fTimeDelta)
{
	m_fWaitTime = clamp(m_fWaitTime - _fTimeDelta, 0.f, 10.f);

	if (0.f < m_fWaitTime)
		return true;

	auto& wstrDescription = (*m_pCurrentDescriptions)[m_iCurrentReadIndex].second;

	/* ���ڿ��� �� ��µ��� �� */
	if (m_iCurrentReadCharEndIndex >= wstrDescription.size())
	{
		// TODO : Ű�� ������ �� ���� ��ȭ�� �Ѿ���� �����
		if (CFunc::Is_KeyDown(DIK_F, false))
		{
			/* ������ �̾��� ��簡 ��� Next_Dialog() ���� �����ϸ� */
			if (FAILED(Next_Dialog()))
				return false;

			/* ������ �̾��� ��簡 �־ Next_Dialog() ���� �����ϸ� */
			else
			{
				/* ��ȭâ ������ ���� �ﰢ�� Disable */
				Level eCurLevel = CGameInstance::Get_Instance()->Get_CurLevel();
				CGameInstance::Get_Instance()->Disable_UI((_uint)eCurLevel, TEXT("Layer_UI_Dialogue"), 1);
				return true;
			}
				
		}
		//return true;

		/* ��ȭâ ������ ���� �ﰢ�� Enable */
		Level eCurLevel = CGameInstance::Get_Instance()->Get_CurLevel();
		CGameInstance::Get_Instance()->Enable_UI((_uint)eCurLevel, TEXT("Layer_UI_Dialogue"), 1);
	}

	// �̺�Ʈ �޽��� ����. ���ӵ� �̺�Ʈ�� ������ �� �ֱ� ������ while�� ó��
	while (m_iCurrentReadCharEndIndex < wstrDescription.size()
		&& L'[' == wstrDescription[m_iCurrentReadCharEndIndex])
	{
		if (FAILED(_ExecuteEvent(wstrDescription)))
			return false;
	}

	if (m_fProgressTime >= TEXT_SPEED)
	{
		if (m_iCurrentReadCharEndIndex < wstrDescription.size())
			++m_iCurrentReadCharEndIndex;

		m_fProgressTime -= TEXT_SPEED;
	}

	m_fProgressTime += _fTimeDelta;

	m_wstrDescription.clear();

	for (auto& description : m_removeEventDescriptions)
		m_wstrDescription += description;

	if ((m_iCurrentReadCharBeginIndex != m_iCurrentReadCharEndIndex) && (m_iCurrentReadCharEndIndex <= wstrDescription.size()))
	{
		wstring wstrTemp = wstrDescription.substr(m_iCurrentReadCharEndIndex - 1, 1);
		// �÷� �±� ���� �� �κ� '{'�� �߰��ϸ� �÷� �±״� �ٷ� �������
		if (wstring::npos != wstrTemp.find(L'{'))
			m_iCurrentReadCharEndIndex = m_iCurrentReadCharEndIndex + (static_cast<_uint>(FONT_COLORTAG_LENGTH) - 1);

		m_wstrDescription += wstring(wstrDescription.begin() + m_iCurrentReadCharBeginIndex, wstrDescription.begin() + m_iCurrentReadCharEndIndex);
	}

	return true;
}

void CDialog_Manager::Free()
{
	for (auto& descriptions : m_mapDescriptions)
	{
		descriptions.second.clear();
	}

	m_mapDescriptions.clear();
}