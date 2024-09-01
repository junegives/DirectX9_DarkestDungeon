#pragma once

#include "GameObject.h"


BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;

END

class CCardGame;


class CCard : public CGameObject
{
public:
	explicit CCard(LPDIRECT3DDEVICE9 _pGraphicDev, _int _iCardNum , _int _iCardImgNum);
	virtual ~CCard();

public:
	HRESULT			ReadyGameObject();
	_int			UpdateGameObject(const _float& fTimeDelta);
	void			LateUpdateGameObject();
	void			RenderGameObject();

protected: 
	void AddComponent();
	virtual void PickingObj();

public:

	//카드뒤집기 -> look을 축으로 잡고 돌리기 
	void FlipCard();

	void ResetFlip();


public:

	_bool GetIsCorrect() { return m_bIsCorrect; }
	_int GetCardNum() { return m_iCardNum; }
	_int GetCardImgNum() { return m_iCardImgNum; }

	void SetIsCorrect() { m_bIsCorrect = true; }

	void SetCardGame(shared_ptr<CCardGame> pCardGame) { m_pCardGame = pCardGame; }

private:
	_bool m_bIsCorrect = false;
	_bool m_bIsFlip = false;

	_int m_iCardNum = 99;
	_int m_iCardImgNum = 99;

private:
	shared_ptr<CTexture> m_pTextureCom = nullptr;
	shared_ptr<CTransform> m_pTransformCom = nullptr;
	shared_ptr<CRcTex> m_pRCtexCom = nullptr;

	weak_ptr<CCardGame> m_pCardGame;

};