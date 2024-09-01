#include "pch.h"
#include "EnvironmentObj.h"
#include"Export_Utility.h"

CEnvironmentObj::CEnvironmentObj(LPDIRECT3DDEVICE9 pGraphicDev, const tstring& _KeyName, _bool _bColliding, _bool _bBillBoard, _int _iCnt, _bool _bRandom)
	: Engine::CGameObject(pGraphicDev), m_strKeyName(_KeyName), m_bBillBoard(_bBillBoard), m_iCnt(_iCnt), m_bRandom(_bRandom)
{
	m_bColliding = _bColliding;
}

CEnvironmentObj::CEnvironmentObj(const CEnvironmentObj& rhs)
	: Engine::CGameObject(rhs)
{
}

CEnvironmentObj::~CEnvironmentObj()
{
}

HRESULT CEnvironmentObj::ReadyGameObject()
{
	m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);

	m_pTransformCom->SetAngle(m_vAngle);

	if (m_bColliding)
	{
		m_eCollideID = ECollideID::ENVIRONMENT;

		if (PI / 2.f == m_vAngle.y)
		{
			m_bHorizontal = true;
			m_pColliderCom->SetScale({ m_vScale.z, m_vScale.y * 2.f, m_vScale.x * 2.f});
		}
		else
		{
			m_bHorizontal = false;
			m_pColliderCom->SetScale({ m_vScale.x * 2.f, m_vScale.y * 2.f, m_vScale.z });
		}

	}

	if (m_bRandom)
		m_iNum = rand() % m_iCnt;

	else
		m_iNum = m_iCnt - 1;


	return S_OK;
}

_int CEnvironmentObj::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = CGameObject::UpdateGameObject(fTimeDelta);

	if (m_bBillBoard)
	{
		_matrix matWorld = *m_pTransformCom->GetWorld();
		SetBillBoard(matWorld);
		m_pTransformCom->SetWorld(&matWorld);
	}

	Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());

	return iExit;
}

void CEnvironmentObj::LateUpdateGameObject()
{
}

void CEnvironmentObj::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pTextureCom->SetTextureKey(m_strKeyName, TEX_NORMAL);
	m_pTextureCom->SetTexture(m_iNum);

	m_pBufferCom->RenderBuffer();
	
	if(m_bColliding)
		m_pColliderCom->RenderCollider();

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CEnvironmentObj::OnCollide(shared_ptr<CGameObject> _pObj)
{
}

void CEnvironmentObj::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	pComponent = m_pBufferCom = (make_shared<CRcTex>(m_pGraphicDev));
	dynamic_pointer_cast<CRcTex>(m_pBufferCom)->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = make_shared<CTransform>(_vec3(0.f, 0.f, 0.f), _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f));
	//m_pTransformCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	if (m_bColliding)
	{
		pComponent = m_pColliderCom = make_shared<CCollider>(m_pGraphicDev);
		m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider",pComponent });
		m_pColliderCom->SetPos(m_pTransformCom->GetPos());
	}
}
