#pragma once
#include "Monster.h"

class CBrigandFusilier : public CMonster
{
public:
	explicit CBrigandFusilier(LPDIRECT3DDEVICE9 pGraphicDev);
	CBrigandFusilier(const CCreature& rhs);
	virtual ~CBrigandFusilier();

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

