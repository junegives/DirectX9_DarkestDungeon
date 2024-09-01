#pragma once

#include "GameObject.h"

class CSkill : public CGameObject
{
public:
	// ���� ��ų ��
	// (��ų �̸�, ��ų �ִϸ��̼�, ��ų �̹���, ����ƮŰ, Ȱ�� ��ġ, Ÿ�� ��ġ, ��������(�ܼ� ����, �ߵ�, ����, ����, �̵�, ��), 
	//	�� ���� Ÿ���� ������ �ƴ���, ��Ʈ��[������][����], ��ų ��� �Ÿ�, ���� Ȯ��, ���ݷ� ���, ġ��Ÿ Ȯ��, ���� �� ��� ���� Ƚ��, �̵���, ����, Ÿ�� ����� ��� �����ϴ���)
	CSkill(tstring _strSkillName, tstring _strAnimKey, tstring _strImgKey, tstring _strEffectKey, tstring _strTargetEffectKey, tstring _strSoundKey,
		_bool* _arrActivatePos, _bool* _arrTargetPos, _bool* _bArrAttack, _bool* _bArrToEnemy,
		_int* _iDotDamage = nullptr, _float _fDistance = 0.f ,_float _fStunRatio = 1.f, _float _fDamageRatio = 1.f,
		_float _fCriticalRatio = 1.f, _int _iActivateCnt = -1, _int _iMoveCnt = 0, _int _iHeal = 0, _bool _bTargetAll = 0, _bool _bApproach = 0, _int _iSkillMoveCnt = 0 );

	// ���� ��ų ��
	// (��ų �̸�, ��ų �ִϸ��̼�(ũ��ó�� �ִϸ��̼�), ��Ʈ��[������][����], ��ų ���Ÿ�(���� ���ݽ� �ִϸ��̼ǿ� ������ؼ� ����)
	//    ���� Ȯ��, ���ݷ� ���, ġ��Ÿ Ȯ��
	//    ��������, ������ �����̰� �� ĭ��, �������� �� ��Ʈ����)
	CSkill(tstring _strSkillName, tstring _strAnimKey, _bool* _arrTargetPos, _int* _iDotDamage, _float _fDistance,
		_float _fStunRatio, _float _fDamageRatio, _float _fCriticalRatio,
		_bool* _bArrAttack, _int _iMoveCnt = 0, _int _iStress = 0, _bool _bTargetAll = 0, _bool _bApproach = 0, _int _iSkillMoveCnt = 0);

	CSkill(const CSkill& rhs);
	virtual ~CSkill();

public:
	_bool		IsEnable();


public:
	tstring		GetSkillName() { return m_strSkillName; }

	_bool		IsActivatePos(_int _idx) { return m_arrActivatePos[_idx]; }
	_bool		IsTargetPos(_int _idx) { return m_arrTargetPos[_idx]; }
	_bool*		GetActivatePos() { return m_arrActivatePos; }
	_bool*		GetTargetPos() { return m_arrTargetPos; }
	_bool*		GetArrToEnemy() { return m_bArrToEnemy; }
	_bool		IsTargetAll() { return m_bTargetAll; }
	_bool		CanActivateCnt() { if (0 == m_iActivateCnt) return false; return true; }
	void		DecreaseActivateCnt() { --m_iActivateCnt; }
	_bool*		GetArrAttack() { return m_bArrAttack; }

	_bool		IsApproach() { return m_bApproach; }

	_bool		IsEquipped() { return m_bEquipped; }
	void		SetEquipped(_bool _bEquipped) { m_bEquipped = _bEquipped; }

	_bool		IsUnlocked() { return m_bUnlocked; }
	void		SetUnlocked(_bool _bUnlocekd) { m_bUnlocked = _bUnlocekd; }

	_int		GetStress() { return m_iStress; }
	_int		GetLevel() { return m_iLevel; }
	void		UpgradeLevel() { m_iLevel++; }

	_int		GetMoveCnt() { return m_iMoveCnt; }
	_int		GetMovingCnt() { return m_iMovingCnt; }
	_int*		GetDotDamage() { return m_iDotDamage; }
	_int		GetHeal() { return m_iHeal; }
	_float		GetStunRatio() { return m_fStunRatio; }
	_float		GetDamageRatio() { return m_fDamageRatio; }
	_float		GetCriticalRatio() { return m_fCriticalRatio; }

	tstring		GetAnimKey() { return m_strAnimKey; }
	tstring		GetImgKey() { return m_strImgKey; }
	tstring		GetEffectAnimKey() { return m_strEffectAnimKey; }
	tstring		GetTargetEffectAnimKey() { return m_strTargetEffectAnimKey; }
	tstring		GetSoundKey() { return m_strSoundKey; }

	_float		GetSkillDistance() { return m_fSkillDistance; }

protected:

	// ��ų �̸�
	tstring		m_strSkillName;

	// �ߵ� ��ġ
	_bool		m_arrActivatePos[4] = { true };
	// Ÿ�� ��ġ
	_bool		m_arrTargetPos[4] = { true };

	// �� ���� �� ��� ���� Ƚ�� (-1�� �� ������)
	_int		m_iActivateCnt = -1;

	// �ɷ� ����
	_bool		m_bArrAttack[8] = { false };		// �ܼ� ����, �ߵ�, ����, ����, �̵�, ��, ����, �����

	// 0: ��ܿ� ����, 1: ��ܿ� �ع���, 2: ��ܿ� ���� �ʱ�ȭ
	// 3: ȸ�� ����, 4: ȸ�� �����
	_bool		m_bArrToEnemy[6] = { true };

	// Ÿ�� ������ ��� ����
	_bool		m_bTargetAll = false;

	// ���� ����
	_bool		m_bEquipped = true;

	// �ر� ����
	_bool		m_bUnlocked = true;

	// ��ų ����
	_int		m_iLevel = 1;

	// �ٰ����� �ϴ� �������� �ƴ���
	_bool		m_bApproach = false;

	// =========��ų��==========
	// ���� ���� �̵���
	_int		m_iMoveCnt;
	// ��ų ����� ���� �̵���
	_int		m_iMovingCnt;
	// �ߵ�or���� ������, ����
	_int		m_iDotDamage[2] = { 0 };
	// ����
	_int		m_iHeal;
	// ��Ʈ����
	_int		m_iStress = 0;

	// =======�ۼ�Ʈ ��=======
	// ���� Ȯ��
	_float		m_fStunRatio = 1.f;
	// ���ݷ� ���
	_float		m_fDamageRatio;
	// ġ��Ÿ ���
	_float		m_fCriticalRatio;

	// ũ��ó �ִϸ��̼� Ű
	tstring		m_strAnimKey;
	// ����Ʈ �ִϸ��̼� Ű
	tstring		m_strEffectAnimKey = L"";
	// Ÿ�� ����Ʈ �ִϸ��̼� Ű
	tstring		m_strTargetEffectAnimKey = L"";
	// ��ų �̹��� Ű
	tstring		m_strImgKey = L"";
	// ���� Ű
	tstring		m_strSoundKey = L"";

	//��ų ���� �����κ��� �������ϴ� �Ÿ�
	_float		m_fSkillDistance = 0.f;
};