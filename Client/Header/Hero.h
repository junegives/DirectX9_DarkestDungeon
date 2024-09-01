#pragma once
#include "Creature.h"
#include "Skill.h"

class CHeroStat;
class CTavernUI;

// �ر� (75%)
enum class EAffliction
{
	// �̱���
	// ������ ���� �� �Ʊ��鿡�� ��Ʈ����
	// �Ʊ��� ���� �ý� �ش� �Ʊ��� ����ϸ� �ش� �Ʊ���Ʈ����
	SELFISH,

	// ���̼���
	// ȸ�� �ź�
	// ����
	IRRATIONAL,

	ENUM_END
};

// ��� (25%)
enum class EVirtue
{
	// �游�� ���
	// ���� �� ������ ��� ��Ƽ���� ��Ʈ������ 4�� ����
	COURAGEOUS,

	// ���� �ռ�
	// �� ���� �� �ڽ��� �ִ� ü���� 10% ȸ��
	VIGOROUS,

	ENUM_END
};

class CHero : public CCreature
{
public:
	explicit CHero(LPDIRECT3DDEVICE9 pGraphicDev);

	// ���� ������ ��
	CHero(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat, _int _iPosition,
		vector<shared_ptr<CSkill>>& _pVecSkill, _float _fCritical);

	// ���� ��ġ, ��ų�� ���߿� ������ ��	
	CHero(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat);

	CHero(const CHero& rhs);
	virtual ~CHero();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:
	vector<shared_ptr<CSkill>>* GetSkillVector() { return &m_pVecSkill; }
	shared_ptr<CSkill>		SelectSkill(_int _iSkillID);
	void					ClearSkillVector() { m_pVecSkill.clear(); }

	vector<shared_ptr<CSkill>>* GetAllSkillVector() { return &m_pVecSkillAll; }

	virtual	void			StressUp();

	// ������ �� ��ų�� ������ �����ϴ� �Լ�
	virtual void			SwapSkill(_int _iIdx1, _int _iIdx2);

	void					GetHeroStatusEvent(_bool _bHpEvent);
	void					GetHeroBuffEvent();

protected:
	virtual void		AddComponent();
	virtual void		SetAnimState(EAnimState _eAnimState) { m_eCurAnimState = _eAnimState; }
	virtual void		ChangeAnimState();
	virtual void		SetAnimDelay(const _float& fTimeDelta);
	virtual void		ChangeAnim() {}

	// ���� �� ���۽ÿ� ȣ���ؼ� �������ֱ�
	virtual void		StressEvent();

	virtual void		PickingObj();

public:
	const _tchar* Get_String4() const { return m_szString4; }

	_int		GetStress() { return m_iStress; }
	void		SetStress(_int _iValue) { m_iStress = _iValue; }
	void		IncreaseStress(_int _iValue, _bool _bStressEvent = false);
	void		DecreaseStress(_int _iValue, _bool _bStressEvent = false);

	virtual void DecreaseHP(_int _iValue, _bool _bStressEvent = false);

	_float		GetCritical() { return m_fCritical; }
	void		SetCritical(_float _fValue) { m_fCritical = _fValue; }
	void		ChangeCritical(_float _fValue) { m_fCritical *= _fValue; }

	_bool		IsDeathDoor() { return m_bDeathDoor; }
	void		SetDeathDoor(_bool _bDeathDoor) { m_bDeathDoor = _bDeathDoor; }

	_bool		GetStressEvent() { return m_bStressEvent; }
	void		SetStressEvent(_bool _bStressEvent) { m_bStressEvent = _bStressEvent;}

	_bool		IsAffliction() { return m_bAffliction; }
	void		SetAffliction(_bool _bAffliction) { m_bAffliction = _bAffliction; }
	void		OnAffliction();

	_bool		IsVirtue() { return m_bVirtue; }
	void		SetVirtue(_bool _bVirtue) { m_bVirtue = _bVirtue; }
	void		OnVirtue();

	EHeroType	GetHeroType() { return m_eHeroType; }

	void		SetHired(_bool _bHired) { m_bHired = _bHired; }
	_bool		IsHired() { return m_bHired; }

	void		SetForSelect(_bool _bForSelect) { m_bForSelect = _bForSelect; }
	_bool		IsForSelect() { return m_bForSelect; }

	void		SetSelected(_bool _bSelected) { m_bSelected = _bSelected; }
	_bool		IsSelected() { return m_bSelected; }

	void		SetObjKey(tstring _objKey) { m_strObjKey = _objKey; }
	tstring		GetObjKey() { return m_strObjKey; }

	void		SetInDungeon(_bool _bInDungeon) { m_bInDungeon = _bInDungeon; }
	_bool		GetInDungeon() { return m_bInDungeon; }

	EAffliction	GetAffliction() { return m_eAffliction; }
	EVirtue		GetVirtue() { return m_eVirtue; }

	void		SetCheck(_bool	_bCheck) { m_bCheck = _bCheck; }
	_bool		GetCheck() { return m_bCheck; }

protected:

	_int				m_iStress = 0;		// ��Ʈ����
	_float				m_fCritical;		// ġ��Ÿ Ȯ��

	_bool				m_bDeathDoor = false;		// ������ ���� ����

	_bool				m_bStressEvent = false;		// ��Ʈ���� �̺�Ʈ �߻� ����

	_bool				m_bAffliction = false;		// �ر� ����
	EAffliction			m_eAffliction = EAffliction::ENUM_END;

	_bool				m_bVirtue = false;			// ��� ����
	EVirtue				m_eVirtue = EVirtue::ENUM_END;

	_bool				m_bDelay = false;			// UI ��� �ð� �� (�ִϸ��̼��� ������ ������ ��ٷ����ϴ� �ð�)

	// �����̸� ����� ���� ���°���
	_float				m_fHittedTime = HITTEDTIME;
	_float				m_fAttackTime = ATTACKTIME;
	_float				m_fChangeTime = CHANGETIME;
	_float				m_fDelayTime = DELAYTIME;

	_bool				m_bInDungeon = true;

	_bool				m_bAttacking1 = false;
	_bool				m_bAttacking2 = false;
	_bool				m_bAttacking3 = false;
	_bool				m_bAttacking4 = false;

	_bool				m_bStressChanged = false;

	EHeroType			m_eHeroType = EHeroType::ENUM_END;

	shared_ptr<CHeroStat> m_pStatUI = nullptr;

	// ���� Info ON/Off
	_bool		m_bShowStatUI = false;
	_bool		m_bHired = false;
	_bool		m_bForSelect = false;
	_bool		m_bSelected = false;

	//shared_ptr<vector<shared_ptr<tstring>>>	m_pVecAnimKey;

	_tchar		m_szString4[128];

	tstring		m_strObjKey;

	// �������� �ʰų� ������� ���� ���� ��ų�� ������ �ִ� ����
	vector<shared_ptr<CSkill>>	m_pVecSkillAll;

	_bool		m_bCheck = false;

protected:
	_bool		m_bReady = false;

private:
	virtual void	Free();
};