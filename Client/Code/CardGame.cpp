#include "pch.h"
#include "CardGame.h"
#include "Card.h"

#include"Export_Utility.h"

#include"CameraMgr.h"
#include"SoundMgr.h"
#include "ParticleMgr.h"

#include"Player.h"

CCardGame::CCardGame(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CCardGame::~CCardGame()
{
}

HRESULT CCardGame::ReadyGameObject()
{

	m_vCardList.reserve(10);
	m_vAnswer.reserve(5);

	{
		m_pParticle1 = make_shared<CFireworkParticle>();
		m_pParticle1->AddComponent();
		m_pParticle1->Reset();
		m_pParticle1->Init(m_pGraphicDev, L"../Bin/Resource/Image/Particle/flare.bmp");

		m_pParticle2 = make_shared<CFireworkParticle>();
		m_pParticle2->AddComponent();
		m_pParticle2->Reset();
		m_pParticle2->Init(m_pGraphicDev, L"../Bin/Resource/Image/Particle/flare.bmp");
	}

	__super::ReadyGameObject();

	return S_OK;
}

_int CCardGame::UpdateGameObject(const _float& fTimeDelta)
{

	if (!m_bGameProcessing)
		return 0;

	if (!m_bInit)
		Start();

	if (m_bFreeze) {
		if (!m_bGameClear) {
			m_fFreezeTime += fTimeDelta;

			if (m_fFreezeTime > 0.7f) {
				m_pFirstFlipCard->ResetFlip();
				m_pSecondFlipCard->ResetFlip();

				m_iFirstFlipNum = 99;
				m_pFirstFlipCard = nullptr;
				m_iSecondFlipNum = 99;
				m_pSecondFlipCard = nullptr;

				m_fFreezeTime = 0.f;
				m_bFreeze = false;

			}
			else {
				return 0;
			}
		}

		else {
			if (!m_bClearSoundOn) {
				CSoundMgr::GetInstance()->StopAll();
				CSoundMgr::GetInstance()->PlaySound(L"Minigame_Clear.wav", CHANNELID::PLAYER, 1.f);

				CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
				CSoundMgr::GetInstance()->PlaySound(L"town_firework.wav", CHANNELID::EFFECT, 1.f);

				m_bClearSoundOn = true;

				if (m_pParticle1 && !m_pParticle1->GetIsActive())
				{
					m_pParticle1->SettingOrthogonal(_vec3(-350.f, -100.f, 0.9f), _vec3(0.f, 0.f, 0.f), 100);
					m_pParticle1->SetActive(true);
				}

				if (m_pParticle2 && !m_pParticle2->GetIsActive())
				{
					m_pParticle2->SettingOrthogonal(_vec3(350.f , -100.f, 0.9f), _vec3(0.f, 0.f, 0.f), 100);
					m_pParticle2->SetActive(true);
				}
			}

			if (m_pParticle1 && m_pParticle1->GetIsActive())
				m_pParticle1->UpdateGameObject(fTimeDelta);
			if (m_pParticle2 && m_pParticle2->GetIsActive())
				m_pParticle2->UpdateGameObject(fTimeDelta);

			m_fFreezeTime += fTimeDelta;

			if (m_fFreezeTime > 6.f) {
				m_pParticle1->Reset();
				m_pParticle2->Reset();
				End();
			}

		}
	}

	_int iExit = __super::UpdateGameObject(fTimeDelta);

	for (auto& iter : m_vCardList) {
		iter->UpdateGameObject(fTimeDelta);
	}

	return iExit;
}

void CCardGame::LateUpdateGameObject()
{
	if (!m_bGameProcessing)
		return;

	for (auto& iter : m_vCardList) {
		iter->LateUpdateGameObject();
	}

	if (m_pParticle1 && m_pParticle1->GetIsActive())
		m_pParticle1->LateUpdateGameObject();

	if (m_pParticle2 && m_pParticle2->GetIsActive())
		m_pParticle2->LateUpdateGameObject();

}

void CCardGame::RenderGameObject()
{
	if (!m_bGameProcessing) {
		return;
	}

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pTextureCom->SetTexture(0);
	m_pRCTexCom->RenderBuffer();

	for (auto& iter : m_vCardList) {
		iter->RenderGameObject();
	}

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CCardGame::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	pComponent = m_pRCTexCom = make_shared<CRcTex>(m_pGraphicDev);
	dynamic_pointer_cast<CRcTex>(m_pRCTexCom)->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_RcTex", pComponent });

	pComponent = m_pTransformCom = make_shared<CTransform>(_vec3(0.f, 0.f, 0.f), _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f));
	m_pTransformCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });
	m_pTransformCom->SetPosition(35.f, -52.f , 50.f + 0.3f );
	m_pTransformCom->SetScale(8.f, 8.f, 2.f );
	m_pTransformCom->SetAngle(_vec3(0.f, 0.f, 0.f));

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_pTextureCom->SetTextureKey(L"Minigame_BackGround", TEX_NORMAL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture",pComponent });

}

void CCardGame::Start()
{

	RandomCardShuffle();
	PlaceCardOnBoard();

	CSoundMgr::GetInstance()->StopAll();
	CSoundMgr::GetInstance()->PlayBGM(L"Minigame_BGM.wav", 0.8f);

	m_bInit = true;
}

void CCardGame::End()
{
	CCameraMgr::GetInstance()->SetVillageMode();
	m_bGameProcessing = false;

	dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetGold(500, true);

	ResetGame();
	CSoundMgr::GetInstance()->StopAll();
	CSoundMgr::GetInstance()->PlayBGM(L"town_tavern_bgm.wav", 1.f);

	CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
	CSoundMgr::GetInstance()->PlaySound(L"ui_town_buy.wav", CHANNELID::EFFECT, 1.f);

	CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);

}

void CCardGame::ResetGame()
{
	m_vAnswer.clear();
	m_vCardList.clear();

	m_iFirstFlipNum = 99;
	m_iSecondFlipNum = 99;

	m_pFirstFlipCard = nullptr;
	m_pSecondFlipCard = nullptr;

	m_bGameProcessing = false;
	m_bInit = false;

	m_bFreeze = false;
	m_fFreezeTime = 0.f;

	m_bClearSoundOn = false;
	m_bGameClear = false;
}

void CCardGame::FlipCard(shared_ptr<CCard> _pCard)
{
	if (99 == m_iFirstFlipNum) {
		m_iFirstFlipNum = _pCard->GetCardNum();
		m_pFirstFlipCard = _pCard;
		return;
	}

	if (99 == m_iSecondFlipNum) {
		m_iSecondFlipNum = _pCard->GetCardNum();

		if (m_pFirstFlipCard->GetCardImgNum() == _pCard->GetCardImgNum()) {

			m_pFirstFlipCard->SetIsCorrect();
			_pCard->SetIsCorrect();

			pair<_int, _int> pairTemp;

			if (m_iFirstFlipNum > m_iSecondFlipNum) {
				pairTemp = make_pair(m_iSecondFlipNum, m_iFirstFlipNum);
			}
			else {
				pairTemp = make_pair(m_iFirstFlipNum, m_iSecondFlipNum);
			}

			auto iter = find(m_vAnswer.begin(), m_vAnswer.end(), pairTemp);
			m_vAnswer.erase(iter);

			CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
			CSoundMgr::GetInstance()->PlaySound(L"Minigame_Kirby_LongHi.wav", CHANNELID::EFFECT, 1.f);

			//초기화
			m_iFirstFlipNum = 99;
			m_pFirstFlipCard = nullptr;
			m_iSecondFlipNum = 99;
			m_pSecondFlipCard = nullptr;

			if (m_vAnswer.empty()) {
				m_bGameClear = true;
				m_bFreeze = true;
			}

		}
		else {

			m_pSecondFlipCard = _pCard;
			m_bFreeze = true;

			CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
			CSoundMgr::GetInstance()->PlaySound(L"Minigame_Wrong.wav", CHANNELID::EFFECT, 1.f);
			
		}
	}


}

void CCardGame::RandomCardShuffle()
{
	vector<_int> numberList = { 0,1,2,3,4,5,6,7,8,9 };

	while (!numberList.empty()) {

		pair<_int, _int> pairTemp;

		pairTemp.first = numberList.front();

		if (2 == numberList.size()) {
			pairTemp.second = numberList[1];

			m_vAnswer.push_back(pairTemp);
			
			auto iter = numberList.begin();
			iter = numberList.erase(iter);
			numberList.erase(iter);

			numberList.clear();
		}
		else {

			_int iSize = (_int)numberList.size();
			_int iSelect = (rand() % (iSize - 1)) + 1;

			if (iSize > iSelect) {
				pairTemp.second = numberList[iSelect];

				m_vAnswer.push_back(pairTemp);
				auto iter = numberList.begin();

				for (int i = 0; i < iSelect; ++i) {
					++iter;
				}

				numberList.erase(iter);
				numberList.erase(numberList.begin());
			}
		}
	}


}

void CCardGame::PlaceCardOnBoard()
{

	for (int i = 0; i < m_vAnswer.size(); ++i) {

		shared_ptr<CCard> pCard1 = make_shared<CCard>(m_pGraphicDev, m_vAnswer[i].first, i);
		shared_ptr<CCard> pCard2 = make_shared<CCard>(m_pGraphicDev, m_vAnswer[i].second, i);
		
		m_vCardList.push_back(pCard1);
		m_vCardList.push_back(pCard2);
	}


	//카드 섞기
	for (int i = 0; i < 100; ++i) {
		_int iRandNum = (rand() % m_vCardList.size());
		_int iRandNum2 = (rand() % m_vCardList.size());

		swap(m_vCardList[iRandNum], m_vCardList[iRandNum2]);
	}


	for (int i = 0; i < m_vCardList.size(); ++i) {
		
		_int iHeight = 0;
		_int iWidth = i;

		if (i > 4) {
			iWidth = iWidth - 5;
			iHeight = 1;
		}

		m_vCardList[i]->SetScale({ 1.f, 0.7f * (417.f / 189.f) , 0.7f });
		m_vCardList[i]->SetPos({30.f + 2.5f * iWidth, -50.f - 3.5f * iHeight, 50.f});
		m_vCardList[i]->SetCardGame(dynamic_pointer_cast<CCardGame>(shared_from_this()));
		m_vCardList[i]->SetColliding(true);
		m_vCardList[i]->AwakeGameObject();
		m_vCardList[i]->ReadyGameObject();
	}


}
