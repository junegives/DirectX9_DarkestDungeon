#pragma once

#include"Engine_Define.h"


BEGIN(Engine)

class CUIObj;
class CGameObject;

class ENGINE_DLL CUIMgr
{
	DECLARE_SINGLETON(CUIMgr)
private:

	explicit CUIMgr();
	virtual ~CUIMgr();

public:
	void AddUIObject(tstring _strKeyName, shared_ptr<CUIObj> _pObj);
	shared_ptr<CUIObj> GetUIObject(tstring _strKeyName);
	
public:

	void SceneUIInitialize();
	void SceneChange();

	
public:


	void AllVisibleOn();
	void AllVisibleOff();
	
	void SelectUIVisibleOn(tstring _strKeyName);
	void SelectUIVisibleOff(tstring _strKeyName);
	
	void NarrationOn(tstring _strKeyName);
	void TextBoardOn(tstring _strText, _vec3 _vPos, _vec3 _vScale, _float _fActTime);
	void RemoveAllListElements();

	shared_ptr<CUIObj> FindUI(tstring _strKeyName);

	// 현재 Visible이 true인 UI를 검사한 후, 그 UI만 대상으로 픽킹
	_bool PickingUI(LONG _fX, LONG _fY , _bool _bLB = true);
	_bool PickingUIRB(LONG _fX, LONG _fY, _bool _bLB = false);

	void HoverUI(LONG _fX, LONG _fY);

	// 호버창 세팅 함수
	void SetDescription(shared_ptr<CGameObject> _pGameObject);
	void SetDescription(shared_ptr<CGameObject> _pGameObject, _vec3 _vPos);
	void SetDescription(tstring _strText, _vec3 _vPos);

	//플레이어 FPS(보스전)모드 전환 UI

	void SwitchFPSMode();

private:

	map<tstring, shared_ptr<CUIObj>> m_UIList;

};


END
