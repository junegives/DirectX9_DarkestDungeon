#pragma once

#include"Component.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CTransform : public CComponent 
{
public:

	explicit CTransform();
	explicit CTransform(const _vec3& _vPos);
	explicit CTransform(const _vec3& _vPos, const _vec3& _vScale);
	explicit CTransform(const _vec3& _vPos, const _vec3& _vScale, const _vec3& _vRotation);
	~CTransform();

public:

	void GetInfo(INFO _eType, _vec3* _pInfo) {
		memcpy(_pInfo, &m_matWorld.m[_eType][0], sizeof(_vec3));
	}

	_vec3* GetPos() { return &m_vPos; }
	const _vec3* GetAngles() { return &m_vAngle; }
	const _vec3* GetScale() { return &m_vScale; }
	const _matrix* GetWorld() { return &m_matWorld; }

	void SetAngle(_vec3 _vAngle) { m_vAngle = _vAngle; }


public:

	void SetPosition(const _float& _fX, const _float& _fY, const _float& _fZ)
	{
		m_vInfo[INFO_POS].x = _fX;
		m_vInfo[INFO_POS].y = _fY;
		m_vInfo[INFO_POS].z = _fZ;

		m_vPos.x = _fX;
		m_vPos.y = _fY;
		m_vPos.z = _fZ;

	}

	void SetScale(const _float& _fX, const _float& _fY, const _float& _fZ)
	{
		m_vScale = { _fX, _fY, _fZ };
	}

	// _vOriginScale: OriginImage의 조정비율, _vOriginImage: OriginImage의 x,y길이, _vMoveImage: MoveImage의 x,y길이
	void SetScaleRatio(_vec3 _vOriginScale, _vec2 _vOriginImage, _vec2 _vMoveImage)
	{
		SetScale(_vOriginScale.x * _vMoveImage.x / _vOriginImage.x,
			_vOriginScale.y * _vMoveImage.y / _vOriginImage.y,
			_vOriginScale.z);
	}

	void Rotation(ROTATION eType, const _float& fAngle)
	{
		*(((_float*)&m_vAngle) + eType) += fAngle;
	}

	void SetWorld(const _matrix* _matWorld) { m_matWorld = *_matWorld; }


public:
	HRESULT			ReadyTransform();
	virtual			_int	UpdateComponent(const _float& fTimeDelta);
	virtual			void	LateUpdateComponent();

	void			MoveForward(const _vec3* pDir, const _float& fTimeDelta, const _float& fSpeed)
	{
		m_vInfo[INFO_POS] += *pDir * fTimeDelta * fSpeed;
	}


private:
	_vec3 m_vInfo[INFO_END];
	_vec3 m_vPos;
	_vec3 m_vScale;
	_vec3 m_vAngle;
	_matrix m_matWorld;

};

END