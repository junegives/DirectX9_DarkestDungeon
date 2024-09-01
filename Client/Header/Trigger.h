#pragma once
#include "EnvironmentObj.h"

class CTrigger : public CEnvironmentObj
{
public:
	explicit	CTrigger(LPDIRECT3DDEVICE9 pGraphicDev, const tstring& _KeyName, _bool _bColliding = false, _bool _bBillBoard = false, _int _iCnt = 1, _bool _bRandom = false);
	virtual		~CTrigger();

public:
	virtual	HRESULT		ReadyGameObject() override;
	virtual	_int		UpdateGameObject(const _float& fTimeDelta) override;
	virtual	void		LateUpdateGameObject() override;
	virtual	void		RenderGameObject() override;

	virtual void		OnCollide(shared_ptr<CGameObject> _pObj) override;
};