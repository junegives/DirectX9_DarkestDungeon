#include "pch.h"
#include "Gambling.h"
#include "GameMgr.h"
#include "SoundMgr.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include"CameraMgr.h"

#include"Player.h"

CGambling::CGambling(LPDIRECT3DDEVICE9 pGraphicDev)
    : CInteractionObj(pGraphicDev)
{
}

CGambling::CGambling(const CGambling& rhs)
    : CInteractionObj(rhs)
{
}

CGambling::~CGambling()
{
}

HRESULT CGambling::ReadyGameObject()
{
	m_pCardGame = make_shared<CCardGame>(m_pGraphicDev);
	m_pCardGame->SetPos({ 30.f, 50.f,50.f });
	m_pCardGame->AwakeGameObject();
	m_pCardGame->ReadyGameObject();

	m_bInteractionKey = L"C";
	m_bInteractionInfo = L"�̴ϰ���";

    __super::ReadyGameObject();

    return S_OK;
}

_int CGambling::UpdateGameObject(const _float& fTimeDelta)
{
	if (m_bDebounce) {

		m_fDebounceTime += fTimeDelta;
		if (m_fDebounceTime > 0.5f) {
			m_bDebounce = true;
		}

	}

	m_pCardGame->UpdateGameObject(fTimeDelta);

    _int	iExit = __super::UpdateGameObject(fTimeDelta);

    return iExit;
}

void CGambling::LateUpdateGameObject()
{
    __super::LateUpdateGameObject();
	m_pCardGame->LateUpdateGameObject();
}

void CGambling::RenderGameObject()
{
    __super::RenderGameObject();
	m_pCardGame->RenderGameObject();
}

void CGambling::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	pComponent = m_pBufferCom = make_shared<CRcTex>(m_pGraphicDev);
	dynamic_pointer_cast<CRcTex>(m_pBufferCom)->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_RcTex", pComponent });

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = make_shared<CTransform>(_vec3(0.f, 0.f, 0.f), _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f));
	m_pTransformCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });
	m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_pTextureCom->SetTextureKey(L"Gambling_Table_Texture", TEX_NORMAL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture",pComponent });
}

void CGambling::GetInteractionKey(const _float& fTimeDelta)
{
	// ���ͷ��� Ű UI �����ֱ�
	// ----------���� �ʿ�------------

	// Ű �Է¹ޱ�
	if (GetAsyncKeyState('C') & 0x8000)
	{
		if (m_bDebounce)
			return;

		CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
		CSoundMgr::GetInstance()->PlaySound(L"town_tavern_gambling.wav", CHANNELID::EFFECT, 1.f);

		m_bInteracting = true;

		// �÷��̾� ������ ����
		CGameMgr::GetInstance()->SetGameState(EGameState::LOCK);

		// ī�޶� �̵�
		CCameraMgr::GetInstance()->SetState(ECameraMode::IDLE);
		CCameraMgr::GetInstance()->CamAngleReset();
		CCameraMgr::GetInstance()->CamSetPos({ 35.f, -52.f , 50.f - 7.5f });

		Interaction();
		m_bDebounce = true;

	}
}

void CGambling::Interaction()
{
	m_pCardGame->GameStart();
}

_bool CGambling::IsFinish()
{
	// Ű �Է¹ޱ�
	if (GetAsyncKeyState('X') & 0x8000)
	{
		m_bInteracting = false;

		// m_pCardGame->End();
		GetReward();
		// �÷��̾� �ൿ Ǯ��
		CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);

		return true;
	}

	return false;
}

void CGambling::FinishInteraction()
{
}

void CGambling::ChangeTexture()
{
}

void CGambling::GetReward()
{
	/*CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
	CSoundMgr::GetInstance()->PlaySound(L"ui_town_buy.wav", CHANNELID::EFFECT, 1.f);*/

	//500 ��� reward �� �־��ֱ� 
	//dynamic_pointer_cast<CPlayer>(m_pPlayer)->SetGold(500, true);


}
