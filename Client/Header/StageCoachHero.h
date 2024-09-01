#pragma once
#include "InteractionObj.h"
class CStageCoachHero : public CInteractionObj
{
public:
	CStageCoachHero(LPDIRECT3DDEVICE9 pGraphicDev, EHeroType _eType);
	CStageCoachHero(const CStageCoachHero& rhs);
	virtual ~CStageCoachHero();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

public:
	EHeroType				GetHeroType() { return m_eHeroType; }

protected:
	virtual void			AddComponent();
	virtual void			GetInteractionKey(const _float& fTimeDelta);

protected:

	// 상호작용 동작
	virtual void	Interaction();

	// 상호작용 종료 체크 함수
	virtual _bool	IsFinish();

	// 상호작용 종료 시 호출되는 함수
	virtual void	FinishInteraction();

	virtual void	ChangeTexture();

private:
	EHeroType		m_eHeroType = EHeroType::ENUM_END;
	tstring			m_strKey;

	shared_ptr<CAnimator> m_pAnimatorCom = nullptr;
};

