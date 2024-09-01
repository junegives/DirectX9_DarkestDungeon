#include"pch.h"
#include "Terrain.h"
#include"Export_Utility.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)

{
}

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev, const tstring& _KeyName, ETerrainType _eType)
	: Engine::CGameObject(pGraphicDev), m_strKeyName(_KeyName), m_eTerrainType(_eType)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: Engine::CGameObject(rhs)

{
}

CTerrain::~CTerrain()
{
}


HRESULT CTerrain::ReadyGameObject()
{
	return E_NOTIMPL;
}

_int CTerrain::UpdateGameObject(const _float& fTimeDelta)
{
	Engine::AddRenderGroup(RENDER_NONALPHA,shared_from_this());
	_int	iExit = CGameObject::UpdateGameObject(fTimeDelta);

	return iExit;
}

void CTerrain::LateUpdateGameObject()
{
}

void CTerrain::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	// m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);


	SetMaterial();


	m_pTextureCom->SetTextureKey(m_strKeyName, TEX_NORMAL);
	m_pTextureCom->SetTexture(0);




	m_pBufferCom->RenderBuffer();


	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CTerrain::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	pComponent = m_pBufferCom = make_shared<CTerrainTex>(m_pGraphicDev);

	pComponent = m_pTransformCom = make_shared<CTransform>(m_vPos, _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f));
	//m_pTransformCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	switch (m_eTerrainType)
	{
	case ETerrainType::VILLAGE:
		dynamic_pointer_cast<CTerrainTex>(m_pBufferCom)->ReadyBuffer(VILLAGE_TILESIZE * 10, VILLAGE_TILESIZE * 12, VTXITV, L"../Bin/Resource/Image/Height_Terrain/TestHeight2.bmp", VILLAGE_TILECNT, VILLAGE_TILECNT);
		break;
	case ETerrainType::VILLAGE_INSIDE:
		dynamic_pointer_cast<CTerrainTex>(m_pBufferCom)->ReadyBuffer(VILLAGE_TILESIZE * 3, VILLAGE_TILESIZE * 3, VTXITV, L"../Bin/Resource/Image/Height_Terrain/TestHeight2.bmp", 5, 5);
		break;
	case ETerrainType::DUNGEON:
		dynamic_pointer_cast<CTerrainTex>(m_pBufferCom)->ReadyBuffer(VTXCNTX, VTXCNTZ, VTXITV, L"../Bin/Resource/Image/Height_Terrain/TestHeight10.bmp", 20, 20);
		break;
	case ETerrainType::DUNGEON2:
		dynamic_pointer_cast<CTerrainTex>(m_pBufferCom)->ReadyBuffer(640, 640, (_ulong)VTXITV, L"../Bin/Resource/Image/Height_Terrain/TestHeight10.bmp", 20, 20);
		break;
	case ETerrainType::DUNGEON2_ROOF:
		dynamic_pointer_cast<CTerrainTex>(m_pBufferCom)->ReadyBuffer(640, 640, (_ulong)VTXITV, L"../Bin/Resource/Image/Height_Terrain/TestHeight10.bmp", 20, 20);
		m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y + 16.f, m_vPos.z);
		break;
	case ETerrainType::ENUM_END:
		dynamic_pointer_cast<CTerrainTex>(m_pBufferCom)->ReadyBuffer(VTXCNTX, VTXCNTZ, VTXITV, L"../Bin/Resource/Image/Height_Terrain/TestHeight10.bmp", 20, 20);
		break;
	default:
		dynamic_pointer_cast<CTerrainTex>(m_pBufferCom)->ReadyBuffer(VTXCNTX, VTXCNTZ, VTXITV, L"../Bin/Resource/Image/Height_Terrain/TestHeight10.bmp", 20, 20);
		break;
	}
	
	m_mapComponent[ID_STATIC].insert({ m_strKeyName, pComponent });

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
}

void CTerrain::KeyInput(const _float& fTimeDelta)
{
}
