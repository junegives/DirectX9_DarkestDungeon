#include "UIObj.h"

CUIObj::CUIObj(LPDIRECT3DDEVICE9 _pGraphicDev)
	:CGameObject(_pGraphicDev)
{
}

CUIObj::~CUIObj()
{
}

HRESULT CUIObj::ReadyGameObject()
{
	CalculateUIRect();
	m_bReady = true;

	return S_OK;
}

_int CUIObj::UpdateGameObject(const _float& fTimeDelta)
{
	int iExit = 0;

	return iExit;
}

void CUIObj::LateUpdateGameObject()
{
}

void CUIObj::RenderGameObject()
{
}

void CUIObj::AddComponent()
{
	if (m_bReady) return;

	shared_ptr<CComponent> pComponent;

	pComponent = m_pRcTexCom = make_shared<CRcTex>(m_pGraphicDev);
	m_pRcTexCom->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_RcTex",pComponent });

	pComponent = m_pTransCom = make_shared<CTransform>();

	if (nullptr == pComponent) {
		//MSG_BOX("Make UI Comopnent Failed!");
	}

	m_pTransCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform",pComponent });
}


void CUIObj::CalculateUIRect()
{ 

	_float fScreenX = 1260.f;
	_float fScreenY = 680.f;

	_vec3* vPos = m_pTransCom->GetPos();
	const _vec3* vAngle = m_pTransCom->GetAngles();
	
	m_UIRect.left = (LONG)((m_vPos.x + fScreenX*0.5f) - m_vSize.x);
	m_UIRect.right = (LONG)(m_UIRect.left + (m_vSize.x * 2.f));
	m_UIRect.top= (LONG)((-1 * m_vPos.y) + fScreenY * 0.5f - m_vSize.y);
	m_UIRect.bottom = (LONG)(m_UIRect.top + (m_vSize.y * 2.f));

}
