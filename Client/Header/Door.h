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

	// 상호작용 동작
	virtual void	Interaction();

	// 상호작용 종료 체크 함수
	virtual _bool	IsFinish();

	// 상호작용 종료 시 호출되는 함수
	virtual void	FinishInteraction();

	virtual void	ChangeTexture();

protected:
	EFacilityType m_eDoorType;
	_bool m_bIsFirstEnter = false;
};

