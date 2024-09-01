#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CCollider;

END

class CEnvironmentObj : public CGameObject
{
public:
	CEnvironmentObj(LPDIRECT3DDEVICE9 pGraphicDev, const tstring& _KeyName, _bool _bColliding = false, _bool _bBillBoard = false, _int _iCnt = 1, _bool _bRandom = false);
	CEnvironmentObj(const CEnvironmentObj& rhs);
	virtual ~CEnvironmentObj();

	shared_ptr<CTransform> Get_TransformCom() { return m_pTransformCom; }

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

	virtual void OnCollide(shared_ptr<CGameObject> _pObj) override;
	_bool	IsHorizontal() { return m_bHorizontal; }

private:
	virtual void			AddComponent();

protected:
	shared_ptr<CRcTex> m_pBufferCom = nullptr;
	shared_ptr<CTexture> m_pTextureCom = nullptr;
	shared_ptr<CTransform> m_pTransformCom = nullptr;
	shared_ptr<CCollider> m_pColliderCom = nullptr;

	_int		m_iNum = 0;
	_int		m_iCnt = 1;
	_bool		m_bRandom = false;
	_bool		m_bBillBoard = false;
	tstring		m_strKeyName = L"";
	_bool		m_bHorizontal = true;
};
