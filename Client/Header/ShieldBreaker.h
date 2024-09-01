#pragma once
#include "Hero.h"
class CShieldBreaker : public CHero
{
public:
	explicit CShieldBreaker(LPDIRECT3DDEVICE9 pGraphicDev);
	CShieldBreaker(const CShieldBreaker& rhs);
	virtual ~CShieldBreaker();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:
	virtual void AddComponent() override;
	virtual void ChangeAnim() override;
};

