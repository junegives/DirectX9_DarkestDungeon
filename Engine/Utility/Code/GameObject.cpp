#include "GameObject.h"
#include"Component.h"
#include"Collider.h"

CGameObject::CGameObject() : m_pGraphicDev(nullptr)
{
}

CGameObject::CGameObject(LPDIRECT3DDEVICE9 _pGraphicDev): m_pGraphicDev(_pGraphicDev)
{
//	m_pGraphicDev->AddRef();
}

CGameObject::~CGameObject()
{
	//m_pGraphicDev->Release();
}

void CGameObject::AwakeGameObject()
{
	AddComponent();
}

_int CGameObject::UpdateGameObject(const _float& fTimeDelta)
{
	int iExit(0);

	for (auto& iter : m_mapComponent[ID_DYNAMIC]) {

		if (!iter.second->GetIsActive()) {
			iter.second.reset();
			m_mapComponent->erase(iter.first);
			continue;
		}

		if(iter.second->GetIsEnable())
			iExit = iter.second->UpdateComponent(fTimeDelta);

	}

	return iExit;
}

void CGameObject::LateUpdateGameObject()
{
	for (auto& iter : m_mapComponent[ID_DYNAMIC]) {

		if (!iter.second->GetIsActive()) {
			iter.second.reset();
			m_mapComponent->erase(iter.first);
			continue;
		}

		if (iter.second->GetIsEnable())
			iter.second->LateUpdateComponent();
	}
}


void CGameObject::AddComponent()
{
}

void CGameObject::SetMaterial()
{
	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = { 1.f, 1.f, 1.f, 1.f };
	tMtrl.Specular = { 1.f, 1.f, 1.f, 1.f };
	tMtrl.Ambient = { 0.2f, 0.2f, 0.2f, 1.f };
	tMtrl.Emissive = { 0.f, 0.f, 0.f, 0.f };
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

}

void CGameObject::GetMinMaxPos(_vec3& vMin , _vec3& vMax )
{
	if (m_pColliderCom != nullptr)
		m_pColliderCom->GetMinMax(vMin, vMax);

}

void CGameObject::GetSphere(_vec3& _vCenter, _float& _fRadius)
{
	if (m_pColliderCom != nullptr)
		m_pColliderCom->GetSphere(_vCenter, _fRadius);
}

void CGameObject::SetBillBoard(_matrix& _matWorld)
{
	_matrix matBill, matView;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);

	matBill._11 =  matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;



	D3DXMatrixInverse(&matBill, 0, &matBill);

	_matWorld = (matBill * _matWorld);
}

void CGameObject::SetFullScreenBillBoard(_matrix& _matWorld)
{

	_matrix matBill, matView;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._12= matView._12;
	matBill._13 = matView._13;

	matBill._21 = matView._21;
	matBill._22 = matView._22;
	matBill._23 = matView._23;

	matBill._31 = matView._31;
	matBill._32 = matView._32;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);

	_matWorld = (matBill * _matWorld);
}

std::shared_ptr<CComponent> CGameObject::GetComponent(const tstring& _strKeyName, COMPONENTID _eComID)
{
	auto iter = m_mapComponent->find(_strKeyName);

	if (iter == m_mapComponent->end())
		return nullptr;


	return iter->second;
}

void CGameObject::Compute_ViewZ(const _vec3* pPos)
{
	_matrix		matCamWorld;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);
	D3DXMatrixInverse(&matCamWorld, 0, &matCamWorld);

	_vec3	vCamPos;
	memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));

	vCamPos = vCamPos - *pPos;
	m_fViewZ = D3DXVec3Length(&(vCamPos));
}

void CGameObject::RemoveComponent()
{
}

void CGameObject::RenderObjAlpha(_int _iAlpha)
{
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);


	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);


	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(_iAlpha, 255, 255, 255));

}
