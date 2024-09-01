#include "pch.h"
#include "HeroSkillUI.h"
#include "Skill.h"
#include "Player.h"
#include "UIMgr.h"

#include"Export_Utility.h"
#include"Export_System.h"

CHeroSkillUI::CHeroSkillUI(LPDIRECT3DDEVICE9 _pGraphicDev, shared_ptr<CHero> _pHero, _int _iIdx)
	:CUIObj(_pGraphicDev), m_pHero(_pHero), m_iIdx(_iIdx)
{
}

CHeroSkillUI::~CHeroSkillUI()
{
}

HRESULT CHeroSkillUI::ReadyGameObject()
{
	m_vSize = { 250.f, 60.f , 0.f };
	m_vAngle = { 0.f, 0.f, 0.f };
	m_vPos = { WINCX / 2.f - m_vSize.x, 200.f - m_iIdx * 130.f, 0.f };

	m_bEnable = true;
	m_bActive = true;

	m_pTransCom->SetScale(m_vSize.x, m_vSize.y, m_vSize.z);
	m_pTransCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransCom->SetAngle(m_vAngle);

	CUIObj::ReadyGameObject();

	SetIconPos();

	return S_OK;
}

_int CHeroSkillUI::UpdateGameObject(const _float& fTimeDelta)
{
	if (!m_bVisible)
		return 0;


	int iExit(0);
	CGameObject::UpdateGameObject(fTimeDelta);
	AddRenderGroup(RENDER_UI, shared_from_this());

	return iExit;
}

void CHeroSkillUI::LateUpdateGameObject()
{

}

void CHeroSkillUI::RenderGameObject()
{
	if (!m_bVisible)
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	m_pTextureCom->SetTexture(0);
	m_pRcTexCom->RenderBuffer();

	TCHAR buf[64];

	//Hp
	_int _iHp = m_pHero->GetHp();
	_int _iMaxHp = m_pHero->GetMaxHp();

	_vec2 vPos = { m_vPos.x + WINCX * 0.5f - 205.f, (m_vPos.y * -1.f) + WINCY * 0.5f + 31.f };

	if (_iHp < 10) {
		_stprintf_s(buf, TEXT("    %d"), _iHp);
	}
	else if (_iHp < 100) {
		_stprintf_s(buf, TEXT("  %d"), _iHp);
	}
	
	Engine::Render_Font(L"Font_Default_Small", buf, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	vPos = { m_vPos.x + WINCX * 0.5f - 185.f, (m_vPos.y * -1.f) + WINCY * 0.5f + 31.f };
	_stprintf_s(buf, TEXT(" /"));
	Engine::Render_Font(L"Font_Default_Small", buf, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	vPos = { m_vPos.x + WINCX * 0.5f - 175.f, (m_vPos.y * -1.f) + WINCY * 0.5f + 31.f };
	_stprintf_s(buf, TEXT("%d"), _iMaxHp);
	Engine::Render_Font(L"Font_Default_Small", buf, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	_matrix matTemp;

	//Skill
	vector<shared_ptr<CSkill>>* pSkillVector = m_pHero->GetAllSkillVector();
	_int test = 2;

	if (pSkillVector) {
		for (int i = 0; i < 6; ++i) {

			if (!(*pSkillVector)[i]->IsUnlocked())
			{
				m_arrSkillTexture[i]->SetTextureKey((*pSkillVector)[i]->GetImgKey() + L"_BW", TEX_NORMAL);

				matTemp = *(m_arrSkillTransform[i]->GetWorld());
				matTemp.m[3][0] += 1.f;
				matTemp.m[3][1] += 45.f;
				matTemp.m[1][0] *= 0.7f;
				matTemp.m[1][1] *= 0.5f;

				m_pGraphicDev->SetTransform(D3DTS_WORLD, &matTemp);
				m_pSkillGoldTexture->SetTexture(0);
				m_pSkillGoldRcTex->RenderBuffer();

				//if (test > 0) {
				//	// 수량 출력
				//	{
				//		test--;
				//		_stprintf_s(buf, TEXT("%d"), 1);

				//		vPos = { (matTemp.m[3][0]) * 2.f, (matTemp.m[3][1]) + 45.f };
				//		Engine::Render_Font(L"Font_Default_Small", buf, &vPos, D3DXCOLOR(0.44f, 0.55f, 0.48f, 1.f));
				//	}
				//}
			}

			else
				m_arrSkillTexture[i]->SetTextureKey((*pSkillVector)[i]->GetImgKey(), TEX_NORMAL);

			m_pGraphicDev->SetTransform(D3DTS_WORLD, m_arrSkillTransform[i]->GetWorld());
			m_arrSkillTexture[i]->SetTexture(0);
			m_arrSkillRcTex[i]->RenderBuffer();

			if ((*pSkillVector)[i]->IsEquipped())
			{
				m_pSkillActivateTexture->SetTexture(0);
				m_arrSkillStateRcTex[0]->RenderBuffer();
			}

			else if (!((*pSkillVector)[i]->IsUnlocked()))
			{
				m_pSkillLockTexture->SetTexture(0);
				m_arrSkillStateRcTex[1]->RenderBuffer();
			}
		}
	}

	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CHeroSkillUI::PickingUI(LONG _fX, LONG _fY, _bool _bLB)
{
	for (int iHeroNum = 0; iHeroNum < 4; ++iHeroNum)
	{
		if (_fY > 192.f + 124.f * iHeroNum || _fY < 146.f + 124.f * iHeroNum)
			continue;

		for (int iSkillNum = 0; iSkillNum < 6; ++iSkillNum)
		{
			if (_fX > 880.f + 65.4f * iSkillNum && _fX < 927.f + 65.4f * iSkillNum)
			{
				vector<shared_ptr<CSkill>>* pSkillVector = m_pHero->GetAllSkillVector();

				// 잠금된 스킬인지 확인
				if (!(*pSkillVector)[iSkillNum]->IsUnlocked())
				{
					// 돈 체크
					if (1 > dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetHeirloom())
					{
						CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
						CSoundMgr::GetInstance()->PlaySound(L"ui_town_use_heirloom_fail.wav", CHANNELID::EFFECT, 1.f);
						return;
					}

					// 돈 내고 잠금 해제
					else
					{
						CSoundMgr::GetInstance()->StopSound(CHANNELID::UI);
						CSoundMgr::GetInstance()->PlaySound(L"ui_town_skill_unlock.wav", CHANNELID::UI, 1.f);

						CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
						CSoundMgr::GetInstance()->PlaySound(L"ui_town_use_heirloom.wav", CHANNELID::EFFECT, 1.f);
						dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetHeirloom(1, false);
						(*pSkillVector)[iSkillNum]->SetUnlocked(true);
					}
				}

				// 스킬 교환을 위한 클릭
				else
				{
					if (!(*pSkillVector)[iSkillNum]->IsEquipped())
					{
						if (m_iEquippedCnt >= 4)
						{
							CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
							CSoundMgr::GetInstance()->PlaySound(L"ui_town_buy_fail.wav", CHANNELID::EFFECT, 1.f);
							return;
						}

						// 장착하기
						else
						{
							CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
							CSoundMgr::GetInstance()->PlaySound(L"ui_town_skill_equip.wav", CHANNELID::EFFECT, 1.f);
							++m_iEquippedCnt;
							(*pSkillVector)[iSkillNum]->SetEquipped(true);
						}
					}

					else
					{
						if (m_iEquippedCnt <= 0) return;

						CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
						CSoundMgr::GetInstance()->PlaySound(L"ui_town_skill_unequip.wav", CHANNELID::EFFECT, 1.f);

						// 장착 해제하기
						--m_iEquippedCnt;
						(*pSkillVector)[iSkillNum]->SetEquipped(false);
					}

				}
			}
		}
	}
}

void CHeroSkillUI::HoverUI(LONG _fX, LONG _fY)
{
	for (int iHeroNum = 0; iHeroNum < 4; ++iHeroNum)
	{
		if (_fY > 192.f + 124.f * iHeroNum || _fY < 146.f + 124.f * iHeroNum)
			continue;

		for (int iSkillNum = 0; iSkillNum < 6; ++iSkillNum)
		{
			if (_fX > 880.f + 65.4f * iSkillNum && _fX < 927.f + 65.4f * iSkillNum)
			{
				vector<shared_ptr<CSkill>>* pSkillVector = m_pHero->GetAllSkillVector();

				CUIMgr::GetInstance()->SetDescription((*pSkillVector)[iSkillNum], { (_float)_fX - WINCX * 0.5f, -(_float)_fY + WINCY * 0.5f, 0.f });
			}
		}
	}
}

void CHeroSkillUI::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	CUIObj::AddComponent();

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);

	switch (m_pHero->GetHeroType())
	{
	case EHeroType::VESTAL:
		m_pTextureCom->SetTextureKey(L"UI_HeroSkill_Vestal", TEX_NORMAL);
		break;
	case EHeroType::HIGHWAYMAN:
		m_pTextureCom->SetTextureKey(L"UI_HeroSkill_Highwayman", TEX_NORMAL);
		break;
	case EHeroType::SHILEDBREAKER:
		m_pTextureCom->SetTextureKey(L"UI_HeroSkill_Shieldbreaker", TEX_NORMAL);
		break;
	case EHeroType::JESTER:
		m_pTextureCom->SetTextureKey(L"UI_HeroSkill_Jester", TEX_NORMAL);
		break;
	case EHeroType::ENUM_END:
		m_pTextureCom->SetTextureKey(L"UI_HeroSkill_Base", TEX_NORMAL);
		break;
	default:
		break;
	}

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture",pComponent });

	//Skill Icon

	for (int i = 0; i < 6; ++i) {

		//Transform 생성
		pComponent = m_arrSkillTransform[i] = make_shared<CTransform>();
		m_arrSkillTransform[i]->ReadyTransform();

		tstring strCurNum = to_wstring(i);
		tstring strComName = L"Com_Transform" + strCurNum;
		tstring strComName2 = L"Com_RcTex" + strCurNum;
		tstring strComName3 = L"Com_Texture" + strCurNum;


		m_mapComponent[ID_DYNAMIC].insert({ strComName ,pComponent });

		m_arrSkillTransform[i]->SetScale(30.f, 30.f, 1.f);

		//RcTex생성
		pComponent = m_arrSkillRcTex[i] = make_shared<CRcTex>(m_pGraphicDev);
		m_arrSkillRcTex[i]->ReadyBuffer();
		m_mapComponent[ID_STATIC].insert({ strComName2,pComponent });

		//Texture 생성
		pComponent = m_arrSkillTexture[i] = make_shared<CTexture>(m_pGraphicDev);
		m_mapComponent[ID_STATIC].insert({ strComName3,pComponent });

	}

	// 스킬 활성화
	pComponent = m_arrSkillStateRcTex[0] = make_shared<CRcTex>(m_pGraphicDev);
	m_arrSkillStateRcTex[0]->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Activate", pComponent });
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Locked", pComponent });

	// 스킬 락
	pComponent = m_arrSkillStateRcTex[1] = make_shared<CRcTex>(m_pGraphicDev);
	m_arrSkillStateRcTex[1]->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Lock", pComponent });
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Locked", pComponent });


	pComponent = m_pSkillActivateTexture = make_shared<CTexture>(m_pGraphicDev);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture6", pComponent });
	pComponent = m_pSkillLockTexture = make_shared<CTexture>(m_pGraphicDev);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture6", pComponent });

	m_pSkillActivateTexture->SetTextureKey(L"UI_HeroStat_Selected_Skill", TEX_NORMAL);
	m_pSkillLockTexture->SetTextureKey(L"UI_HeroStat_Locked_Skill", TEX_NORMAL);

	// 잠금 금액
	pComponent = m_pSkillGoldRcTex = make_shared<CRcTex>(m_pGraphicDev);
	m_pSkillGoldRcTex->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_RcTex_Gold", pComponent });

	pComponent = m_pSkillGoldTexture = make_shared<CTexture>(m_pGraphicDev);
	m_pSkillGoldTexture->SetTextureKey(L"UI_Icon_Crest", TEX_NORMAL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Gold", pComponent });
}

void CHeroSkillUI::SetIconPos()
{
	for (int i = 0; i < 6; ++i) {
		m_arrSkillTransform[i]->SetPosition(m_vPos.x - 115.f + 65.f * i, m_vPos.y - 19.f, m_vPos.z);
	}

}
