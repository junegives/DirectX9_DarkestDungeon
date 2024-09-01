#pragma once

#include "Engine_Define.h"
#include "Effect.h"

#define	POOLSIZE 50

BEGIN(Engine)

class ENGINE_DLL CEffectMgr
{
	DECLARE_SINGLETON(CEffectMgr)
public:
	explicit CEffectMgr();
	virtual ~CEffectMgr();

public:
	int		Update(const float& _fDeltaTime);
	void	LateUpdate();
	void	Render(LPDIRECT3DDEVICE9 _pGraphicDev);

public:
	void	SetEffectMgr(LPDIRECT3DDEVICE9 _pGraphicDev);

public:
	shared_ptr<CEffect> GetEffect();
	void	ReturnEffect(shared_ptr<CEffect> _pEffect);

private:
	vector<shared_ptr<CEffect>> m_pEffectList;
	LPDIRECT3DDEVICE9	m_pGraphicDev;
};

END