#pragma once
#include "Hero.h"
class CHighwayman : public CHero
{
public:
	explicit CHighwayman(LPDIRECT3DDEVICE9 pGraphicDev);
	CHighwayman(const CHighwayman& rhs);
	virtual ~CHighwayman();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:
	virtual void AddComponent() override;
	virtual void ChangeAnim() override;
};

