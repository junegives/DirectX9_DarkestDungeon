#include "pch.h"
#include "Trigger.h"
#include "Export_Utility.h"

#include "SoundMgr.h"
#include "Weald_Dungeon.h"
#include "Ruin_Dungeon.h"
#include "LoadingScreen.h"

CTrigger::CTrigger(LPDIRECT3DDEVICE9 pGraphicDev, const tstring& _KeyName, _bool _bColliding, _bool _bBillBoard, _int _iCnt, _bool _bRandom)
	: CEnvironmentObj(pGraphicDev, _KeyName, _bColliding, _bBillBoard, _iCnt, _bRandom)
{
}

CTrigger::~CTrigger()
{
}

HRESULT CTrigger::ReadyGameObject()
{
	m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);

	m_pTransformCom->SetAngle(m_vAngle);

	if (m_bColliding)
	{
		m_eCollideID = ECollideID::TRIGGER;

		if (m_strKeyName == L"RuinDungeon")
		{
			m_eCollideID = ECollideID::SCENE_CHANGE_TRIGGER;
		}

		if (PI / 2.f == m_vAngle.y)
		{
			m_bHorizontal = true;
			m_pColliderCom->SetScale({ m_vScale.z, m_vScale.y * 2.f, m_vScale.x * 2.f });
		}
		else
		{
			m_bHorizontal = false;
			m_pColliderCom->SetScale({ m_vScale.x * 2.f, m_vScale.y * 2.f, m_vScale.z });
		}

	}


	return S_OK;
}

_int CTrigger::UpdateGameObject(const _float& fTimeDelta)
{
	Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());

	_int _iExit = __super::UpdateGameObject(fTimeDelta);

	return _iExit;
}

void CTrigger::LateUpdateGameObject()
{
}

void CTrigger::RenderGameObject()
{
	if (m_bColliding)
		m_pColliderCom->RenderCollider();
}

void CTrigger::OnCollide(shared_ptr<CGameObject> _pObj)
{
	if (_pObj->GetColType() == ECollideID::PLAYER)
	{
		if (m_strKeyName == L"Tavern")
		{
			CSoundMgr::GetInstance()->StopAll();
			CSoundMgr::GetInstance()->PlaySound(L"Mus_Town_Base.wav", CHANNELID::BGM, 1.f);

			CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
			CSoundMgr::GetInstance()->PlaySound(L"town_door_close.wav", CHANNELID::EFFECT, 1.f);

			_pObj->SetPos({ 51.16f, 0.0f, 68.27f + VILLAGE_TILESIZE * 2.f - 2.f });
			dynamic_pointer_cast<CTransform>(_pObj->GetComponent(L"Com_Transform", ID_DYNAMIC))->SetPosition(_pObj->GetPos().x, _pObj->GetPos().y, _pObj->GetPos().z);
			_pObj->SetAngle({0.f, 3.22f, 0.f});
		}

		else if (m_strKeyName == L"Guild")
		{
			CSoundMgr::GetInstance()->StopAll();
			CSoundMgr::GetInstance()->PlaySound(L"Mus_Town_Base.wav", CHANNELID::BGM, 1.f);

			CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
			CSoundMgr::GetInstance()->PlaySound(L"town_door_close.wav", CHANNELID::EFFECT, 1.f);

			_pObj->SetPos({ 94.35f, 0.0f, 79.65f + VILLAGE_TILESIZE });
			dynamic_pointer_cast<CTransform>(_pObj->GetComponent(L"Com_Transform", ID_DYNAMIC))->SetPosition(_pObj->GetPos().x, _pObj->GetPos().y, _pObj->GetPos().z);
			_pObj->SetAngle({ 0.f, 4.07f, 0.f });
		}

		else if (m_strKeyName == L"WealdDungeon")
		{
			m_eCollideID = ECollideID::SCENE_CHANGE_TRIGGER;
			CSoundMgr::GetInstance()->StopAll();
			CSoundMgr::GetInstance()->StopSound(CHANNELID::BGM);
			shared_ptr<CWeald_Dungeon> pScene = make_shared<CWeald_Dungeon>(m_pGraphicDev);
			CSceneMgr::GetInstance()->ChangeScene(pScene);
			pScene->ReadyScene();
		}
		else if (m_strKeyName == L"RuinDungeon")
		{
			m_eCollideID = ECollideID::SCENE_CHANGE_TRIGGER;
			CSoundMgr::GetInstance()->StopAll();
			CSoundMgr::GetInstance()->StopSound(CHANNELID::BGM);
			shared_ptr<CScene> pLoadingScreen = make_shared<CLoadingScreen>(m_pGraphicDev, ELoadingSceneType::RUIN);
			CSceneMgr::GetInstance()->SetLoadingState(false);
			CSceneMgr::GetInstance()->ChangeScene(pLoadingScreen);
			pLoadingScreen->ReadyScene();
		}
	}
}
