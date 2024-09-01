#pragma once
#include "MyFont.h"

BEGIN(Engine)

class ENGINE_DLL CFontMgr 
{
	DECLARE_SINGLETON(CFontMgr)

private:
	explicit CFontMgr();
	virtual ~CFontMgr();

public:

	HRESULT ReadyResourceFont(LPDIRECT3DDEVICE9 pGraphicDev,
		const _tchar* pFontTag,
		const _tchar* pFontType,
		const _uint& iWidth,
		const _uint& iHeight,
		const _uint& iWeight);

	HRESULT		Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev,
		const _tchar* pFontTag,
		const _tchar* pFontType,
		const _uint& iWidth,
		const _uint& iHeight,
		const _uint& iWeight);

	void		Render_Font(const _tchar* pFontTag,
		const _tchar* pString,
		const _vec2* pPos,
		D3DXCOLOR Color);

	void		RenderFontCenter(const _tchar* pFontTag,
		const _tchar* pString,
		const _vec2* pStartPos,
		const _vec2* pEndPos,
		D3DXCOLOR Color);

private:
	map<const _tchar*, shared_ptr<CMyFont>>			m_mapFont;

private:
	shared_ptr<CMyFont> Find_Font(const _tchar* pFontTag);

public:
	virtual		void	Free();
};

END
