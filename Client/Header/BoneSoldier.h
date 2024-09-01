#pragma once
#include "Monster.h"

class CBoneSoldier : public CMonster
{
public:
	explicit CBoneSoldier(LPDIRECT3DDEVICE9 pGraphicDev);
	CBoneSoldier(const CCreature& rhs);
	virtual ~CBoneSoldier();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

private:
	virtual void			AddComponent();

private:
	virtual void	Free();

private:
	_float		m_fHittedTime = HITTEDTIME;
	_float		m_fAttack1Time = ATTACKTIME;
};

