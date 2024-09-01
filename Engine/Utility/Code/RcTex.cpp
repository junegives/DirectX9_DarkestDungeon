#include "RcTex.h"

CRcTex::CRcTex(const CRcTex& rhs)
	: CVIBuffer(rhs)
{
}

CRcTex::CRcTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CRcTex::~CRcTex()
{
}

HRESULT CRcTex::ReadyBuffer(void)
{
	m_dwVtxCnt = 4;
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwTriCnt = 2;
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::ReadyBuffer()))
		return E_FAIL;

	VTXTEX* pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	_vec3		vDst, vSrc, vNormal;


	// 위쪽
	pVertex[0].vPosition = { -1.f, 1.f, 0.f };
	pVertex[0].vTexUV = { 0.f, 0.f };

	pVertex[1].vPosition = { 1.f, 1.f, 0.f };
	pVertex[1].vTexUV = { 1.f, 0.f };

	pVertex[2].vPosition = { 1.f, -1.f, 0.f };
	pVertex[2].vTexUV = { 1.f, 1.f };

	pVertex[3].vPosition = { -1.f, -1.f, 0.f };
	pVertex[3].vTexUV = { 0.f, 1.f };

	m_pVB->Unlock();

	INDEX16* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// 오른쪽 위
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	vDst = pVertex[pIndex[0]._1].vPosition - pVertex[pIndex[0]._0].vPosition;
	vSrc = pVertex[pIndex[0]._2].vPosition - pVertex[pIndex[0]._1].vPosition;
	D3DXVec3Cross(&vNormal, &vDst, &vSrc);

	pVertex[pIndex[0]._0].vNormal += vNormal;
	pVertex[pIndex[0]._1].vNormal += vNormal;
	pVertex[pIndex[0]._2].vNormal += vNormal;

	// 왼쪽 아래
	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	vDst = pVertex[pIndex[1]._1].vPosition - pVertex[pIndex[1]._0].vPosition;
	vSrc = pVertex[pIndex[1]._2].vPosition - pVertex[pIndex[1]._1].vPosition;
	D3DXVec3Cross(&vNormal, &vDst, &vSrc);

	pVertex[pIndex[1]._0].vNormal += vNormal;
	pVertex[pIndex[1]._1].vNormal += vNormal;
	pVertex[pIndex[1]._2].vNormal += vNormal;


	for (_ulong i = 0; i < m_dwVtxCnt; ++i)
		D3DXVec3Normalize(&pVertex[i].vNormal, &pVertex[i].vNormal);

	m_pIB->Unlock();



	return S_OK;
}

void CRcTex::RenderBuffer(void)
{
	CVIBuffer::RenderBuffer();

}

void CRcTex::Free()
{
	CVIBuffer::Free();
}
