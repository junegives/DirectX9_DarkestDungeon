#include "pch.h"
#include "TavernNPC.h"
#include "GameMgr.h"
#include "CameraMgr.h"
#include "Player.h"
#include "Hero.h"

#include "Export_System.h"
#include "Export_Utility.h"

CTavernNPC::CTavernNPC(LPDIRECT3DDEVICE9 pGraphicDev)
	: CInteractionObj(pGraphicDev)
{
}

CTavernNPC::CTavernNPC(const CTavernNPC& rhs)
	: CInteractionObj(rhs)
{
}

CTavernNPC::~CTavernNPC()
{
}

HRESULT CTavernNPC::ReadyGameObject()
{
	__super::ReadyGameObject();

	m_bInteractionKey = L"C";
	m_bInteractionInfo = L"���� ȸ��";

	return S_OK;
}

_int CTavernNPC::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	/*for (auto& iter : *dynamic_pointer_cast<CPlayer>(m_pPlayer)->GetHeroVec())
		iter->UpdateGameObject(fTimeDelta);*/

	return iExit;
}

void CTavernNPC::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();

	/*for (auto& iter : *dynamic_pointer_cast<CPlayer>(m_pPlayer)->GetHeroVec())
		iter->LateUpdateGameObject();*/
}

void CTavernNPC::RenderGameObject()
{
	__super::RenderGameObject();
}

void CTavernNPC::AddComponent()
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
	m_pTextureCom->SetTextureKey(L"Tavern_NPC_Texture", TEX_NORMAL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture",pComponent });
}

void CTavernNPC::GetInteractionKey(const _float& fTimeDelta)
{
	// ���ͷ��� Ű UI �����ֱ�
	// ----------���� �ʿ�------------

	// Ű �Է¹ޱ�
	if (GetAsyncKeyState('C') & 0x8000)
	{
		CSoundMgr::GetInstance()->StopSound(CHANNELID::NPC);
		CSoundMgr::GetInstance()->PlaySound(L"vo_town_tavern.wav", CHANNELID::NPC, 1.f);

		m_bInteracting = true;

		// �÷��̾� ������ ����
		CGameMgr::GetInstance()->SetGameState(EGameState::LOCK);

		// ī�޶� �̵�
		CCameraMgr::GetInstance()->SetState(ECameraMode::INTERACTION);
		dynamic_pointer_cast<CPlayer>(m_pPlayer)->ShowHeroesBackVillage();

		Interaction();
	}
}

void CTavernNPC::Interaction()
{
	// ���� ����
	{
		// ���� �ؽ�ó ����

		// ��ŷ �ޱ�

		// ���� Ű �Է�ó�� (���� �Բ� �Ѿ�� �ٽ�)
	}
}

_bool CTavernNPC::IsFinish()
{
	// Ű �Է¹ޱ�
	if (GetAsyncKeyState('X') & 0x8000)
	{
		m_bInteracting = false;

		// ��� ���� IDLE�� ����
		for (auto& iter : *dynamic_pointer_cast<CPlayer>(m_pPlayer)->GetHeroVec())
			dynamic_pointer_cast<CHero>(iter)->SetSelected(false);

		// ī�޶� ���󺹱�
		CCameraMgr::GetInstance()->CameraRotation(ECameraMode::ROTATION, 180.f);

		// �÷��̾� �ൿ Ǯ��
		CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);
		dynamic_pointer_cast<CPlayer>(m_pPlayer)->HideHeroesBackVillage();

		return true;
	}

	return false;
}

void CTavernNPC::FinishInteraction()
{
}

void CTavernNPC::ChangeTexture()
{
}
