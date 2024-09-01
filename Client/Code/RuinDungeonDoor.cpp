#include "pch.h"
#include "RuinDungeonDoor.h"
#include "Player.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "SoundMgr.h"

CRuinDungeonDoor::CRuinDungeonDoor(LPDIRECT3DDEVICE9 pGraphicDev, ERuinDungeonDoorType _eDoorType)
	: CInteractionObj(pGraphicDev), m_eDoorType(_eDoorType)
{
}

CRuinDungeonDoor::CRuinDungeonDoor(const CRuinDungeonDoor& rhs)
	: CInteractionObj(rhs)
{
}

CRuinDungeonDoor::~CRuinDungeonDoor()
{
}

HRESULT CRuinDungeonDoor::ReadyGameObject()
{
	__super::ReadyGameObject();

	m_pTransformCom2->SetScale(0.305f * m_pTransformCom->GetScale()->x,
		0.595f * m_pTransformCom->GetScale()->y,
		0.305f * m_pTransformCom->GetScale()->z);
	m_pTransformCom2->SetPosition(m_vPos.x + 0.01f, m_vPos.y - 0.9f, m_vPos.z+0.01f);
	m_pTransformCom2->SetAngle(*m_pTransformCom->GetAngles());
	m_pTransformCom3->SetScale(0.305f * m_pTransformCom->GetScale()->x,
		0.595f * m_pTransformCom->GetScale()->y,
		0.305f * m_pTransformCom->GetScale()->z);
	m_pTransformCom3->SetPosition(m_vPos.x + 0.01f, m_vPos.y - 0.9f, m_vPos.z + 0.01f);
	m_pTransformCom3->SetAngle(*m_pTransformCom->GetAngles());

	m_bInteractionKey = L"C";
	m_bInteractionInfo = L"문 열기";

	return S_OK;
}

_int CRuinDungeonDoor::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	if (m_bOpenStart)
	{
		m_fOpenTime -= fTimeDelta;

		_vec3 DirR = _vec3(1.f, 0.f, 0.f);
		_vec3 DirL = _vec3(-1.f, 0.f, 0.f);

		_vec3 DirU = _vec3(0.f, 0.f, 1.f);
		_vec3 DirD = _vec3(0.f, 0.f, -1.f);

		if (m_pTransformCom2->GetAngles()->y != 0.f)
		{
			m_pTransformCom2->MoveForward(&DirU, fTimeDelta, 2.f);
			m_pTransformCom3->MoveForward(&DirD, fTimeDelta, 2.f);
		}
		else
		{
			m_pTransformCom2->MoveForward(&DirL, fTimeDelta, 2.f);
			m_pTransformCom3->MoveForward(&DirR, fTimeDelta, 2.f);
		}

		if (m_fOpenTime < 0.f)
		{
			m_fOpenTime = 1.f;
			m_bOpenStart = false;
		}
	}

	return iExit;
}

void CRuinDungeonDoor::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CRuinDungeonDoor::RenderGameObject()
{
	__super::RenderGameObject();

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom2->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	SetMaterial();

	m_pTextureCom2->SetTexture(0);
	m_pBufferCom2->RenderBuffer();


	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom3->GetWorld());
	m_pTextureCom3->SetTexture(0);
	m_pBufferCom3->RenderBuffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CRuinDungeonDoor::AddComponent()
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
	m_pTextureCom->SetTextureKey(L"Ruin_Door_Closed", TEX_NORMAL);
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
	m_pTextureCom2->SetTextureKey(L"Ruin_Door_Left", TEX_NORMAL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture2",pComponent });



	pComponent = m_pBufferCom3 = make_shared<CRcTex>(m_pGraphicDev);
	dynamic_pointer_cast<CRcTex>(m_pBufferCom3)->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_RcTex3", pComponent });

	pComponent = m_pTextureCom3 = make_shared<CTexture>(m_pGraphicDev);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture3", pComponent });

	pComponent = m_pTransformCom3 = make_shared<CTransform>(_vec3(0.f, 0.f, 0.f), _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f));
	m_pTransformCom3->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform3", pComponent });
	m_pTransformCom3->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom3->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);

	pComponent = m_pTextureCom3 = make_shared<CTexture>(m_pGraphicDev);
	m_pTextureCom3->SetTextureKey(L"Ruin_Door_Right", TEX_NORMAL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture3",pComponent });
}

void CRuinDungeonDoor::GetInteractionKey(const _float& fTimeDelta)
{
	// 인터랙션 키 UI 보여주기
	// ----------수정 필요------------

	// 키 입력받기
	if (GetAsyncKeyState('C') & 0x8000)
	{
		m_bInteracting = true;

		// 플레이어 행동 막기
		//CGameMgr::GetInstance()->SetGameState(EGameState::LOCK);

		// 텍스처 또는 애니메이션 변경
		ChangeTexture();

		CSoundMgr::GetInstance()->PlaySound(L"door_open_ruin.wav", CHANNELID::DOOR, 1.f);


		m_bOpenStart = true;

		switch (m_eDoorType)
		{
		case ERuinDungeonDoorType::ROOM1:
			m_iCurrent = 2;
			break;
		case ERuinDungeonDoorType::ROOM2:
			m_iCurrent = 3;
			break;
		case ERuinDungeonDoorType::ROOM3:
			m_iCurrent = 4;
			break;
		case ERuinDungeonDoorType::ROOM4:
			m_iCurrent = 5;
			break;
		case ERuinDungeonDoorType::ROOM5:
			m_iCurrent = 6;
			break;
		}
		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetRoomChange(true);
		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetCurrentRoom(m_iCurrent);

		Interaction();
	}
}

void CRuinDungeonDoor::Interaction()
{
}

void CRuinDungeonDoor::FinishInteraction()
{
	m_bInteracting = false;

	m_pTextureCom->SetTextureKey(L"Ruin_Door_Closed", TEX_NORMAL);
}

void CRuinDungeonDoor::ChangeTexture()
{
	m_pTextureCom->SetTextureKey(L"Ruin_Door_Opened", TEX_NORMAL);
}

_bool CRuinDungeonDoor::IsFinish()
{
	// 키 입력받기
	if (GetAsyncKeyState('X') & 0x8000)
	{
		m_bInteracting = false;
		m_iCurrent = 10;
		m_pTextureCom->SetTextureKey(L"Ruin_Door_Closed", TEX_NORMAL);

		// 플레이어 행동 풀기
		//CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);

		return true;
	}

	return false;
}