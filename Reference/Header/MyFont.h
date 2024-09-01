#pragma once
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CMyFont 
{
public:
	explicit CMyFont(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMyFont();

public:
	HRESULT		ReadyResourceFont(const _tchar* pFontPath, const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight);
	HRESULT		Ready_Font(const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight);
	void		Render_Font(const _tchar* pString, const _vec2* pPos, D3DXCOLOR Color);
	void		RenderFontCenter(const _tchar* pString, const _vec2* pStartPos, const _vec2* pEndPos, D3DXCOLOR Color);

private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	LPD3DXSPRITE				m_pSprite;
	LPD3DXFONT					m_pFont;

public:
	static shared_ptr<CMyFont> Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight);

private:
	virtual void	Free();
};

END