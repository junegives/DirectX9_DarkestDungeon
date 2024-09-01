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
	// 플레이어 받아오기 (거리 계산, 상태 변경)
	m_pPlayer = CGameMgr::GetInstance()->GetPlayer();

	// 컴퍼넌트 받아오기
	m_pPlayerTransformCom = dynamic_pointer_cast<CTransform>(m_pPlayer->GetComponent(L"Com_Transform", ID_DYNAMIC));

	m_pTransformCom->SetAngle(m_vAngle);

	return S_OK;
}

_int CInteractionObj::UpdateGameObject(const _float& fTimeDelta)
{
	Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());
	_int	iExit = CGameObject::UpdateGameObject(fTimeDelta);

	m_bCanInteract = false;

	// 상호 작용 중이 아니고, 상호 작용 거리 안에 드는 경우
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


	// 상호 작용 중인 경우
	else if(m_bInteracting)
	{
		// 상호 작용을 하다가 멀리 이동한 경우
		if (20.f < CalcDistance())
		{
			// 자동으로 상호 작용 종료하고 이동 제한 풀기
			FinishInteraction();
		}

		// 거리 내에서 상호작용중인 경우
		else
		{
			// 상호 작용 종료 트리거 받기
			// 상호작용 종료 조건 하위 클래스에서 생성하기
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
	// 인터랙션 키 UI 보여주기
	// ----------수정 필요------------

	// 키 입력받기
	if (GetAsyncKeyState('C') & 0x8000)
	{
		m_bInteracting = true;
		// 플레이어 행동 막기
		CGameMgr::GetInstance()->SetGameState(EGameState::LOCK);

		// 텍스처 또는 애니메이션 변경
		ChangeTexture();

		Interaction();
	}
}

void CInteractionObj::FinishInteraction()
{
	m_bInteracting = false;

	// 플레이어 행동 풀기
	CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);
}