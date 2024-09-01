#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainTex : public CVIBuffer
{
public:
	explicit CTerrainTex();
	explicit CTerrainTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrainTex(const CTerrainTex& rhs);
	virtual ~CTerrainTex();

public:
	const _vec3* Get_VtxPos() const { return m_pPos; }
	_ulong		Get_VtxCntX() { return m_iH.biWidth; }
	_ulong		Get_VtxCntZ() { return m_iH.biHeight; }

public:
	HRESULT ReadyBuffer(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv, const tstring& _strPath, const _ulong& tileCntX, const _ulong& tileCntZ);
	virtual void RenderBuffer(void) override;


private:
	HANDLE				m_hFile;
	BITMAPFILEHEADER	m_fH;
	BITMAPINFOHEADER	m_iH;

	_vec3* m_pPos;

private:
	virtual void Free() override;

};

END