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

	// 상호작용 동작
	virtual void	Interaction();

	// 상호작용 종료 체크 함수
	virtual _bool	IsFinish();

	// 상호작용 종료 시 호출되는 함수
	virtual void	FinishInteraction();

	virtual void	ChangeTexture();

private:

	_float		m_fActiveTime = CURIOACTIVETIME;
	_bool		m_bActive = false;

	EState m_eAnimState = EState::IDLE;	// 애니메이션 변경을 위한 상태값
	EState m_eCurAnimState = EState::IDLE;	// 애니메이션 변경을 위한 상태값
	EState m_ePrevAnimState = EState::IDLE;	// 애니메이션 변경을 위한 상태값
	tstring		m_strAnimKey = L"";				// 애니메이션 키
	tstring		m_strEffectKey = L"";				// 이펙트 키


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

