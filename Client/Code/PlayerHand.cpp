#include"pch.h"
#include "PlayerHand.h"
#include"Export_Utility.h"
#include"CameraMgr.h"
#include"PlayerProj.h"

#include "SoundMgr.h"



CPlayerHand::CPlayerHand(LPDIRECT3DDEVICE9 _pGraphicDev)
	:CGameObject(_pGraphicDev)
{
}

CPlayerHand::~CPlayerHand()
{
}

HRESULT CPlayerHand::ReadyGameObject()
{
	if (m_bReady) return S_OK;

	m_bWalking = false;
	m_bEnable = false;

	m_pPlrTransmCom = dynamic_pointer_cast<CTransform>(Engine::Get_Component(L"Layer_4_GameObj", L"Obj_Player", L"Com_Transform", COMPONENTID::ID_DYNAMIC));
	m_bReady = true;
	return S_OK;
}

_int CPlayerHand::UpdateGameObject(const _float& fTimeDelta)
{
	if (!m_pPlrTransmCom) {
		m_pPlrTransmCom = dynamic_pointer_cast<CTransform>(Engine::Get_Component(L"Layer_4_GameObj", L"Obj_Player", L"Com_Transform", COMPONENTID::ID_DYNAMIC));
	}

	if (!m_bEnable)
		return 0;

	int iExit(0);
	iExit = __super::UpdateGameObject(fTimeDelta);
	

	if (m_bShoot) {

		m_fEffectTime += fTimeDelta;

		if (m_fEffectTime > 1.2f) {
			m_bShoot = false;
			m_fEffectTime = 0.f;
		}

	}
	
	_vec3 vPos;
	_matrix matWorld;

	matWorld = *m_pItemTransmCom->GetWorld();
	m_pItemTransmCom->GetInfo(INFO_POS, &vPos);

	AddRenderGroup(RENDER_FRONT, shared_from_this());
	m_fTime = fTimeDelta;
	return iExit;
}

void CPlayerHand::LateUpdateGameObject()
{
	if (!m_bEnable)
		return;

	//보스 전투 모드 X , 일반 아이템 사용 : Player 기준으로 Pos 변환
	if (!m_bPlrSpellHand) {
		_matrix matWorld;
		_vec3 vPos, vItemPos;
		_vec3 vLook, vRight, vUp;
		_matrix mat1;

		_matrix matParent;

		matWorld = *m_pPlrTransmCom->GetWorld();
		_vec3 vAngle = *m_pPlrTransmCom->GetAngles();
		m_pPlrTransmCom->GetInfo(INFO_LOOK, &vLook);
		m_pPlrTransmCom->GetInfo(INFO_POS, &vPos);
		m_pPlrTransmCom->GetInfo(INFO_RIGHT, &vRight);
		m_pPlrTransmCom->GetInfo(INFO_UP, &vUp);

		m_pItemTransmCom->GetInfo(INFO_POS, &vItemPos);

		D3DXVec3Normalize(&vLook, &vLook);
		D3DXVec3Normalize(&vRight, &vRight);

		vPos = vPos + ((vLook * 4.5f + vUp * 1.f + vRight * -3.f));
		//anti venom : 4.2f 

		if (m_bWalking) {
			m_fActTime += m_fTime;
			m_fTotalHeight += 0.3f * m_CurrentDir * m_fTime;


			if (0.4f < m_fTime + m_fActTime) {

				m_CurrentDir *= -1.f;
				m_fActTime = 0.f;
			}

		}

		_float fYAngle = 0.f;

		fYAngle = CCameraMgr::GetInstance()->GetYAngle();

		m_pItemTransmCom->SetPosition(vPos.x, vPos.y + m_fTotalHeight, vPos.z);
		m_pItemTransmCom->SetAngle({ D3DXToRadian(fYAngle) , vAngle.y, vAngle.z });
	}
	else { // 보스전 손 세팅 : View 행렬에 맞춰서 변환
		_matrix matWorld;
		_vec3 vPos, vItemPos;
		_vec3 vLook, vRight, vUp;
		_matrix mat1;
		_matrix matParent;


		_matrix matView;
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
		D3DXMatrixInverse(&matView, 0, &matView);


		memcpy(&vUp, &matView.m[1][0], sizeof(_vec3));
		memcpy(&vLook, &matView.m[2][0], sizeof(_vec3));
		memcpy(&vRight, &matView.m[0][0], sizeof(_vec3));
		memcpy(&vPos, &matView.m[3][0], sizeof(_vec3));
		_vec3 vAngle = *m_pPlrTransmCom->GetAngles();

		_float fYAngle = 0.f;
		fYAngle = CCameraMgr::GetInstance()->GetYAngle();

		vPos = vPos + ((vLook * 4.5f + vUp * -2.f + vRight * -3.f));
		_vec3 vCopyPos = vPos + ((vLook * 5.f + vUp * -2.f + vRight * -3.f));

		if (m_bWalking) {
			m_fActTime += m_fTime;
			m_fTotalHeight += 0.3f * m_CurrentDir * m_fTime;


			if (0.4f < m_fTime + m_fActTime) {

				m_CurrentDir *= -1.f;
				m_fActTime = 0.f;
			}

		}

		m_pItemTransmCom->SetPosition(vPos.x, vPos.y + m_fTotalHeight, vPos.z);
		m_pItemTransmCom->SetAngle({ D3DXToRadian(fYAngle) , vAngle.y, vAngle.z });


		m_pEffectTransmCom->SetPosition(vCopyPos.x, vCopyPos.y + m_fTotalHeight, vCopyPos.z);
		m_pEffectTransmCom->SetAngle({ D3DXToRadian(fYAngle) , vAngle.y, vAngle.z });

	}

}

void CPlayerHand::RenderGameObject()
{
	if (!m_bEnable)
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pItemTransmCom->GetWorld());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);


	if (m_bPlrSpellHand) {

		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		if (m_bShoot) {
			m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pEffectTransmCom->GetWorld());
			m_pEffectTextureCom->SetTexture(0);
			m_pEffectBufCom->RenderBuffer();
		}

		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pItemTransmCom->GetWorld());
		m_pAnimCom->SetAnimTexture();

	}
	else {
		m_pTextureCom->SetTexture(0);
	}

	m_pItemBufCom->RenderBuffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);

	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);


	if (m_bPlrSpellHand) {
		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	}

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CPlayerHand::SetCurrentItem(EHandItem _handItem)
{
	switch (_handItem)
	{
	case EHandItem::SHOVEL:
		m_pTextureCom->SetTextureKey(L"Player_Item_Shovel", TEX_NORMAL);
		m_pItemTransmCom->SetScale(1.5f, 1.5f, 1.f);
		m_pItemTransmCom->SetAngle({ D3DXToRadian(50.f),0.f, D3DXToRadian(-10.f) });
		m_bPlrSpellHand = false;
		m_bEnable = true;
		break;
	case EHandItem::ANTI_VENOM:
		m_pTextureCom->SetTextureKey(L"Player_Item_Antivenom", TEX_NORMAL);
		m_pItemTransmCom->SetScale(1.5f, 1.5f, 1.f);
		m_pItemTransmCom->SetAngle({ 0.f,0.f, D3DXToRadian(-10.f) });
		m_bPlrSpellHand = false;
		m_bEnable = true;
		break;
	case EHandItem::KEYS:
		m_pTextureCom->SetTextureKey(L"Item_Key", TEX_NORMAL);
		m_pItemTransmCom->SetScale(1.5f, 1.5f, 1.f);
		m_pItemTransmCom->SetAngle({ 0.f ,0.f, D3DXToRadian(-10.f) });
		m_bPlrSpellHand = false;
		m_bEnable = true;
		break;
	case EHandItem::BANDAGE:
		m_pTextureCom->SetTextureKey(L"Player_Item_Bandage", TEX_NORMAL);
		m_pItemTransmCom->SetScale(1.5f, 1.5f, 1.f);
		m_pItemTransmCom->SetAngle({ 0.f ,0.f, D3DXToRadian(-10.f) });
		m_bPlrSpellHand = false;
		m_bEnable = true;
		break;

	case EHandItem::TORCH:
		m_pTextureCom->SetTextureKey(L"Item_Torch", TEX_NORMAL);
		m_pItemTransmCom->SetScale(1.5f, 1.5f, 1.f);
		m_pItemTransmCom->SetAngle({ 0.f ,0.f, D3DXToRadian(-10.f) });
		m_bPlrSpellHand = false;
		m_bEnable = true;
		break;

	case EHandItem::SPELLHAND_FIRE:
		m_pAnimCom->SetAnimKey(L"Player_SpellHand_Fire", 0.3f);
		m_pItemTransmCom->SetAngle({ 0.f ,0.f, 0.f });
		m_pItemTransmCom->SetScale(1.2f, 1.5f, 1.f);
		m_bPlrSpellHand = true;
		m_bEnable = true;
		break;

	case EHandItem::SPELLHAND_STORM:
		break;

	case EHandItem::RED_GEM:
		m_pTextureCom->SetTextureKey(L"Player_Item_RedGem", TEX_NORMAL);
		m_pItemTransmCom->SetScale(1.5f, 1.5f, 1.f);
		m_pItemTransmCom->SetAngle({ 0.f,0.f, 0.f });
		m_bPlrSpellHand = false;
		m_bEnable = true;
		break;

	case EHandItem::BLUE_GEM:
		m_pTextureCom->SetTextureKey(L"Player_Item_BlueGem", TEX_NORMAL);
		m_pItemTransmCom->SetScale(1.5f, 1.5f, 1.f);
		m_pItemTransmCom->SetAngle({ 0.f,0.f, 0.f });
		m_bPlrSpellHand = false;
		m_bEnable = true;
		break;

	case EHandItem::GREEN_GEM:
		m_pTextureCom->SetTextureKey(L"Player_Item_GreenGem", TEX_NORMAL);
		m_pItemTransmCom->SetScale(1.5f, 1.5f, 1.f);
		m_pItemTransmCom->SetAngle({ 0.f,0.f, 0.f });
		m_bPlrSpellHand = false;
		m_bEnable = true;
		break;

	case EHandItem::ENUM_END:
		m_bEnable = false;
		break;
	default:
		break;
	}

}

void CPlayerHand::CreateProjection()
{

	_matrix matView;
	m_pGraphicDev->GetTransform(D3DTRANSFORMSTATETYPE::D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);


	//Player_Magic_Proj

	CSoundMgr::GetInstance()->StopSound(CHANNELID::PLAYER_PROJ);
	CSoundMgr::GetInstance()->PlaySound(L"Player_Magic_Proj.wav", CHANNELID::PLAYER_PROJ, 1.f);
	 
	shared_ptr<CGameObject> pProj = make_shared<CPlayerProj>(m_pGraphicDev, L"SpellHand_Proj_Fire", *m_pItemTransmCom->GetPos(), matView);
	pProj->AwakeGameObject();
	pProj->ReadyGameObject();

	if(!m_bShoot)
		m_bShoot = true;

	CSceneMgr::GetInstance()->AddNewObject(L"Layer_4_GameObj", L"Player_Proj", pProj);
}

void CPlayerHand::AddComponent()
{
	if (m_bReady) return;

	shared_ptr<CComponent> pComponent;

	pComponent = m_pItemBufCom = make_shared<CRcTex>(m_pGraphicDev);
	m_pItemBufCom->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_PlrItemRcTex",pComponent });

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_mapComponent[ID_STATIC].insert({ L"Com_PlrItemTex",pComponent });

	pComponent = m_pItemTransmCom = make_shared<CTransform>();
	m_pItemTransmCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_PlrItemTrans",pComponent });

	pComponent = m_pAnimCom = make_shared<CAnimator>(m_pGraphicDev);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });


	pComponent = m_pEffectBufCom = make_shared<CRcTex>(m_pGraphicDev);
	m_pEffectBufCom->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_EffectRcTex",pComponent });

	pComponent = m_pEffectTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_mapComponent[ID_STATIC].insert({ L"Com_EffectTex",pComponent });
	m_pEffectTextureCom->SetTextureKey(L"Effect_Spell_Blur", TEXTUREID::TEX_NORMAL);

	pComponent = m_pEffectTransmCom = make_shared<CTransform>();
	m_pEffectTransmCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_EffectTrans",pComponent });
	m_pEffectTransmCom->SetScale(5.f, 5.f, 5.f);


}

