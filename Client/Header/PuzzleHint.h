#pragma once

#include"UIObj.h"

BEGIN(Engine)

class CTexture;

END


class CPuzzleHint : public CUIObj
{
public:
	explicit CPuzzleHint(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CPuzzleHint();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

private:
	void			AddComponent();

private:

	shared_ptr<CTexture> m_pTextureCom;


};


