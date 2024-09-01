#include"pch.h"
#include "HeroStat.h"
#include"Hero.h"
#include "Player.h"

#include"Export_Utility.h"
#include"Export_System.h"

CHeroStat::CHeroStat(LPDIRECT3DDEVICE9 _pGraphicDev)
	:Engine::CUIObj(_pGraphicDev)
{
}

CHeroStat::~CHeroStat()
{
}

HRESULT CHeroStat::ReadyGameObject()
{
	m_vSize = { 345.f , 338.f ,0.f };
	m_vAngle = { 0.f, 0.f, 0.f };
	m_vPos = { 0.f, 0.f, 0.5f };

	m_bVisible = false;
	m_bEnable = true;
	m_bActive = true;

	{
		m_pTransformCom[0]->SetScale(m_vSize.x, m_vSize.y, m_vSize.z);
		m_pTransformCom[0]->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);

		// 추천 위치
		for (int i = 1; i < 5; ++i)
		{
			m_pTransformCom[i]->SetScale(25.f, 25.f, 1.f);
			m_pTransformCom[i]->SetPosition(-128.f + 47.f * i, 67.f, m_vPos.z);
		}

		// 추천 목표
		for (int i = 5; i < 9; ++i)
		{
			m_pTransformCom[i]->SetScale(25.f, 25.f, 1.f);
			m_pTransformCom[i]->SetPosition(-104.f + 47.f * i, 67.f, m_vPos.z);
		}

		// 스킬 이미지
		for (int i = 9; i < 15; ++i)
		{
			m_pTransformCom[i]->SetScale(33.f, 33.f, 1.f);
			m_pTransformCom[i]->SetPosition(-81.f + 71.f * (i - 9), -10.f, m_vPos.z);
		}

		// 활성 스킬 표시
		for (int i = 15; i < 21; ++i)
		{
			m_pTransformCom[i]->SetScale(38.f, 38.f, 1.f);
			m_pTransformCom[i]->SetPosition(-81.f + 71.f * (i - 15), -10.f, m_vPos.z);
		}

		// 활성 스킬 표시
		for (int i = 21; i < 26; ++i)
		{
			m_pTransformCom[i]->SetScale(15.f, 15.f, 1.f);
			m_pTransformCom[i]->SetPosition(-81.f + 71.f * (i - 15), -10.f, m_vPos.z);
		}
	}

	// 값 채워넣기
	{
		// 추천 위치, 추천 목표 계산
		for (auto& iter : *m_pVecSkill)
		{
			if (!iter->IsEquipped()) continue;

			for (_int j = 0; j < 4; ++j)
			{
				if (iter->GetActivatePos()[j])
					m_iRecommendPos[3 - j]++;

				if (iter->GetTargetPos()[j])
					m_iTargetPos[j]++;
			}
		}
	}

	CUIObj::ReadyGameObject();

	return S_OK;
}

_int CHeroStat::UpdateGameObject(const _float& fTimeDelta)
{
	int iExit(0);

	if (m_pHero.lock() && m_bVisible)
	{
		/*_vec3 vHeroPos = m_pHero.lock()->GetPos();
		m_vPos = { vHeroPos.x + 50.f, vHeroPos.y, vHeroPos.z };*/

		iExit = CGameObject::UpdateGameObject(fTimeDelta);
		AddRenderGroup(RENDER_UI, shared_from_this());

		if (GetAsyncKeyState('X') & 0x8000)
		{
			m_bVisible = false;

			CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
			CSoundMgr::GetInstance()->PlaySound(L"ui_page_close.wav", CHANNELID::EFFECT, 1.f);

			// 플레이어 행동 풀기
			CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);
		}

		else if (GetAsyncKeyState('C') & 0x8000)
		{
			if (m_bForSelect)
			{
				if (m_bSelectedAlready)
					return iExit;

				if (m_pHero.lock()->GetHp() == m_pHero.lock()->GetMaxHp() && m_pHero.lock()->GetStress() <= 0)
					return iExit;

				m_bSelectedAlready = true;

				// 돈 사용해서
				if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetGold() < 500)
					return iExit;

				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetGold(500, false);

				// 체력이랑 스트레스 회복하기
				m_pHero.lock()->IncreaseHP(20);
				m_pHero.lock()->DecreaseStress(50);
				if (m_pHero.lock()->GetHp() > m_pHero.lock()->GetMaxHp()) m_pHero.lock()->SetHp(m_pHero.lock()->GetMaxHp());
				if (m_pHero.lock()->GetStress() < 0) m_pHero.lock()->SetStress(0);
			}

			else
			{
				CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
				CSoundMgr::GetInstance()->PlaySound(L"town_stagecoach_hire.wav", CHANNELID::EFFECT, 1.f);

				CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);

				// 영웅 추가 코드
				if (m_pHero.lock()->GetHeroType() == EHeroType::SHILEDBREAKER)
				{
					dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->AddHero(m_pHero.lock());
					if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetHeroVec()->size() == 3)
					{
						dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SwapHero(0, 2);
						dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SwapHero(1, 2);
					}
					if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetHeroVec()->size() == 4)
					{
						dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SwapHero(0, 3);
						dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SwapHero(1, 3);
						dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SwapHero(2, 3);
					}
				}
				if (m_pHero.lock()->GetHeroType() == EHeroType::VESTAL)
				{
					dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->AddHero(m_pHero.lock());
				}
				CSceneMgr::GetInstance()->AddNewObject(L"Layer_4_GameObj", m_pHero.lock()->GetObjKey(), m_pHero.lock());

				m_bVisible = false;
				
				m_pHero.lock()->SetPos({ -100.f, 100.f, -100.f });
				dynamic_pointer_cast<CTransform>(m_pHero.lock()->GetComponent(L"Com_Transform", ID_DYNAMIC))->SetPosition(-100.f, 100.f, -100.f);

				// 역마차에서 영웅 삭제
				m_pHero.lock()->SetHired(true);
			}
		}
	}

	return iExit;
}

void CHeroStat::LateUpdateGameObject()
{
	if (m_pHero.lock() && m_bVisible)
	{
		__super::LateUpdateGameObject();
	}
}

void CHeroStat::RenderGameObject()
{

	if (m_pHero.lock() && m_bVisible)
	{
		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[0]->GetWorld());
		m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

		m_pTextureCom[0]->SetTextureKey(m_strBackKey, TEXTUREID::TEX_NORMAL);
		m_pTextureCom[0]->SetTexture(0);
		m_pRCTexCom[0]->RenderBuffer();

		// 추천 위치
		for (int i = 1; i < 5; ++i)
		{
			m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[i]->GetWorld());

			if (m_iRecommendPos[i - 1] == 0) continue;
			m_pTextureCom[m_iRecommendPos[i - 1]]->SetTexture(0);
			m_pRCTexCom[i]->RenderBuffer();
		}

		// 추천 목표
		for (int i = 5; i < 9; ++i)
		{
			m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[i]->GetWorld());

			if (m_iTargetPos[i - 5] == 0) continue;
			m_pTextureCom[m_iTargetPos[i - 5] + 4]->SetTexture(0);
			m_pRCTexCom[i]->RenderBuffer();
		}

		// 스킬 이미지
		_int idx = 9;
		for (auto& iter : *m_pVecSkill)
		{
			m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[idx]->GetWorld());

			if (!iter->IsUnlocked())
			{
				m_pTextureCom[idx]->SetTextureKey(iter->GetImgKey() + L"_BW", TEXTUREID::TEX_NORMAL);
			}
			else
			{
			}

			m_pTextureCom[idx]->SetTexture(0);
			m_pRCTexCom[idx]->RenderBuffer();

			idx++;
		}

		// 스킬 상태 출력
		idx = 15;
		for (auto& iter : *m_pVecSkill)
		{
			if (iter->IsEquipped())
			{
				// 활성 스킬 표시
				m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[idx]->GetWorld());

				m_pTextureCom[15]->SetTexture(0);
				m_pRCTexCom[idx]->RenderBuffer();
			}

			else if (!iter->IsUnlocked())
			{
				// 잠금 스킬 표시
				m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[idx]->GetWorld());

				m_pTextureCom[16]->SetTexture(0);
				m_pRCTexCom[idx]->RenderBuffer();
			}

			idx++;
		}

		// HP 출력
		{
			TCHAR buf[64];

			_int _iHp = m_pHero.lock()->GetHp();
			_int _iMaxHp = m_pHero.lock()->GetMaxHp();

			_vec2 vPos = { m_vPos.x + WINCX * 0.5f - 305.f, (m_vPos.y) + WINCY * 0.5f - 225.f };

			if (_iHp < 10) {
				_stprintf_s(buf, TEXT("HP :    %d"), _iHp);

			}
			else if (_iHp < 100) {
				_stprintf_s(buf, TEXT("HP :  %d"), _iHp);
			}

			Engine::Render_Font(L"Font_Default_Bold", buf, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

			vPos = { m_vPos.x + WINCX * 0.5f - 195.f, (m_vPos.y) + WINCY * 0.5f - 225.f };
			_stprintf_s(buf, TEXT(" /"));
			Engine::Render_Font(L"Font_Default_Bold", buf, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

			vPos = { m_vPos.x + WINCX * 0.5f - 175.f, (m_vPos.y) + WINCY * 0.5f - 225.f };
			_stprintf_s(buf, TEXT("%d"), _iMaxHp);
			Engine::Render_Font(L"Font_Default_Bold", buf, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		}

		// 스트레스 출력
		{
			TCHAR buf[64];

			_int _iStress = m_pHero.lock()->GetStress();

			_vec2 vPos = { m_vPos.x + WINCX * 0.5f - 305.f, (m_vPos.y) + WINCY * 0.5f - 200.f };

			if (_iStress < 10) {
				_stprintf_s(buf, TEXT("Stress :    %d"), _iStress);

			}
			else if (_iStress < 100) {
				_stprintf_s(buf, TEXT("Stress :  %d"), _iStress);
			}

			Engine::Render_Font(L"Font_Default_Bold", buf, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

		}

		m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}
}


void CHeroStat::PickingUI(LONG _fX, LONG _fY, _bool _bLB)
{
	int i = 0;

}

void CHeroStat::SetForHire(_bool _bForHire)
{
	m_bForHire = _bForHire;
	m_bForSelect = false;

	if (!m_bForHire) return;

	switch (m_pHero.lock()->GetHeroType())
	{
	case EHeroType::SHILEDBREAKER:
		m_strBackKey = L"UI_HeroStat_Shieldbreaker_Base_Hire";
		break;

	case EHeroType::HIGHWAYMAN:
		m_strBackKey = L"UI_HeroStat_Highwayman_Base_Hire";
		break;

	case EHeroType::JESTER:
		m_strBackKey = L"UI_HeroStat_Jester_Base_Hire";
		break;

	case EHeroType::VESTAL:
		m_strBackKey = L"UI_HeroStat_Vestal_Base_Hire";
		break;

	case EHeroType::ENUM_END:
		m_strBackKey = L"UI_HeroStat_Base";
		break;

	default:
		m_strBackKey = L"UI_HeroStat_Base";
		break;
	}

	m_pTextureCom[0]->SetTextureKey(m_strBackKey, TEXTUREID::TEX_NORMAL);
}

void CHeroStat::SetForSelect(_bool _bForSelect)
{
	m_bForSelect = _bForSelect;
	m_bForHire = false;

	if (!m_bForSelect) return;

	switch (m_pHero.lock()->GetHeroType())
	{
	case EHeroType::SHILEDBREAKER:
		m_strBackKey = L"UI_HeroStat_Shieldbreaker_Base_Select";
		break;

	case EHeroType::HIGHWAYMAN:
		m_strBackKey = L"UI_HeroStat_Highwayman_Base_Select";
		break;

	case EHeroType::JESTER:
		m_strBackKey = L"UI_HeroStat_Jester_Base_Select";
		break;

	case EHeroType::VESTAL:
		m_strBackKey = L"UI_HeroStat_Vestal_Base_Select";
		break;

	case EHeroType::ENUM_END:
		m_strBackKey = L"UI_HeroStat_Base";
		break;

	default:
		m_strBackKey = L"UI_HeroStat_Base";
		break;
	}

	m_pTextureCom[0]->SetTextureKey(m_strBackKey, TEXTUREID::TEX_NORMAL);
}

void CHeroStat::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	//CUIObj::AddComponent();

	switch (m_pHero.lock()->GetHeroType())
	{
	case EHeroType::SHILEDBREAKER:
		m_strBackKey = L"UI_HeroStat_Shieldbreaker_Base";
		break;

	case EHeroType::HIGHWAYMAN:
		m_strBackKey = L"UI_HeroStat_Highwayman_Base";
		break;

	case EHeroType::JESTER:
		m_strBackKey = L"UI_HeroStat_Jester_Base";
		break;

	case EHeroType::VESTAL:
		m_strBackKey = L"UI_HeroStat_Vestal_Base";
		break;

	case EHeroType::ENUM_END:
		m_strBackKey = L"UI_HeroStat_Base";
		break;

	default:
		m_strBackKey = L"UI_HeroStat_Base";
		break;
	}


	for (int i = 0; i < 27; ++i) {

		shared_ptr<CComponent> pComponent;

		//Transform 생성
		pComponent = m_pTransformCom[i] = make_shared<CTransform>();
		m_pTransformCom[i]->ReadyTransform();

		tstring strCurNum = to_wstring(i);
		tstring strComName = L"Com_Transform" + strCurNum;
		tstring strComName2 = L"Com_RcTex" + strCurNum;

		m_mapComponent[ID_DYNAMIC].insert({ strComName ,pComponent });
		m_pTransformCom[i]->SetScale(1.f, 1.f, 1.f);
		m_pTransformCom[i]->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);

		//RcTex생성
		pComponent = m_pRCTexCom[i] = make_shared<CRcTex>(m_pGraphicDev);
		m_pRCTexCom[i]->ReadyBuffer();
		m_mapComponent[ID_STATIC].insert({ strComName2,pComponent });

	}


	for (int i = 0; i < 17; ++i) {

		tstring strCurNum = to_wstring(i);
		tstring strComName = L"Com_Texture" + strCurNum;

		shared_ptr<CComponent> pComponent;
		pComponent = m_pTextureCom[i] = make_shared<CTexture>(m_pGraphicDev);
		m_mapComponent[ID_STATIC].insert({ strComName,pComponent });

	}

	m_pVecSkill = m_pHero.lock()->GetAllSkillVector();

	{
		/*
		// 0: 틀
		// 1~4 : 추천 위치1~4
		// 5~8 : 추천 목표1~4
		// 9~14: 스킬 이미지
		// 15 : 스킬 활성 표시
		*/

		//ImageSet
		m_pTextureCom[0]->SetTextureKey(m_strBackKey, TEXTUREID::TEX_NORMAL);

		m_pTextureCom[1]->SetTextureKey(L"UI_HeroStat_Recommend_Position_1", TEXTUREID::TEX_NORMAL);
		m_pTextureCom[2]->SetTextureKey(L"UI_HeroStat_Recommend_Position_2", TEXTUREID::TEX_NORMAL);
		m_pTextureCom[3]->SetTextureKey(L"UI_HeroStat_Recommend_Position_3", TEXTUREID::TEX_NORMAL);
		m_pTextureCom[4]->SetTextureKey(L"UI_HeroStat_Recommend_Position_4", TEXTUREID::TEX_NORMAL);

		m_pTextureCom[5]->SetTextureKey(L"UI_HeroStat_Recommend_Target_1", TEXTUREID::TEX_NORMAL);
		m_pTextureCom[6]->SetTextureKey(L"UI_HeroStat_Recommend_Target_2", TEXTUREID::TEX_NORMAL);
		m_pTextureCom[7]->SetTextureKey(L"UI_HeroStat_Recommend_Target_3", TEXTUREID::TEX_NORMAL);
		m_pTextureCom[8]->SetTextureKey(L"UI_HeroStat_Recommend_Target_4", TEXTUREID::TEX_NORMAL);

		m_pTextureCom[9]->SetTextureKey((*m_pVecSkill)[0]->GetImgKey(), TEXTUREID::TEX_NORMAL);
		m_pTextureCom[10]->SetTextureKey((*m_pVecSkill)[1]->GetImgKey(), TEXTUREID::TEX_NORMAL);
		m_pTextureCom[11]->SetTextureKey((*m_pVecSkill)[2]->GetImgKey(), TEXTUREID::TEX_NORMAL);
		m_pTextureCom[12]->SetTextureKey((*m_pVecSkill)[3]->GetImgKey(), TEXTUREID::TEX_NORMAL);
		m_pTextureCom[13]->SetTextureKey((*m_pVecSkill)[4]->GetImgKey(), TEXTUREID::TEX_NORMAL);
		m_pTextureCom[14]->SetTextureKey((*m_pVecSkill)[5]->GetImgKey(), TEXTUREID::TEX_NORMAL);

		m_pTextureCom[15]->SetTextureKey(L"UI_HeroStat_Selected_Skill", TEXTUREID::TEX_NORMAL);
		m_pTextureCom[16]->SetTextureKey(L"UI_HeroStat_Locked_Skill", TEXTUREID::TEX_NORMAL);
	}
}
