#pragma once

#include"GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;

END

class CPlayerHand : public CGameObject
{

public:
	explicit CPlayerHand(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CPlayerHand();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:
	//현재 들고 있는 아이템을 변경
	void SetCurrentItem(EHandItem _handItem);
	void CreateProjection();

private:
	void			AddComponent();

public:
	//플레이어가 걸을시 value를 true로 바꿔 y값 위치를 조정해 움직이는 효과를 주는 함수
	void ShakingHand() {
		m_bWalking = true; 
	}

	//플레이어가 멈출시 value를 true로 바꿔 y값 위치를 조정해 움직이는 효과를 주는 함수
	void StopShakingHand() {
		m_bWalking = false;
	}

	_bool m_bWalking = false;

private:
	//플레이어 Transform
	shared_ptr<CTransform> m_pPlrTransmCom = nullptr;

	shared_ptr<CTransform> m_pItemTransmCom = nullptr;
	shared_ptr<CRcTex> m_pItemBufCom = nullptr;
	shared_ptr<CTexture> m_pTextureCom = nullptr;

	shared_ptr<CTransform> m_pEffectTransmCom = nullptr;
	shared_ptr<CRcTex> m_pEffectBufCom = nullptr;
	shared_ptr<CTexture> m_pEffectTextureCom = nullptr;

	shared_ptr<CAnimator> m_pAnimCom = nullptr;

	//LateUpdate에서 포지션을 잡아주기 때문에 deltaTime을 변수에 따로 받아서 넘겨줌
	_float m_fTime = 0.f;
	//위/아래로 규칙적으로 움직여야하기 때문에 특정 시간이 지나면 -1을 곱해줘 반대 방향으로 움직일 수 있게한다
	_float m_CurrentDir = 1.f;
	//특정시간마다 위 아래로 움
	_float m_fActTime = 0.f;

	//현재 높이를 기록해서 다음 연산값으로 사용
	_float m_fTotalHeight = 0.f;

	_bool m_bPlrSpellHand = false;

	_bool m_bReady = false;

	_bool m_bShoot = false;
	_float m_fEffectTime = 0.f;

};



