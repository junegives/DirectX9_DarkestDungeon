#pragma once

#include"InteractionObj.h"
#include "Light.h"

enum class StatueState {
	STATUE_EMPTY,
	STATUE_RED,
	STATUE_BLUE,
	STATUE_GREEN,
	ENUM_END
};

class CRuin_Curio_Statue : public CInteractionObj
{
public:
	explicit CRuin_Curio_Statue(LPDIRECT3DDEVICE9 _pGraphicDev, StatueState _eAnswerState);
	virtual ~CRuin_Curio_Statue();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

public:

	_bool GetIsAnswer() { return m_bIsAnswer; }
	void SetTabInteractionKeyState() { m_bTabInteractionKey = true; }

protected:
	virtual void			AddComponent();
	virtual void			GetInteractionKey(const _float& fTimeDelta);

protected:
	virtual void	Interaction();
	virtual _bool	IsFinish();
	virtual void	ChangeTexture();


private:

	StatueState m_eAnswerState = StatueState::STATUE_EMPTY;
	StatueState m_eCurrentState = StatueState::STATUE_EMPTY;

	_bool m_bIsAnswer = false;
	_bool m_bDebounce = false;

	_float m_fDebounceTime = 0.f;


	shared_ptr<CLight> m_pLight = nullptr;

};

