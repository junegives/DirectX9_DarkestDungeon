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
	//true�� ��� �ʵ� ���� ��ġ�� �������̰�, false�� �� �κ��丮�� �� ������
	void SetOnField(_bool _bValue) { 
 		m_bOnField = _bValue; 
	}
	_bool GetOnFieldState() { return m_bOnField; }

	void SetOnStore(_bool _bValue)
	{
		m_bOnStore = _bValue;
	}
	_bool	IsOnStore() { return m_bOnStore; }

	// �ʵ忡 ������ �� �۷ο� ��ƼŬ ����
	void StartParticle();

	//���Ͱ� �װ� �������� ����� �� ������, �ؽ��� ���� �������� �� ����(�ʼ��� ���� ��)
	void SetDropItemInfo(_vec3 _vPos, const tstring& _strName , _int _iAmount = 1);
	void SetScale(_vec3 _vScale);

	void SetAmount(_int _iAmount) { m_iAmount = _iAmount; }
	_int GetAmount() { return m_iAmount; }

	void SetMoney(_int _iMoney) { m_iMoney = _iMoney; }
	_int GetMoney() { return m_iMoney; }

	void UpdateInventory(_vec3 _vPos);

	//�÷��� ������ �ؽ��� Ű���� ���� UI �ؽ��� Ű���� ��ȯ����
	void GetUITextureKeyName(const tstring& _strOriginName);
	tstring GetItemKeyName() { return m_strItemKey; }
	
	EHandItem GetItemTypeEnum() { return m_eItemState; }
	void	SetItemTypeEnum(EHandItem _eItemState) { m_eItemState = _eItemState; }

	virtual void PickingObj() override;
	
	//void KeyInput();

protected:
	void AddComponent();
	//�ʵ忡 �������� �� �Ʒ��� �յ� ����ִ� ȿ��
	void FloatingOnField();

private:
	//Componenet
	shared_ptr<CTransform> m_pTransCom = nullptr;
	shared_ptr<CRcTex> m_pBufCom = nullptr;
	shared_ptr<CTexture> m_pTextureCom = nullptr;

private:
	//Member variable
	tstring m_strItemKey = L"";
	_bool	m_bOnField = true; // �ʵ忡 �ִ� ���������� �κ��丮�� �ִ� ���������� �ĺ�
	_bool	m_bOnStore = false; // �������� �Ǹ����� ���������� �ĺ�
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

