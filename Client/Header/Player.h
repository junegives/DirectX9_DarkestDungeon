#pragma once

#include"GameObject.h"
#include "PlayerHand.h"

#include "FireworkParticle.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;
class CCollider;
class CLight;

END


class CPlayerHand;
class CInventory;
class CTavernUI;
class CItem;
class CHero;
class CDescription;
class CPlayerFPSUI;

enum class EPlayerMove
{
	RIGHT,
	LEFT,
	UP,
	DOWN,
	ENUM_END
};


enum class EPlayerMode {
	DEFAULT,	//�⺻ ����
	BOSS_FIELD,	//������(�÷��̾ ���� �ο�� �ʵ�)
	ENUM_END
};

class CPlayer : public CGameObject
{

public:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:
	_bool			GetRoomChange() { return m_bRoomChange; }
	void			SetRoomChange(_bool _bRoomChange) { m_bRoomChange = _bRoomChange; }
	_int			GetCurrentRoom() { return m_iCurrentRoom; }
	void			SetCurrentRoom(_int _iCurrentRoom) { m_iCurrentRoom = _iCurrentRoom; }
	void			SetInventory(shared_ptr<CInventory> _pInventory);

	void			SetTavernUI(shared_ptr<CTavernUI> _pTavernUI);
	void			ShowTavernUI(shared_ptr<CHero> _pHero);

	void			SetInBossRoom(_bool _bInBossRoom) { m_bInBossRoom = _bInBossRoom; }
	void			SetInBattle(_bool _bInBattle) { 
		m_bInBattle = _bInBattle;
		
		if (_bInBattle) {
			m_eCurrentItem = EHandItem::ENUM_END;
			m_pPlayerHand->SetCurrentItem(EHandItem::ENUM_END);
			PlayerLightOff();
		}
		else {
			PlayerLightOn();
		}
	}
	_bool			GetInBattle() { return m_bInBattle; }

	// ���� ���� ��, �� / ���� �߰� / ���� ����
	vector<shared_ptr<CGameObject>>* GetHeroVec() { return &m_pVecHero; }
	void			SetHeroVec(vector<shared_ptr<CGameObject>>* _pVecHero) { m_pVecHero = *_pVecHero; }
	void			AddHero(shared_ptr<CGameObject> _pHero) { m_pVecHero.push_back(_pHero); }
	void			SwapHero(_int iIdx1, _int iIdx2) { m_pVecHero[iIdx1].swap(m_pVecHero[iIdx2]); }

	//���� ������ Setting
	void			SetCurrentItem(EHandItem _eItem);
	EHandItem		GetCurrentItem() { return m_eCurrentItem; }

	// �κ��丮 ������
	void			InsertItem(shared_ptr<CItem> _pItem);
	void			DeleteItem(tstring _strItmeName);

	shared_ptr<CGameObject> GetPlayerHand() { return m_pPlayerHand; }

	//��ȭ
	void SetGold(_int _iNum, _bool _bIsEarn);
	void SetHeirloom(_int _iNum, _bool _bIsEarn);

	void InitGold(_int _iNum) { m_iGold = _iNum; }
	void InitHeirloom(_int _iNum) { m_iHeirlooms = _iNum; }

	_int GetGold() { return m_iGold; }
	_int GetHeirloom() { return m_iHeirlooms; }

	void SetInDungeon(_bool _bInDungeon) { m_bInDungeon = _bInDungeon; }
	_bool IsInDungeon() { return m_bInDungeon; }

	// Ʈ����
	_bool	GetBattleTrigger() { return m_bBattleTrigger; }
	void	SetBattleTrigger(_bool _bBattleTrigger) { m_bBattleTrigger = _bBattleTrigger; }
	_bool	GetEventTrigger() { return m_bEventTrigger; }
	void	SetEventTrigger(_bool _bEventTrigger) { m_bEventTrigger = _bEventTrigger; }
	_bool	GetEvent2Trigger() { return m_bEvent2Trigger; }
	void	SetEvent2Trigger(_bool _bEventTrigger) { m_bEvent2Trigger = _bEventTrigger; }
	_bool	GetEvent3Trigger() { return m_bEvent3Trigger; }
	void	SetEvent3Trigger(_bool _bEventTrigger) { m_bEvent3Trigger = _bEventTrigger; }

	void	SetHP(_int _iHP) { m_iHP = _iHP; }
	void	DecreaseHP(_int _iDamage);

	void	SetSpeed(_float _fSpeed) { m_fSpeed = _fSpeed; }

	_bool	GetPrevVirtue() { return m_bPrevVirtue; }
	void	SetPrevVirtue(_bool m_bVirtue) { m_bPrevVirtue = m_bVirtue; }

public:
//Light ���� ��� �Լ�

	//Scene�� �ٲ� �� �ʿ信 ���� �����
	void SettingLight();
	void SpendTorchItem();

	void PlayerLightOn();
	void PlayerLightOff();
	

public:

	void SetPlayerMode(EPlayerMode _ePlrMode);
	//���� ����� ��ȭ��ȯ �Լ�
	void DungeonClear();

private:
	void			AddComponent();
	void			KeyInput(const _float& fTimeDelta);
	void			MouseInput();

	void			ShakingHand();


private:
	virtual void	OnCollide(shared_ptr<CGameObject> _pObj);
	virtual void	OnCollide(shared_ptr<CGameObject> _pObj, _float _fGap, EDirection _eDir);

protected:
	// Player Component
	shared_ptr<CTransform> m_pTransformCom = nullptr;

	//Player HandComponenet
	shared_ptr<CPlayerHand> m_pPlayerHand = nullptr;

public:
	void SetPlayerHand(shared_ptr<CPlayerHand> _pHand) { m_pPlayerHand = _pHand; }
	void ShowHeroesBack();	// ���� ������ �����ֱ�
	void ShowHeroesBackVillage(); // �������� ���� ������ �����ֱ�
	void HideHeroesBackVillage(); // �������� ���� ������ �����


private:
	_float		m_fSpeed = 10.f;
	EHandItem m_eCurrentItem = EHandItem::ENUM_END;
	shared_ptr<CInventory> m_pInventory = nullptr;

	vector <shared_ptr<CGameObject>> m_pVecHero;			// ���� ����

	_float		m_fDeltaTime = 0.f;
	EPlayerMove m_eLastMove = EPlayerMove::ENUM_END;	

	_bool		m_bMoveLock[4] = { false };
	_bool		m_bMoveSave[4] = { false };

	// ��ȭ
	_int		m_iGold = 0; //���(��ȭ)
	_int		m_iHeirlooms = 0; //����(���׷��̵� ��ȭ)

	_bool		m_bRoomChange = false;
	_int		m_iCurrentRoom = 10;		// ���� ��

// ���콺 �ߺ� ����
	_bool		m_bPrevMouse = true;
	_bool		m_bCurMouse = false;
	_float		m_fMouseDebTime = 0.f;

	_bool		m_bLookBack = false;

	// �� ���ƺ� ����
	_bool		m_bSeeBack = false;

	// ���� ���� ����
	_bool		m_bInBattle = false;

	// ���� or ����
	_bool		m_bInDungeon = true;
	_bool		m_bInBossRoom = false;

	// Ʈ����
	_bool		m_bBattleTrigger = false;
	_bool		m_bEventTrigger = false;
	_bool		m_bEvent2Trigger = false;
	_bool		m_bEvent3Trigger = false;
	_bool		m_bReady = false;

	_bool		m_bHitted = false;

	_float		m_fHittedTime = 1.f;

	// ��ŷ ������Ʈ ���� (�� ���ƺ� �� �����Ǵ� ������)
	vector<shared_ptr<CGameObject>>		m_vecPickingObject = {};

	// TavernUI ON/Off
	shared_ptr<CTavernUI> m_pTavernUI = nullptr;
	_bool		m_bShowTavernUI = false;
	_bool		m_bFirstLighUp = false;

	_bool		m_bPlrWalkingDeb = false;
	_float		m_fWalkDebTime = 0.f;

	shared_ptr<CLight> m_pLight = nullptr;

	_int m_iLightIntensity = 3;
	_float m_fLightActTime = 0.f;

	EPlayerMode m_ePlayerMode = EPlayerMode::DEFAULT;

	//------------------------------------FPS BOSS ----------------------------------------------------

private:

	_int m_iHP = 100;
	_int m_iMaxHP = 100;

	shared_ptr<CPlayerFPSUI> m_pPlrFPSUI = nullptr;

	// �ӽ� �����(����̳� �ر� ĳ���Ͱ� ���� ���Գ� �ȳ��Գ�)
	_bool	m_bPrevVirtue = true;


private:
	virtual void	Free();



	// �׽�Ʈ�� ���� ���� ����
	private:
		shared_ptr<CFireworkParticle> m_pParticle = nullptr;
};