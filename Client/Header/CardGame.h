#pragma once
#include "GameObject.h"
#include "FireworkParticle.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CCard;

class CCardGame :public CGameObject
{
public:
	explicit CCardGame(LPDIRECT3DDEVICE9 pGraphicDev);
	~CCardGame();

public:
	HRESULT			ReadyGameObject();
	_int			UpdateGameObject(const _float& fTimeDelta);
	void			LateUpdateGameObject();
	void			RenderGameObject();

protected:
	void AddComponent();

public:
	void	Start();

	void	GameStart() { m_bGameProcessing = true; }
	void	FlipCard(shared_ptr<CCard> _pCard);

public:

	void RandomCardShuffle();
	void PlaceCardOnBoard();


private:
	void	End();
	void	ResetGame();

private:
	vector<pair<_int, _int>> m_vAnswer;
	vector<shared_ptr<CCard>> m_vCardList;


	_int m_iFirstFlipNum = 99;
	_int m_iSecondFlipNum = 99;

	shared_ptr<CCard> m_pFirstFlipCard = nullptr;
	shared_ptr<CCard> m_pSecondFlipCard = nullptr;


	_bool m_bGameProcessing = false;
	_bool m_bInit = false;

	_bool m_bFreeze = false;
	_float m_fFreezeTime = 0.f;

	_bool m_bGameClear = false;
	_bool m_bClearSoundOn = false;

private:

	shared_ptr<CTransform>	m_pTransformCom = nullptr;
	shared_ptr<CRcTex>		m_pRCTexCom = nullptr;
	shared_ptr<CTexture>	m_pTextureCom = nullptr;

	shared_ptr<CFireworkParticle> m_pParticle1 = nullptr;
	shared_ptr<CFireworkParticle> m_pParticle2 = nullptr;

};

