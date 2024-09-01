#pragma once

#include"UIObj.h"

BEGIN(Engine)

class CTexture;

END

class CNarration : public CUIObj
{
public:

	explicit CNarration(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CNarration();


public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:
	virtual void ActiveUI(tstring _strKey) override;

protected:
	void			AddComponent();
	void			SetNarrationMap();

private:

	_float m_fActTime = 0.f;
	tstring m_strCurrentKey = L"";


	_float m_fTotalTime = 0.f;

private:
	shared_ptr<CTexture> m_pTextureCom;
	unordered_map<tstring, tstring> m_NarrationMap;

};

