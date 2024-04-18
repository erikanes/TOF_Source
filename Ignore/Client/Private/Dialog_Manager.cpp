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
	// 마찬가지로 csv로 처리하기
	// 한글은 어쩔수 없이 수동입력으로..

	/*
	하나의 대화 씬별로 대화를 저장
	하나의 대화 씬 안에는 대화의 순서가 정해져있음.
	
	DialogSceneID는 대화창만 생성된다면 퀘스트, NPC대화 등등 무엇이든 될 수 있음
	*/

	// DialogSceneID : 대화 뭉치
	// GameObjectID : NPC_ID

	//m_mapDescriptions[0].push_back({ 1, TEXT("{color:#ff0000}첫번째 테스트 대화\n{color:#00ff00}줄바꿈 테스트") });
	//m_mapDescriptions[0].push_back({ 2, TEXT("{color:#0000ff}두번째 테스트 대화\n\n{color:#ffffff}줄바꿈 테스트") });
	////m_mapDescriptions[0].push_back({ 2, TEXT("세번째 테스트 대화[sound:Sickle_Impact_1] 작동 확인") });
	////m_mapDescriptions[0].push_back({ 2, TEXT("네번째 테스트 대화[sound:Sickle_Impact_2][sound:Sickle_Impact_3] 작동[sound:Sickle_Impact_4] 확인[sound:Sickle_Impact_5]") });
	////m_mapDescriptions[0].push_back({ 2, TEXT("다섯번째 테스트 대화[questbegin:123]") });
	////m_mapDescriptions[0].push_back({ 2, TEXT("여섯섯번째 테스트 대화[questclear:123]") });
	//m_mapDescriptions[0].push_back({ 2, TEXT("아이템 지급 {color:#00ff00}테스트 대화[reward:200001/1]\n{color:#00ff00}줄바꿈 테스트\n{color:#00ff00}줄바꿈 테스트") });
	//m_mapDescriptions[0].push_back({ 2, TEXT("퀘스트{color:#ff0f0f} 시작 테스트 대화[questbegin:1]") });

	// 0~3 : Hit , 4 : Dialog

	// 린 대화
	m_mapDescriptions[10].push_back({ ID_NPC_LIN, TEXT("[camera:1][fsm:Interaction/4]미안해요. 지금은 바빠서.. {color:#f7e600}루비{color:#ffffff}에게 찾아가보세요.") });
	m_mapDescriptions[11].push_back({ ID_NONE, TEXT("[camera:1](많이 바빠보인다.)") });
	m_mapDescriptions[12].push_back({ ID_NONE, TEXT("[camera:1](심각한 표정을 짓고있다.)") });
	m_mapDescriptions[13].push_back({ ID_NONE, TEXT("[camera:1](심각한 표정을 짓고있다.)") });

	// 루비 대화
	m_mapDescriptions[20].push_back({ ID_NONE, TEXT("[camera:1](뭐가 재밌는지 실실 웃고있다.)") });

	m_mapDescriptions[21].push_back({ ID_NPC_RUBY, TEXT("[camera:1][fsm:Interaction/4]{color:#f7e600}린 {color:#ffffff}언니는 선인장 음료를 싫어하지롱~") });
	m_mapDescriptions[21].push_back({ ID_PLAYER, TEXT("날 놀린거구나...") });
	m_mapDescriptions[21].push_back({ ID_NPC_RUBY, TEXT("히히!") });

	// 시리 대화
	m_mapDescriptions[30].push_back({ ID_NONE, TEXT("[camera:1]({color:#f7e600}린{color:#ffffff}과 통화중인것 같다.)") });
	m_mapDescriptions[31].push_back({ ID_NONE, TEXT("[camera:1](생각이 많아보인다.)") });
	m_mapDescriptions[32].push_back({ ID_NONE, TEXT("[camera:1](표정이 좋지 않아보인다.)") });

	// 퀘스트 대화
	m_mapDescriptions[101].push_back({ ID_NPC_LIN, TEXT("[camera:1][fsm:Interaction/4]미러시티에 온것을 환영합니다. 만나서 반가워요.") });
	m_mapDescriptions[101].push_back({ ID_NPC_LIN, TEXT("저는 미러시티의 총책임자 대행을 맡고 있는 {color:#f7e600}린{color:#ffffff}입니다.\n실례가 안된다면 자기소개를 해줄 수 있을까요?") });
	m_mapDescriptions[101].push_back({ ID_PLAYER, TEXT("{color:#f7e600}에리카네스{color:#ffffff}야. 만나서 반가워.") });
	m_mapDescriptions[101].push_back({ ID_NPC_LIN, TEXT("[animation:npc_pe_dialogue1]외지인이 찾아오는 일은 드문 일인데, 혹시 포트폴리오 때문에\n찾아왔나요?") });
	m_mapDescriptions[101].push_back({ ID_PLAYER, TEXT("...[wait:1.0] 어떻게 알았어?") });
	m_mapDescriptions[101].push_back({ ID_NPC_LIN, TEXT("[animation:npc_pe_dialogue2]얼마전에 아스트라 대피소에서 연락을 받았어요.\n그래서 슬슬 누군가 찾아올때가 되었다 싶었죠.") });
	m_mapDescriptions[101].push_back({ ID_NPC_LIN, TEXT("그래서, 퀘스트가 필요한거죠?") });
	m_mapDescriptions[101].push_back({ ID_PLAYER, TEXT("맞아.") });
	m_mapDescriptions[101].push_back({ ID_NPC_LIN, TEXT("[animation:npc_pe_dialogue1]그럼 사양않고 마음껏 부탁좀 할게요.\n마침 저희도 일손이 많이 부족하던 참이거든요.") });
	m_mapDescriptions[101].push_back({ ID_NPC_LIN, TEXT("우선은 {color:#f7e600}루비{color:#ffffff}에게 찾아가보세요.\n다음 퀘스트를 안내해줄거에요.") });
	m_mapDescriptions[101].push_back({ ID_NONE, TEXT("[nextdialog:10][questclear:1][autoend:1]") });

	m_mapDescriptions[102].push_back({ ID_NPC_RUBY, TEXT("[camera:1][fsm:Interaction/4]{color:#f7e600}린 {color:#ffffff}언니에게 얘기 들었어요.{color:#f7e600}에리카네스 {color:#ffffff}맞죠?") });
	m_mapDescriptions[102].push_back({ ID_PLAYER, TEXT("응. 반가워.") });
	m_mapDescriptions[102].push_back({ ID_NPC_RUBY, TEXT("{color:#f7e600}린{color:#ffffff} 언니에게 {color:#b8f8fb}선인장 음료{color:#ffffff}를 만들어서 가져다 줄 수 있나요?\n재료는 제가 드릴게요.") });
	m_mapDescriptions[102].push_back({ ID_PLAYER, TEXT("직접 가져다 줘도 되지 않아?") });
	m_mapDescriptions[102].push_back({ ID_NPC_RUBY, TEXT("음..[wait:0.5] 그게..[wait:0.5] 아무튼 그런게 있어요!") });
	m_mapDescriptions[102].push_back({ ID_NPC_RUBY, TEXT("아, 그리고 제가 가져다주라고 했다고 얘기하지 마세요!") });
	m_mapDescriptions[102].push_back({ ID_PLAYER, TEXT("..? 알겠어.") });
	m_mapDescriptions[102].push_back({ ID_NONE, TEXT("[nextdialog:20][questclear:2][reward:300001/2][autoend:1]") });

	m_mapDescriptions[104].push_back({ ID_NPC_LIN, TEXT("[camera:1][fsm:Interaction/4]금방 돌아왔군요. 손에 든건 혹시..") });
	m_mapDescriptions[104].push_back({ ID_PLAYER, TEXT("{color:#b8f8fb}선인장 음료{color:#ffffff}야.\n{color:#f7e600}루비{color:#ffffff}한테 부탁.. 아니 너 주려고 만들어왔어.") });
	m_mapDescriptions[104].push_back({ ID_NPC_LIN, TEXT("미안하지만 사양할게요.\n별로 좋아하지 않는 음료수라서요.") });
	m_mapDescriptions[104].push_back({ ID_PLAYER, TEXT("어.. 그래..") });
	m_mapDescriptions[104].push_back({ ID_NPC_LIN, TEXT("마침 잘 왔어요. 사막지대에서 연락이 왔는데 약탈자들 때문에 활동에 지장이 있나봐요.\n포탈을 타고 나가서 {color:#f7e600}시리{color:#ffffff}를 찾아가보면 될 거에요.") });
	m_mapDescriptions[104].push_back({ ID_NONE, TEXT("[nextdialog:11][questclear:4][autoend:1]") });

	m_mapDescriptions[105].push_back({ ID_PLAYER, TEXT("[camera:1][fsm:Interaction/4]혹시 당신이..") });
	m_mapDescriptions[105].push_back({ ID_NPC_SIRI, TEXT("앗! 누군지 몰라도 잘왔어요!") });
	m_mapDescriptions[105].push_back({ ID_NPC_SIRI, TEXT("렌딜을관찰하고있는데약탈자무리가들이닥치더니갑자기렌딜을자극하지뭐에요?\n자극받은렌딜이흥분상태가되어서갑자기날뛰기시작하는데어찌나사납던지죽을뻔...") });
	m_mapDescriptions[105].push_back({ ID_PLAYER, TEXT("미안하지만 천천히 말해줘.\n무슨 얘기를 하는지 하나도 모르겠어.") });
	m_mapDescriptions[105].push_back({ ID_NPC_SIRI, TEXT("아! 미안해요. 당신이 미러시티에서 보내준다던 그 조력자인가요?") });
	m_mapDescriptions[105].push_back({ ID_PLAYER, TEXT("아마도 맞을거야.") });
	m_mapDescriptions[105].push_back({ ID_NPC_SIRI, TEXT("저는 사막지대의 생태계를 관찰 및 연구하는 {color:#f7e600}시리{color:#ffffff}라고 해요.\n이번에 모처럼 {color:#ff6f61}렌딜{color:#ffffff}을 관찰하고 있는데 약탈자 무리들이 갑자기 들이닥치더니\n{color:#ff6f61}렌딜{color:#ffffff}을 자극하기 시작하지 뭐에요?") });
	m_mapDescriptions[105].push_back({ ID_NPC_SIRI, TEXT("{color:#ff6f61}렌딜{color:#ffffff}은 온순한 성격으로 알려져 있는데, 위협을 받으면 또 그만큼 포악할수가 없어요.\n그래서 어쩔 수 없이 관찰을 포기하고 돌아왔는데 생각할수록 열받더라구요.\n그래서...") });
	m_mapDescriptions[105].push_back({ ID_PLAYER, TEXT("그래서?") });
	m_mapDescriptions[105].push_back({ ID_NPC_SIRI, TEXT("다음에도 또 그런일이 생기면 안되니까 다 처리해주셨으면 해요~") });
	m_mapDescriptions[105].push_back({ ID_PLAYER, TEXT("나참.. 알겠어.") });
	m_mapDescriptions[105].push_back({ ID_NPC_SIRI, TEXT("오늘길에 겸사겸사 {color:#b8f8fb}선인장 볼{color:#ffffff}도 채집해줄래요? 고마워요!") });
	m_mapDescriptions[105].push_back({ ID_NONE, TEXT("[nextdialog:30][questclear:5][autoend:1]") });

	m_mapDescriptions[109].push_back({ ID_NONE, TEXT("[camera:1][fsm:Interaction/4]벌써 왔어요? 고생했어요.") });
	m_mapDescriptions[109].push_back({ ID_NONE, TEXT("[questclear:9][autoend:1]") });

	m_mapDescriptions[110].push_back({ ID_NPC_SIRI, TEXT("[camera:1][fsm:Interaction/4]{color:#ff6f61}렌딜{color:#ffffff}의 상태가 이상해요. 위협을 받더라도 시간이 지나면 안정되어서 다시 온순해져야 하는데\n오히려 더 공격적으로 변했어요.") });
	m_mapDescriptions[110].push_back({ ID_NPC_SIRI, TEXT("아마 그 약탈자놈들이 뭔가를 한 것 같은데...\n미안하지만 {color:#ff6f61}렌딜{color:#ffffff}의 상태를 다시 확인하고 와줄래요?") });
	m_mapDescriptions[110].push_back({ ID_NPC_SIRI, TEXT("만약 안정될 기미가 보이지 않는다면 {color:#ff6f61}렌딜{color:#ffffff}을 처치해주세요.\n더 이상 관찰을 못하는건 아쉽지만 그냥 두기에는 너무 위험하거든요.") });
	m_mapDescriptions[110].push_back({ ID_NONE, TEXT("[nextdialog:31][questclear:10][autoend:1]") });

	m_mapDescriptions[111].push_back({ ID_NPC_SIRI, TEXT("[camera:1][fsm:Interaction/4]{color:#ff6f61}렌딜{color:#ffffff}은 어떻게 됐나요?!") });
	m_mapDescriptions[111].push_back({ ID_PLAYER, TEXT("처치했어.") });
	m_mapDescriptions[111].push_back({ ID_NPC_SIRI, TEXT("...그렇군요.") });
	m_mapDescriptions[111].push_back({ ID_NPC_SIRI, TEXT("미러시티로 돌아가서 {color:#f7e600}린{color:#ffffff}에게 보고해줄래요? 저는 할일이 더 남아있어서요.\n부탁할게요.") });
	m_mapDescriptions[111].push_back({ ID_PLAYER, TEXT("알겠어.") });
	m_mapDescriptions[111].push_back({ ID_NONE, TEXT("[nextdialog:33][questclear:11][autoend:1]") });

	m_mapDescriptions[112].push_back({ ID_NPC_LIN, TEXT("[camera:1][fsm:Interaction/4]사막지대의 상황은 어땠나요?") });
	m_mapDescriptions[112].push_back({ ID_PLAYER, TEXT("(상황을 설명한다.)") });
	m_mapDescriptions[112].push_back({ ID_NPC_LIN, TEXT("그렇군요.. 고생하셨어요. 우선은 미니게임 같은거라도 하면서 쉬고 계세요.") });
	m_mapDescriptions[112].push_back({ ID_NPC_LIN, TEXT("보상으로 얻은 {color:#b8f8fb}미러시티 코인{color:#ffffff}을 사용해서 도움되는 아이템 몇가지를 교환할 수 있을거에요.\n어디보자.. 100개정도 모아오시면 새로운 퀘스트를 드릴게요.") });
	m_mapDescriptions[112].push_back({ ID_NPC_LIN, TEXT("[nextdialog:12][questclear:12][autoend:1]") });

	m_mapDescriptions[113].push_back({ ID_NPC_LIN, TEXT("[camera:1][fsm:Interaction/4]재밌게 잘 즐기고 왔나요?") });
	m_mapDescriptions[113].push_back({ ID_NPC_LIN, TEXT("미안하지만 긴급한 일이 생겼어요.\n{color:#f7e600}시리{color:#ffffff}로부터 심연의 상태가 심상치 않다는 연락이 왔어요.\n지금 바로 심연으로 이동해줄래요?") });
	m_mapDescriptions[113].push_back({ ID_NPC_LIN, TEXT("아참, 가기 전에{color:#b8f8fb} 미러시티 코인{color:#ffffff}을 사용하지 않았다면 장비를 교환해서 가세요.") });
	m_mapDescriptions[113].push_back({ ID_NONE, TEXT("[nextdialog:13][questclear:13][autoend:1]") });

	m_mapDescriptions[116].push_back({ ID_NPC_LIN, TEXT("[camera:1][fsm:Interaction/4]고생했어요.") });
	m_mapDescriptions[116].push_back({ ID_NPC_LIN, TEXT("여기서의 일은 끝났지만... 당신은 이제 시작이겠네요.") });
	m_mapDescriptions[116].push_back({ ID_NPC_LIN, TEXT("무운을 빌어요.") });
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
	[anim:어떤 객체, 어떤 애니메이션]
	{color:#ffffff} color 처리는 따로 함
	[sound:~~]
	[camera:~~]
	[wait:1.0]
	[reward:id, num / id, num ....]
	[questbegin: questID]
	[questclear : questID]
	[nextdialog : dialogID]
	[autoend : 대화 자동종료]
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

	/* 문자열이 다 출력됐을 때 */
	if (m_iCurrentReadCharEndIndex >= wstrDescription.size())
	{
		// TODO : 키를 눌렀을 때 다음 대화로 넘어가도록 만들기
		if (CFunc::Is_KeyDown(DIK_F, false))
		{
			/* 다음에 이어질 대사가 없어서 Next_Dialog() 동작 실패하면 */
			if (FAILED(Next_Dialog()))
				return false;

			/* 다음에 이어질 대사가 있어서 Next_Dialog() 동작 성공하면 */
			else
			{
				/* 대화창 오른쪽 밑의 삼각형 Disable */
				Level eCurLevel = CGameInstance::Get_Instance()->Get_CurLevel();
				CGameInstance::Get_Instance()->Disable_UI((_uint)eCurLevel, TEXT("Layer_UI_Dialogue"), 1);
				return true;
			}
				
		}
		//return true;

		/* 대화창 오른쪽 밑의 삼각형 Enable */
		Level eCurLevel = CGameInstance::Get_Instance()->Get_CurLevel();
		CGameInstance::Get_Instance()->Enable_UI((_uint)eCurLevel, TEXT("Layer_UI_Dialogue"), 1);
	}

	// 이벤트 메시지 검출. 연속된 이벤트가 존재할 수 있기 때문에 while로 처리
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
		// 컬러 태그 형식 앞 부분 '{'을 발견하면 컬러 태그는 바로 집어넣음
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