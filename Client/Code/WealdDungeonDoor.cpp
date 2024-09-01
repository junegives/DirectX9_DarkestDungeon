#include "pch.h"
#include "WealdDungeonDoor.h"
#include "Player.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "SoundMgr.h"

CWealdDungeonDoor::CWealdDungeonDoor(LPDIRECT3DDEVICE9 pGraphicDev, EWealdDungeonDoorType _eDoorType)
	: CInteractionObj(pGraphicDev), m_eDoorType(_eDoorType)
{
}

CWealdDungeonDoor::CWealdDungeonDoor(const CWealdDungeonDoor& rhs)
	: CInteractionObj(rhs)
{
}

CWealdDungeonDoor::~CWealdDungeonDoor()
{
}

HRESULT CWealdDungeonDoor::ReadyGameObject()
{
	__super::ReadyGameObject();

	m_pTransformCom2->SetScale(0.509f * m_pTransformCom->GetScale()->x,
		0.484f * m_pTransformCom->GetScale()->y,
		0.509f * m_pTransformCom->GetScale()->z);
	m_pTransformCom2->SetPosition(m_vPos.x, m_vPos.y - 2.8f, m_vPos.z);


	m_bInteractionKey = L"C";
	m_bInteractionInfo = L"문 열기";
	return S_OK;
}

_int CWealdDungeonDoor::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	if (m_bOpenStart)
	{
		m_fOpenTime -= fTimeDelta;

		_vec3 DirU = _vec3(0.f, 1.f, 0.f);
		//_vec3 DirL = _vec3(-1.f, 0.f, 0.f);

		m_pTransformCom2->MoveForward(&DirU, fTimeDelta, 7.f);

		if (m_fOpenTime < 0.f)
		{
			m_fOpenTime = 1.f;
			m_bOpenStart = false;
		}
	}

	return iExit;
}

void CWealdDungeonDoor::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CWealdDungeonDoor::RenderGameObject()
{
	__super::RenderGameObject();

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom2->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);


	SetMaterial();
	m_pTextureCom2->SetTexture(0);
	m_pBufferCom2->RenderBuffer();
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CWealdDungeonDoor::AddComponent()
{

	shared_ptr<CComponent> pComponent;

	pComponent = m_pBufferCom = make_shared<CRcTex>(m_pGraphicDev);
	dynamic_pointer_cast<CRcTex>(m_pBufferCom)->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_RcTex", pComponent });

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = make_shared<CTransform>(_vec3(0.f, 0.f, 0.f), _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f));
	m_pTransformCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });
	m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_pTextureCom->SetTextureKey(L"Weald_Door_Closed", TEX_NORMAL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture",pComponent });



	pComponent = m_pBufferCom2 = make_shared<CRcTex>(m_pGraphicDev);
	dynamic_pointer_cast<CRcTex>(m_pBufferCom2)->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_RcTex2", pComponent });

	pComponent = m_pTextureCom2 = make_shared<CTexture>(m_pGraphicDev);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture2", pComponent });

	pComponent = m_pTransformCom2 = make_shared<CTransform>(_vec3(0.f, 0.f, 0.f), _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f));
	m_pTransformCom2->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform2", pComponent });
	m_pTransformCom2->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom2->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);

	pComponent = m_pTextureCom2 = make_shared<CTexture>(m_pGraphicDev);
	m_pTextureCom2->SetTextureKey(L"Weald_Door", TEX_NORMAL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture2",pComponent });
}

void CWealdDungeonDoor::GetInteractionKey(const _float& fTimeDelta)
{
	// 인터랙션 키 UI 보여주기
	// ----------수정 필요------------

	// 키 입력받기
	if (GetAsyncKeyState('C') & 0x8000)
	{
		m_bInteracting = true;
		m_bTabInteractionKey = true;

		// 플레이어 행동 막기
		//CGameMgr::GetInstance()->SetGameState(EGameState::LOCK);

		// 텍스처 또는 애니메이션 변경
		ChangeTexture();

		CSoundMgr::GetInstance()->PlaySound(L"door_open_weald.wav", CHANNELID::DOOR, 1.f);

		m_bOpenStart = true;

		switch (m_eDoorType)
		{
		case EWealdDungeonDoorType::ROOM1:
			m_iCurrent = 2;
			break;
		case EWealdDungeonDoorType::ROOM2:
			m_iCurrent = 3;
			break;
		case EWealdDungeonDoorType::ROOM3:
			m_iCurrent = 4;
			break;
		case EWealdDungeonDoorType::ROOM4:
			m_iCurrent = 5;
			break;
		}
		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetRoomChange(true);
		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetCurrentRoom(m_iCurrent);

		Interaction();
	}
}

void CWealdDungeonDoor::Interaction()
{
}

void CWealdDungeonDoor::FinishInteraction()
{
	m_bInteracting = false;

	m_pTextureCom->SetTextureKey(L"Weald_Door_Closed", TEX_NORMAL);
}

void CWealdDungeonDoor::ChangeTexture()
{
	m_pTextureCom->SetTextureKey(L"Weald_Door_Opened", TEX_NORMAL);
}

_bool CWealdDungeonDoor::IsFinish()
{
	// 키 입력받기
	if (GetAsyncKeyState('X') & 0x8000)
	{
		m_bInteracting = false;
		m_iCurrent = 10;
		m_pTextureCom->SetTextureKey(L"Weald_Door_Closed", TEX_NORMAL);

		// 플레이어 행동 풀기
		//CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);

		return true;
	}

	return false;
}