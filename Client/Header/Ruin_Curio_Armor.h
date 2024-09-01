#pragma once
#include "InteractionObj.h"

class CRuin_Curio_Armor : public CInteractionObj
{
public:
	CRuin_Curio_Armor(LPDIRECT3DDEVICE9 pGraphicDev);
	CRuin_Curio_Armor(const CRuin_Curio_Armor& rhs);
	virtual ~CRuin_Curio_Armor();

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

