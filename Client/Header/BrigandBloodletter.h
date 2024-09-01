#pragma once
#include "Monster.h"

class CBrigandBloodletter : public CMonster
{
public:
	explicit CBrigandBloodletter(LPDIRECT3DDEVICE9 pGraphicDev);
	CBrigandBloodletter(const CCreature& rhs);
	virtual ~CBrigandBloodletter();

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
	_float		m_fAttack2Time = ATTACKTIME;
};

