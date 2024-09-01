#pragma once
#include"GameObject.h"

class CBattleSystem;

class CDungeonRoom
{
public:
	explicit CDungeonRoom();
	virtual ~CDungeonRoom();

public:
	vector<shared_ptr<CGameObject>>	GetGameObjectVector() { return m_vGameObject; }
	vector<shared_ptr<CGameObject>>	GetHeroesVector() { return m_vHeroes; }
	vector<shared_ptr<CGameObject>>	GetMonstersVector() { return m_vMonsters; }
	shared_ptr<CBattleSystem>		GetBattleSystem() { return m_pBattleSystem; }
	void	SetBattleSystem(shared_ptr<CBattleSystem> _pBattleSystem) { m_pBattleSystem = _pBattleSystem; }
	void	SetBattleTrigger(_bool _bTrigger) { m_bBattleTrigger = _bTrigger; }
	_bool	GetBattleTrigger() { return m_bBattleTrigger; }

public:
	void	PushGameObjectVector(vector<shared_ptr<CGameObject>>& _vVec)
	{
		m_vGameObject = _vVec;
	}
	void	PushHeroesVector(vector<shared_ptr<CGameObject>>& _vVec)
	{
		m_vHeroes = _vVec;
	}
	void	PushMonstersVector(vector<shared_ptr<CGameObject>>& _vVec)
	{
		m_vMonsters = _vVec;
	}

	// ������ ī�޶� ��ġ ��ȯ �Լ�
	_vec3	GetBattleCameraPos() { return m_vBattleCameraPos; }
	void	SetBattleCameraPos(_vec3 _vBattleCameraPos) { m_vBattleCameraPos = _vBattleCameraPos; }

	_vec3	GetBattleCameraOriginPos() { return m_vBattleCameraOriginPos; }
	void	SetBattleCameraOriginPos(_vec3 _vBattleCameraPos) { m_vBattleCameraOriginPos = _vBattleCameraPos; }

	_bool	GetBattleStart() { return m_bBattleStart; }
	void	SetBattleStart(_bool _bBattleStart) { m_bBattleStart = _bBattleStart; }
	_bool	GetBattleDone() { return m_bBattleDone; }
	void	SetBattleDone(_bool _bBattleDone) { m_bBattleDone = _bBattleDone; }
	
public:
	void	MakeDisableAllObject();
	void	MakeAbleAllObject();
	_bool	BattleUpdate(const _float& fTimeDelta);		// �ο�
	void	BattleReady();	// �ο� ���� �ڸ�����ֱ�

private:
	vector<shared_ptr<CGameObject>>		m_vGameObject = {};
	vector<shared_ptr<CGameObject>>		m_vHeroes = {};
	vector<shared_ptr<CGameObject>>		m_vMonsters = {};
	shared_ptr<CBattleSystem>			m_pBattleSystem = nullptr;

	_vec3	m_vBattleCameraPos;		// ���� �� �ο� ��ġ
	_vec3	m_vBattleCameraOriginPos; //���� �� ī�޶��� ����ġ(Eye ��ġ)
	_bool	m_bBattleDone = false;	// ���� �� �� �ο��� ��������
	_bool	m_bBattleStart = false;	// ���� �� �� �ο��� ���۵ƴ���
	_bool	m_bBattleTrigger = false;
};

