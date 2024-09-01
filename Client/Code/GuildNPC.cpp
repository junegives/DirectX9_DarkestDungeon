#include "pch.h"
#include "GuildNPC.h"
#include "GameMgr.h"
#include "CameraMgr.h"
#include "UIMgr.h"

#include "PickingGame.h"
#include "Player.h"
#include "Hero.h"
#include "HeroSkillUI.h"

#include "Export_System.h"
#include "Export_Utility.h"

CGuildNPC::CGuildNPC(LPDIRECT3DDEVICE9 pGraphicDev)
	: CInteractionObj(pGraphicDev)
{
}	

CGuildNPC::CGuildNPC(const CGuildNPC& rhs)
	: CInteractionObj(rhs)
{
}

CGuildNPC::~CGuildNPC()
{
}

HRESULT CGuildNPC::ReadyGameObject()
{
	m_pPickingGame = make_shared<CPickingGame>(m_pGraphicDev);
	//m_vecGameObject.push_back(m_pPickingGame);

	__super::ReadyGameObject();

	m_bInteractionKey = L"C";
	m_bInteractionInfo = L"스킬 해금";

	return S_OK;
}

_int CGuildNPC::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	if (!m_pHeroVec)
	{
		for (auto& iter : m_vecGameObject)
		{
			iter->AwakeGameObject();
			iter->ReadyGameObject();
		}
	}

	for (auto& iter : m_vecGameObject)
	{
		iter->UpdateGameObject(fTimeDelta);
	}

	for (auto& iter : m_pUIVec)
	{
		iter->UpdateGameObject(fTimeDelta);
	}

	return iExit;
}

void CGuildNPC::LateUpdateGameObject()
{
	for (auto& iter : m_vecGameObject)
	{
		iter->LateUpdateGameObject();
	}

	__super::LateUpdateGameObject();
}

void CGuildNPC::RenderGameObject()
{
	for (auto& iter : m_vecGameObject)
	{
		iter->RenderGameObject();
	}
	__super::RenderGameObject();
}

void CGuildNPC::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	pComponent = m_pBufferCom = make_shared<CRcTex>(m_pGraphicDev);
	dynamic_pointer_cast<CRcTex>(m_pBufferCom)->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_RcTex", pComponent });

	pComponent = m_pTransformCom = make_shared<CTransform>(_vec3(0.f, 0.f, 0.f), _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f));
	m_pTransformCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });
	m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_pTextureCom->SetTextureKey(L"Guild_SandBack_Texture", TEX_NORMAL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture",pComponent });
}

void CGuildNPC::GetInteractionKey(const _float& fTimeDelta)
{
	// 인터랙션 키 UI 보여주기
	// ----------수정 필요------------

	// 키 입력받기
	if (GetAsyncKeyState('C') & 0x8000)
	{

		if (!m_bInteractionFirst) {
			CUIMgr::GetInstance()->NarrationOn(L"Narr_Visit_Guild");
			m_bInteractionFirst = true;
		}

		CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
		CSoundMgr::GetInstance()->PlaySound(L"town_guild_npc.wav", CHANNELID::EFFECT, 1.f);

		shared_ptr<CUIObj> pHeroSkillUI;
		_int	iIdx = 0;

		m_pHeroVec = dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetHeroVec();

		// 영웅 스킬 UI 생성
		for (auto& iter :
			*dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetHeroVec()
			)
		{
			pHeroSkillUI = make_shared<CHeroSkillUI>(m_pGraphicDev, dynamic_pointer_cast<CHero>(iter), iIdx);
			pHeroSkillUI->SetVisible(false);
			CUIMgr::GetInstance()->AddUIObject(L"UI_HeroSkill" + to_wstring(iIdx), pHeroSkillUI);

			pHeroSkillUI->AwakeGameObject();
			pHeroSkillUI->ReadyGameObject();

			m_pUIVec.push_back(pHeroSkillUI);

			iIdx++;
		}

		m_bInteracting = true;

		// 플레이어 움직임 막기
		CGameMgr::GetInstance()->SetGameState(EGameState::LOCK);

		// 카메라 이동
		Interaction();
	}
}

void CGuildNPC::Interaction()
{
	// 영웅 UI 출력
	{
		for (auto& iter : m_pUIVec)
			iter->SetVisible(true);

		// 피킹 받기

		// 술집 키 입력처리 (술과 함께 넘어가는 근심)
	}
}

_bool CGuildNPC::IsFinish()
{
	// 키 입력받기
	if (GetAsyncKeyState('X') & 0x8000)
	{
		_int _iCnt = 0;

		for (auto& iter : m_pUIVec)
			_iCnt += dynamic_pointer_cast<CHeroSkillUI>(iter)->GetEquippedCnt();

		if (_iCnt < 8)
		{
			// 팝업 창 하나 종료 막기

			return false;
		}

		m_bInteracting = false;

		for (auto& iter : m_pUIVec)
			iter->SetVisible(false);


		for (auto& iterHero : *m_pHeroVec)
		{
			dynamic_pointer_cast<CHero>(iterHero)->ClearSkillVector();

			for (auto& iterSkill : *dynamic_pointer_cast<CHero>(iterHero)->GetAllSkillVector())
			{
				if (iterSkill->IsEquipped())
					dynamic_pointer_cast<CHero>(iterHero)->GetSkillVector()->push_back(iterSkill);
			}
		}

		// 카메라 원상복귀

		// 플레이어 행동 풀기
		CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);
		CCameraMgr::GetInstance()->SetVillageMode();
		return true;
	}

	return false;
}

void CGuildNPC::FinishInteraction()
{
}

void CGuildNPC::ChangeTexture()
{
}
