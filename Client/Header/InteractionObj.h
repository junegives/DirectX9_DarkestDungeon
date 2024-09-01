#pragma once

#include"GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;

END

class CInteractionObj : public CGameObject
{
public:
	CInteractionObj(LPDIRECT3DDEVICE9 pGraphicDev);
	CInteractionObj(const CInteractionObj& rhs);
	virtual ~CInteractionObj();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

protected:
	virtual void			AddComponent();

protected:
	// 플레이어와 Object의 거리를 계산하는 함수
	_float	CalcDistance();

	// 상호작용 키 UI를 띄우고 키를 입력받는 함수
	virtual void	GetInteractionKey(const _float& fTimeDelta);

	// 상호작용 동작
	virtual void	Interaction() = 0;

	// 상호작용 종료 체크 함수
	virtual _bool	IsFinish() = 0;

	// 상호작용 종료 시 호출되는 함수
	virtual void	FinishInteraction();

	virtual void	ChangeTexture() = 0;

public:
	// 상호작용 중인지 확인하는 함수
	virtual _bool	IsInteracting() { return m_bInteracting; }

	// 외부에서 상호작용 상태를 시작 or 종료시키는 함수
	virtual void	SetInteracting(_bool _bInteracting) { m_bInteracting = _bInteracting; }

protected:
	shared_ptr<CRcTex> m_pBufferCom = nullptr;
	shared_ptr<CTexture> m_pTextureCom = nullptr;
	shared_ptr <CTransform> m_pTransformCom = nullptr;
	shared_ptr<CAnimator> m_pAnimatorCom = nullptr;

	// 플레이어의 위치를 받아오기 위한 플레이어 오브젝트
	shared_ptr<CGameObject> m_pPlayer = nullptr;
	shared_ptr<CTransform> m_pPlayerTransformCom = nullptr;

	// 거리 안에 있는지
	_bool	m_bCanInteract = false;

	// 상호작용 상태를 저장하는 변수
	_bool	m_bInteracting = false;

	tstring m_bInteractionKey = L"";
	tstring m_bInteractionInfo = L"";

	_bool	m_bTabInteractionKey = false;

};