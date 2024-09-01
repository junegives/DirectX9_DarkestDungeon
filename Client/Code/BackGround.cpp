#include "pch.h"
#include "BackGround.h"
#include"Export_Utility.h"
#include"Export_System.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 _pGraphicDev)
	:CUIObj(_pGraphicDev)
{
}

CBackGround::~CBackGround()
{
}


HRESULT CBackGround::ReadyGameObject()
{

	m_vSize = { WINCX * 0.5f, WINCY , 0.f };
	m_vPos = { 0.f, WINCY * -1.f * 0.5f , 0.f };

	m_bVisible = true;
	m_bEnable = true;
	m_bActive = true;

	m_pTransCom->SetScale(m_vSize.x, m_vSize.y, m_vSize.z);
	m_pTransCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransCom->SetAngle({ 0.f, 0.f, 0.f });

	CUIObj::ReadyGameObject();

	return S_OK;
}

_int CBackGround::UpdateGameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::UpdateGameObject(fTimeDelta);
	
	if (!m_bAnimDone && m_strCurrentAnim == L"UI_Title_Show") {

		_bool bFin = dynamic_pointer_cast<CAnimator>(m_pAnimCom)->CheckFinish();

		if (bFin) {
			m_bAnimDone = true;
			m_pAnimCom->SetAnimKey(L"UI_Title_Torch", 0.1f);
			m_strCurrentAnim = L"UI_Title_Torch";
			m_pTitleTransCom->SetScale(399.f, 165.f, 0.f);

		}
	}


	if (m_bLoadingDone)
		PosChange(fTimeDelta);
	
	AddRenderGroup(RENDER_UI,shared_from_this());

    return iExit;
}

void CBackGround::LateUpdateGameObject()
{

	__super::LateUpdateGameObject();

}

void CBackGround::RenderGameObject()
{
	if (!m_bVisible)
		return;

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	//BackGround
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->GetWorld());
	m_pTextureCom->SetTexture(0);
	m_pRcTexCom->RenderBuffer();


	if (m_bSetDone) {

		if (L"" == m_strCurrentAnim && !m_bAnimDone) {
			m_strCurrentAnim = L"UI_Title_Show";
			m_pAnimCom->SetAnimKey(L"UI_Title_Show", 0.1f);
		}

		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		//Title
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTitleTransCom->GetWorld());
		m_pAnimCom->SetAnimTexture();
		m_pTitleRcTexCom->RenderBuffer();

		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		tstring strStart = L"Press Enter";
		_vec2 vStart = { 0.f, 400.f };
		_vec2 vEnd = { WINCX, 450.f };

		RenderFontCenter(L"Font_Point", strStart.c_str(), &vStart, &vEnd, D3DXCOLOR(0.878f, 0.807f, 0.6f, 1.f));
	}

	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CBackGround::PosChange(_float fTimeDelta)
{
	if (m_fTotalRange >= WINCY) {
		return;
	}


	if (m_fTotalRange + 250.f * fTimeDelta >= WINCY) {

		m_vPos.y += (WINCY - m_fTotalRange);
		m_fTotalRange = WINCY;
		m_bSetDone = true;
	}
	else {
		m_vPos.y += 250.f * fTimeDelta;
		m_fTotalRange += 250.f * fTimeDelta;
	}

	m_pTransCom->SetPosition(m_vPos.x , m_vPos.y , m_vPos.z);
	const _vec3* vPos = m_pTitleTransCom->GetPos();
	m_pTitleTransCom->SetPosition(0.f, vPos->y + 250.f * fTimeDelta, 0.f);

}

void CBackGround::AddComponent()
{
	CUIObj::AddComponent();

	shared_ptr<CComponent> pComponent;

	pComponent = m_pTitleRcTexCom = make_shared<CRcTex>(m_pGraphicDev);
	m_pTitleRcTexCom->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_TitleRcTex",pComponent });

	pComponent = m_pAnimCom = make_shared<CAnimator>(m_pGraphicDev);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator",pComponent });

	//m_pAnimCom->SetAnimKey(L"UI_Title_Torch", 0.1f);

	pComponent = m_pTitleTransCom = make_shared<CTransform>();
	m_pTitleTransCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_TitleTrans",pComponent });

	m_pTitleTransCom->SetPosition(0.f, m_vPos.y - 500.f, 0.f);
	m_pTitleTransCom->SetAngle({0.f,0.f,0.f});
	m_pTitleTransCom->SetScale(399.f, 141.f, 0.f);


	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_mapComponent[ID_STATIC].insert({ L"Com_BGTex",pComponent });
	m_pTextureCom->SetTextureKey(L"UI_Title_BG", TEXTUREID::TEX_NORMAL);
}
