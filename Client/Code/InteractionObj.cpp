#include "pch.h"
#include "InteractionObj.h"
#include "Export_Utility.h"
#include "Export_System.h"
#include "UIMgr.h"

#include "Player.h"
#include"InteractionInfo.h"


CInteractionObj::CInteractionObj(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CInteractionObj::CInteractionObj(const CInteractionObj& rhs)
	: Engine::CGameObject(rhs)
{
}

CInteractionObj::~CInteractionObj()
{
}


HRESULT CInteractionObj::ReadyGameObject()
{
	// �÷��̾� �޾ƿ��� (�Ÿ� ���, ���� ����)
	m_pPlayer = CGameMgr::GetInstance()->GetPlayer();

	// ���۳�Ʈ �޾ƿ���
	m_pPlayerTransformCom = dynamic_pointer_cast<CTransform>(m_pPlayer->GetComponent(L"Com_Transform", ID_DYNAMIC));

	m_pTransformCom->SetAngle(m_vAngle);

	return S_OK;
}

_int CInteractionObj::UpdateGameObject(const _float& fTimeDelta)
{
	Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());
	_int	iExit = CGameObject::UpdateGameObject(fTimeDelta);

	m_bCanInteract = false;

	// ��ȣ �ۿ� ���� �ƴϰ�, ��ȣ �ۿ� �Ÿ� �ȿ� ��� ���
	if (!m_bInteracting && 11.f > CalcDistance())
	{
		m_bCanInteract = true;
		dynamic_pointer_cast<CInteractionInfo>(CUIMgr::GetInstance()->FindUI(L"UI_InteractionInfo"))->SetInteractionText(m_bInteractionKey, m_bInteractionInfo);
		

		GetInteractionKey(fTimeDelta);

		if (m_bTabInteractionKey) {
			CUIMgr::GetInstance()->SelectUIVisibleOff(L"UI_InteractionInfo");
		}
	}

	//if(!m_bCanInteract || m_bTabInteractionKey)
	//	CUIMgr::GetInstance()->SelectUIVisibleOff(L"UI_InteractionInfo");


	// ��ȣ �ۿ� ���� ���
	else if(m_bInteracting)
	{
		// ��ȣ �ۿ��� �ϴٰ� �ָ� �̵��� ���
		if (20.f < CalcDistance())
		{
			// �ڵ����� ��ȣ �ۿ� �����ϰ� �̵� ���� Ǯ��
			FinishInteraction();
		}

		// �Ÿ� ������ ��ȣ�ۿ����� ���
		else
		{
			// ��ȣ �ۿ� ���� Ʈ���� �ޱ�
			// ��ȣ�ۿ� ���� ���� ���� Ŭ�������� �����ϱ�
			if (IsFinish())
				FinishInteraction();
		}
	}

	return iExit;
}

void CInteractionObj::LateUpdateGameObject()
{
}

void CInteractionObj::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);


	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = { 1.f, 1.f, 1.f, 1.f };
	tMtrl.Specular = { 1.f, 1.f, 1.f, 1.f };
	tMtrl.Ambient = { 0.2f, 0.2f, 0.2f, 1.f };
	tMtrl.Emissive = { 0.f, 0.f, 0.f, 0.f };
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);


	m_pTextureCom->SetTexture(0);

	m_pBufferCom->RenderBuffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CInteractionObj::AddComponent()
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
	m_pTextureCom->SetTextureKey(L"Village_Door_Close", TEX_NORMAL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture",pComponent });

}

_float CInteractionObj::CalcDistance()
{
	_vec3 vPos;
	m_pTransformCom->GetInfo(INFO_POS, &vPos);

	_vec3 vPosPlayer;
	m_pPlayerTransformCom->GetInfo(INFO_POS, &vPosPlayer);

	_vec3 vDist = vPosPlayer - vPos;

	return D3DXVec3Length(&vDist);
}

void CInteractionObj::GetInteractionKey(const _float& fTimeDelta)
{
	// ���ͷ��� Ű UI �����ֱ�
	// ----------���� �ʿ�------------

	// Ű �Է¹ޱ�
	if (GetAsyncKeyState('C') & 0x8000)
	{
		m_bInteracting = true;
		// �÷��̾� �ൿ ����
		CGameMgr::GetInstance()->SetGameState(EGameState::LOCK);

		// �ؽ�ó �Ǵ� �ִϸ��̼� ����
		ChangeTexture();

		Interaction();
	}
}

void CInteractionObj::FinishInteraction()
{
	m_bInteracting = false;

	// �÷��̾� �ൿ Ǯ��
	CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);
}