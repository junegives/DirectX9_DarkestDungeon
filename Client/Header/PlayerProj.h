#pragma once

#include"GameObject.h"
#include "ParticleSystem.h"

struct tagProjInfo {
	_int iDamage;
	_int iSpeed;
};


BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;

END

class CEffect;

class CPlayerProj : public CGameObject{

public:

	explicit CPlayerProj(LPDIRECT3DDEVICE9 _pGraphicDev , tstring _strKeyName, _vec3 _vInitPos , _matrix _matPlrWorld);
	virtual ~CPlayerProj();


public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;


public:
	_int GetProjDamage() { return m_tProjInfo.iDamage; }
	_int GetProjSpeed() { return m_tProjInfo.iSpeed; }


private:
	virtual void			AddComponent();
	virtual void			OnCollide(shared_ptr<CGameObject> _pObj) override;

private:
	shared_ptr<CTransform> m_pTransmCom = nullptr;
	shared_ptr<CRcTex> m_pRcTexCom = nullptr;
	shared_ptr<CTexture> m_pTextureCom = nullptr;
	shared_ptr<CCollider> m_pColliderCom = nullptr;


	tstring m_strAnimKeyName;
	tstring m_strEffectAnimKey;

	tagProjInfo m_tProjInfo;

	_vec3	m_vStartPos = { 0.f, 0.f, 0.f };
	_matrix m_matWorld;
	_vec3	m_vLook;

	_bool	m_bTestBool = true;

	shared_ptr<CParticleSystem> m_pParticle = nullptr;

	_float	m_fTimeDelta = 0.f;

	_float m_fTotalRange = 0.f;

};