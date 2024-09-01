#include "pch.h"
#include "Door.h"
#include "Export_System.h"
#include "Export_Utility.h"

#include "Player.h"
#include "Hero.h"

#include "UIMgr.h"

CDoor::CDoor(LPDIRECT3DDEVICE9 pGraphicDev, EFacilityType _eDoorType)
	: CInteractionObj(pGraphicDev), m_eDoorType(_eDoorType)
{
}

CDoor::CDoor(const CDoor& rhs)
	: CInteractionObj(rhs)
{
}

CDoor::~CDoor()
{
}

HRESULT CDoor::ReadyGameObject()
{
	__super::ReadyGameObject();

	m_bInteractionKey = L"C";
	m_bInteractionInfo = L"문 열기";

	return S_OK;
}

_int CDoor::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	return iExit;
}

void CDoor::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CDoor::RenderGameObject()
{
	__super::RenderGameObject();
}

void CDoor::AddComponent()
{

	shared_ptr<CComponent> pComponent;

	pComponent = m_pBufferCom = make_shared<CRcTex>(m_pGraphicDev);
	dynamic_pointer_cast<CRcTex>(m_pBufferCom)->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_RcTex", pComponent });

	pComponent = m_pTransformCom = make_shared<CTransform>(_vec3(0.f, 0.f, 0.f), _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f));
	m_pTransformCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });
	m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_pTextureCom->SetTextureKey(L"Village_Door_Close", TEX_NORMAL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture",pComponent });
}

void CDoor::GetInteractionKey(const _float& fTimeDelta)
{
	// 인터랙션 키 UI 보여주기
	// ----------수정 필요------------

	// 키 입력받기
	if (GetAsyncKeyState('C') & 0x8000)
	{
		//m_bInteracting = true;

		// 텍스처 또는 애니메이션 변경
		//ChangeTexture();


		if (EFacilityType::TERVARN == m_eDoorType)
		{
			// 여관 내부로 이동
			dynamic_pointer_cast<CTransform>(CGameMgr::GetInstance()->GetPlayer()->GetComponent(L"Com_Transform", ID_DYNAMIC))->SetPosition(50.f, -97.f, 4.f);

			CSoundMgr::GetInstance()->StopAll();
			CSoundMgr::GetInstance()->PlaySound(L"town_tavern_bgm.wav", CHANNELID::BGM, 1.f);
		}
			
		else if (EFacilityType::GUILD == m_eDoorType)
		{


			// 훈련소 내부로 이동
			dynamic_pointer_cast<CTransform>(CGameMgr::GetInstance()->GetPlayer()->GetComponent(L"Com_Transform", ID_DYNAMIC))->SetPosition(0.f, -197.f, 104.f);

			CSoundMgr::GetInstance()->StopAll();
			CSoundMgr::GetInstance()->PlaySound(L"town_guild_bgm.wav", CHANNELID::BGM, 1.f);
		}

		CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
		CSoundMgr::GetInstance()->PlaySound(L"town_enter_tavern.wav", CHANNELID::EFFECT, 1.f);

		Interaction();
		}
}

void CDoor::Interaction()
{
}

void CDoor::FinishInteraction()
{
}

void CDoor::ChangeTexture()
{
	m_pTextureCom->SetTextureKey(L"Village_Door_Open", TEX_NORMAL);
}

_bool CDoor::IsFinish()
{
	// 키 입력받기
	//if (GetAsyncKeyState('X') & 0x8000)
	//{
	//	m_bInteracting = false;
	//	m_pTextureCom->SetTextureKey(L"Village_Door_Close", TEX_NORMAL);

	//	// 플레이어 행동 풀기
	//	CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);

	//	return true;
	//}

	return false;
}