#include"pch.h"
#include "BossRoomDoor.h"

#include"Export_Utility.h"
#include"Export_System.h"

#include"SceneMgr.h"
#include"BossMap.h"
#include"SoundMgr.h"

#include "LoadingScreen.h"

CBossRoomDoor::CBossRoomDoor(LPDIRECT3DDEVICE9 _pGraphicDev)
	:CInteractionObj(_pGraphicDev)
{
}

CBossRoomDoor::~CBossRoomDoor()
{
}

HRESULT CBossRoomDoor::ReadyGameObject()
{
	__super::ReadyGameObject();

	m_bInteractionKey = L"C";
	m_bInteractionInfo = L"보스방으로 이동";

	return S_OK;
}

_int CBossRoomDoor::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	_matrix matWorld;

	matWorld = *m_pTransformCom->GetWorld();
	SetBillBoard(matWorld);
	m_pTransformCom->SetWorld(&matWorld);

	return iExit;
}

void CBossRoomDoor::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();

}

void CBossRoomDoor::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	SetMaterial();


	m_pAnimatorCom->SetAnimTexture();
	m_pBufferCom->RenderBuffer();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


}

void CBossRoomDoor::AddComponent()
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

	pComponent = m_pAnimatorCom = make_shared<CAnimator>(m_pGraphicDev);
	m_pAnimatorCom->SetAnimKey(L"Door_BossRoom", 0.2f);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator",pComponent });


}

void CBossRoomDoor::GetInteractionKey(const _float& fTimeDelta)
{
	if (m_bDebounce)
		return;

	if (GetAsyncKeyState('C') & 0x8000) {

		CSceneMgr::GetInstance()->SetSceneChange();
		m_bDebounce = true;

	}


}
