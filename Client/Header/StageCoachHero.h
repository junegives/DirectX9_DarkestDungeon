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

	// ��ȣ�ۿ� ����
	virtual void	Interaction();

	// ��ȣ�ۿ� ���� üũ �Լ�
	virtual _bool	IsFinish();

	// ��ȣ�ۿ� ���� �� ȣ��Ǵ� �Լ�
	virtual void	FinishInteraction();

	virtual void	ChangeTexture();

private:
	EHeroType		m_eHeroType = EHeroType::ENUM_END;
	tstring			m_strKey;

	shared_ptr<CAnimator> m_pAnimatorCom = nullptr;
};

