#pragma once

#include"Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer : public CComponent
{
protected:
	explicit CVIBuffer();
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev,
		_ulong _dwVtxCnt, _ulong _m_dwVtxSize);

	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer();

public:
	virtual HRESULT				ReadyBuffer(void);
	virtual void				RenderBuffer(void);

protected:
	LPDIRECT3DVERTEXBUFFER9			m_pVB;
	LPDIRECT3DINDEXBUFFER9			m_pIB;

	_ulong				m_dwVtxCnt;			// ������ ������ ����
	_ulong				m_dwVtxSize;		// ������ ũ��
	_ulong				m_dwTriCnt;			// �׸����� �ϴ� �ﰢ���� ����
	_ulong				m_dwFVF;			// ���ؽ��� �Ӽ�

	_ulong				m_dwIdxSize;
	D3DFORMAT			m_IdxFmt;

protected:
	virtual void		Free();


};


END