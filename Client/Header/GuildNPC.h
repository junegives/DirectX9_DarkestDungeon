#pragma once
#include "InteractionObj.h"
#include "PickingGame.h"
#include "Hero.h"
#include "UIObj.h"

class CGuildNPC : public CInteractionObj
{
public:
	CGuildNPC(LPDIRECT3DDEVICE9 pGraphicDev);
	CGuildNPC(const CGuildNPC& rhs);
	virtual ~CGuildNPC();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

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
	shared_ptr<CPickingGame> m_pPickingGame = nullptr;
	vector<shared_ptr<CGameObject>>* m_pHeroVec = nullptr;

	vector<shared_ptr<CUIObj>> m_pUIVec;
	vector<shared_ptr<CGameObject>> m_vecGameObject;

	_bool m_bInteractionFirst = false;
};

