#pragma once
#include "UIObj.h"

BEGIN(Engine)

class CTexture;

END

class CTextBoard : public CUIObj
{
public:
	explicit CTextBoard(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CTextBoard();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int	UpdateGameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdateGameObject() override;
	virtual void	RenderGameObject() override;

public:
	virtual void	ActiveUI(tstring _strKey, _vec3 _vPos, _vec3 _vScale, _float _fActTime) override;

protected:
	void			AddComponent();
	void			SetText(tstring _strText) { m_strText = _strText; }

private:
	_float			m_fActTime = 0.f;
	tstring			m_strText = L"";

private:
	shared_ptr<CTexture> m_pTextureCom;
};

