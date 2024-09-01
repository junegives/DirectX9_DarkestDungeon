#pragma once
#include "InteractionObj.h"

class CWeald_Curio_Tent : public CInteractionObj
{
public:
	CWeald_Curio_Tent(LPDIRECT3DDEVICE9 pGraphicDev);
	CWeald_Curio_Tent(const CWeald_Curio_Tent& rhs);
	virtual ~CWeald_Curio_Tent();

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

	_float		m_fActiveTime = CURIOACTIVETIME;
	_bool		m_bActive = false;

	EState m_eAnimState = EState::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	EState m_eCurAnimState = EState::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	EState m_ePrevAnimState = EState::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	tstring		m_strAnimKey = L"";				// �ִϸ��̼� Ű
	tstring		m_strEffectKey = L"";				// ����Ʈ Ű


};

