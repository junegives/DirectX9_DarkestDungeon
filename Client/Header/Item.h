#pragma once

#include"GameObject.h"
#include "GlowParticle.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;
class CCollider;

END

class CItem : public CGameObject
{
public:
	explicit CItem(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CItem();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

	virtual void OnCollide(shared_ptr<CGameObject> _pObj) override;

public:
	//true일 경우 필드 위에 배치된 아이템이고, false일 시 인벤토리에 들어간 아이템
	void SetOnField(_bool _bValue) { 
 		m_bOnField = _bValue; 
	}
	_bool GetOnFieldState() { return m_bOnField; }

	void SetOnStore(_bool _bValue)
	{
		m_bOnStore = _bValue;
	}
	_bool	IsOnStore() { return m_bOnStore; }

	// 필드에 떠있을 때 글로우 파티클 생성
	void StartParticle();

	//몬스터가 죽고 아이템이 드랍될 때 포지션, 텍스쳐 값을 지정해줄 수 있음(필수로 해줄 것)
	void SetDropItemInfo(_vec3 _vPos, const tstring& _strName , _int _iAmount = 1);
	void SetScale(_vec3 _vScale);

	void SetAmount(_int _iAmount) { m_iAmount = _iAmount; }
	_int GetAmount() { return m_iAmount; }

	void SetMoney(_int _iMoney) { m_iMoney = _iMoney; }
	_int GetMoney() { return m_iMoney; }

	void UpdateInventory(_vec3 _vPos);

	//플로팅 아이템 텍스쳐 키값을 통해 UI 텍스쳐 키값을 반환받음
	void GetUITextureKeyName(const tstring& _strOriginName);
	tstring GetItemKeyName() { return m_strItemKey; }
	
	EHandItem GetItemTypeEnum() { return m_eItemState; }
	void	SetItemTypeEnum(EHandItem _eItemState) { m_eItemState = _eItemState; }

	virtual void PickingObj() override;
	
	//void KeyInput();

protected:
	void AddComponent();
	//필드에 떠있을때 위 아래로 둥둥 띄워주는 효과
	void FloatingOnField();

private:
	//Componenet
	shared_ptr<CTransform> m_pTransCom = nullptr;
	shared_ptr<CRcTex> m_pBufCom = nullptr;
	shared_ptr<CTexture> m_pTextureCom = nullptr;

private:
	//Member variable
	tstring m_strItemKey = L"";
	_bool	m_bOnField = true; // 필드에 있는 아이템인지 인벤토리에 있는 아이템인지 식별
	_bool	m_bOnStore = false; // 상점에서 판매중인 아이템인지 식별
	_vec3	m_vPosition;
	EHandItem m_eItemState = EHandItem::ENUM_END;

	_int m_iAmount= 1;
	_int m_iMoney = 10000;

	_float m_fTime = 0.f;
	_float m_CurrentDir = 1.f;
	_float m_fActTime = 0.f;
	_float m_fTotalHeight = 0.f;

	shared_ptr<CGlowParticle> m_pParticle = nullptr;

};

