#include "MyFont.h"

CMyFont::CMyFont(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev), m_pSprite(nullptr), m_pFont(nullptr)
{
   // m_pGraphicDev->AddRef();

}

CMyFont::~CMyFont()
{
	//m_pGraphicDev->Release();
}

HRESULT CMyFont::ReadyResourceFont(const _tchar* pFontPath, const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight)
{
	if (FAILED(AddFontResourceEx(pFontPath, FR_PRIVATE, 0))) {
		MSG_BOX("Resource Font Create Failed");
		return E_FAIL;
	}

	D3DXFONT_DESC			Font_Desc;

	Font_Desc.CharSet = DEFAULT_CHARSET;
	lstrcpy(Font_Desc.FaceName, pFontType);

	Font_Desc.Width = iWidth;
	Font_Desc.Height = iHeight;
	Font_Desc.Weight = iWeight;

	if (FAILED(D3DXCreateFontIndirect(m_pGraphicDev, &Font_Desc, &m_pFont)))
		return E_FAIL;

	if (FAILED(D3DXCreateSprite(m_pGraphicDev, &m_pSprite)))
		return E_FAIL;

	return E_NOTIMPL;
}

HRESULT CMyFont::Ready_Font(const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight)
{
	D3DXFONT_DESC			Font_Desc;
	ZeroMemory(&Font_Desc, sizeof(D3DXFONT_DESC));

	Font_Desc.CharSet = HANGEUL_CHARSET;
	lstrcpy(Font_Desc.FaceName, pFontType);

	Font_Desc.Width = iWidth;
	Font_Desc.Height = iHeight;
	Font_Desc.Weight = iWeight;

	if(FAILED(D3DXCreateFontIndirect(m_pGraphicDev, &Font_Desc, &m_pFont)))
		return E_FAIL;

	if (FAILED(D3DXCreateSprite(m_pGraphicDev, &m_pSprite)))
		return E_FAIL;

	return S_OK;
}

void CMyFont::Render_Font(const _tchar* pString, const _vec2* pPos, D3DXCOLOR Color)
{
	RECT	rc = { (long)pPos->x, (long)pPos->y };

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_pFont->DrawTextW(m_pSprite, pString, lstrlen(pString), &rc, DT_NOCLIP , Color);

	m_pSprite->End();
}

void CMyFont::RenderFontCenter(const _tchar* pString, const _vec2* pStartPos, const _vec2* pEndPos, D3DXCOLOR Color)
{
	RECT	rc = { 0,0,0,0 };
	RECT	fontRc;

	m_pFont->DrawTextW(m_pSprite, pString, lstrlen(pString), &rc, DT_CALCRECT, Color);

	long fWidth = rc.right - rc.left;
	long fHeight = rc.bottom - rc.top;

	fontRc.left = pStartPos->x +((long)(pEndPos->x - fWidth) / 2);
	fontRc.top = pStartPos->y + ((long)(pEndPos->y - fHeight) / 2);
	fontRc.right = fontRc.left + fWidth;
	fontRc.bottom = fontRc.top + fHeight;

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_pFont->DrawTextW(m_pSprite, pString, lstrlen(pString), &fontRc, DT_NOCLIP | DT_CENTER, Color);

	m_pSprite->End();

}

shared_ptr<CMyFont> CMyFont::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight)
{
	std::shared_ptr<CMyFont> pInstance = std::make_shared<CMyFont>(pGraphicDev);


	if (FAILED(pInstance->Ready_Font(pFontType, iWidth, iHeight, iWeight)))
	{
		pInstance.reset();

		//MSG_BOX("MyFont Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMyFont::Free()
{
	//RemoveFontResourceEx()
	m_pFont->Release();
	m_pSprite->Release();
}
