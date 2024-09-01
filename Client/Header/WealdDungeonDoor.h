#pragma once
#include "InteractionObj.h"

class CWealdDungeonDoor : public CInteractionObj
{
public:
	CWealdDungeonDoor(LPDIRECT3DDEVICE9 pGraphicDev, EWealdDungeonDoorType _eDoorType);
	CWealdDungeonDoor(const CWealdDungeonDoor& rhs);
	virtual ~CWealdDungeonDoor();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

public:
	_int	GetCurrent() { return m_iCurrent; }

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
	shared_ptr<CRcTex> m_pBufferCom2 = nullptr;
	shared_ptr<CTexture> m_pTextureCom2 = nullptr;
	shared_ptr <CTransform> m_pTransformCom2 = nullptr;

	EWealdDungeonDoorType m_eDoorType;

private:
	_int	m_iCurrent = 10;

	_bool	m_bOpenStart = false;
	float	m_fOpenTime = 1.f;
	
};

