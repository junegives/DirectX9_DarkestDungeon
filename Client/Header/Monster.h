#pragma once
#include "Creature.h"

class CMonster : public CCreature
{
public:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);

	// ��ų���� ������ ��
	CMonster(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat, _int _iPosition,
		vector<shared_ptr<CSkill>>& _pVecSkill);

	// ���� ��ġ�� ��ų�� ���߿� ������ ��	
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

