#pragma once
#include "InteractionObj.h"

class CRuin_Curio_Statue;

class CRuin_Curio_Sarcophagus : public CInteractionObj
{
public:
	CRuin_Curio_Sarcophagus(LPDIRECT3DDEVICE9 pGraphicDev);
	CRuin_Curio_Sarcophagus(const CRuin_Curio_Sarcophagus& rhs);
	virtual ~CRuin_Curio_Sarcophagus();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

public:

	void InsertStatue(shared_ptr<CRuin_Curio_Statue> pStatue) {
		if (m_StatueVec.size() < 3) {
			m_StatueVec.push_back(pStatue);
		}
	}
	
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


	_bool m_bPuzzleDone = false;
	_bool m_bAnimComplete = false;
	_bool m_bEffectDone = false;
	_bool m_bEffectDone2 = false;

	_bool m_bIsUION = false;

	_float m_fSceneTime = 0.f;

	_float m_fDebTime = 0.f;
	_bool m_bDebounce = false;

	vector<shared_ptr<CRuin_Curio_Statue>> m_StatueVec;

};

