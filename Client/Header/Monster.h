#pragma once
#include "Creature.h"

class CMonster : public CCreature
{
public:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);

	// 스킬까지 세팅할 때
	CMonster(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat, _int _iPosition,
		vector<shared_ptr<CSkill>>& _pVecSkill);

	// 전투 위치와 스킬은 나중에 세팅할 때	
	CMonster(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat);

	CMonster(const CCreature& rhs);
	virtual ~CMonster();

protected:
	virtual void		PickingObj();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

protected:
	virtual void	Free();
};

