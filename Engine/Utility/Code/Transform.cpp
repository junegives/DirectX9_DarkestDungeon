#include "Transform.h"

CTransform::CTransform()
{
}

CTransform::CTransform(const _vec3& _vPos)
{
	m_vInfo[INFO_POS] = _vPos;
}

CTransform::CTransform(const _vec3& _vPos, const _vec3& _vScale)
	:m_vScale(_vScale)
{
	m_vInfo[INFO_POS] = _vPos;
}

CTransform::CTransform(const _vec3& _vPos, const _vec3& _vScale, const _vec3& _vRotation)
	:m_vScale(_vScale), m_vAngle(_vRotation)

{
	m_vInfo[INFO_POS] = _vPos;
}

CTransform::~CTransform()
{
}

HRESULT CTransform::ReadyTransform()
{
	D3DXMatrixIdentity(&m_matWorld);
	m_vAngle = { 0.f , 0.f ,0.f };
	m_vScale = { 1.f, 1.f, 1.f };

	for (size_t i = 0; i < INFO_END; ++i)
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));

	return S_OK;
}

_int CTransform::UpdateComponent(const _float& fTimeDelta)
{
	__super::UpdateComponent(fTimeDelta);

	D3DXMatrixIdentity(&m_matWorld);

	for (_uint i = 0; i < INFO_POS; ++i)
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));

	// 크기 변환

	for (_uint i = 0; i < INFO_POS; ++i)
	{
		D3DXVec3Normalize(&m_vInfo[i], &m_vInfo[i]);
		m_vInfo[i] *= *(((_float*)&m_vScale) + i);
	}

	_matrix			matRot[ROT_END];
	 
	D3DXMatrixRotationX(&matRot[ROT_X], m_vAngle.x);
	D3DXMatrixRotationY(&matRot[ROT_Y], m_vAngle.y);
	D3DXMatrixRotationZ(&matRot[ROT_Z], m_vAngle.z);

	for (_uint i = 0; i < INFO_POS; ++i)
	{
		for (_uint j = 0; j < ROT_END; ++j)
		{
			D3DXVec3TransformNormal(&m_vInfo[i], &m_vInfo[i], &matRot[j]);
		}
	}

	for (_uint i = 0; i < INFO_END; ++i)
		memcpy(&m_matWorld.m[i][0], &m_vInfo[i], sizeof(_vec3));

	m_vPos = &m_matWorld.m[INFO_POS][0];

	return 0;

}

void CTransform::LateUpdateComponent()
{
	__super::LateUpdateComponent();

	for (_uint i = 0; i < INFO_END; ++i)
		memcpy(&m_matWorld.m[i][0], &m_vInfo[i], sizeof(_vec3));

	m_vPos = &m_matWorld.m[INFO_POS][0];
}
