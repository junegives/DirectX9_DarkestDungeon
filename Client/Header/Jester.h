#pragma once
#include "Hero.h"

class CJester : public CHero
{
public:
	explicit CJester(LPDIRECT3DDEVICE9 pGraphicDev);
	CJester(const CJester& rhs);
	virtual ~CJester();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:
	virtual void AddComponent() override;
	virtual void ChangeAnim() override;
};

