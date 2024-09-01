#pragma once

#include"UIObj.h"

BEGIN(Engine)

class CTexture;
END

class CSkill;


class CBattleHeroUI : public CUIObj
{
public:
	explicit CBattleHeroUI(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CBattleHeroUI();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:
	_int	GetSkill() { return m_iSelectSkill; }
	void	SetSkill(_int _iSkill) { m_iSelectSkill = _iSkill; }

public:
	void			SettingHeroInfo(_int _iMaxHP, _int _iHP, _int _iStress, tstring _strHeroName, vector<shared_ptr<CSkill>>* _vecSkill);
	virtual			void PickingUI(LONG _fX, LONG _fY , _bool _bLB) override;
	virtual			void HoverUI(LONG _fX, LONG _fY) override;

protected:
	void			AddComponent();
	void			SetIconPos();

private:

	void IconClickEvent();

private:
	shared_ptr<CTexture> m_pTextureCom;

private:
	//체력(현재, max), 스트레스(현재/max), 직업군

	tstring m_strHeroName = L"";
	
	_int m_iMaxHP = 0;
	_int m_iHP = 0;

	_int m_iStress = 0;

	vector<shared_ptr<CSkill>>* m_pSkillVec= nullptr;
	
	//스킬 틀 , 1~5: 7: Cursor
	shared_ptr<CRcTex> m_arrSkillRcTex[7];
	shared_ptr<CTransform> m_arrSkillTransform[7];
	shared_ptr<CTexture> m_arrSkillTexture[7];

	//0~4 : 각 스킬슬롯 Index , 99 = NONE
	_int m_iCursorPos = 99;
	_int m_iSelectSkill = 0;

	//아이콘 클릭 이벤트 관련 변수
	_bool m_bIconClick = false;
	_float m_fActTime = 0.f;
	_float m_fDeltaTime = 0.f;

};

