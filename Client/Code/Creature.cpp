#include "pch.h"
#include "Creature.h"
#include "Hero.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include"StatView.h"

#include "TextBoard.h"
#include "UIMgr.h"

#include "EffectMgr.h"
#include "Effect.h"

#include "UIMgr.h"
#include "ScreenEffect.h"

#include "Alien.h"

CCreature::CCreature(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
	memset(&m_tCommonStat, NULL, sizeof(STAT));
	ZeroMemory(m_szString, sizeof(m_szString));
	ZeroMemory(m_szString2, sizeof(m_szString2));
	ZeroMemory(m_szString3, sizeof(m_szString3));
}

CCreature::CCreature(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat, _int _iPosition,
	vector<shared_ptr<CSkill>>& _pVecSkill)
	: CGameObject(pGraphicDev), m_tCommonStat(_tCommonStat), m_iPosition(_iPosition), m_pVecSkill(_pVecSkill)
{
}

CCreature::CCreature(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat)
	: CGameObject(pGraphicDev), m_tCommonStat(_tCommonStat)
{
}

CCreature::CCreature(const CCreature& rhs)
	: Engine::CGameObject(rhs)
{
}

CCreature::~CCreature()
{
}

HRESULT CCreature::ReadyGameObject()
{
	__super::ReadyGameObject();
	SettingStatBar();

	return S_OK;
}

_int CCreature::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = CGameObject::UpdateGameObject(fTimeDelta);

	m_vPos = *m_pTransformCom->GetPos();

	//������ ����
	_matrix matWorld;

	matWorld = *m_pTransformCom->GetWorld();
	SetBillBoard(matWorld);
	m_pTransformCom->SetWorld(&matWorld);

	Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());

	// ���Ȱ���
	m_pStatInfo->SetHp(m_tCommonStat.iHp);
	for (int i = 0; i < 3; i++)
	{
		if (m_bState[i])
			m_pStatInfo->SetAttribute(i);
		else
			m_pStatInfo->SetAttributeOff(i);
	}
	// ���, �ر�

	// ����, �����
	for (int i = 5; i < 7; i++)
	{
		if (m_bState3[i - 5])
			m_pStatInfo->SetAttribute(i);
		else
			m_pStatInfo->SetAttributeOff(i);
	}

	// ������ �ϰ� ǥ��
	if (m_bBeforeDeath)	m_pStatInfo->SetAttribute(7);
	else
		m_pStatInfo->SetAttributeOff(7);

	if (m_pStatInfo != nullptr && bStatBarOn) {
		m_pStatInfo->SettingPos(*(m_pTransformCom->GetPos()), m_bFront);
		m_pStatInfo->UpdateGameObject(fTimeDelta);
	}

	return iExit;
}

void CCreature::LateUpdateGameObject()
{
	ClimbingTerrain();
}

void CCreature::RenderGameObject()
{
	TCHAR szBuff[32] = { };
	_stprintf_s(szBuff, TEXT("%d"), GetHp());
	lstrcpy(m_szString, szBuff);

	TCHAR szBuff2[32] = { };
	_stprintf_s(szBuff, TEXT("%d"), GetCurrentBleed());
	lstrcpy(m_szString2, szBuff);

	TCHAR szBuff3[32] = { };
	_stprintf_s(szBuff, TEXT("%d"), GetCurrentPoision());
	lstrcpy(m_szString3, szBuff);

	if (m_pStatInfo != nullptr && bStatBarOn)
		m_pStatInfo->RenderGameObject();

}

_bool CCreature::IsAttacking()
{
	for (int i = 0; i < 7; i++)
	{
		if (m_bAttacking[i])
		{
			return true;
		}
	}
	return false;
}

void CCreature::SetStun(_bool _bStun)
{
	if (!_bStun)
	{
		m_bState[2] = false;
		return;
	}

	m_bPrevStun = true;
	m_bState[2] = true;

	shared_ptr<CEffect> pEffect;
	// ��Ʈ ����Ʈ
	{
		pEffect = CEffectMgr::GetInstance()->GetEffect();
		pEffect->SetFontEffect(L"UI_Stun", m_pTransformCom->GetPos(), ATTACKTIME);
		pEffect->SetActive(true);
	}

	// ��� ����Ʈ
	{
		m_pLoopEffect = pEffect = CEffectMgr::GetInstance()->GetEffect();
		pEffect->SetHeadEffect(L"UI_Head_Stun", m_pTransformCom->GetPos(), ATTACKTIME, true);
		pEffect->SetActive(true);
	}
}

//shared_ptr<CSkill> CCreature::GetSkill(tstring _strSkillName)
//{
//	for (auto iter : *m_pVecSkill)
//	{
//		// ���� �ʿ� // �̷��� ���ص� �Ǵ��� Ȯ���غ��ߵ�
//		if (iter->GetSkillName() == _strSkillName)
//		{
//			return iter;
//		}
//	}
//
//	return nullptr;
//}

//HRESULT CCreature::SetSkill(shared_ptr<vector<shared_ptr<CSkill>>> _pSkill)
//{
//	m_pVecSkill = _pSkill;
//
//	return S_OK;
//}

void CCreature::IncreaseHP(_int _iValue, _bool _bStressEvent)
{
	if (_iValue == 0) return;

	shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();
	
	// �ر�/��� �̺�Ʈ�� ���� �������� ���
	if (_bStressEvent)
	{
		m_vStressEventPos = m_pTransformCom->GetPos();
		m_vStressEventPos->y -= 2.f;
		pEffect->SetDamageEffect(1, _iValue, m_vStressEventPos, ATTACKTIME);
	}

	// �� ���� ���
	else
	{
		pEffect->SetDamageEffect(1, _iValue, m_pTransformCom->GetPos(), ATTACKTIME);
	}

	pEffect->SetActive(true);

	m_tCommonStat.iHp += _iValue;
	if (m_tCommonStat.iHp >= m_tCommonStat.iMaxHp)
		m_tCommonStat.iHp = m_tCommonStat.iMaxHp;
}

void CCreature::DecreaseHP(_int _iValue, _bool _bStressEvent)
{
	if (_iValue == 0) return;

	shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

	// �ر�/��� �̺�Ʈ�� ���� �������� ���
	if (_bStressEvent)
	{
		m_vStressEventPos = m_pTransformCom->GetPos();
		m_vStressEventPos->y -= 2.f;
		pEffect->SetDamageEffect(0, _iValue, m_vStressEventPos, ATTACKTIME);
	}

	// �� ���� ���
	else
	{
		pEffect->SetDamageEffect(0, _iValue, m_pTransformCom->GetPos(), ATTACKTIME);
	}

	pEffect->SetActive(true);

	// ���� ���� ������ ������ ���� ����Ű��
	if (_iValue > 40 && m_bIsHero)
	{
		m_bDeath = true;
		m_tCommonStat.iHp = -100;

		bStatBarOn = false;
	}
	else
	{
		m_tCommonStat.iHp -= _iValue;
		if (m_tCommonStat.iHp < 0)
		{
			m_tCommonStat.iHp = 0;
		}
	}
}

void CCreature::StartCalculate(_bool _bAutoEffect, _int& _iValue)
{
	// �����̳� �ߵ� ���¶��
	if (m_bState[0] || m_bState[1])
	{
		if (_bAutoEffect)
		{
			if (m_bState[0])
			{
				shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

				pEffect->SetFontEffect(L"UI_Blight", m_pTransformCom->GetPos(), ATTACKTIME);
				pEffect->SetActive(true);

				CSoundMgr::GetInstance()->StopSound(CHANNELID::STATUS);
				CSoundMgr::GetInstance()->PlaySound(L"Gen_Status_Poison_Dot.wav", CHANNELID::STATUS, 1.f);
			}

			if (m_bState[1])
			{
				shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

				pEffect->SetFontEffect(L"UI_Blood", m_pTransformCom->GetPos(), ATTACKTIME);
				pEffect->SetActive(true);

				CSoundMgr::GetInstance()->StopSound(CHANNELID::STATUS);
				CSoundMgr::GetInstance()->PlaySound(L"Gen_Status_Bleed_Dot.wav", CHANNELID::STATUS, 1.f);
			}

			// ��Ʈ�� ���
			_int _iDamage = m_iBlightDot[0] + m_iBleedDot[0];

			if (_iDamage > 0)
			{
				DecreaseHP(_iDamage);
			}
		}

		else
			m_tCommonStat.iHp -= (m_iBlightDot[0] + m_iBleedDot[0]);

		_iValue = m_iBlightDot[0] + m_iBleedDot[0];

		if (m_iBlightDot[0] > 0) {

			CSoundMgr::GetInstance()->StopSound(CHANNELID::STATUS);
			CSoundMgr::GetInstance()->PlaySound(L"Gen_Status_Poison_Dot.wav", CHANNELID::STATUS, 1.f);
		}

		if (m_iBleedDot[0] > 0) {
			CSoundMgr::GetInstance()->StopSound(CHANNELID::STATUS);
			CSoundMgr::GetInstance()->PlaySound(L"Gen_Status_Bleed_Dot.wav", CHANNELID::STATUS, 1.f);
		}

		// ������� �ʾ������� �ǰ� 0�Ʒ��� �ȴ�� �ϱ�
		if (!m_bDeath)
		{
			if (m_tCommonStat.iHp < 0)
			{
				m_tCommonStat.iHp = 0;
			}
		}
		for (int i = 1; i < 3; i++)
		{
			m_iBlightDot[i - 1] = m_iBlightDot[i];
			m_iBleedDot[i - 1] = m_iBleedDot[i];
		}
		m_iBlightDot[3] = 0;
		m_iBleedDot[3] = 0;

		if (!m_iBlightDot[0]) m_bState[0] = false;
		if (!m_iBleedDot[0]) m_bState[1] = false;
	}

	// ����
	if (m_bState[2] && _bAutoEffect)
	{
		// ���� ����Ʈ ���ִ� Ÿ�̹�
		m_pLoopEffect->Reset();
		m_pLoopEffect->SetActive(false);


		if (_bAutoEffect)
		{
			shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

			pEffect->SetFontEffect(L"UI_Stun", m_pTransformCom->GetPos(), ATTACKTIME);
			pEffect->SetActive(true);
			m_bMyTurn = false;
			m_bState[2] = false;
		}
	}

	// ����
	if (m_bState3[0])
	{
		for (int i = 1; i < 3; i++)
		{
			m_iBuff1Dot[i - 1] = m_iBuff1Dot[i];
			m_iBuff2Dot[i - 1] = m_iBuff2Dot[i];
		}
		m_iBuff1Dot[3] = 0;
		m_iBuff2Dot[3] = 0;


		CSoundMgr::GetInstance()->StopSound(CHANNELID::STATUS);
		CSoundMgr::GetInstance()->PlaySound(L"Gen_Status_Buff.wav", CHANNELID::STATUS, 1.f);

		if (!m_iBuff1Dot[0] && !m_iBuff2Dot[0]) m_bState3[0] = false;
	}

	// �����
	if (m_bState3[1])
	{
		for (int i = 1; i < 3; i++)
		{
			m_iDeBuff1Dot[i - 1] = m_iDeBuff1Dot[i];
			m_iDeBuff1Dot[i - 1] = m_iDeBuff1Dot[i];
		}
		m_iDeBuff1Dot[3] = 0;


		CSoundMgr::GetInstance()->StopSound(CHANNELID::STATUS);
		CSoundMgr::GetInstance()->PlaySound(L"Gen_Status_Debuff.wav", CHANNELID::STATUS, 1.f);

		if (!m_iDeBuff1Dot[0]) m_bState3[1] = false;
	}

	if (!m_bIsHero)
	{
		// ��ü ����	
		if (m_tCommonStat.iHp <= 0 && !m_bCorpse && !m_bDeath)
		{
			BleedCure();
			BlightCure();
			m_bCorpse = true;
			m_tCommonStat.iHp = 10;
			m_tCommonStat.iMaxHp = 10;
			// ���Ȱ���
			m_pStatInfo->SetMaxHp(m_tCommonStat.iMaxHp);
			m_pStatInfo->SetHp(m_tCommonStat.iHp);
			m_pStatInfo->SetCorpse(true);
		}

		// ��� ����
		if (m_tCommonStat.iHp <= 0 && m_bCorpse)
		{
			m_bCorpse = false;
			m_bDeath = true;
			m_tCommonStat.iHp = -100;

			bStatBarOn = false;
		}
	}
	else
	{
		// ������ �ϰ� ����
		if (m_tCommonStat.iHp <= 0 && m_bBeforeDeath)
		{
			int iNum = rand() % 100;
			// ���
			if (iNum < DEATHRATE)
			{
				m_bDeath = true;
				m_tCommonStat.iHp = -100;

				bStatBarOn = false;
			}
			else
			{
				CSoundMgr::GetInstance()->StopSound(CHANNELID::NPC);
				CSoundMgr::GetInstance()->PlaySound(L"dun_deathdoor.wav", CHANNELID::NPC, 1.5f);
			}
		}
		else if (m_tCommonStat.iHp <= 0 && !m_bBeforeDeath && !m_bDeath)
		{
			CSoundMgr::GetInstance()->StopSound(CHANNELID::NPC);
			CSoundMgr::GetInstance()->PlaySound(L"dun_deathdoor.wav", CHANNELID::NPC, 1.5f);
			m_bBeforeDeath = true;
		}
	}
}

void CCreature::StartCalcBlight(_int& _iValue)
{
	// �ߵ� ���¶��
	if (m_bState[0])
	{
		m_tCommonStat.iHp -= m_iBlightDot[0];

		_iValue = m_iBlightDot[0];

		if (m_iBlightDot[0] > 0) {

			CSoundMgr::GetInstance()->StopSound(CHANNELID::STATUS);
			CSoundMgr::GetInstance()->PlaySound(L"Gen_Status_Poison_Dot.wav", CHANNELID::STATUS, 1.f);
		}

		// ������� �ʾ������� �ǰ� 0�Ʒ��� �ȴ�� �ϱ�
		if (!m_bDeath)
		{
			if (m_tCommonStat.iHp < 0)
			{
				m_tCommonStat.iHp = 0;
			}
		}
		for (int i = 1; i < 3; i++)
		{
			m_iBlightDot[i - 1] = m_iBlightDot[i];
		}

		m_iBlightDot[3] = 0;

		if (!m_iBlightDot[0]) m_bState[0] = false;
	}

	if (!m_bIsHero)
	{
		// ��ü ����	
		if (m_tCommonStat.iHp <= 0 && !m_bCorpse && !m_bDeath)
		{
			BleedCure();
			BlightCure();
			m_bCorpse = true;
			m_tCommonStat.iHp = 10;
			m_tCommonStat.iMaxHp = 10;
			// ���Ȱ���
			m_pStatInfo->SetMaxHp(m_tCommonStat.iMaxHp);
			m_pStatInfo->SetHp(m_tCommonStat.iHp);
			m_pStatInfo->SetCorpse(true);
		}

		// ��� ����
		if (m_tCommonStat.iHp <= 0 && m_bCorpse)
		{
			m_bCorpse = false;
			m_bDeath = true;
			m_tCommonStat.iHp = -100;

			bStatBarOn = false;
		}
	}
	else
	{
		// ������ �ϰ� ����
		if (m_tCommonStat.iHp <= 0 && m_bBeforeDeath)
		{
			int iNum = rand() % 100;
			// ���
			if (iNum < DEATHRATE)
			{
				m_bDeath = true;
				m_tCommonStat.iHp = -100;

				bStatBarOn = false;
			}
			else
			{
				CSoundMgr::GetInstance()->StopSound(CHANNELID::NPC);
				CSoundMgr::GetInstance()->PlaySound(L"dun_deathdoor.wav", CHANNELID::NPC, 1.5f);
			}
		}
		else if (m_tCommonStat.iHp <= 0 && !m_bBeforeDeath && !m_bDeath)
		{
			CSoundMgr::GetInstance()->StopSound(CHANNELID::NPC);
			CSoundMgr::GetInstance()->PlaySound(L"dun_deathdoor.wav", CHANNELID::NPC, 1.5f);
			m_bBeforeDeath = true;
		}
	}
}

void CCreature::StartCalcBleed(_int& _iValue)
{
	// �ߵ� ���¶��
	if (m_bState[1])
	{
		m_tCommonStat.iHp -= m_iBleedDot[0];

		_iValue = m_iBleedDot[0];

		if (m_iBleedDot[0] > 0) {
			CSoundMgr::GetInstance()->StopSound(CHANNELID::STATUS);
			CSoundMgr::GetInstance()->PlaySound(L"Gen_Status_Bleed_Dot.wav", CHANNELID::STATUS, 1.f);
		}

		// ������� �ʾ������� �ǰ� 0�Ʒ��� �ȴ�� �ϱ�
		if (!m_bDeath)
		{
			if (m_tCommonStat.iHp < 0)
			{
				m_tCommonStat.iHp = 0;
			}
		}
		for (int i = 1; i < 3; i++)
		{
			m_iBleedDot[i - 1] = m_iBleedDot[i];
		}
		m_iBleedDot[3] = 0;

		if (!m_iBleedDot[0]) m_bState[1] = false;
	}

	if (!m_bIsHero)
	{
		// ��ü ����	
		if (m_tCommonStat.iHp <= 0 && !m_bCorpse && !m_bDeath)
		{
			BleedCure();
			BlightCure();
			m_bCorpse = true;
			m_tCommonStat.iHp = 10;
			m_tCommonStat.iMaxHp = 10;
			// ���Ȱ���
			m_pStatInfo->SetMaxHp(m_tCommonStat.iMaxHp);
			m_pStatInfo->SetHp(m_tCommonStat.iHp);
			m_pStatInfo->SetCorpse(true);
		}

		// ��� ����
		if (m_tCommonStat.iHp <= 0 && m_bCorpse)
		{
			m_bCorpse = false;
			m_bDeath = true;
			m_tCommonStat.iHp = -100;

			bStatBarOn = false;
		}
	}
	else
	{
		// ������ �ϰ� ����
		if (m_tCommonStat.iHp <= 0 && m_bBeforeDeath)
		{
			int iNum = rand() % 100;
			// ���
			if (iNum < DEATHRATE)
			{
				m_bDeath = true;
				m_tCommonStat.iHp = -100;

				bStatBarOn = false;
			}
			else
			{
				CSoundMgr::GetInstance()->StopSound(CHANNELID::NPC);
				CSoundMgr::GetInstance()->PlaySound(L"dun_deathdoor.wav", CHANNELID::NPC, 1.5f);
			}
		}
		else if (m_tCommonStat.iHp <= 0 && !m_bBeforeDeath && !m_bDeath)
		{
			CSoundMgr::GetInstance()->StopSound(CHANNELID::NPC);
			CSoundMgr::GetInstance()->PlaySound(L"dun_deathdoor.wav", CHANNELID::NPC, 1.5f);
			m_bBeforeDeath = true;
		}
	}
}

void CCreature::StartCalcStun(_int& _iValue)
{
	// ����
	if (m_bState[2])
	{
		// ���� ����Ʈ ���ִ� Ÿ�̹�
		m_pLoopEffect->Reset();
		m_pLoopEffect->SetActive(false);
	}
}

void CCreature::StartCalcBuff(_int& _iValue)
{
	// ����
	if (m_bState3[0])
	{
		for (int i = 1; i < 3; i++)
		{
			m_iBuff1Dot[i - 1] = m_iBuff1Dot[i];
			m_iBuff2Dot[i - 1] = m_iBuff2Dot[i];
		}
		m_iBuff1Dot[3] = 0;
		m_iBuff2Dot[3] = 0;


		CSoundMgr::GetInstance()->StopSound(CHANNELID::STATUS);
		CSoundMgr::GetInstance()->PlaySound(L"Gen_Status_Buff.wav", CHANNELID::STATUS, 1.f);

		if (!m_iBuff1Dot[0] && !m_iBuff2Dot[0]) m_bState3[0] = false;
	}

	// �����
	if (m_bState3[1])
	{
		for (int i = 1; i < 3; i++)
		{
			m_iDeBuff1Dot[i - 1] = m_iDeBuff1Dot[i];
			m_iDeBuff1Dot[i - 1] = m_iDeBuff1Dot[i];
		}
		m_iDeBuff1Dot[3] = 0;


		CSoundMgr::GetInstance()->StopSound(CHANNELID::STATUS);
		CSoundMgr::GetInstance()->PlaySound(L"Gen_Status_Debuff.wav", CHANNELID::STATUS, 1.f);

		if (!m_iDeBuff1Dot[0]) m_bState3[1] = false;
	}
}

void CCreature::AttackCreature(shared_ptr<CCreature> _pCreature, shared_ptr<CCreature> _pCreature2, shared_ptr<CSkill> _pSkill)
{
	if (!_pCreature) return;
	if (!_pSkill) return;

	m_bGetStress = false;

	// ���� ����
	int iDodgeNum;

	// ġ��Ÿ ����
	int iCritical = rand() % 100;

	_bool* arrAttack = _pSkill->GetArrAttack();


	// �ܼ� ����
	if (arrAttack[0])
	{
		// ������ �����̿��� ���
		if (dynamic_pointer_cast<CAlien>(_pCreature2))
		{
			iDodgeNum = 100;
			CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS);
			CSoundMgr::GetInstance()->PlaySound(L"Boss_Scream.wav", CHANNELID::BOSS, 1.2f);
		}
		else
		{
			iDodgeNum = rand() % 100;
		}

		// �ڸ� �̵���Ű�� �����̿��� ���
		if (_pSkill->GetMoveCnt() != 0)
		{
			iDodgeNum = 100;
		}

		if (iDodgeNum >= (_pCreature->GetDodge() + m_iBuff2Dot[0] + m_iDeBuff1Dot[0]))
		{
			if (iCritical < CRIRATE)
			{
				//ũ��Ƽ�� ����Ʈ(������)
				if (m_bIsHero) {

					shared_ptr<CUIObj> pScreenEffect = CUIMgr::GetInstance()->FindUI(L"UI_ScreenEffect");

					if (pScreenEffect) {
						dynamic_pointer_cast<CScreenEffect>(pScreenEffect)->SetScreenEffect(EScreenEffect::BLOOD);
					}


					_int iRandNum = rand() % 4;

					tstring strBaseSoundKey = L"Narr_Battle_Critical";
					strBaseSoundKey = strBaseSoundKey + to_wstring(iRandNum);

					CUIMgr::GetInstance()->GetInstance()->NarrationOn(strBaseSoundKey);

				}

				// ��ܿ��϶�
				if (!_pSkill->GetArrToEnemy()[2])
				{
					_pCreature->DecreaseHP((_int)((_float)m_tCommonStat.iAttackPower * _pSkill->GetCriticalRatio()
						* (m_iBuff1Dot[0] / 100.f + 1.f)));
				}
				else
				{
					_pCreature->DecreaseHP((_int)((_float)m_tCommonStat.iAttackPower * _pSkill->GetCriticalRatio()));
				}

				if (dynamic_cast<CHero*>(this))
				{
					//dynamic_cast<CHero*>(this)->DecreaseStress(5);
				}
			}
			else
			{
				// ��ܿ��϶�
				if (!_pSkill->GetArrToEnemy()[2])
				{
					_pCreature->DecreaseHP((_int)((_float)m_tCommonStat.iAttackPower * _pSkill->GetDamageRatio()
						* (m_iBuff1Dot[0] / 100.f + 1.f)));
				}
				else
				{
					_pCreature->DecreaseHP((_int)((_float)m_tCommonStat.iAttackPower * _pSkill->GetDamageRatio()));
				}
			}
		}
		// �ִϸ��̼�, ����Ʈ �ٲٴ� �ڵ� �־���ҵ�
	}

	// �ߵ�
	if (arrAttack[1])
	{
		iDodgeNum = rand() % 100;
		if (iDodgeNum >= (_pCreature->GetDodge() + m_iBuff2Dot[0] + m_iDeBuff1Dot[0]))
		{
			if (iCritical < CRIRATE)
			{
				_pCreature->DecreaseHP((_int)((_float)m_tCommonStat.iAttackPower * _pSkill->GetCriticalRatio()));
				if (dynamic_cast<CHero*>(this))
				{
					//dynamic_cast<CHero*>(this)->DecreaseStress(5);
				}
			}
			else
				_pCreature->DecreaseHP((_int)((_float)m_tCommonStat.iAttackPower * _pSkill->GetDamageRatio()));
			_pCreature->BlightAttack(_pSkill->GetDotDamage());
			_pCreature->SetBlight(true);
		}

		CSoundMgr::GetInstance()->StopSound(CHANNELID::STATUS);
		CSoundMgr::GetInstance()->PlaySound(L"Gen_Status_Poison_Onset.wav", CHANNELID::STATUS, 1.f);
	}

	// ����
	if (arrAttack[2])
	{
		iDodgeNum = rand() % 100;
		if (iDodgeNum >= (_pCreature->GetDodge() + m_iBuff2Dot[0] + m_iDeBuff1Dot[0]))
		{
			if (iCritical < CRIRATE)
			{
				_pCreature->DecreaseHP((_int)((_float)m_tCommonStat.iAttackPower * _pSkill->GetCriticalRatio()));
				if (dynamic_cast<CHero*>(this))
				{
					//dynamic_cast<CHero*>(this)->DecreaseStress(5);
				}
			}
			else
				_pCreature->DecreaseHP((_int)((_float)m_tCommonStat.iAttackPower * _pSkill->GetDamageRatio()));
			_pCreature->BleedAttack(_pSkill->GetDotDamage());
			_pCreature->SetBleed(true);
		}


		CSoundMgr::GetInstance()->StopSound(CHANNELID::STATUS);
		CSoundMgr::GetInstance()->PlaySound(L"Gen_Status_Bleed_Onset.wav", CHANNELID::STATUS, 1.f);
	}

	// ����
	if (arrAttack[3])
	{
		iDodgeNum = rand() % 100;
		if (iDodgeNum >= (_pCreature->GetDodge() + m_iBuff2Dot[0] + m_iDeBuff1Dot[0]))
		{
			if (iCritical < CRIRATE)
			{
				_pCreature->DecreaseHP((_int)((_float)m_tCommonStat.iAttackPower * _pSkill->GetCriticalRatio()));
				if (dynamic_cast<CHero*>(this))
				{
					//dynamic_cast<CHero*>(this)->DecreaseStress(5);
				}
			}
			else
				_pCreature->DecreaseHP((_int)((_float)m_tCommonStat.iAttackPower * _pSkill->GetDamageRatio()));
			_pCreature->SetStun(true);
		}

		CSoundMgr::GetInstance()->StopSound(CHANNELID::STATUS);
		CSoundMgr::GetInstance()->PlaySound(L"Gen_Status_Stun_Onset.wav", CHANNELID::STATUS, 1.f);
	}

	// �̵�
	if (arrAttack[4])
	{
		iDodgeNum = 100;
		if (iDodgeNum >= (_pCreature->GetDodge() + m_iBuff2Dot[0] + m_iDeBuff1Dot[0]))
		{
			if (iCritical < CRIRATE)
			{
				_pCreature->DecreaseHP((_int)((_float)m_tCommonStat.iAttackPower * _pSkill->GetCriticalRatio()));
				if (dynamic_cast<CHero*>(this))
				{
					//dynamic_cast<CHero*>(this)->DecreaseStress(5);
				}
			}
			else
				_pCreature->DecreaseHP((_int)((_float)m_tCommonStat.iAttackPower * _pSkill->GetDamageRatio()));
		}
	}

	// ��
	if (arrAttack[5])
	{
		iDodgeNum = 100;
		if (iCritical < CRIRATE)
		{
			_pCreature->IncreaseHP(static_cast<_int>(_pSkill->GetHeal() * _pSkill->GetCriticalRatio()));
			//dynamic_pointer_cast<CHero>(_pCreature)->DecreaseStress(5);
		}
		else
			_pCreature->IncreaseHP(static_cast<_int>(_pSkill->GetHeal() * _pSkill->GetDamageRatio()));

		// ������ ���� ����
		if (_pCreature->GetIsBeforeDeath()) _pCreature->SetBeforeDeath(false);
	}

	// ����
	if (arrAttack[6])
	{
		// ��ܿ� ����
		if (!_pSkill->GetArrToEnemy()[0])
		{
			_int  DotBuff[2] = { 30, 3 };
			_pCreature2->Buff1Skill(DotBuff);
			_pCreature2->SetBuff(true);
		}
		// ��ܿ� �ع���
		if (!_pSkill->GetArrToEnemy()[1])
		{
			_int  DotBuff[2] = { 75, 3 };
			_pCreature2->Buff1Skill(DotBuff);
			_pCreature2->SetBuff(true);
		}
		// ��ܿ� ���� �ʱ�ȭ
		if (!_pSkill->GetArrToEnemy()[2])
		{
			_pCreature2->Buff1Reset();
			if (!m_iBuff1Dot[0] && !m_iBuff2Dot[0]) m_bState3[0] = false;
		}
		// ȸ�� ����
		if (!_pSkill->GetArrToEnemy()[3])
		{
			iDodgeNum = 100;
			_int  DotBuff[2] = { 10, 3 };
			_pCreature->Buff2Skill(DotBuff);
			_pCreature->SetBuff(true);
		}
	}

	// �����
	if (arrAttack[7])
	{
		// ȸ�� �����
		if (iDodgeNum >= (_pCreature->GetDodge() + m_iBuff2Dot[0] + m_iDeBuff1Dot[0]))
		{
			if (!_pSkill->GetArrToEnemy()[4])
			{
				_int  DotDeBuff[2] = { -10, 3 };
				_pCreature->DeBuffSkill(DotDeBuff);
				_pCreature->SetDeBuff(true);
			}
		}
	}

	// ��Ʈ����
	if (dynamic_pointer_cast<CHero>(_pCreature))
	{
		if (iDodgeNum >= (_pCreature->GetDodge() + m_iBuff2Dot[0] + m_iDeBuff1Dot[0]))
		{
			dynamic_pointer_cast<CHero>(_pCreature)->SetIncreaseStress(true);
			//dynamic_pointer_cast<CHero>(_pCreature)->IncreaseStress(_pSkill->GetStress());

		}
	}

	// ���϶�, ���������϶��� �ǰ� �ִϸ��̼� x
	if (arrAttack[5])
	{
		dynamic_pointer_cast<CCreature>(_pCreature)->SetHitted(false);
	}
	else if (m_bIsHero && !_pSkill->GetArrToEnemy()[3])
	{
		dynamic_pointer_cast<CCreature>(_pCreature)->SetHitted(false);
	}
	else
	{
		dynamic_pointer_cast<CCreature>(_pCreature)->SetHitted(true);
	}

	// �¾�����
	if (iDodgeNum >= (_pCreature->GetDodge() + m_iBuff2Dot[0] + m_iDeBuff1Dot[0]))
	{
		// ���
		dynamic_pointer_cast<CCreature>(_pCreature)->SetEffectOn(true);

		// ���� �ְ� �����̸�
		if (dynamic_pointer_cast<CCreature>(_pCreature)->m_bIsHero)
		{
			// ������ �ϰ� ����
			if (dynamic_pointer_cast<CCreature>(_pCreature)->GetHp() <= 0
				&& dynamic_pointer_cast<CCreature>(_pCreature)->GetIsBeforeDeath())
			{
				int iNum = rand() % 100;
				// ���
				if (iNum < DEATHRATE)
				{
					/*shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

					pEffect->SetSkillEffect(L"Effect_Death", m_pTextureCom->GetTextureSize(), m_pTransformCom->GetPos(), m_pTransformCom->GetScale(), ATTACKTIME);
					pEffect->SetActive(true);*/

					dynamic_pointer_cast<CCreature>(_pCreature)->SetDeath(true);
					dynamic_pointer_cast<CCreature>(_pCreature)->SetHp(-100);
					dynamic_pointer_cast<CCreature>(_pCreature)->SetStartBarOn(false);
				}
				else
				{
					CSoundMgr::GetInstance()->StopSound(CHANNELID::NPC);
					CSoundMgr::GetInstance()->PlaySound(L"dun_deathdoor.wav", CHANNELID::NPC, 1.5f);
				}
			}
			else if (dynamic_pointer_cast<CCreature>(_pCreature)->GetHp() <= 0
				&& !dynamic_pointer_cast<CCreature>(_pCreature)->GetIsBeforeDeath())
			{
				CSoundMgr::GetInstance()->StopSound(CHANNELID::NPC);
				CSoundMgr::GetInstance()->PlaySound(L"dun_deathdoor.wav", CHANNELID::NPC, 1.5f);

				dynamic_pointer_cast<CCreature>(_pCreature)->SetBeforeDeath(true);
			}
		}

		// ���� �ְ� ���͸�
		else
		{
			shared_ptr<CEffect> pEffect;

			shared_ptr<CTransform> pTargetTransform = dynamic_pointer_cast<CTransform>(_pCreature->GetComponent(L"Com_Transform", ID_DYNAMIC));
			shared_ptr<CAnimator> pTargetTexture = dynamic_pointer_cast<CAnimator>(_pCreature->GetComponent(L"Com_Animator", ID_DYNAMIC));


			// ��ü ����	
			if (dynamic_pointer_cast<CCreature>(_pCreature)->GetHp() <= 0 && !dynamic_pointer_cast<CCreature>(_pCreature)->GetIsCorpse())
			{
				pEffect = CEffectMgr::GetInstance()->GetEffect();

				pEffect->SetSkillEffect(L"Effect_Death", pTargetTexture->GetTextureSize(), pTargetTransform->GetPos(), pTargetTransform->GetScale(), ATTACKTIME);
				pEffect->SetActive(true);
			}

			// ��� ����
			else if (dynamic_pointer_cast<CCreature>(_pCreature)->GetHp() <= 0 && dynamic_pointer_cast<CCreature>(_pCreature)->GetIsCorpse())
			{
				pEffect = CEffectMgr::GetInstance()->GetEffect();

				pEffect->SetSkillEffect(L"Effect_Corpse", pTargetTexture->GetTextureSize(), pTargetTransform->GetPos(), pTargetTransform->GetScale(), ATTACKTIME);
				pEffect->SetActive(true);
			}
		}

		// Ÿ�� ����Ʈ
		{
			_pCreature->SetEffectInfo(_pSkill, true, false);
		}
	}
	// ȸ��������
	else
	{
		_pCreature->SetEffectInfo(_pSkill, true, true);
	}

	// ��
	m_bEffectOn = true;

}

void CCreature::MovePos(_vec3 _vPos, const _float& fTimeDelta, _float _fSpeed)
{
	_vec3		vDir;
	vDir = _vPos - m_vPos;

	D3DXVec3Normalize(&vDir, &vDir);
	m_pTransformCom->MoveForward(&vDir, fTimeDelta, _fSpeed);
}

_float CCreature::MovingSpeed(_vec3 _vPos, _float _fMovingTime)
{
	_vec3		vDir;
	vDir = _vPos - m_vPos;

	D3DXVec3Normalize(&vDir, &vDir);
	_float m_fSpeed = 0.f;
	if (vDir.x != 0)
		m_fSpeed = (_vPos - m_vPos).x / vDir.x / _fMovingTime;
	else if (vDir.y != 0)
		m_fSpeed = (_vPos - m_vPos).y / vDir.y / _fMovingTime;
	else if (vDir.z != 0)
		m_fSpeed = (_vPos - m_vPos).z / vDir.z / _fMovingTime;

	return m_fSpeed;
}

void CCreature::OffTurnCursor()
{
	m_pStatInfo->SetIsTurn(false);
}

void CCreature::OnTurnCursor()
{
	m_pStatInfo->SetIsTurn(true);
}

void CCreature::OnTargetTeamCursor()
{
	m_pStatInfo->SetIsTeamTarget(true);
}

void CCreature::OffTargetTeamCursor()
{
	m_pStatInfo->SetIsTeamTarget(false);
}

void CCreature::OnTargetTeamsCursor()
{
	m_pStatInfo->SetIsTeamsTarget(true);
}

void CCreature::OffTargetTeamsCursor()
{
	m_pStatInfo->SetIsTeamsTarget(false);
}

void CCreature::OnTargetEnemyCursor()
{
	m_pStatInfo->SetIsEnemyTarget(true);
}

void CCreature::OffTargetEnemyCursor()
{
	m_pStatInfo->SetIsEnemyTarget(false);
}

void CCreature::OnTargetEnemiesCursor()
{
	m_pStatInfo->SetIsEnemiesTarget(true);
}

void CCreature::OffTargetEnemiesCursor()
{
	m_pStatInfo->SetIsEnemiesTarget(false);
}

void CCreature::OnTurnUi()
{
	m_pStatInfo->SetIsTurnOff(false);
}

void CCreature::OffTurnUi()
{
	m_pStatInfo->SetIsTurnOff(true);
}

void CCreature::OffVirtue()
{
	m_pStatInfo->SetVirtue(false);
}

void CCreature::OffAffliction()
{
	m_pStatInfo->SetAffliction(false);
}

void CCreature::EndAttack(shared_ptr<CGameObject> _pCreature)
{
	if (!dynamic_pointer_cast<CCreature>(_pCreature)) return;

	// ���
	dynamic_pointer_cast<CCreature>(_pCreature)->SetHitted(false);
	dynamic_pointer_cast<CCreature>(_pCreature)->SetEffectOn(false);

	// ��
	m_bEffectOn = false;

	// ������
	m_bMyTurn = false;
}

void CCreature::Buff1Skill(_int* _iDotBuff)
{
	shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

	pEffect->SetFontEffect(L"UI_Buff", m_pTransformCom->GetPos(), ATTACKTIME);
	pEffect->SetActive(true);

	for (int i = 0; i < _iDotBuff[1]; i++)
	{
		m_iBuff1Dot[i] += _iDotBuff[0];
	}
}

void CCreature::Buff2Skill(_int* _iDotBuff)
{
	shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

	pEffect->SetFontEffect(L"UI_Buff", m_pTransformCom->GetPos(), ATTACKTIME);
	pEffect->SetActive(true);

	for (int i = 0; i < _iDotBuff[1]; i++)
	{
		m_iBuff2Dot[i] += _iDotBuff[0];
	}
}

void CCreature::DeBuffSkill(_int* _iDotDeBuff)
{
	shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

	pEffect->SetFontEffect(L"UI_Debuff", m_pTransformCom->GetPos(), ATTACKTIME);
	pEffect->SetActive(true);

	for (int i = 0; i < _iDotDeBuff[1]; i++)
	{
		m_iDeBuff1Dot[i] += _iDotDeBuff[0];
	}
}

void CCreature::CurioBuff2()
{
	_int  DotBuff[2] = { 30, 3 };
	
	for (int i = 0; i < DotBuff[1]; i++)
	{
		m_iBuff2Dot[i] += DotBuff[0];
	}
	SetBuff(true);
	UpdateAttribute();
}

void CCreature::Buff1Reset()
{
	m_bState3[0] = false;
	for (int i = 0; i < 4; i++) m_iBuff1Dot[i] = 0;
}

void CCreature::BuffReset()
{
	m_bState3[0] = false;
	m_bState3[1] = false;
	for (int i = 0; i < 4; i++) m_iBuff1Dot[i] = 0;
	for (int i = 0; i < 4; i++) m_iBuff2Dot[i] = 0;
}

void CCreature::BlightAttack(_int* _iDotDam)
{
	shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

	pEffect->SetFontEffect(L"UI_Blight", m_pTransformCom->GetPos(), ATTACKTIME);
	pEffect->SetActive(true);

	for (int i = 0; i < _iDotDam[1]; i++)
	{
		m_iBlightDot[i] += _iDotDam[0];
	}
}

void CCreature::BleedAttack(_int* _iDotDam)
{
	shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

	pEffect->SetFontEffect(L"UI_Blood", m_pTransformCom->GetPos(), ATTACKTIME);
	pEffect->SetActive(true);

	for (int i = 0; i < _iDotDam[1]; i++)
	{
		m_iBleedDot[i] += _iDotDam[0];
	}
}

void CCreature::BlightCure()
{
	m_bState[0] = false;
	for (int i = 0; i < 4; i++) m_iBlightDot[i] = 0;
}

void CCreature::BleedCure()
{
	m_bState[1] = false;
	for (int i = 0; i < 4; i++) m_iBleedDot[i] = 0;
}



void CCreature::SetEffectInfo(shared_ptr<CSkill> _pSkill, _bool _bTarget, _bool _bDodge)
{
	if (_bTarget)
	{
		shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

		tstring strEffectAnimKey;

		if (!_bDodge)
		{

			if (_pSkill->GetTargetEffectAnimKey() != L"")
			{
				strEffectAnimKey = _pSkill->GetTargetEffectAnimKey();
			}

			else
			{
				strEffectAnimKey = L"Effect_Blood";
			}

			pEffect->SetSkillEffect(strEffectAnimKey, m_pTextureCom->GetTextureSize(), m_pTransformCom->GetPos(), m_pTransformCom->GetScale(), ATTACKTIME);

			pEffect->SetActive(true);
		}

		// ȸ������ ���
		else
		{
			strEffectAnimKey = L"UI_Dodge";
			pEffect->SetFontEffect(strEffectAnimKey, m_pTransformCom->GetPos(), ATTACKTIME);

			pEffect->SetActive(true);
		}
	}

	else
	{
		if (_pSkill->GetEffectAnimKey() != L"")
		{
			shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

			shared_ptr< tagTextureInfo> textureInfo = *CResourceMgr::GetInstance()->GetTexture(_pSkill->GetAnimKey(), TEX_NORMAL)->begin();

			pEffect->SetSkillEffect(_pSkill->GetEffectAnimKey(), textureInfo->vImgSize, m_pTransformCom->GetPos(), m_pTransformCom->GetScale(), ATTACKTIME);
			pEffect->SetActive(true);
		}
	}
}

void CCreature::UpdateAttribute()
{
	// ���Ȱ���
	m_pStatInfo->SetHp(m_tCommonStat.iHp);
	for (int i = 0; i < 3; i++)
	{
		if (m_bState[i])
			m_pStatInfo->SetAttribute(i);
		else
			m_pStatInfo->SetAttributeOff(i);
	}

	// ����, �����
	for (int i = 5; i < 7; i++)
	{
		if (m_bState3[i - 5])
			m_pStatInfo->SetAttribute(i);
		else
			m_pStatInfo->SetAttributeOff(i);
	}

	// ������ �ϰ� ����
	if (m_bBeforeDeath)	m_pStatInfo->SetAttribute(7);
	else
		m_pStatInfo->SetAttributeOff(7);
}

void CCreature::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	pComponent = m_pBufCom = make_shared<CRcTex>(m_pGraphicDev);
	m_pBufCom->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_RCTex",pComponent });

	pComponent = m_pEffectBufCom = make_shared<CRcTex>(m_pGraphicDev);
	m_pEffectBufCom->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_EffectRCTex",pComponent });
}

void CCreature::ClimbingTerrain()
{
	//_vec3		vPos;
	//m_pTransformCom->GetInfo(INFO_POS, &vPos);
	//
	//CTerrainTex* pTerrainBufferCom = dynamic_cast<CTerrainTex*>(Engine::Get_Component(L"Layer_Environment", L"OBJ_Terrain", L"Proto_TerrainTex", ID_STATIC));
	//NULL_CHECK(pTerrainBufferCom);
	//
	//_float		fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPos,
	//	pTerrainBufferCom->Get_VtxPos(),
	//	pTerrainBufferCom->Get_VtxCntX(),
	//	pTerrainBufferCom->Get_VtxCntZ());
	//
	//m_pTransformCom->Set_Pos(vPos.x, fHeight + 0.5f, vPos.z);
}

void CCreature::SettingStatBar()
{
	m_pStatInfo = make_shared<CStatView>(m_pGraphicDev);
	m_pStatInfo->AwakeGameObject();
	m_pStatInfo->ReadyGameObject();

	//m_pStatInfo->SettingInit(*(m_pTransformCom->GetPos()),100, 100, true);

}

void CCreature::Free()
{
}
