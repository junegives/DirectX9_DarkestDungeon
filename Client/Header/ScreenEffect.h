#pragma once

#include"UIObj.h"
#include"Enums.h"

BEGIN(Engine)

class CTexture;

END

class CScreenEffect : public CUIObj
{
public:
	explicit CScreenEffect(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CScreenEffect();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:
	void			SetScreenEffect(EScreenEffect _eEffect);

protected:
	void			AddComponent();

private:
	shared_ptr<CTexture> m_pTextureCom;
	EScreenEffect m_eCurrentEffect = EScreenEffect::ENUM_END;

	_float m_fEffectTime = 0.f;
	_float m_fActTime = 0.f;

	_int  m_iAlpha = 225;

	_bool m_bFadeInDone = false;

};

