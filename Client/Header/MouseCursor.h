#pragma once

#include"UIObj.h"

BEGIN(Engine)

class CTexture;

END


class CMouseCursor : public CUIObj
{
public:
	explicit CMouseCursor(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CMouseCursor();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

private:
	virtual void			AddComponent();

private:
	shared_ptr<CTexture> m_pTextureCom = nullptr;

};

