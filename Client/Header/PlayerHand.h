#pragma once

#include"GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;

END

class CPlayerHand : public CGameObject
{

public:
	explicit CPlayerHand(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CPlayerHand();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:
	//���� ��� �ִ� �������� ����
	void SetCurrentItem(EHandItem _handItem);
	void CreateProjection();

private:
	void			AddComponent();

public:
	//�÷��̾ ������ value�� true�� �ٲ� y�� ��ġ�� ������ �����̴� ȿ���� �ִ� �Լ�
	void ShakingHand() {
		m_bWalking = true; 
	}

	//�÷��̾ ����� value�� true�� �ٲ� y�� ��ġ�� ������ �����̴� ȿ���� �ִ� �Լ�
	void StopShakingHand() {
		m_bWalking = false;
	}

	_bool m_bWalking = false;

private:
	//�÷��̾� Transform
	shared_ptr<CTransform> m_pPlrTransmCom = nullptr;

	shared_ptr<CTransform> m_pItemTransmCom = nullptr;
	shared_ptr<CRcTex> m_pItemBufCom = nullptr;
	shared_ptr<CTexture> m_pTextureCom = nullptr;

	shared_ptr<CTransform> m_pEffectTransmCom = nullptr;
	shared_ptr<CRcTex> m_pEffectBufCom = nullptr;
	shared_ptr<CTexture> m_pEffectTextureCom = nullptr;

	shared_ptr<CAnimator> m_pAnimCom = nullptr;

	//LateUpdate���� �������� ����ֱ� ������ deltaTime�� ������ ���� �޾Ƽ� �Ѱ���
	_float m_fTime = 0.f;
	//��/�Ʒ��� ��Ģ������ ���������ϱ� ������ Ư�� �ð��� ������ -1�� ������ �ݴ� �������� ������ �� �ְ��Ѵ�
	_float m_CurrentDir = 1.f;
	//Ư���ð����� �� �Ʒ��� ��
	_float m_fActTime = 0.f;

	//���� ���̸� ����ؼ� ���� ���갪���� ���
	_float m_fTotalHeight = 0.f;

	_bool m_bPlrSpellHand = false;

	_bool m_bReady = false;

	_bool m_bShoot = false;
	_float m_fEffectTime = 0.f;

};



