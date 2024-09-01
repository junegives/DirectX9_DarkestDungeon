#pragma once

#include"UIObj.h"

BEGIN(Engine)

class CTexture;

END


class CPlayerFPSUI : public CUIObj
{
public:

	explicit CPlayerFPSUI(LPDIRECT3DDEVICE9 _GraphicDev);
	virtual ~CPlayerFPSUI();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

private:
	virtual void			AddComponent();

public:

	void SetHP(_int _iHP);
	void SetMaxHP(_int _iMaxHP) { m_iMaxHP = _iMaxHP; }



private:
	//Component

	shared_ptr<CTexture> m_pTextureCom = nullptr;

	//Hpbar
	shared_ptr<CRcTex> m_pHPRCTexCom[2];
	shared_ptr<CTexture> m_pHPTextureCom[2];
	shared_ptr<CTransform> m_pHPTransCom[2];


	shared_ptr<CRcTex> m_pShieldRCTexCom = nullptr;
	shared_ptr<CTexture> m_pShieldTextureCom = nullptr;
	shared_ptr<CTransform> m_pShieldTransCom = nullptr;

private:
	//Player Info

	_int m_iHP = 100;
	_int m_iMaxHP = 100;

	_int m_iHPGap = 0;

};

