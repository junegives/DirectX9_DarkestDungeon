#pragma once

#include"GameObject.h"
#include "Skill.h"

#include"SoundMgr.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;
class CEffect;

END

class CStatView;

// Creature ���� ����
typedef struct tagCreatureCommonStat
{
	_int		iHp;			// ü��
	_int		iMaxHp;			// �ִ�ü��
	_int		iDodge;			// ȸ����
	_int		iSpeed;			// �ӵ�
	_int		iAttackPower;	// �������ݷ�(���� �������� ��ų�� ���� ���ݷ� ����� ��)
	_int		iOrder;			// ����(�߽ɿ������� 0~3)
}STAT;

enum class EAnimState
{
	// ������
	IDLE,
	WALK,

	// ����
	COMBAT,			// ���� �� IDLE
	BESHOT,			// �ǰ�
	DEATH,			// ���

	// ���Ϳ�
	CORPSE,			// ��ü

	// ��ų	
	SKILL,
	SKILL1,
	SKILL2,
	SKILL3,
	SKILL4,
	SKILL5,
	SKILL6,
	SKILL7,

	// ������
	AFFLICTION,		// �ر�
	VIRTUE,			// ���
	ENUM_END
};

class CCreature : public CGameObject
{
public:
	explicit CCreature(LPDIRECT3DDEVICE9 pGraphicDev);

	// ��ų���� ������ ��
	CCreature(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat, _int _iPosition,
		vector<shared_ptr<CSkill>>& _pVecSkill);

	// ���� ��ġ�� ��ų�� ���߿� ������ ��	
	CCreature(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat);

	CCreature(const CCreature& rhs);
	virtual ~CCreature();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:
	// �ε����� ��ų �ҷ����� (���� �� �غ��� �ҵ�,, ��ų�� �ǳʶٰ� ������ �� �־)
	shared_ptr<CSkill> GetSkill(_int _iIdx) { return m_pVecSkill[_iIdx]; }
	// �̸����� ��ų �ҷ����� (�ϴ� �̰� ���)
	shared_ptr<CSkill> GetSkill(tstring _strSkillName);

	//HRESULT	SetSkill(shared_ptr<vector<shared_ptr<CSkill>>> _pSkill);	// ��ų vector�� �� ���� �־��ֱ�
	void	SetSkill(vector<shared_ptr<CSkill>>& _vVec)
	{
		m_pVecSkill = _vVec;
	}

	_int	GetSkillNum() { return (_int)size(m_pVecSkill); }

	_bool	GetMoving() { return m_bMoving; }
	void	SetMoving(_bool _bMoving) { m_bMoving = _bMoving; }

	_bool	GetPicked() { return m_bPicked; }
	void	SetPicked(_bool _bPicked) { m_bPicked = _bPicked; }

	_bool	GetAttackMoving() { return m_bAttackMoving; }
	void	SetAttackMoving(_bool _bMoving) { m_bAttackMoving = _bMoving; }

	_float	GetMovingSpeed() { return m_fMovingSpeed; }
	void	SetMovingSpeed(_float _bMovingSpeed) { m_fMovingSpeed = _bMovingSpeed; }

	_vec3	GetTargetPos() { return m_vTargetPos; }
	void	SetTargetPos(_vec3 _vTargetPos) { m_vTargetPos = _vTargetPos; }

	_vec3	GetTargetPos2() { return m_vTargetPos2; }
	void	SetTargetPos2(_vec3 _vTargetPos) { m_vTargetPos2 = _vTargetPos; }

	STAT	GetCommonStat() { return m_tCommonStat; }
	void	SetCommonStat(STAT _tStat) { m_tCommonStat = _tStat; }

	_int	GetPosition() { return m_iPosition; }
	void	SetPosition(_int _iPosition) { m_iPosition = _iPosition; }

	_int	GetSize() { return m_iSize; }
	void	SetSize(_int _iSize) { m_iSize = _iSize; }

	_bool	GetHitted() { return m_bHitted; }
	void	SetHitted(_bool _bHitted) { m_bHitted = _bHitted; }
	void	SetEffectOn(_bool _bEffectOn) { m_bEffectOn = _bEffectOn; }
	_bool	GetAttacking(_int _iIndex) { return m_bAttacking[_iIndex]; }
	void	SetAttacking(_bool _bAttacking, _int _iIndex) { m_bAttacking[_iIndex] = _bAttacking; }
	_bool	IsAttacking();	// �����ϰ� �ִ���

	_bool	GetIsStun() { return m_bState[2]; }
	_bool	GetIsBlight() { return m_bState[0]; }
	_bool	GetIsBleed() { return m_bState[1]; }
	_bool	GetIsCorpse() { return m_bCorpse; }
	_bool	GetIsDeath() { return m_bDeath; }
	_bool	GetIsBeforeDeath() { return m_bBeforeDeath; }
	void	SetDeath(_bool _bDeath) { m_bDeath = _bDeath; }
	void	SetBeforeDeath(_bool _bBeforeDeath) { m_bBeforeDeath = _bBeforeDeath; }
	void	SetBlight(_bool _bBlight) { m_bState[0] = true; }
	void	SetBleed(_bool _bBleed) { m_bState[1] = true; }
	void	SetStun(_bool _bStun);
	void	SetBuff(_bool _bBuff) { m_bState3[0] = true; }
	void	SetDeBuff(_bool _bDeBuff) { m_bState3[1] = true; }
	void	SetStartBarOn(_bool _bStatBarOn) { bStatBarOn = _bStatBarOn; }

	_int	GetMaxHp() { return m_tCommonStat.iMaxHp; }
	void	SetFront(_bool _bFront) { m_bFront = _bFront; }

	_int	GetHp() { return m_tCommonStat.iHp; }
	void	SetHp(_int _iValue) { m_tCommonStat.iHp = _iValue; }
	void	IncreaseHP(_int _iValue, _bool _bStressEvent = false);
	virtual void	DecreaseHP(_int _iValue, _bool _bStressEvent = false);

	_int	GetCurrentBleed() { return m_iBleedDot[0]; }
	_int	GetCurrentPoision() { return m_iBlightDot[0]; }

	_int	GetDodge() { return m_tCommonStat.iDodge; }
	void	SetDodge(_int _iValue) { m_tCommonStat.iDodge = _iValue; }

	_int	GetSpeed() { return m_tCommonStat.iSpeed; }
	void	SetSpeed(_int _iValue) { m_tCommonStat.iSpeed = _iValue; }

	_int	GetAttackPower() { return m_tCommonStat.iAttackPower; }
	void	SetAttackPower(_int _iValue) { m_tCommonStat.iAttackPower = _iValue; }

	_int	GetOrder() { return m_tCommonStat.iOrder; }
	void	SetOrder(_int _iValue) { m_tCommonStat.iOrder = _iValue; }

	const _tchar* Get_String() const { return m_szString; }
	const _tchar* Get_String2() const { return m_szString2; }
	const _tchar* Get_String3() const { return m_szString3; }

	void	StartCalculate(_bool _bAutoEffect, _int& _iValue);
	void	StartCalcBleed(_int& _iValue);
	void	StartCalcBlight(_int& _iValue);
	void	StartCalcStun(_int& _iValue);
	void	StartCalcBuff(_int& _iValue);

	tstring GetSoundKey() { return m_strSoundKey; }
	void	SetSoundKey(tstring _strSoundKey) { m_strSoundKey = _strSoundKey; }


	// ������??
	_bool	GetDone() { return m_bDone; }
	_bool	GetAbleAct() { return m_bAbleAct; }
	_bool	GetTurn() { return m_bMyTurn; }
	void	SetTurn(_bool _bTurn) { m_bMyTurn = _bTurn; }
	void	SetDone(_bool _bDone) { m_bDone = _bDone; }
	void	SetAbleAct(_bool _bAbleAct) { m_bAbleAct = _bAbleAct; }

	_bool	GetPrevStun() { return m_bPrevStun; }
	void	SetPrevStun(_bool _bPrevStun) { m_bPrevStun = _bPrevStun; }

	// ���ݴ��ϴ¾�, �����ϴ¾�, ��ų
	virtual void	AttackCreature(shared_ptr<CCreature> _pCreature, shared_ptr<CCreature> _pCreature2, shared_ptr<CSkill> _pSkill);
	void	MovePos(_vec3 _vPos, const _float& fTimeDelta, _float _fSpeed);
	_float	MovingSpeed(_vec3 _vPos, _float _fMovingTime);
	void	OffTurnCursor();			// �ڱ��������� ��Ÿ���� cursor ����
	void	OnTurnCursor();				// �ڱ��������� ��Ÿ���� cursor Ű��
	void	OnTargetTeamCursor();		// ���� target Ű��
	void	OffTargetTeamCursor();		// ���� target ����
	void	OnTargetTeamsCursor();		// ������ target Ű��
	void	OffTargetTeamsCursor();		// ������ target ����
	void	OnTargetEnemyCursor();		// �� target Ű��
	void	OffTargetEnemyCursor();		// �� target ����
	void	OnTargetEnemiesCursor();	// ���� target Ű��
	void	OffTargetEnemiesCursor();	// ���� target ����
	void	OnTurnUi();					// �� ui Ű��
	void	OffTurnUi();				// �� ui ����
	void	OnVirtue();					// ��� Ű��
	void	OffVirtue();				// ��� ����
	void	OnAffliction();				// �ر� Ű��
	void	OffAffliction();			// �ر� ����

	_bool	IsIncreaseStress() { return m_bGetStress; }
	void	SetIncreaseStress(_bool _bGetStress) { m_bGetStress = _bGetStress; }

public:

	// �ٸ� ũ��ó ���ݽ�

	// ���� �����
	virtual void	EndAttack(shared_ptr<CGameObject> _pCreature);

	// ���� ����
	virtual void	Buff1Skill(_int* _iDotBuff);	// ��ܿ� ����
	virtual void	Buff2Skill(_int* _iDotBuff);	// ȸ�� ����
	virtual void	DeBuffSkill(_int* _iDotDeBuff);	// ȸ�� �����

	virtual void	CurioBuff2();		// ��ǰ ȸ�� ����

	// ���� �ʱ�ȭ
	virtual void	Buff1Reset();					// ��ܿ� ���� �ʱ�ȭ
	virtual void	BuffReset();					// ���� �ʱ�ȭ

	// �ߵ� ����
	virtual void	BlightAttack(_int* _iDotDam);

	// ���� ����
	virtual void	BleedAttack(_int* _iDotDam);

	// �ߵ� ġ��
	virtual void	BlightCure();

	// ���� ġ��
	virtual void	BleedCure();

	// ����Ʈ
	void				SetEffectInfo(shared_ptr<CSkill> _pSkill, _bool _bTarget, _bool _bDodge);

	void UpdateAttribute();

	// ���� ����Ʈ ��������
	shared_ptr<CEffect> GetLoopEffect() { return m_pLoopEffect; }

protected:
	virtual void			AddComponent();
	virtual void			ClimbingTerrain();

	//���� ���� ����
	void SettingStatBar();

protected:
	// Creature Common Component
	// ��ü ���� Component
	shared_ptr<CTransform> m_pTransformCom = nullptr;
	shared_ptr<CRcTex> m_pBufCom = nullptr;
	shared_ptr<CAnimator> m_pTextureCom = nullptr;

	// ��ü �ǰ�, Ÿ�� ����Ʈ ���� Component
	shared_ptr<CTransform> m_pEffectTransformCom = nullptr;
	shared_ptr<CRcTex> m_pEffectBufCom = nullptr;
	shared_ptr<CAnimator> m_pEffectTextureCom = nullptr;

	shared_ptr<CEffect> m_pLoopEffect = nullptr;

protected:
	_bool		m_bIsHero = false;
	STAT		m_tCommonStat;				// ����

	_int		m_iPosition = 0;			// ��ġ (0~3)

	_bool		m_bDeath = false;			// ��� ����(���ʹ� ��ü���� �Ҹ��Ҷ�, ������ ����Ҷ�)
	_bool		m_bCorpse = false;			// ������ ��ü����
	_bool		m_bBeforeDeath = false;		// ������ ������ �ϰ� ����
	_bool		m_bState[4] = { 0 };	// ������� �ߵ�, ����, ����, ��ü ����
	_bool		m_bState3[2] = {0};		// ������� ����, �����

	_bool		m_bPrevStun = false;

	_int		m_iBlightDot[4] = { 0 };	// �ϸ��� �ߵ� ��Ʈ��
	_int		m_iBleedDot[4] = { 0 };		// �ϸ��� ���� ��Ʈ��

	_int		m_iBuff1Dot[4] = { 0 };		// ��ܿ� ����(���� ����)
	_int		m_iBuff2Dot[4] = { 0 };		// ȸ�� ����
	_int		m_iDeBuff1Dot[4] = { 0 };	// ȸ�� �����

	_vec3		PrevTeleportPos = _vec3(0.f,0.f,0.f);
	_vec3		CurrentTeleportPos = _vec3(0.f, 0.f, 0.f);

	_bool		m_bHitted = false;			// �ڽ��� �ǰݻ��� ����
	_bool		m_bEffectOn = false;		// ����Ʈ ����
	// m_bHitted && m_bEffectOn -> �ǰ� �ִϸ��̼� on
	// !m_bHitted && m_bEffectOn -> Ÿ�� �ִϸ��̼� on
	// ����) ���� ���ݽ� ������ ����Ʈ ���, ���� �ǰ� ����Ʈ ���
	_bool		m_bGetStress = false;		// ��Ʈ������ ���� ���

	vector<shared_ptr<CSkill>>	m_pVecSkill;	// ��ų

	EAnimState m_eAnimState = EAnimState::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	EAnimState m_eCurAnimState = EAnimState::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	EAnimState m_ePrevAnimState = EAnimState::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	tstring		m_strAnimKey = L"";				// �ִϸ��̼� Ű
	tstring		m_strEffectKey = L"";			// ����Ʈ Ű
	tstring		m_strSoundKey = L"";			// ���� Ű

	// ������??
	_bool		m_bMyTurn = false;			// �ڽ��� �� ���� 
	_bool		m_bDone = false;			// �̹� �Ͽ� �ൿ �ߴ��� ����
	_bool		m_bAbleAct = true;			// �̹� �Ͽ� �ൿ �������� ����(�����̳� ��ü������� �Ұ���)

	// ��ų �����
	_bool		m_bAttacking[7] = { 0 };

	_int		m_iSize = 1;	// ĳ���� ũ��

	_tchar		m_szString[128];
	_tchar		m_szString2[128];
	_tchar		m_szString3[128];

	_bool		m_bMoving = false;			// �¾Ƽ� �����̴���(���, ���� ��)
	_bool		m_bAttackMoving = false;	// ���ݽ�ų�� ���� �����̴���
	_vec3		m_vTargetPos = { 0, 0, 0 };		// ������ 1
	_vec3		m_vTargetPos2 = { 0, 0, 0 };	// ������ 2
	_float		m_fMovingSpeed = 0.f;

	//���� ǥ��â
	shared_ptr<CStatView> m_pStatInfo = nullptr;
	//����Info ON/Off
	_bool		bStatBarOn = true;

	// �÷��̾� ������ ������
	_bool		m_bFront = true;

	// ��ŷ�ƴ��� �ȵƴ���
	_bool		m_bPicked = false;

	tstring		m_strName = L"";

	// �ر�/��� �̺�Ʈ �߻� �� ��Ʈ�� ����� ������ (�ٸ� ����Ʈ�� �Ȱ�ġ�� ���ؼ� ���� ��)
	_vec3* m_vStressEventPos;


protected:
	virtual void	Free();
};
