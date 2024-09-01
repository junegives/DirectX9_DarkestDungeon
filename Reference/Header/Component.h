#pragma once
#include"Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CComponent : public enable_shared_from_this<CComponent>
{
protected:
	explicit CComponent();
	explicit CComponent(LPDIRECT3DDEVICE9 _pGraphic);
	virtual ~CComponent();

public:
	virtual			_int	UpdateComponent(const _float& _fTimeDelta) { return 0; }
	virtual			void	LateUpdateComponent() {}

public:
	_bool GetIsActive() { return m_bActive; }
	_bool GetIsEnable() { return m_bEnable; }

public:

	void SetActive(_bool _bValue) { m_bActive = _bValue; }
	void SetEnable(_bool _bValue) { m_bEnable = _bValue; }

protected:

	_bool m_bActive = true; // ==IsDead
	_bool m_bEnable = true; // OnOff Switch

	LPDIRECT3DDEVICE9 m_pGraphicDev;

	static _vec3 m_vComponentVec;


};

END