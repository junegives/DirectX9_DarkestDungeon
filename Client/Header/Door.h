#pragma once
#include "InteractionObj.h"

class CDoor : public CInteractionObj
{
public:
	CDoor(LPDIRECT3DDEVICE9 pGraphicDev, EFacilityType _eDoorType);
	CDoor(const CDoor& rhs);
	virtual ~CDoor();

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

protected:
	EFacilityType m_eDoorType;
	_bool m_bIsFirstEnter = false;
};

