#include "FontMgr.h"

IMPLEMENT_SINGLETON(CFontMgr)

CFontMgr::CFontMgr()
{
}

CFontMgr::~CFontMgr()
{
	Free();
}

HRESULT CFontMgr::ReadyResourceFont(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFontTag, const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight)
{
	std::shared_ptr<CMyFont> pFont = nullptr;

	pFont = Find_Font(pFontTag);

	if (nullptr != pFont)
		return E_FAIL;

	pFont = make_shared<CMyFont>(pGraphicDev);
	pFont->ReadyResourceFont(pFontTag, pFontType, iWidth, iHeight, iWeight);
	m_mapFont.insert({ pFontType, pFont });

	return S_OK;

}

HRESULT CFontMgr::Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFontTag, const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight)
{
	std::shared_ptr<CMyFont> pFont = nullptr;

	pFont = Find_Font(pFontTag);

	if (nullptr != pFont)
		return E_FAIL;

	pFont = CMyFont::Create(pGraphicDev, pFontType, iWidth, iHeight, iWeight);
	m_mapFont.insert({ pFontTag, pFont });

	return S_OK;
}

void CFontMgr::Render_Font(const _tchar* pFontTag, const _tchar* pString, const _vec2* pPos, D3DXCOLOR Color)
{
	std::shared_ptr<CMyFont> pMyFont = Find_Font(pFontTag);
	if (nullptr == pMyFont)
		return;

	pMyFont->Render_Font(pString, pPos, Color);
}

void CFontMgr::RenderFontCenter(const _tchar* pFontTag, const _tchar* pString, const _vec2* pStartPos, const _vec2* pEndPos, D3DXCOLOR Color)
{
	std::shared_ptr<CMyFont> pMyFont = Find_Font(pFontTag);
	if (nullptr == pMyFont)
		return;

	pMyFont->RenderFontCenter(pString, pStartPos, pEndPos, Color);
}

shared_ptr<CMyFont> CFontMgr::Find_Font(const _tchar* pFontTag)
{
	auto		iter = find_if(m_mapFont.begin(), m_mapFont.end(), CTag_Finder(pFontTag));

	if (iter == m_mapFont.end())
		return nullptr;

	return iter->second;
}

void CFontMgr::Free()
{
	//for_each(m_mapFont.begin(), m_mapFont.end(), CDeleteMap());
	//m_mapFont.clear();
}
