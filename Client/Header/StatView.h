#pragma once

#include"GameObject.h"


BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END


class CStatView : public CGameObject
{
public:
	explicit CStatView(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CStatView();
public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:
	//�ʱ�ȭ
	void	SettingInit(_vec3 _vCreaturePos, _int _iHp, _int _iMaxHp, _bool _isHero);
	void	SettingPos(_vec3 _vPos, _bool _bFront);

public:
	//���� Creature���� ����ؾ��ϴ� ��ġ ���� �Լ� -------------------------------------------------------
	void SetStress(int _iStress);
	void SetHp(int _iHp);
	void SetMaxHp(int _iHp) { m_iMaxHp = _iHp; };
	void SetCorpse(_bool _bCorpse) { m_bCorpse = _bCorpse; }

	void SetIsHero(_bool _bIsHero) { m_bHero = _bIsHero; }
	void SetIsHeroType(_int _iHeroType) { m_iHeroType = _iHeroType; }
	void SetAffliction(_bool _bAffliction) { m_bAffliction = _bAffliction; }
	void SetVirtue(_bool _bVirtue) { m_bVirtue = _bVirtue; }
	void SetIsTurn(_bool _bIsTurn) { m_bTurn = _bIsTurn; }
	void SetIsTeamTarget(_bool _bIsTurn) { m_bTargetTeam = _bIsTurn; }
	void SetIsTeamsTarget(_bool _bIsTurn) { m_bTargetTeams = _bIsTurn; }
	void SetIsEnemyTarget(_bool _bIsTurn) { m_bTargetEnemy = _bIsTurn; }
	void SetIsEnemiesTarget(_bool _bIsTurn) { m_bTargetEnemies = _bIsTurn; }
	void SetIsTurnOff(_bool _bTurnOff) { m_bTurnOff = _bTurnOff; }

	// 0 : �ߵ� , 1: ���� , 2: ���� , 3: ���, 4: �ر�, 5: ����, 6: �����, 7: ������ �ϰ�
	void SetAttribute(_int _iTypeNum) { m_bAttributeArr[_iTypeNum] = true;  }
	void SetAttributeOff(_int _iTypeNum) { m_bAttributeArr[_iTypeNum] = false;  }

	//------------------------------------------------------------------------------------------------------
private:
	void	AddComponent();
	//�� Stat������ ������ ũ������ Top Position�� ��� �׿� ���� �������ش�

	//�ǽð����� scale�� �ٲ�� UI�� Ȯ�� �� �������ݴϴ�
	void	ChangeActiveUI(_float fTimeDelta);

private:

	//0~9 : ��Ʈ���� /10~11 : ü�¹� /12~19 :���°�/ 20: ��ü ü�¹�/ 21: �ڱ����� ǥ��, 22: �� Ÿ�� ǥ��, 23: ����, 24: ��, 25: ���� Ÿ��, 26: ��ǥ��, 27: gap, 28 ~ 35: ���, �ر�
	shared_ptr<CRcTex> m_pRCTexCom[36];
	shared_ptr<CTransform> m_pTransformCom[36];
	// 0:��Ʈ����X 1:��Ʈ����o 2:�� HP�� 3:HP�� 4: �ߵ� 5:���� 6:���� 7: ��� 8: �ر� 9: ����
	// 10: ����� 11: ������ �ϰ� 12: ��ü HP�� 13: ���� ǥ��, 14: �� Ÿ�� ǥ��, 15: ���� Ÿ�� ǥ��
	// 16: �� Ÿ�� ǥ��, 17: ���� Ÿ�� ǥ��, 18: ����,  19~26: ���, �ر�
	shared_ptr<CTexture> m_pTextureCom[27];

	_bool m_bAttributeArr[8] = {false, false, false, false, false, false, false, false };

	_vec3 m_vCenterPos; //���� ����â�� ���� ��ǥ 

	_int m_iStress = 0;
	_int m_iHp;
	_int m_iMaxHp;

	//���� ��Ʈ���� UI ON ����
	_int m_iStressNum = 0;
	//��ü ü�¿� ���� ���� ü�� ����
	_float m_fHpBarRange = 1.f;
	//ü�� ���� �� ����
	_float m_fHpGapRange = 0.f;
	_int m_iGapHp = 0;

	_bool m_bHero = true;
	_int m_iHeroType = 0;				// 1: �����ı���, 2: ��󰭵�, 3: ����, 4: ����
	_bool m_bVirtue = false;			// ���
	_bool m_bAffliction = false;		// �ر�
	_bool m_bCorpse = false;
	_bool m_bTurn = false;

	_bool m_bTargetTeam = false;
	_bool m_bTargetTeams = false;
	_bool m_bTargetEnemy = false;
	_bool m_bTargetEnemies = false;
	_bool m_bTurnOff = false;

	_bool	m_bHpGap = false;
	_float	m_fHpRegemTime = 0.f;
};

