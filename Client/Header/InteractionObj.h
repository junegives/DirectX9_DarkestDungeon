#pragma once

#include"GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;

END

class CInteractionObj : public CGameObject
{
public:
	CInteractionObj(LPDIRECT3DDEVICE9 pGraphicDev);
	CInteractionObj(const CInteractionObj& rhs);
	virtual ~CInteractionObj();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

protected:
	virtual void			AddComponent();

protected:
	// �÷��̾�� Object�� �Ÿ��� ����ϴ� �Լ�
	_float	CalcDistance();

	// ��ȣ�ۿ� Ű UI�� ���� Ű�� �Է¹޴� �Լ�
	virtual void	GetInteractionKey(const _float& fTimeDelta);

	// ��ȣ�ۿ� ����
	virtual void	Interaction() = 0;

	// ��ȣ�ۿ� ���� üũ �Լ�
	virtual _bool	IsFinish() = 0;

	// ��ȣ�ۿ� ���� �� ȣ��Ǵ� �Լ�
	virtual void	FinishInteraction();

	virtual void	ChangeTexture() = 0;

public:
	// ��ȣ�ۿ� ������ Ȯ���ϴ� �Լ�
	virtual _bool	IsInteracting() { return m_bInteracting; }

	// �ܺο��� ��ȣ�ۿ� ���¸� ���� or �����Ű�� �Լ�
	virtual void	SetInteracting(_bool _bInteracting) { m_bInteracting = _bInteracting; }

protected:
	shared_ptr<CRcTex> m_pBufferCom = nullptr;
	shared_ptr<CTexture> m_pTextureCom = nullptr;
	shared_ptr <CTransform> m_pTransformCom = nullptr;
	shared_ptr<CAnimator> m_pAnimatorCom = nullptr;

	// �÷��̾��� ��ġ�� �޾ƿ��� ���� �÷��̾� ������Ʈ
	shared_ptr<CGameObject> m_pPlayer = nullptr;
	shared_ptr<CTransform> m_pPlayerTransformCom = nullptr;

	// �Ÿ� �ȿ� �ִ���
	_bool	m_bCanInteract = false;

	// ��ȣ�ۿ� ���¸� �����ϴ� ����
	_bool	m_bInteracting = false;

	tstring m_bInteractionKey = L"";
	tstring m_bInteractionInfo = L"";

	_bool	m_bTabInteractionKey = false;

};