#pragma once

#include"Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture : public CComponent
{
public:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev , tstring _strName);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture();

public:
	void		SetTexture(const _uint& iIndex = 0);
	//어떤 텍스쳐를 사용할건지 결정 or 초기 세팅
	HRESULT	SetTextureKey(const tstring& _keyName, TEXTUREID _eID);


private:
	vector<shared_ptr<tagTextureInfo>>*		m_vecTexture;


private:
	virtual void Free();

};

END
