#include "pch.h"
#include "Wall.h"
#include"Export_Utility.h"

CWall::CWall(LPDIRECT3DDEVICE9 pGraphicDev, const tstring& _KeyName, _int _iCnt, _bool _bRandom)
	: Engine::CGameObject(pGraphicDev), m_strKeyName(_KeyName), m_iCnt(_iCnt), m_bRandom(_bRandom)
{
}

CWall::CWall(const CWall& rhs)
	: Engine::CGameObject(rhs)
{
}

CWall::~CWall()
{
}

HRESULT CWall::ReadyGameObject()
{
	m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);
	
	m_pTransformCom->SetAngle(m_vAngle);

	if (PI / 2.f == m_vAngle.y || PI / -2.f == m_vAngle.y)
	{
		m_bHorizontal = true;
		m_pColliderCom->SetScale({ m_vScale.x * 2.f, m_vScale.y * 2.f, m_vScale.z * 2.f });
	}
	else
	{
		m_bHorizontal = false;
		m_pColliderCom->SetScale({ m_vScale.z * 2.f, m_vScale.x * 2.f, m_vScale.y * 2.f });
	}

	m_pTransformCom->Rotation(ROT_Y, PI /2.f);
	
	if (m_bRandom)
		m_iNum = rand() % m_iCnt;

	else
		m_iNum = m_iCnt - 1;

	m_eCollideID = ECollideID::WALL;
	m_bColliding = true;
	return E_NOTIMPL;
}

_int CWall::UpdateGameObject(const _float& fTimeDelta)
{
	Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());
	_int	iExit = CGameObject::UpdateGameObject(fTimeDelta);

	return iExit;
}

void CWall::LateUpdateGameObject()
{
}

void CWall::RenderGameObject()
{

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	// m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pTextureCom->SetTextureKey(m_strKeyName, TEX_NORMAL);

	//if (m_strKeyName == L"Com_Village_Back_Texture")
	//{
	//	m_pTextureCom->SetTexture(m_iNum / 3);
	//}
	//else if (m_strKeyName == L"Com_Village_Left_Buildings_Texture" || m_strKeyName == L"Com_Village_Right_Buildings_Texture")
	//{
	//	m_pTextureCom->SetTexture();
	//}
	//else if (m_strKeyName == L"Com_Weald_BackWallTexture")
	//{
	//	m_pTextureCom->SetTexture(0);
	//}
	//else
	//{
	//	m_pTextureCom->SetTextureKey(L"Com_Weald_WallTexture", TEX_NORMAL);
	//	m_pTextureCom->SetTexture(m_iNum);
	//}

	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = { 1.f, 1.f, 1.f, 1.f };
	tMtrl.Specular = { 1.f, 1.f, 1.f, 1.f };
	tMtrl.Ambient = { 0.2f, 0.2f, 0.2f, 1.f };
	tMtrl.Emissive = { 0.f, 0.f, 0.f, 0.f };
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	m_pTextureCom->SetTexture(m_iNum);

	m_pBufferCom->RenderBuffer();
	m_pColliderCom->RenderCollider();



	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CWall::OnCollide(shared_ptr<CGameObject> _pObj)
{
}

void CWall::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	pComponent = m_pBufferCom = (make_shared<CRcTex>(m_pGraphicDev));
	dynamic_pointer_cast<CRcTex>(m_pBufferCom)->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_Weald_WallTexture", pComponent });

	//pComponent = m_pBufferCom2 = (make_shared<CRcTex>(m_pGraphicDev));
	//dynamic_pointer_cast<CRcTex>(m_pBufferCom2)->ReadyBuffer();
	//m_mapComponent[ID_STATIC].insert({ L"Com_Weald_BackWallTexture", pComponent });

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	//pComponent = m_pTextureCom2 = make_shared<CTexture>(m_pGraphicDev);
	//m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = make_shared<CTransform>(_vec3(0.f, 0.f, 0.f), _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f));
	//m_pTransformCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pColliderCom = make_shared<CCollider>(m_pGraphicDev);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider",pComponent });
	m_pColliderCom->SetPos(m_pTransformCom->GetPos());
}
