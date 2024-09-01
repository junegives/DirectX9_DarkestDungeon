#pragma once

#include"UIObj.h"
#include "Hero.h"

BEGIN(Engine)

class CTexture;

END

class CHeroSkillUI : public CUIObj
{
public:
	explicit CHeroSkillUI(LPDIRECT3DDEVICE9 _pGraphicDev, shared_ptr<CHero> _pHero, _int _iIdx);
	virtual ~CHeroSkillUI();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:
	void			SettingHeroInfo(_int _iMaxHp, _int _iHp, vector<shared_ptr<CSkill>>* _pSkillVec);
	virtual			void PickingUI(LONG _fX, LONG _fY, _bool _bLB) override;
	virtual			void HoverUI(LONG _fX, LONG _fY) override;

	_int			GetEquippedCnt() { return m_iEquippedCnt; }

protected:
	void			AddComponent();
	void			SetIconPos();

private:
	shared_ptr<CTexture> m_pTextureCom;

private:
	// �����
	shared_ptr<CHero> m_pHero = nullptr;

	// ����� ���� (UI ��ġ �������� ����)
	_int	m_iIdx = 0;

	// ����� ��ų ��� ������Ʈ
	shared_ptr<CRcTex> m_arrSkillRcTex[6];
	shared_ptr<CTransform> m_arrSkillTransform[6];
	shared_ptr<CTexture> m_arrSkillTexture[6];

	// ����� ��ų ���� (Ȱ��, ���) ��� ������Ʈ
	shared_ptr<CRcTex> m_arrSkillStateRcTex[2];
	shared_ptr<CTexture> m_pSkillActivateTexture;
	shared_ptr<CTexture> m_pSkillLockTexture;

	// ��� ��ų �ݾ� �ؽ�ó
	shared_ptr<CRcTex> m_pSkillGoldRcTex;
	shared_ptr<CTexture> m_pSkillGoldTexture;

	// ���� ��ų ����
	_int	m_iSelectIdx = -1;
	_int	m_iEquippedCnt = 4;
};

