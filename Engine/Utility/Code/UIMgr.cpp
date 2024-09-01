#include "UIMgr.h"
#include "UIObj.h"
#include "GameObject.h"

#include"SceneMgr.h"

 IMPLEMENT_SINGLETON(CUIMgr)

CUIMgr::CUIMgr()
{
}

CUIMgr::~CUIMgr()
{
}

void CUIMgr::AddUIObject(tstring _strKeyName, shared_ptr<CUIObj> _pObj)
{

	if (m_UIList.end() == m_UIList.find(_strKeyName))
		m_UIList.insert({ _strKeyName ,_pObj });

}

shared_ptr<CUIObj> CUIMgr::GetUIObject(tstring _strKeyName)
{
	auto iter = m_UIList.find(_strKeyName);

	return iter->second;
}

void CUIMgr::SceneUIInitialize()
{
 	for (auto& iter : m_UIList) {
		CSceneMgr::GetInstance()->AddNewObject(L"Layer_2_UI", iter.first, dynamic_pointer_cast<CGameObject>(iter.second));
		if (L"UI_Inventory" == iter.first) {
			iter.second->SetVisible(true);
		}
	
	}
}

void CUIMgr::SceneChange()
{
	for (auto iter = m_UIList.begin(); iter != m_UIList.end(); ) {
		if (L"UI_Inventory" != iter->first && L"UI_Narration" != iter->first &&
			L"UI_Mouse" != iter->first && L"UI_Player_FPSUI" != iter->first && L"UI_TextBoard" != iter->first &&
			L"Obj_DescriptionUI" != iter->first && L"UI_InteractionInfo" != iter->first && L"UI_ScreenEffect" != iter->first)
		{
			m_UIList.erase(iter++);
 		}
		else {
			++iter;
		}
	}
}

void CUIMgr::AllVisibleOn()
{
	for (auto& iter : m_UIList) {
		iter.second->SetVisible(true);
	}
}

void CUIMgr::AllVisibleOff()
{
	for (auto& iter : m_UIList) {
		iter.second->SetVisible(false);
	}
}

void CUIMgr::SelectUIVisibleOn(tstring _strKeyName)
{
	auto iter = m_UIList.find(_strKeyName);

	if (iter != m_UIList.end())
		(*iter).second->SetVisible(true);

}

void CUIMgr::SelectUIVisibleOff(tstring _strKeyName)
{
	auto iter = m_UIList.find(_strKeyName);

	if (iter != m_UIList.end())
		(*iter).second->SetVisible(false);

}

void CUIMgr::NarrationOn(tstring _strKeyName)
{

	auto iter = m_UIList.find(L"UI_Narration");

	if (iter != m_UIList.end())
		(*iter).second->ActiveUI(_strKeyName);

}

void CUIMgr::TextBoardOn(tstring _strText, _vec3 _vPos, _vec3 _vScale, _float _fActTime)
{
	auto iter = m_UIList.find(L"UI_TextBoard");

	if (iter != m_UIList.end())
		(*iter).second->ActiveUI(_strText, _vPos, _vScale, _fActTime);
}

void CUIMgr::RemoveAllListElements()
{
	m_UIList.clear();
}

shared_ptr<CUIObj> CUIMgr::FindUI(tstring _strKeyName)
{
	auto iter = m_UIList.find(_strKeyName);
	if (m_UIList.end() != iter){
		return iter->second;
	}

	return nullptr;

}

_bool CUIMgr::PickingUI(LONG _fX, LONG _fY, _bool _bLB)
{
	for (auto& iter : m_UIList) {
		if (iter.second->GetVisible()) {
			RECT rc = iter.second->GetUIRect();

			if (rc.left <= _fX && rc.right >= _fX
				&& rc.top <= _fY && rc.bottom >= _fY) {
				iter.second->PickingUI(_fX, _fY ,true);
				return true;
			}
		}
	}
	return false;
}

_bool CUIMgr::PickingUIRB(LONG _fX, LONG _fY, _bool _bLB)
{

	auto iter = m_UIList.find(L"UI_Inventory");

	if (m_UIList.end() != iter) {
		if (iter->second->GetVisible()) {
			RECT rc = iter->second->GetUIRect();

			if (rc.left <= _fX && rc.right >= _fX
				&& rc.top <= _fY && rc.bottom >= _fY) {
				iter->second->PickingUI(_fX, _fY, false);
				return true;
			}
		}
	}

	return false;
}

void CUIMgr::HoverUI(LONG _fX, LONG _fY)
{
	for (auto& iter : m_UIList) {
		if (iter.second->GetVisible()) {
			RECT rc = iter.second->GetUIRect();

			if (rc.left <= _fX && rc.right >= _fX
				&& rc.top <= _fY && rc.bottom >= _fY) {
				iter.second->HoverUI(_fX, _fY);
			}
		}
	}
}

void CUIMgr::SetDescription(shared_ptr<CGameObject> _pGameObject)
{
	auto iter = m_UIList.find(L"Obj_DescriptionUI");

	(*iter).second->SetVisible(true);
	(*iter).second->ActiveUI(_pGameObject);
}

void CUIMgr::SetDescription(shared_ptr<CGameObject> _pGameObject, _vec3 _vPos)
{
	auto iter = m_UIList.find(L"Obj_DescriptionUI");


	(*iter).second->SetVisible(true);
	(*iter).second->ActiveUI(_pGameObject, _vPos);
}

void CUIMgr::SetDescription(tstring _strText, _vec3 _vPos)
{
	auto iter = m_UIList.find(L"Obj_DescriptionUI");

	(*iter).second->SetVisible(true);
	(*iter).second->ActiveUI(_strText, _vPos);
}

void CUIMgr::SwitchFPSMode()
{
	for (auto& iter : m_UIList) {
		if (L"UI_Player_FPSUI" == iter.first) {
			iter.second->SetVisible(true);
		}

		if (L"UI_Inventory" == iter.first) {
			iter.second->SetVisible(false);
		}
	}


}
