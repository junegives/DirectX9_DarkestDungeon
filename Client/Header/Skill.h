#pragma once

#include "GameObject.h"

class CSkill : public CGameObject
{
public:
	// 영웅 스킬 용
	// (스킬 이름, 스킬 애니메이션, 스킬 이미지, 이펙트키, 활성 위치, 타겟 위치, 공격종류(단순 공격, 중독, 출혈, 기절, 이동, 힐), 
	//	각 공격 타겟이 적인지 아닌지, 도트뎀[데미지][라운드], 스킬 사용 거리, 스턴 확률, 공격력 계수, 치명타 확률, 라운드 당 사용 가능 횟수, 이동량, 힐량, 타겟 대상을 모두 공격하는지)
	CSkill(tstring _strSkillName, tstring _strAnimKey, tstring _strImgKey, tstring _strEffectKey, tstring _strTargetEffectKey, tstring _strSoundKey,
		_bool* _arrActivatePos, _bool* _arrTargetPos, _bool* _bArrAttack, _bool* _bArrToEnemy,
		_int* _iDotDamage = nullptr, _float _fDistance = 0.f ,_float _fStunRatio = 1.f, _float _fDamageRatio = 1.f,
		_float _fCriticalRatio = 1.f, _int _iActivateCnt = -1, _int _iMoveCnt = 0, _int _iHeal = 0, _bool _bTargetAll = 0, _bool _bApproach = 0, _int _iSkillMoveCnt = 0 );

	// 몬스터 스킬 용
	// (스킬 이름, 스킬 애니메이션(크리처의 애니메이션), 도트뎀[데미지][라운드], 스킬 사용거리(근접 공격시 애니메이션에 맞춰야해서 넣음)
	//    스턴 확률, 공격력 계수, 치명타 확률
	//    공격종류, 영웅을 움직이게 할 칸수, 영웅에게 줄 스트레스)
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

	// 스킬 이름
	tstring		m_strSkillName;

	// 발동 위치
	_bool		m_arrActivatePos[4] = { true };
	// 타겟 위치
	_bool		m_arrTargetPos[4] = { true };

	// 한 전투 당 사용 가능 횟수 (-1일 시 무제한)
	_int		m_iActivateCnt = -1;

	// 능력 개수
	_bool		m_bArrAttack[8] = { false };		// 단순 공격, 중독, 출혈, 기절, 이동, 힐, 버프, 디버프

	// 0: 대단원 버프, 1: 대단원 극버프, 2: 대단원 버프 초기화
	// 3: 회피 버프, 4: 회피 디버프
	_bool		m_bArrToEnemy[6] = { true };

	// 타겟 포지션 모두 공격
	_bool		m_bTargetAll = false;

	// 장착 여부
	_bool		m_bEquipped = true;

	// 해금 여부
	_bool		m_bUnlocked = true;

	// 스킬 레벨
	_int		m_iLevel = 1;

	// 다가가며 하는 공격인지 아닌지
	_bool		m_bApproach = false;

	// =========스킬값==========
	// 맞은 애의 이동량
	_int		m_iMoveCnt;
	// 스킬 사용한 애의 이동량
	_int		m_iMovingCnt;
	// 중독or출혈 데미지, 라운드
	_int		m_iDotDamage[2] = { 0 };
	// 힐량
	_int		m_iHeal;
	// 스트레스
	_int		m_iStress = 0;

	// =======퍼센트 값=======
	// 기절 확률
	_float		m_fStunRatio = 1.f;
	// 공격력 계수
	_float		m_fDamageRatio;
	// 치명타 계수
	_float		m_fCriticalRatio;

	// 크리처 애니메이션 키
	tstring		m_strAnimKey;
	// 이펙트 애니메이션 키
	tstring		m_strEffectAnimKey = L"";
	// 타겟 이펙트 애니메이션 키
	tstring		m_strTargetEffectAnimKey = L"";
	// 스킬 이미지 키
	tstring		m_strImgKey = L"";
	// 사운드 키
	tstring		m_strSoundKey = L"";

	//스킬 사용시 적으로부터 띄워줘야하는 거리
	_float		m_fSkillDistance = 0.f;
};