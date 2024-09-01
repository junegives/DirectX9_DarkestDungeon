#pragma once
#include"GameObject.h"
#include"Transform.h"
#include"RcTex.h"

BEGIN(Engine)

class ENGINE_DLL CUIObj : public CGameObject
{
public:
	explicit CUIObj(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CUIObj();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

protected:
	void			AddComponent();


public:

	void SetSize(_vec3 _vSize) { m_vSize = _vSize; }
	_vec3 GetSize() { return m_vSize; }

	void SetVisible(_bool _bValue) { m_bVisible = _bValue; }
	_bool GetVisible() { return m_bVisible; }

	RECT GetUIRect() { return m_UIRect; }

	virtual void PickingUI(LONG _fX, LONG _fY , _bool _bLB) {};
	virtual void HoverUI(LONG _fX, LONG _fY) {};

	virtual void ActiveUI(tstring _strKey) {};
	virtual void ActiveUI(tstring _strKey, _vec3 _vPos) {};
	virtual void ActiveUI(tstring _strKey, _vec3 _vPos, _vec3 _vScale, _float _fActTime) {};
	virtual void ActiveUI(shared_ptr<CGameObject> _pGameObject) {};
	virtual void ActiveUI(shared_ptr<CGameObject> _pGameObject, _vec3 _vPos) {};

protected:

	void CalculateUIRect();

protected:

	shared_ptr<CTransform> m_pTransCom = nullptr;
	shared_ptr<CRcTex> m_pRcTexCom = nullptr;

	//UI Width(x), Height(y) 로컬상태에서의 크기가 2이기때문에 원래 크기의 1/2의 값으로 넣음
	//각 UI ReadyObject() 에서 세팅해주기
	_vec3 m_vSize = { 0.f, 0.f, 0.f};
	_bool m_bVisible = true;

	RECT m_UIRect;

	_bool	m_bReady = false;


};

END