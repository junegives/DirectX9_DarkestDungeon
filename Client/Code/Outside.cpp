#include "pch.h"
#include "Outside.h"

#include "Export_Utility.h"
#include "Export_System.h"

#include "Door.h"
#include "StageCoachHero.h"
#include "EnvironmentObj.h"
#include "Item.h"
#include "PickingMgr.h"

#include "Jester.h"
#include "ShieldBreaker.h"
#include "Vestal.h"
#include "Highwayman.h"


COutside::COutside(LPDIRECT3DDEVICE9 pGraphicDev, EFacilityType _eFacilityType)
	: Engine::CGameObject(pGraphicDev), m_eFacilityType(_eFacilityType)
{
}

COutside::COutside(const COutside& rhs)
    : Engine::CGameObject(rhs)
{
}

COutside::~COutside()
{
}

HRESULT COutside::ReadyGameObject()
{	
	if (m_eFacilityType == EFacilityType::TERVARN)
	{
		shared_ptr<CGameObject> pDoor = make_shared<CDoor>(m_pGraphicDev, EFacilityType::TERVARN);
		pDoor->SetPos({ m_vPos.x + 4.5f, m_vPos.y + 4.f, m_vPos.z - 0.1f });
		pDoor->SetScale({ 1.7f, 4.f, 1.f });
		m_vecGameObject.push_back(pDoor);
	}

	else if (m_eFacilityType == EFacilityType::GUILD)
	{
		shared_ptr<CGameObject> pDoor = make_shared<CDoor>(m_pGraphicDev, EFacilityType::GUILD);
		pDoor->SetPos({ m_vPos.x + 3.8f, m_vPos.y + 5.7f, m_vPos.z - 2.8f });
		pDoor->SetScale({ 3.f, 5.5f, 1.f });
		pDoor->SetAngle({ 0.f, 10.f, 0.f });
		m_vecGameObject.push_back(pDoor);
	}

	else if (m_eFacilityType == EFacilityType::STAGECOACH)
	{
		shared_ptr<CGameObject> pHero1 = make_shared<CShieldBreaker>(m_pGraphicDev);
		pHero1->SetColliding(true);
		pHero1->SetScale({ 3.f, 5.f, 3.f });
		pHero1->SetPos({ m_vPos.x, pHero1->GetScale().y / 2.f + pHero1->GetScale().y / 10.f, m_vPos.z - 2.f});
		pHero1->SetAngle({ 0.f, - PI / 2.f, 0.f });
		m_vecPickingObject.push_back(pHero1);
		CPickingMgr::GetInstance()->AddList(pHero1);
		m_vecGameObject.push_back(pHero1);
			
		shared_ptr<CGameObject> pHero2 = make_shared<CVestal>(m_pGraphicDev);
		//shared_ptr<CGameObject> pHero2 = make_shared<CStageCoachHero>(m_pGraphicDev, EHeroType::JESTER);
		pHero2->SetColliding(true);
		pHero2->SetScale({ 3.f, 5.f, 3.f });
		pHero2->SetPos({ m_vPos.x + 7.f, pHero2->GetScale().y / 2.f + pHero2->GetScale().y / 10.f, m_vPos.z - 2.f });
		pHero2->SetAngle({ 0.f, -PI / 2.f, 0.f });
		m_vecPickingObject.push_back(pHero2);
		CPickingMgr::GetInstance()->AddList(pHero2);
		m_vecGameObject.push_back(pHero2);
	}

	else if (m_eFacilityType == EFacilityType::STORE)
	{
		shared_ptr<CGameObject> pTable = make_shared<CEnvironmentObj>(m_pGraphicDev, L"Store_Table", true, false, 1, false);
		pTable->SetPos({ m_vPos.x + 3.f, m_vPos.y + 1.f, m_vPos.z + 2.f });
		pTable->SetScale({ 7.f, 3.f, 1.f });
		pTable->SetHorizontal(true);
		m_vecGameObject.push_back(pTable);

		// »óÁ¡ ¾ÆÀÌÅÛ
		{
			// ÇØµ¶Á¦
			shared_ptr<CGameObject> m_pItem = make_shared<CItem>(m_pGraphicDev);
			m_pItem->SetColliding(true);
			dynamic_pointer_cast<CItem>(m_pItem)->SetOnField(false);
			dynamic_pointer_cast<CItem>(m_pItem)->SetOnStore(true);
			dynamic_pointer_cast<CItem>(m_pItem)->SetDropItemInfo({ m_vPos.x + 3.f, m_vPos.y + 3.f, m_vPos.z + 2.1f }, L"Player_Item_Antivenom");
			m_vecGameObject.push_back(m_pItem);
			m_vecPickingObject.push_back(m_pItem);

			// »ð
			m_pItem = make_shared<CItem>(m_pGraphicDev);
			m_pItem->SetColliding(true);
			dynamic_pointer_cast<CItem>(m_pItem)->SetOnField(false);
			dynamic_pointer_cast<CItem>(m_pItem)->SetOnStore(true);
			dynamic_pointer_cast<CItem>(m_pItem)->SetDropItemInfo({ m_vPos.x + 1.f, m_vPos.y + 3.f, m_vPos.z + 2.1f }, L"Player_Item_Shovel");
			m_vecGameObject.push_back(m_pItem);
			m_vecPickingObject.push_back(m_pItem);

			// ºØ´ë
			m_pItem = make_shared<CItem>(m_pGraphicDev);
			m_pItem->SetColliding(true);
			dynamic_pointer_cast<CItem>(m_pItem)->SetOnField(false);
			dynamic_pointer_cast<CItem>(m_pItem)->SetOnStore(true);
			dynamic_pointer_cast<CItem>(m_pItem)->SetDropItemInfo({ m_vPos.x - 1.f, m_vPos.y + 3.f, m_vPos.z + 2.1f }, L"Player_Item_Bandage");
			m_vecGameObject.push_back(m_pItem);
			m_vecPickingObject.push_back(m_pItem);

			// È¶ºÒ
			m_pItem = make_shared<CItem>(m_pGraphicDev);
			m_pItem->SetColliding(true);
			dynamic_pointer_cast<CItem>(m_pItem)->SetOnField(false);
			dynamic_pointer_cast<CItem>(m_pItem)->SetOnStore(true);
			dynamic_pointer_cast<CItem>(m_pItem)->SetDropItemInfo({ m_vPos.x + 5.f, m_vPos.y + 3.f, m_vPos.z + 2.1f }, L"Item_Torch");
			m_vecGameObject.push_back(m_pItem);
			m_vecPickingObject.push_back(m_pItem);

			// À½½Ä
			/*m_pItem = make_shared<CItem>(m_pGraphicDev);
			m_pItem->SetColliding(true);
			dynamic_pointer_cast<CItem>(m_pItem)->SetOnField(false);
			dynamic_pointer_cast<CItem>(m_pItem)->SetDropItemInfo({ m_vPos.x + 5.f, m_vPos.y + 3.f, m_vPos.z + 2.1f }, L"Item_Food");
			m_vecGameObject.push_back(m_pItem);*/

			// ¿­¼è
			m_pItem = make_shared<CItem>(m_pGraphicDev);
			m_pItem->SetColliding(true);
			dynamic_pointer_cast<CItem>(m_pItem)->SetOnField(false);
			dynamic_pointer_cast<CItem>(m_pItem)->SetOnStore(true);
			dynamic_pointer_cast<CItem>(m_pItem)->SetDropItemInfo({ m_vPos.x + 7.f, m_vPos.y + 3.f, m_vPos.z + 2.1f }, L"Item_Key");
			m_vecGameObject.push_back(m_pItem);

		}
	}

	for (auto& iter : m_vecGameObject) {
		iter->AwakeGameObject();
		iter->ReadyGameObject();
	}

    return S_OK;
}

_int COutside::UpdateGameObject(const _float& fTimeDelta)
{
    Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());

	for (auto& iter : m_vecGameObject) {
		if (iter->GetIsActive() && iter->GetIsEnable())
			iter->UpdateGameObject(fTimeDelta);
	}

	for (auto& iter : m_vecPickingObject)
	{
		if (iter->GetIsActive() && iter->GetIsEnable())
			CPickingMgr::GetInstance()->AddList(iter);
	}

	_int iExit = __super::UpdateGameObject(fTimeDelta);

	return iExit;
}

void COutside::LateUpdateGameObject()
{
	for (auto iter = m_vecGameObject.begin(); iter != m_vecGameObject.end(); iter++)
	{
		if (ECollideID::HERO == (*iter)->GetColType())
		{
			if (dynamic_pointer_cast<CHero>(*iter)->IsHired())
				iter = m_vecGameObject.erase(iter);
		}

		if (iter == m_vecGameObject.end())
			return;

		// Collision Check
		if ((*iter)->IsColliding())
		{
			if (ECollideID::PLAYER != (*iter)->GetColType())
				CCollisionMgr::GetInstance()->CheckCollision(*iter);
		}

		(*iter)->LateUpdateGameObject();
	}
}

void COutside::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->SetTexture(0);

	m_pBufferCom->RenderBuffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	for (auto& iter : m_vecGameObject) {
		iter->RenderGameObject();
	}
}

void COutside::AddComponent()
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
	m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y + m_vScale.y / 2 + m_vScale.y / 3 + 0.3f, m_vPos.z);
	m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);
	m_pTransformCom->SetAngle(m_vAngle);

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	switch (m_eFacilityType)
	{
	case EFacilityType::TERVARN:
		m_strKey = L"Tervarn_Outside";
		break;
	case EFacilityType::GUILD:
		m_strKey = L"Guild_Outside";
		break;
	case EFacilityType::STAGECOACH:
		m_strKey = L"StageCoach_Outside";
		break;
	case EFacilityType::STORE:
		m_strKey = L"Store_Outside";
		break;
	case EFacilityType::ENUM_END:
		break;
	}
	m_pTextureCom->SetTextureKey(m_strKey, TEX_NORMAL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture",pComponent });
}
