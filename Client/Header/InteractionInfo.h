#pragma once

#include"UIObj.h"

BEGIN(Engine)

class CTexture;

END

class CInteractionInfo : public CUIObj
{

public:
	explicit CInteractionInfo(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CInteractionInfo();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:

	void	SetInteractionText(tstring _strKey, tstring _strInfo) {
		m_strInfo = _strInfo;
		m_strMainKey = _strKey;

		m_bVisible = true;
		m_bCheckObj = true;
	}

	void SetIsBattle(_bool _bValue) { m_bIsBattle = _bValue; }

private:
	void			AddComponent();

private:
	shared_ptr<CTexture> m_pTextureCom;

	tstring m_strInfo = L"";
	tstring m_strMainKey = L"";

	_bool m_bCheckObj = false;
	_bool m_bIsBattle = false;


};

