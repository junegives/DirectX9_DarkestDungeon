#pragma once

#include"Engine_Define.h"
#include"UIObj.h"


BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;

END

class CBackGround : public CUIObj
{
public:
	explicit CBackGround(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CBackGround();


public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;


public:
	void PosChange(_float fTimeDelta);
	void SetLoadingState() { m_bLoadingDone = true; }

protected:
	virtual void AddComponent();

private:
	shared_ptr<CTexture> m_pTextureCom = nullptr;

	shared_ptr<CTransform> m_pTitleTransCom = nullptr;
	shared_ptr<CRcTex> m_pTitleRcTexCom = nullptr;
	shared_ptr<CAnimator> m_pAnimCom = nullptr;

private:

	_bool m_bLoadingDone = false;
	_float m_fTotalRange = 0.f;
	_bool m_bSetDone = false;

	_bool m_bAnimDone = false;

	tstring m_strCurrentAnim = L"";
};

