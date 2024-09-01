#include "VIBuffer.h"

CVIBuffer::CVIBuffer()
{
}

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev), m_pVB(nullptr), m_pIB(nullptr),
	m_dwVtxSize(0), m_dwFVF(0), m_dwVtxCnt(0), m_dwTriCnt(0)
	, m_dwIdxSize(0)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent(rhs), m_pVB(rhs.m_pVB), m_pIB(rhs.m_pIB)
{
}

CVIBuffer::~CVIBuffer()
{
}

HRESULT CVIBuffer::ReadyBuffer(void)
{

	if (FAILED(m_pGraphicDev->CreateVertexBuffer(m_dwVtxCnt * m_dwVtxSize,
		0,
		m_dwFVF,
		D3DPOOL_MANAGED,
		&m_pVB,
		nullptr))) {

		//MSG_BOX("Create Vertex Buffer Failed");
		return E_FAIL;
	}


	if (FAILED(m_pGraphicDev->CreateIndexBuffer(m_dwIdxSize * m_dwTriCnt,
		0,
		m_IdxFmt,
		D3DPOOL_MANAGED,
		&m_pIB,
		nullptr))) {

		//MSG_BOX("Create Index Buffer Failed");
		return E_FAIL;
	}

	return S_OK;
}

void CVIBuffer::RenderBuffer(void)
{
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);

	m_pGraphicDev->SetFVF(m_dwFVF);
	m_pGraphicDev->SetIndices(m_pIB);

	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);

}

void CVIBuffer::Free()
{
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

}
