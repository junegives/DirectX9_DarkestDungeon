#include "Texture.h"
#include"ResourceMgr.h"

CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev) 
{
}

CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphicDev, tstring _strName)
	:CComponent(pGraphicDev)
{
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs) 
{
	size_t	iSize = rhs.m_vecTexture->size();
	m_vecTexture->reserve(iSize);

	m_vecTexture = rhs.m_vecTexture;
}

CTexture::~CTexture()
{
}

void CTexture::SetTexture(const _uint& iIndex)
{
	if (m_vecTexture->size() < iIndex)
		return;

	m_pGraphicDev->SetTexture(0, ((*m_vecTexture)[iIndex])->pTexture);
}

HRESULT CTexture::SetTextureKey(const tstring& _keyName, TEXTUREID _eID)
{

	m_vecTexture = CResourceMgr::GetInstance()->GetTexture(_keyName, _eID);

	if (m_vecTexture == nullptr) {
		return E_FAIL;
	}

	return S_OK;
}


void CTexture::Free()
{


}
