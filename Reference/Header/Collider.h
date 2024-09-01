#pragma once

#include"Component.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CCollider : public CComponent
{
public:
	explicit CCollider();
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev, ECollideID _eCollideID);
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev, ECollideID _eCollideID, _vec3* _vPos, const _vec3& _vScale);
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev, ECollideID _eCollideID, _vec3* _vPos, const _float& _fRadius);
	~CCollider();

public:

	ECollideID GetType() { return m_eCollideID; }
	const _vec3* GetPos() { return m_vPos; }
	const _vec3* GetScale() { return &m_vScale; }
	const _vec3* GetAngle() { return &m_vAngle; }
	const _float GetRadius() { return m_fRadius; }

	void SetType(ECollideID _eCollideID) { m_eCollideID = _eCollideID; }
	void SetPos(_vec3* _vPos) { m_vPos = _vPos; }
	void SetScale(const _vec3 _vScale) { m_vScale = _vScale; }
	void SetAngle(const _vec3 _vAngle) { m_vAngle = _vAngle; }
	void SetRadius(const _float _fRadius) {	m_fRadius = _fRadius; }

	void SetVisible(_bool _bVisible) { m_bVisible = _bVisible; }
	_bool GetVisible() { return m_bVisible; }
	
	void GetMinMax(_vec3& vMin, _vec3& vMax);
	void GetSphere(_vec3& _vCenter, _float& _fRadius);

public:
	virtual	void	RenderCollider(void);

private:

	ECollideID m_eCollideID;

	_vec3* m_vPos;
	_vec3 m_vScale;
	_vec3 m_vAngle;

	_float m_fRadius;
	_bool	m_bVisible = false;

	_vec3 m_vPoint[2][9];
	ID3DXLine* m_pLine;
};

END