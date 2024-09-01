#pragma once
#include "UIObj.h"
#include "Hero.h"

BEGIN(Engine)

class CTexture;

END

class CTavernUI : public CUIObj
{
public:
	explicit CTavernUI(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CTavernUI();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

	void SetHero(shared_ptr<CHero> _pHero);

public:
	virtual void PickingUI(LONG _fX, LONG _fY, _bool _bLB) override;
	virtual void HoverUI(LONG _fX, LONG _fY) override;

public:
	void			SettingHeroInfo(_int _iMaxHP, _int _iHP, _int _iStress, _int _iGold);

protected:
	void			AddComponent();
	void			SetIconPos();

private:
	void IconClickEvent();

private:
	_int m_iHP = 0;
	_int m_iMaxHP = 0;
	_int m_iStress = 0;
	_int m_iGold = 0;

	weak_ptr<CHero> m_pHero;

	// Base 출력
	shared_ptr<CTexture> m_pTextureCom = nullptr;

	// 체크 버튼 클릭
	shared_ptr<CRcTex> m_pCheckRcTex;
	shared_ptr<CTransform> m_pCheckTransform;
	shared_ptr<CTexture> m_pCheckTexture;

	//아이콘 클릭 이벤트 관련 변수
	_bool m_bIconClick = false;
	_float m_fActTime = 0.f;
	_float m_fDeltaTime = 0.f;

	_int	m_iCursorPos = 99;

	tstring	m_strBackKey = L"";

};