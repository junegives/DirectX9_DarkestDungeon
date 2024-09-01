#include "pch.h"
#include "Skill.h"

CSkill::CSkill(tstring _strSkillName, tstring _strAnimKey, tstring _strImgKey, tstring _strEffectKey, tstring _strTargetEffectKey, tstring _strSoundKey,
	_bool* _arrActivatePos, _bool* _arrTargetPos, _bool* _bArrAttack, _bool* _bArrToEnemy,
	_int* _iDotDamage, _float _fDistance, _float _fStunRatio, _float _fDamageRatio,
	_float _fCriticalRatio, _int _iActivateCnt, _int _iMoveCnt, _int _iHeal, _bool _bTargetAll, _bool _bApproach, _int _iSkillMoveCnt)
	: m_strSkillName(_strSkillName), m_iActivateCnt(_iActivateCnt), m_strTargetEffectAnimKey(_strTargetEffectKey), m_strSoundKey(_strSoundKey),
	m_iMoveCnt(_iMoveCnt), m_iHeal(_iHeal), m_strAnimKey(_strAnimKey), m_strImgKey(_strImgKey), m_strEffectAnimKey(_strEffectKey),
	m_fStunRatio(_fStunRatio), m_fDamageRatio(_fDamageRatio), m_fCriticalRatio(_fCriticalRatio), m_bTargetAll(_bTargetAll), m_bApproach(_bApproach), m_iMovingCnt(_iSkillMoveCnt), m_fSkillDistance(_fDistance)
{
	memcpy(m_arrActivatePos, _arrActivatePos, sizeof(_bool) * 4);
	memcpy(m_arrTargetPos, _arrTargetPos, sizeof(_bool) * 4);
	memcpy(m_bArrAttack, _bArrAttack, sizeof(_bool) * 8);
	memcpy(m_bArrToEnemy, _bArrToEnemy, sizeof(_bool) * 6);
	memcpy(m_iDotDamage, _iDotDamage, sizeof(_int) * 2);

	if (!m_tDescription)
		m_tDescription = make_shared<tagDescription>();
}

CSkill::CSkill(tstring _strSkillName, tstring _strAnimKey, _bool* _arrTargetPos, _int* _iDotDamage, _float _fDistance,
	_float _fStunRatio, _float _fDamageRatio, _float _fCriticalRatio,
	_bool* _bArrAttack, _int _iMoveCnt, _int _iStress, _bool _bTargetAll, _bool _bApproach, _int _iSkillMoveCnt)
	: m_strSkillName(_strSkillName), m_strAnimKey(_strAnimKey), m_iMoveCnt(_iMoveCnt), m_iStress(_iStress),
	m_fStunRatio(_fStunRatio), m_fDamageRatio(_fDamageRatio), m_fCriticalRatio(_fCriticalRatio), m_bTargetAll(_bTargetAll), m_bApproach(_bApproach), m_iMovingCnt(_iSkillMoveCnt), m_fSkillDistance(_fDistance)
{
	memcpy(m_arrTargetPos, _arrTargetPos, sizeof(_bool) * 4);
	memcpy(m_bArrAttack, _bArrAttack, sizeof(_bool) * 8);
	memcpy(m_iDotDamage, _iDotDamage, sizeof(_int) * 2);

	if (!m_tDescription)
		m_tDescription = make_shared<tagDescription>();
}

CSkill::CSkill(const CSkill& rhs)
{
}

CSkill::~CSkill()
{
}

_bool CSkill::IsEnable()
{
	if (m_iActivateCnt == 0)
		return false;

	return true;
}