#include"pch.h"
#include "LoadingBack.h"

#include"Export_Utility.h"
#include"Export_System.h"

CLoadingBack::CLoadingBack(LPDIRECT3DDEVICE9 _pGraphicDev, ELoadingSceneType _eType)
	:CUIObj(_pGraphicDev) , m_eLoadingType(_eType)
{
}

CLoadingBack::~CLoadingBack()
{
}

HRESULT CLoadingBack::ReadyGameObject()
{
	m_vSize = { WINCX * 0.5f, WINCY * 0.5f , 0.f };
	m_vPos = { 0.f, 0.f , 0.1f };

	m_bVisible = true;
	m_bEnable = true;
	m_bActive = true;

	m_pTransCom->SetScale(m_vSize.x, m_vSize.y, m_vSize.z);
	m_pTransCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransCom->SetAngle({ 0.f, 0.f, 0.f });

	CUIObj::ReadyGameObject();

    return S_OK;
}

_int CLoadingBack::UpdateGameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::UpdateGameObject(fTimeDelta);
	AddRenderGroup(RENDER_UI, shared_from_this());

    return iExit;
}

void CLoadingBack::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();

}

void CLoadingBack::RenderGameObject()
{
	if (!m_bVisible)
		return;

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);


	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	 
	//BackGround
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->GetWorld());
	m_pBackGroundTex->SetTexture(0);
	m_pRcTexCom->RenderBuffer();

	//LayOut
	for (int i = 0; i < 2; ++i) {

		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pLayoutTransCom[i]->GetWorld());
		m_pTextureCom[i]->SetTexture(0);
		m_pLayoutRcTexCom[i]->RenderBuffer();
	}

	// Torch
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pLayoutTransCom[2]->GetWorld());
		m_pAnimatorCom->SetAnimTexture();
		m_pLayoutRcTexCom[2]->RenderBuffer();
	}

	//Font

	//m_pLayoutTransCom[0]->SetPosition(0.f, 280.f, 0.f);
	//m_pLayoutTransCom[0]->SetAngle({ 0.f, 0.f, 0.f });
	//m_pLayoutTransCom[0]->SetScale(209.f, 50.f, 0.f);

	//m_pLayoutTransCom[1]->SetPosition(0.f, -200.f, 0.f);
	//m_pLayoutTransCom[1]->SetAngle({ 0.f, 0.f, 0.f });
	//m_pLayoutTransCom[1]->SetScale(223.f, 82.f, 0.f);

	//_vec2 vTitleStartPos = {-209.f , 330.f};
	//_vec2 vTitleEndPos = {209.f , 230.f};

	_vec2 vTitleStartPos = { 0.f , 45.f };
	_vec2 vTitleEndPos = { WINCX, 75.f };

	_vec2 vTipStartPos = {0.f , 330.f};
	_vec2 vTipeEndPos = {WINCX , 380.f};

	if (ELoadingSceneType::VILLAGE == m_eLoadingType) {


		RenderFontCenter(L"Font_Point_Title", L"영지", &vTitleStartPos, &vTitleEndPos, YELLOW);
		RenderFontCenter(L"Font_Default_Light", L"마을의 도박장을 찾아가면 돈을 벌 수 있습니다.", &vTipStartPos, &vTipeEndPos, WHITE);

	}
	else if (ELoadingSceneType::RUIN == m_eLoadingType) {

		RenderFontCenter(L"Font_Point_Title", L"폐허", &vTitleStartPos, &vTitleEndPos, YELLOW);
		RenderFontCenter(L"Font_Default_Light", L"가문의 폐허로 향하는 첫 번째 여정입니다. 꼼꼼히 조사하되,\n항상 조심하십시오.", &vTipStartPos, &vTipeEndPos, WHITE);
	}
	else if (ELoadingSceneType::BOSS == m_eLoadingType) {

		RenderFontCenter(L"Font_Point_Title", L"가장 어두운 던전", &vTitleStartPos, &vTitleEndPos, YELLOW);
		RenderFontCenter(L"Font_Default_Light", L"", &vTipStartPos, &vTipeEndPos, WHITE);
	}

	//_vec2 vInfoStartPos = { 0.f , 430.f };
	//_vec2 vInfoEndPos = { WINCX , 480.f };

	//if(m_bLoadingDone)
	//	RenderFontCenter(L"Font_Default_Light", L"계속 진행하려면 [Enter]를 누르세요. ", &vInfoStartPos, &vInfoEndPos, YELLOW);


	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);


	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


}

void CLoadingBack::AddComponent()
{

	CUIObj::AddComponent();

	shared_ptr<CComponent> pComponent;


	//Layout
	for (int i = 0; i < 3; ++i) {

		tstring strCurNum = to_wstring(i);

		tstring strComName = L"Com_TitleRcTex" + strCurNum;
		tstring strComName2 = L"Com_TitleTrans" + strCurNum;
		tstring strComName3 = L"Com_TitleTex" + strCurNum;

		pComponent = m_pLayoutRcTexCom[i] = make_shared<CRcTex>(m_pGraphicDev);
		m_pLayoutRcTexCom[i]->ReadyBuffer();
		m_mapComponent[ID_STATIC].insert({ strComName,pComponent });

		pComponent = m_pLayoutTransCom[i] = make_shared<CTransform>();
		m_pLayoutTransCom[i]->ReadyTransform();
		m_mapComponent[ID_DYNAMIC].insert({ strComName2,pComponent });

		if (i >= 2) break;
		pComponent = m_pTextureCom[i] = make_shared<CTexture>(m_pGraphicDev);
		m_mapComponent[ID_STATIC].insert({ strComName3,pComponent });
	}

	pComponent = m_pAnimatorCom = make_shared<CAnimator>(m_pGraphicDev);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent});

	m_pTextureCom[0]->SetTextureKey(L"UI_Loading_TitleOverlay", TEXTUREID::TEX_NORMAL);
	m_pTextureCom[1]->SetTextureKey(L"UI_Loading_TipOverlay", TEXTUREID::TEX_NORMAL);

	m_pAnimatorCom->SetAnimKey(L"UI_Loading_Torch", 0.1f);

	m_pLayoutTransCom[0]->SetPosition(0.f, 280.f, 0.f);
	m_pLayoutTransCom[0]->SetAngle({ 0.f, 0.f, 0.f });
	m_pLayoutTransCom[0]->SetScale(209.f, 50.f, 0.f);

	m_pLayoutTransCom[1]->SetPosition(0.f, -200.f, 0.f);
	m_pLayoutTransCom[1]->SetAngle({ 0.f, 0.f, 0.f });
	m_pLayoutTransCom[1]->SetScale(223.f, 82.f, 0.f);

	m_pLayoutTransCom[2]->SetPosition(22.f, -252.f, 0.f);
	m_pLayoutTransCom[2]->SetAngle({ 0.f, 0.f, 0.f });
	m_pLayoutTransCom[2]->SetScale(100.f, 100.f, 0.f);

	//BackGround-----------------------------------------------------------------------------------------
	pComponent = m_pBackGroundTex = make_shared<CTexture>(m_pGraphicDev);
	m_mapComponent[ID_STATIC].insert({ L"Com_BGTex",pComponent });

	if (ELoadingSceneType::VILLAGE == m_eLoadingType) {

		m_pBackGroundTex->SetTextureKey(L"UI_Loading_Village", TEXTUREID::TEX_NORMAL);

	}
	else if(ELoadingSceneType::RUIN == m_eLoadingType) {
		m_pBackGroundTex->SetTextureKey(L"UI_Loading_Ruin", TEXTUREID::TEX_NORMAL);

	}
	else if (ELoadingSceneType::BOSS == m_eLoadingType) {
		m_pBackGroundTex->SetTextureKey(L"UI_Loading_Boss", TEXTUREID::TEX_NORMAL);

	}

}
