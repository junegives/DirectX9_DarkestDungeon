#pragma once

#include"Engine_Define.h"

BEGIN(Engine)

class CComponent;
class CCollider;

enum class EDescriptionType
{
	CONTENT,	// 내용
	ITEM,		// 제목 + 내용
	SKILL1,		// 위치, 타겟 텍스처 + 제목 + 부제목 + 내용
	SKILL2,		// 위치, 타겟 텍서츠 + 제목 + 부제목 + 내용 + 능력1 + 설명1
	SKILL3,		// 위치, 타겟 텍스처 + 제목 + 부제목 + 내용 + 능력1 + 설명1 + 능력2 + 설명2
	ENUM_END
};

struct tagDescription {
	EDescriptionType	m_eDescriptionType;

	tstring		m_strMainContent = L"";
	tstring		m_strTitle = L"";
	tstring		m_strSubTitle = L"";

	D3DCOLOR	m_tColor1 = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tstring		m_strColorTitle1 = L"";
	tstring		m_strColorContent1 = L"";

	D3DCOLOR	m_tColor2 = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tstring		m_strColorTitle2 = L"";
	tstring		m_strColorContent2 = L"";
};

class ENGINE_DLL CGameObject : public enable_shared_from_this<CGameObject>
{
public:
	explicit CGameObject();
	explicit CGameObject(LPDIRECT3DDEVICE9 _pGraphicDev);
	~CGameObject();

public:
	virtual void			AwakeGameObject();
	virtual HRESULT			ReadyGameObject() { return S_OK; }
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject() {}

public:
	_bool GetIsActive() { return m_bActive; }
	_bool GetIsEnable() { return m_bEnable; }

	void SetActive(_bool _bValue) { m_bActive = _bValue; }
	void SetEnable(_bool _bValue) { m_bEnable = _bValue; }

	void SetPos(_vec3 _vPos) { m_vPos = _vPos; }
	void SetScale(_vec3 _vScale) { m_vScale = _vScale; }
	_vec3 GetPos() { return m_vPos; }
	_vec3 GetScale() { return m_vScale; }
	void SetAngle(_vec3 _vAngle) { m_vAngle = _vAngle; }

	void	SetHorizontal(_bool _bHorizontal) { m_bHorizontal = _bHorizontal; }
	_bool IsHorizontal() { return m_bHorizontal; }

	//Get Collider Position(Picking)
	void GetMinMaxPos(_vec3& vMin, _vec3& vMax);
	void GetSphere(_vec3& _vCenter, _float& _fRadius);

	void SetColliding(_bool _bColliding) { m_bColliding = _bColliding; }
	_bool IsColliding() { return m_bColliding; }

	void SetColType(ECollideID _eType) { m_eCollideID = _eType; }
	ECollideID GetColType() { return m_eCollideID; }

	virtual void OnCollide(shared_ptr<CGameObject> _pObj) {}
	virtual void OnCollide(shared_ptr<CGameObject> _pObj, _float _fGap, EDirection _eDir) {}

	shared_ptr<tagDescription> GetDescription() { return m_tDescription; }

	void SetRenderState(_bool _bValue) {	m_bSetAlpha = _bValue;	}

public:
	void SetLock(_bool _bLock) { m_bLock = _bLock; }
	void	SetBillBoard(_matrix& _matWorld);
	void	SetFullScreenBillBoard(_matrix& _matWorld);
	std::shared_ptr<CComponent> GetComponent(const tstring& _strKeyName, COMPONENTID _eComID);

public:
	_float				Get_ViewZ() { return m_fViewZ; }
	void				Compute_ViewZ(const _vec3* pPos);

public:
	//PickingObj
	virtual void PickingObj() {}

	//Component Manage
protected:
	void RemoveComponent();
	void RenderObjAlpha(_int _iAlpha);

protected:
	virtual void AddComponent();

protected:
	void	SetMaterial();

protected:
	LPDIRECT3DDEVICE9 m_pGraphicDev;
	shared_ptr<CCollider> m_pColliderCom = nullptr;

	_bool m_bActive = true;
	_bool m_bEnable = true;

	_vec3 m_vPos = _vec3{ 0.f, 0.f, 0.f };
	_vec3 m_vAngle = _vec3{ 0.f, 0.f, 0.f };
	_vec3 m_vScale = _vec3{ 1.f, 1.f, 1.f };

	_float			m_fViewZ;

	_bool m_bLock = false;

	_bool m_bColliding = false;
	ECollideID m_eCollideID = ECollideID::ENUM_END;

	_bool m_bHorizontal = false;

	_vec2 m_vOriginSize = {0.f, 0.f};

	shared_ptr<tagDescription> m_tDescription;
	//알파값을 조절해서 렌더하는지의 여부
	_bool m_bSetAlpha = false;

protected:
	map<tstring, std::shared_ptr<CComponent>> m_mapComponent[ID_END];

};

END
