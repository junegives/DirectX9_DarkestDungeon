#include "ResourceMgr.h"

IMPLEMENT_SINGLETON(CResourceMgr)

CResourceMgr::CResourceMgr()
{

}

CResourceMgr::~CResourceMgr()
{
}

void CResourceMgr::ReadyResource(LPDIRECT3DDEVICE9 _pGraphicDev)
{
	m_pGraphicDev = _pGraphicDev;

}

HRESULT CResourceMgr::CreateNewTexture(const tstring& _KeyName, TEXTUREID _eType, wstring _pPath, const _uint& _iCnt, _bool _bDelete)
{
	if (_eType >= TEXTUREID::TEX_END)
		return E_FAIL;

	IDirect3DBaseTexture9* pTexture = nullptr;

	vector<shared_ptr<tagTextureInfo>> vTempVec;
	vTempVec.reserve(_iCnt);

	for (_uint i = 0; i < _iCnt; ++i)
	{
		shared_ptr<tagTextureInfo> pInfo = make_shared<tagTextureInfo>();
		pInfo->bDelete = _bDelete;

		TCHAR		szFileName[128] = L"";
		wstring strPathCopy = _pPath;

		if (i < 10 && 1 != _iCnt) {
			size_t iSize = strPathCopy.find(L"%");
			strPathCopy.insert(iSize, L"0");
		}

		wsprintf(szFileName, strPathCopy.c_str(), i);

		switch (_eType)
		{
		case TEX_NORMAL:

			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DTEXTURE9*)&pTexture)))
				return E_FAIL;

			break;

		case TEX_CUBE:
			if (FAILED(D3DXCreateCubeTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DCUBETEXTURE9*)&pTexture)))
				return E_FAIL;
			break;
		}

		pInfo->pTexture = pTexture;
		
		//GetImgSize
		D3DXIMAGE_INFO pImgInfo;
		D3DXGetImageInfoFromFile(szFileName, &pImgInfo);
		pInfo->vImgSize = { (float)pImgInfo.Width, (float)pImgInfo.Height };

		vTempVec.push_back(pInfo);
	}

	if (TEXTUREID::TEX_CUBE == _eType) {

		m_CubeMap.insert({ _KeyName, vTempVec });
	}
	else if (TEXTUREID::TEX_NORMAL == _eType)
		m_TextureMap.insert({ _KeyName,vTempVec });

	return S_OK;
}

vector<shared_ptr<tagTextureInfo>>* CResourceMgr::GetTexture(const tstring& _keyName, TEXTUREID _eType)
{

	if (_eType == TEXTUREID::TEX_NORMAL) {
		auto iter = m_TextureMap.find(_keyName);

		if (iter == m_TextureMap.end())
			return nullptr;

		return &(iter->second);
	}
	else if (_eType == TEXTUREID::TEX_CUBE) {
		auto iter = m_CubeMap.find(_keyName);

		if (iter == m_CubeMap.end())
			return nullptr;

		return &(iter->second);
	}

	return nullptr;
}

void CResourceMgr::RemoveAllTexture()
{
	for (auto& iter : m_TextureMap) {

		for (auto& it : iter.second) {
			it->pTexture->Release();
		}
		//for_each(iter.second.begin(), iter.second.end(), CDeleteObj());
	}

	m_TextureMap.clear();

	//for (auto& iter : m_CubeMap) {
	//	for_each(iter.second.begin(), iter.second.end(), CDeleteObj());
	//	iter.second.clear();
	//	m_CubeMap.erase(iter.first);
	//}

}

void CResourceMgr::RemoveSceneTexture()
{

	for (auto iter = m_TextureMap.begin(); iter != m_TextureMap.end(); ) {
		if (true == iter->second.front()->bDelete) {
			for (auto& it : iter->second) {
				it->pTexture->Release();
			}
			m_TextureMap.erase(iter++);

		}
		else {
			++iter;
		}
	}
}

void CResourceMgr::Free()
{
	RemoveAllTexture();
}

void CResourceMgr::MainTitleTextureLoad()
{
	_bool bDefault = true;

	CreateNewTexture(L"UI_Title_BG", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/MainLogo/Title_BackGround.png", 1, bDefault);

	CreateNewTexture(L"UI_Title_Show", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/MainLogo/Title/armatureName_show_%d.png", 18, bDefault);

	CreateNewTexture(L"UI_Title_Torch", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/MainLogo/Title/armatureName_torch_loop_0%d.png", 21, bDefault);

}

void CResourceMgr::BaseTextureLoad()
{
	_bool bDefault = false;

	// Debug
	CreateNewTexture(L"Empty", TEX_NORMAL,
		L"../Bin/Resource/Image/Debug/Empty.png", 1, bDefault);

	//UIResource
	CreateNewTexture(L"UI_Inventory", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/panel_inventory.png", 1, bDefault);

	CreateNewTexture(L"UI_Description", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/UI/ToolTip.png", 1, bDefault);
	CreateNewTexture(L"UI_Active_Pos", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Hero/Skill/UI_active_pos.png", 1, bDefault);
	CreateNewTexture(L"UI_Active_Target", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Hero/Skill/UI_active_target.png", 1, bDefault);
	CreateNewTexture(L"UI_Deactive", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Hero/Skill/UI_deactive.png", 1, bDefault);
	CreateNewTexture(L"UI_Active_Pos_Link", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Hero/Skill/UI_active_pos_link.png", 1, bDefault);
	CreateNewTexture(L"UI_Active_Target_Link", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Hero/Skill/UI_active_target_link.png", 1, bDefault);

	//PlayerHandItem
	CreateNewTexture(L"Player_Item_Shovel", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/supply_shovel.png", 1, bDefault);
	CreateNewTexture(L"Player_Item_Antivenom", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/supply_antivenom.png", 1, bDefault);
	CreateNewTexture(L"Player_Item_Bandage", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/supply_bandage.png", 1, bDefault);

	CreateNewTexture(L"Player_Item_RedGem", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/item_RedGem.png", 1, bDefault);
	CreateNewTexture(L"Player_Item_GreenGem", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/item_GreenGem.png", 1, bDefault);
	CreateNewTexture(L"Player_Item_BlueGem", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/item_BlueGem.png", 1, bDefault);
	
	//PlayerHand + Player FPS Mode-----------------------------------------------------------------

	CreateNewTexture(L"Player_SpellHand_Fire", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerHand/Fire/spellhand_fire_00%d.png", 5, bDefault);
	CreateNewTexture(L"Player_SpellHand_Storm", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerHand/Storm/spellhand_storm_00%d.png", 5, bDefault);


	CreateNewTexture(L"SpellHand_Proj_Fire", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerHand/Fire/Projectile/particle_fire_0000.png", 1, bDefault);

	CreateNewTexture(L"SpellHand_Proj_Effect", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerHand/Fire/Effect/explosion_big2_00%d.png", 8, bDefault);

	CreateNewTexture(L"Player_HUD_bottom_left", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Player/HUD_bottom_right.png", 1, bDefault);

	CreateNewTexture(L"Player_HP_Empty1", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Player/Player_FPSUI_EmptyBar.png", 1, bDefault);
	CreateNewTexture(L"Player_HP_Empty2", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Player/Player_FPSUI_EmptyBar2.png", 1, bDefault);

	CreateNewTexture(L"Player_UI_Shield", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Player/spell_shield.png", 1, bDefault);

	CreateNewTexture(L"Particle_Flare", TEX_NORMAL,
		L"../Bin/Resource/Image/Particle/flare.png", 1, bDefault);

	CreateNewTexture(L"Effect_Spell_Blur", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerHand/Fire/Overlay/Effect_Spell_Blur.png", 1, bDefault);

	//--------------------------------------------------------------------------------------------

	// Item
	CreateNewTexture(L"Item_Torch", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/supply_torch.png", 1, bDefault);
	CreateNewTexture(L"Item_Key", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/supply_key.png", 1, bDefault);
	CreateNewTexture(L"Item_Food", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/supply_food.png", 1, bDefault);
	CreateNewTexture(L"Item_Shovel", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/item_food.png", 1, bDefault);

	CreateNewTexture(L"Item_Heirlooms", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/item_Heirlooms.png", 1, bDefault);

	CreateNewTexture(L"Item_MagicRing", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/Item_MagicRing.png", 1, bDefault);
	 
	// Gold
	CreateNewTexture(L"Item_Gold1", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/inv_gold+_0.png", 1, bDefault);
	CreateNewTexture(L"Item_Gold2", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/inv_gold+_1.png", 1, bDefault);
	CreateNewTexture(L"Item_Gold3", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/inv_gold+_2.png", 1, bDefault);
	CreateNewTexture(L"Item_Gold4", TEX_NORMAL,
		L"../Bin/Resource/Image/PlayerItem/inv_gold+_3.png", 1, bDefault);

	//Hero
	CreateNewTexture(L"Hero_Death", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Cutthroat/5.png", 1, bDefault);

	CreateNewTexture(L"Effect_Blood", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Effect/Blood/armatureName_blood_splatter_%d.png", 35, bDefault);

	//방패파괴자
	{
		// 영웅 애니메이션

		CreateNewTexture(L"ShieldBreaker_Combat", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Combat/armatureName_combat_%d.png", 13, bDefault);
		CreateNewTexture(L"ShieldBreaker_Idle", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Idle/armatureName_idle_%d.png", 15, bDefault);
		CreateNewTexture(L"ShieldBreaker_Walk", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Walk/armatureName_walk_%d.png", 16, bDefault);

		CreateNewTexture(L"ShieldBreaker_Pierce", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Pierce.png", 1, bDefault);
		CreateNewTexture(L"ShieldBreaker_Puncture", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Puncture.png", 1, bDefault);
		CreateNewTexture(L"ShieldBreaker_AddersKiss", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/AddersKiss.png", 1, bDefault);
		CreateNewTexture(L"ShieldBreaker_Impale", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Impale.png", 1, bDefault);

		CreateNewTexture(L"ShieldBreaker_Defend", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Defend.png", 1, bDefault);

		// 스킬 이미지
		
		CreateNewTexture(L"Pierce_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Skill/Pierce_Img.png", 1, bDefault);
		CreateNewTexture(L"Puncture_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Skill/Puncture_Img.png", 1, bDefault);
		CreateNewTexture(L"AddersKiss_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Skill/AddersKiss_Img.png", 1, bDefault);
		CreateNewTexture(L"Impale_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Skill/Impale_Img.png", 1, bDefault);
		CreateNewTexture(L"Captivate_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Skill/Captivate_Img.png", 1, bDefault);
		CreateNewTexture(L"Expose_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Skill/Expose_Img.png", 1, bDefault);

		CreateNewTexture(L"Pierce_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Skill/Pierce_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"Puncture_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Skill/Puncture_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"AddersKiss_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Skill/AddersKiss_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"Impale_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Skill/Impale_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"Captivate_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Skill/Captivate_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"Expose_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Skill/Expose_Img_BW.png", 1, bDefault);

		// 스킬 이펙트 (본인에게)

		CreateNewTexture(L"AddersKiss_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/fx/adders_kiss/armatureName_adders_kiss_%d.png", 20, bDefault);
		CreateNewTexture(L"Pierce_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/fx/pierce/armatureName_pierce_%d.png", 15, bDefault);
		CreateNewTexture(L"Puncture_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/fx/break_guard/armatureName_break_guard_%d.png", 20, bDefault);
		CreateNewTexture(L"Impale_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/fx/spearing/armatureName_spearing_%d.png", 17, bDefault);



		// 스킬 이펙트 (타겟)

		CreateNewTexture(L"AddersKiss_Target_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/fx/adders_kiss_target/armatureName_adders_kiss_target_%d.png", 20, bDefault);
		CreateNewTexture(L"Impale_Target_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/fx/spearing_target/armatureName_spearing_target_%d.png", 20, bDefault);
	}

	//노상강도
	{
		CreateNewTexture(L"Highwayman_Combat", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Combat/armatureName_combat_%d.png", 14, bDefault);
		CreateNewTexture(L"Highwayman_Idle", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Idle/armatureName_idle_%d.png", 15, bDefault);
		CreateNewTexture(L"Highwayman_Walk", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Walk/armatureName_walk_%d.png", 16, bDefault);

		CreateNewTexture(L"Highwayman_WickedSlice", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/WickedSlice.png", 1, bDefault);
		CreateNewTexture(L"Highwayman_PistolShot", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/PistolShot.png", 1, bDefault);
		CreateNewTexture(L"Highwayman_PointBlankShot", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/PointBlankShot.png", 1, bDefault);
		CreateNewTexture(L"Highwayman_DuelistsAdvance", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/DuelistsAdvance.png", 1, bDefault);

		CreateNewTexture(L"Highwayman_Defend", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Defend.png", 1, bDefault);


		// 스킬 이미지

		CreateNewTexture(L"WickedSlice_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Skill/WickedSlice_Img.png", 1, bDefault);
		CreateNewTexture(L"PistolShot_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Skill/PistolShot_Img.png", 1, bDefault);
		CreateNewTexture(L"PointBlankShot_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Skill/PointBlankShot_Img.png", 1, bDefault);
		CreateNewTexture(L"DuelistsAdvance_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Skill/DuelistsAdvance_Img.png", 1, bDefault);
		CreateNewTexture(L"GrapeshotBlast_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Skill/GrapeshotBlast_Img.png", 1, bDefault);
		CreateNewTexture(L"TrackingShot_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Skill/TrackingShot_Img.png", 1, bDefault);

		CreateNewTexture(L"WickedSlice_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Skill/WickedSlice_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"PistolShot_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Skill/PistolShot_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"PointBlankShot_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Skill/PointBlankShot_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"DuelistsAdvance_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Skill/DuelistsAdvance_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"GrapeshotBlast_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Skill/GrapeshotBlast_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"TrackingShot_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Skill/TrackingShot_Img_BW.png", 1, bDefault);


		// 스킬 이펙트 (본인에게)

		CreateNewTexture(L"PointBlankShot_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/fx/point_blank_shot/armatureName_point_blank_shot_%d.png", 20, bDefault);


		// 스킬 이펙트 (타겟)

		CreateNewTexture(L"PistolShot_Target_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/fx/pistol_shot_target/armatureName_pistol_shot_target_%d.png", 15, bDefault);
		CreateNewTexture(L"PointBlankShot_Target_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/fx/point_blank_shot_target/armatureName_point_blank_shot_target_%d.png", 15, bDefault);
	}

	//성녀
	{
		CreateNewTexture(L"Vestal_Combat", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Combat/armatureName_combat_%d.png", 16, bDefault);
		CreateNewTexture(L"Vestal_Idle", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Idle/armatureName_idle_%d.png", 18, bDefault);
		CreateNewTexture(L"Vestal_Walk", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Walk/armatureName_walk_%d.png", 16, bDefault);

		CreateNewTexture(L"Vestal_MaceBash", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Attack.png", 1, bDefault);
		CreateNewTexture(L"Vestal_DazzlingLight", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Stun.png", 1, bDefault);
		CreateNewTexture(L"Vestal_DivineGrace", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Heal.png", 1, bDefault);
		CreateNewTexture(L"Vestal_DivineComfort", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Heal.png", 1, bDefault);

		CreateNewTexture(L"Vestal_Defend", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Defend.png", 1, bDefault);

		// 스킬 이미지

		CreateNewTexture(L"MaceBash_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Skill/MaceBashImg.png", 1, bDefault);
		CreateNewTexture(L"DazzlingLight_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Skill/DazzlingLightImg.png", 1, bDefault);
		CreateNewTexture(L"DivineGrace_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Skill/DivineGraceImg.png", 1, bDefault);
		CreateNewTexture(L"DivineComfort_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Skill/DivineComfortImg.png", 1, bDefault);
		CreateNewTexture(L"HandOfLight_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Skill/HandOfLight_Img.png", 1, bDefault);
		CreateNewTexture(L"Illumination_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Skill/Illumination_Img.png", 1, bDefault);

		CreateNewTexture(L"MaceBash_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Skill/MaceBashImg_BW.png", 1, bDefault);
		CreateNewTexture(L"DazzlingLight_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Skill/DazzlingLightImg_BW.png", 1, bDefault);
		CreateNewTexture(L"DivineGrace_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Skill/DivineGraceImg_BW.png", 1, bDefault);
		CreateNewTexture(L"DivineComfort_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Skill/DivineComfortImg_BW.png", 1, bDefault);
		CreateNewTexture(L"HandOfLight_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Skill/HandOfLight_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"Illumination_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Skill/Illumination_Img_BW.png", 1, bDefault);


		// 스킬 이펙트 (본인에게)

		CreateNewTexture(L"DazzlingLight_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/fx/dazzling_light/armatureName_dazzling_light_%d.png", 22, bDefault);
		CreateNewTexture(L"DivineGrace_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/fx/divine_grace/armatureName_divine_grace_%d.png", 20, bDefault);
		CreateNewTexture(L"DivineComfort_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/fx/gods_comfort/armatureName_gods_comfort_%d.png", 20, bDefault);


	// 스킬 이펙트 (타겟)

		CreateNewTexture(L"DazzlingLight_Target_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/fx/dazzling_light_target/armatureName_dazzling_light_target_%d.png", 20, bDefault);
		CreateNewTexture(L"DivineComfort_Target_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/fx/gods_comfort_target/armatureName_gods_comfort_target_%d.png", 20, bDefault);
		CreateNewTexture(L"DivineGrace_Target_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/fx/gods_comfort_target/armatureName_gods_comfort_target_%d.png", 20, bDefault);
	}

	//광대
	{
		CreateNewTexture(L"Jester_Combat", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Combat/armatureName_combat_%d.png", 14, bDefault);
		CreateNewTexture(L"Jester_Idle", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Idle/armatureName_idle_%d.png", 14, bDefault);
		CreateNewTexture(L"Jester_Walk", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Walk/armatureName_walk_%d.png", 16, bDefault);

		CreateNewTexture(L"Jester_DirkStab", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/DirkStab.png", 1, bDefault);
		CreateNewTexture(L"Jester_Harvest", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Harvest.png", 1, bDefault);
		CreateNewTexture(L"Jester_Finale", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Finale.png", 1, bDefault);
		CreateNewTexture(L"Jester_Solo", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Solo.png", 1, bDefault);
		CreateNewTexture(L"Jester_SliceOff", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/SliceOff.png", 1, bDefault);
		CreateNewTexture(L"Jester_BattleBallad", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/BattleBallad.png", 1, bDefault);

		CreateNewTexture(L"Jester_Defend", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Defend.png", 1, bDefault);


		// 스킬 이미지

		CreateNewTexture(L"BattleBallad_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Skill/BattleBallad_Img.png", 1, bDefault);
		CreateNewTexture(L"DirkStab_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Skill/DirkStab_Img.png", 1, bDefault);
		CreateNewTexture(L"Finale_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Skill/Finale_Img.png", 1, bDefault);
		CreateNewTexture(L"Harvest_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Skill/Harvest_Img.png", 1, bDefault);
		CreateNewTexture(L"SliceOff_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Skill/SliceOff_Img.png", 1, bDefault);
		CreateNewTexture(L"Solo_Img", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Skill/Solo_Img.png", 1, bDefault);

		CreateNewTexture(L"BattleBallad_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Skill/BattleBallad_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"DirkStab_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Skill/DirkStab_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"Finale_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Skill/Finale_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"Harvest_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Skill/Harvest_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"SliceOff_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Skill/SliceOff_Img_BW.png", 1, bDefault);
		CreateNewTexture(L"Solo_Img_BW", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Skill/Solo_Img_BW.png", 1, bDefault);


		// 스킬 이펙트 (본인에게)

		CreateNewTexture(L"BattleBallad_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/fx/battle_ballad/armatureName_battle_ballad_%d.png", 28, bDefault);
		CreateNewTexture(L"Finale_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/fx/finale/armatureName_finale_%d.png", 20, bDefault);
		CreateNewTexture(L"Solo_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/fx/solo/armatureName_solo_%d.png", 28, bDefault);


		// 스킬 이펙트 (타겟)

		CreateNewTexture(L"BattleBallad_Target_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/fx/battle_ballad_target/armatureName_battle_ballad_target_%d.png", 20, bDefault);
		CreateNewTexture(L"Finale_Target_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/fx/finale_target/armatureName_finale_target_%d.png", 20, bDefault);
		CreateNewTexture(L"Solo_Target_Effect", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/fx/solo_target/armatureName_solo_target_%d.png", 20, bDefault);

	}



	// 붕괴, 기상 (진짜)
	{
		CreateNewTexture(L"Highwayman_Affliction", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Affliction/armatureName_afflicted_%d.png", 76, bDefault);
		CreateNewTexture(L"Jester_Affliction", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Affliction/armatureName_afflicted_%d.png", 76, bDefault);
		CreateNewTexture(L"Vestal_Affliction", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Affliction/armatureName_afflicted_%d.png", 76, bDefault);
		CreateNewTexture(L"ShieldBreaker_Affliction", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Affliction/armatureName_afflicted_%d.png", 76, bDefault);


		CreateNewTexture(L"Highwayman_Virtue", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Virtue/armatureName_heroic_human_%d.png", 76, bDefault);
		CreateNewTexture(L"Jester_Virtue", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Jester/Virtue/armatureName_heroic_human_%d.png", 76, bDefault);
		CreateNewTexture(L"Vestal_Virtue", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Virtue/armatureName_heroic_human_%d.png", 76, bDefault);
		CreateNewTexture(L"ShieldBreaker_Virtue", TEX_NORMAL,
			L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Virtue/armatureName_heroic_human_%d.png", 76, bDefault);


	}

	// 붕괴, 기상(로딩 줄이기 용)
	//{
	//	CreateNewTexture(L"Highwayman_Affliction", TEX_NORMAL,
	//		L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Affliction/armatureName_afflicted_%d.png", 2, bDefault);
	//	CreateNewTexture(L"Jester_Affliction", TEX_NORMAL,
	//		L"../Bin/Resource/Image/Creatures/Heroes/Jester/Affliction/armatureName_afflicted_%d.png", 2, bDefault);
	//	CreateNewTexture(L"Vestal_Affliction", TEX_NORMAL,
	//		L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Affliction/armatureName_afflicted_%d.png", 2, bDefault);
	//	CreateNewTexture(L"ShieldBreaker_Affliction", TEX_NORMAL,
	//		L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Affliction/armatureName_afflicted_%d.png", 2, bDefault);
	//	
	//	
	//	CreateNewTexture(L"Highwayman_Virtue", TEX_NORMAL,
	//			L"../Bin/Resource/Image/Creatures/Heroes/Highwayman/Virtue/armatureName_heroic_human_%d.png", 2, bDefault);
	//	CreateNewTexture(L"Jester_Virtue", TEX_NORMAL,
	//		L"../Bin/Resource/Image/Creatures/Heroes/Jester/Virtue/armatureName_heroic_human_%d.png", 2, bDefault);
	//	CreateNewTexture(L"Vestal_Virtue", TEX_NORMAL,
	//		L"../Bin/Resource/Image/Creatures/Heroes/Vestal/Virtue/armatureName_heroic_human_%d.png", 2, bDefault);
	//	CreateNewTexture(L"ShieldBreaker_Virtue", TEX_NORMAL,
	//		L"../Bin/Resource/Image/Creatures/Heroes/ShieldBreaker/Virtue/armatureName_heroic_human_%d.png", 2, bDefault);


	//}

	// 영웅 스탯 UI
	{
		// 틀
		CreateNewTexture(L"UI_HeroStat_Base", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/Stat/HeroStat.png", 1, bDefault);
		// 성녀 틀
		CreateNewTexture(L"UI_HeroStat_Vestal_Base", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/Stat/VestalBaseStat.png", 1, bDefault);
		// 방패파괴자 틀
		CreateNewTexture(L"UI_HeroStat_Shieldbreaker_Base", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/Stat/ShieldbreakerBaseStat.png", 1, bDefault);
		// 광대 틀
		CreateNewTexture(L"UI_HeroStat_Jester_Base", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/Stat/JesterBaseStat.png", 1, bDefault);
		// 노상강도 틀
		CreateNewTexture(L"UI_HeroStat_Highwayman_Base", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/Stat/HighwaymanBaseStat.png", 1, bDefault);

		// 고용
		{
			// 성녀 틀
			CreateNewTexture(L"UI_HeroStat_Vestal_Base_Hire", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Stat/VestalBaseStatHire.png", 1, bDefault);
			// 방패파괴자 틀
			CreateNewTexture(L"UI_HeroStat_Shieldbreaker_Base_Hire", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Stat/ShieldbreakerBaseStatHire.png", 1, bDefault);
			// 광대 틀
			CreateNewTexture(L"UI_HeroStat_Jester_Base_Hire", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Stat/JesterBaseStatHire.png", 1, bDefault);
			// 노상강도 틀
			CreateNewTexture(L"UI_HeroStat_Highwayman_Base_Hire", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Stat/HighwaymanBaseStatHire.png", 1, bDefault);
		}

		// 선택
		{
			// 성녀 틀
			CreateNewTexture(L"UI_HeroStat_Vestal_Base_Select", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Stat/VestalBaseStatSelect.png", 1, bDefault);
			// 방패파괴자 틀
			CreateNewTexture(L"UI_HeroStat_Shieldbreaker_Base_Select", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Stat/ShieldbreakerBaseStatSelect.png", 1, bDefault);
			// 광대 틀
			CreateNewTexture(L"UI_HeroStat_Jester_Base_Select", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Stat/JesterBaseStatSelect.png", 1, bDefault);
			// 노상강도 틀
			CreateNewTexture(L"UI_HeroStat_Highwayman_Base_Select", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Stat/HighwaymanBaseStatSelect.png", 1, bDefault);
		}

		// 추천 위치, 목표
		CreateNewTexture(L"UI_HeroStat_Recommend_Position_1", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/Skill/armatureName_skill_strength_hero_1_1.png", 1, bDefault);
		CreateNewTexture(L"UI_HeroStat_Recommend_Position_2", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/Skill/armatureName_skill_strength_hero_2_1.png", 1, bDefault);
		CreateNewTexture(L"UI_HeroStat_Recommend_Position_3", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/Skill/armatureName_skill_strength_hero_3_1.png", 1, bDefault);
		CreateNewTexture(L"UI_HeroStat_Recommend_Position_4", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/Skill/armatureName_skill_strength_hero_4_1.png", 1, bDefault);

		CreateNewTexture(L"UI_HeroStat_Recommend_Target_1", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/Skill/armatureName_skill_strength_target_1_1.png", 1, bDefault);
		CreateNewTexture(L"UI_HeroStat_Recommend_Target_2", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/Skill/armatureName_skill_strength_target_2_1.png", 1, bDefault);
		CreateNewTexture(L"UI_HeroStat_Recommend_Target_3", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/Skill/armatureName_skill_strength_target_3_1.png", 1, bDefault);
		CreateNewTexture(L"UI_HeroStat_Recommend_Target_4", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/Skill/armatureName_skill_strength_target_4_1.png", 1, bDefault);
		 
		// 스킬 활성화 상태
		CreateNewTexture(L"UI_HeroStat_Selected_Skill", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/selected_ability.png", 1, bDefault);
		CreateNewTexture(L"UI_HeroStat_Positive_Skill", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/hero_slot.positive_frame.png", 1, bDefault);
		CreateNewTexture(L"UI_HeroStat_Negative_Skill", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/hero_slot.negative_frame.png", 1, bDefault);
		CreateNewTexture(L"UI_HeroStat_Locked_Skill", TEX_NORMAL,
			L"../Bin/Resource/Image/UI/Hero/lockedskill.png", 1, bDefault);
	}


	//스탯 표시 

	CreateNewTexture(L"Stat_StressFull", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/HeroStat/stress_full.png", 1, bDefault);
	CreateNewTexture(L"Stat_StressEmpty", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/HeroStat/stress_empty.png", 1, bDefault);
	CreateNewTexture(L"Stat_HealthFull", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/HeroStat/health_pip_full.png", 1, bDefault);
	CreateNewTexture(L"Stat_HealthEmpty", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/HeroStat/health_pip_Empty.png", 1, bDefault);
	CreateNewTexture(L"Stat_HealthGap", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/HeroStat/health_pip_gap.png", 1, bDefault);
	CreateNewTexture(L"Stat_CorpseFull", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/HeroStat/health_corpse_full.png", 1, bDefault);

	
	//전투 스테이터스

	CreateNewTexture(L"Stat_Attribute_Poison", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/skill_attributes/skill_attribute_poison.png", 1, bDefault);
	CreateNewTexture(L"Stat_Attribute_Bleed", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/skill_attributes/skill_attribute_bleed.png", 1, bDefault);
	CreateNewTexture(L"Stat_Attribute_Stun", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/skill_attributes/skill_attribute_stun.png", 1, bDefault);
	CreateNewTexture(L"Stat_Attribute_Virtue", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/skill_attributes/skill_attribute_virtue.png", 1, bDefault);
	CreateNewTexture(L"Stat_Attribute_Stress", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/skill_attributes/skill_attribute_stress.png", 1, bDefault);
	CreateNewTexture(L"Stat_Attribute_Buff", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/skill_attributes/skill_attribute_buff.png", 1, bDefault);
	CreateNewTexture(L"Stat_Attribute_Debuff", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/skill_attributes/skill_attribute_debuff.png", 1, bDefault);
	CreateNewTexture(L"Stat_Attribute_Death", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/poptext/poptext_death_avoided.png", 1, bDefault);

	// 타겟
	CreateNewTexture(L"Target_TurnCreature", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/target/selected_1.png", 1, bDefault);
	CreateNewTexture(L"Target_AttackCreature", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/target/target_1.png", 1, bDefault);
	CreateNewTexture(L"Target_AttackCreatures", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/target/target_4.png", 1, bDefault);
	CreateNewTexture(L"Target_TeamCreature", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/target/target_h_1.png", 1, bDefault);
	CreateNewTexture(L"Target_TeamCreatures", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/target/target_h_4.png", 1, bDefault);
	CreateNewTexture(L"Target_TeamPlus", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/target/plus_h.png", 1, bDefault);
	CreateNewTexture(L"Target_EnemyPlus", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/target/plus.png", 1, bDefault);


	//전투시작
	CreateNewTexture(L"Effect_BattleStart", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Battle/BattleStart/armatureName_start_%d.png", 28, bDefault);

	// BackBoard
	CreateNewTexture(L"Backboard_Text", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Battle/Backboard/TextBackboard.png", 1, bDefault);

	// 시체 사망 이펙트
	CreateNewTexture(L"Effect_Corpse", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Battle/Death/Corpse/armatureName_death_%d.png", 40, bDefault);

	// 사망 이펙트
	CreateNewTexture(L"Effect_Death", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Battle/Death/Death/armatureName_death_%d.png", 40, bDefault);

	// 로딩 스크린 횃불
	CreateNewTexture(L"UI_Loading_Torch", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Effect/Fire/armatureName_dim_loop_%d.png", 73, bDefault);
}

void CResourceMgr::UITextureLoad()
{
	_bool bDefault = false;

	//UIResource
	CreateNewTexture(L"UI_Inventory", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/panel_inventory.png", 1, bDefault);

	//MouseCursor

	CreateNewTexture(L"UI_MouseCursor_Main", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/cursors/arrow.png", 1, bDefault);


	//InventoryUI
	CreateNewTexture(L"Item_UI_Antivenom", TEX_NORMAL, 
		L"../Bin/Resource/Image/UI/Item/supply/inv_supply+antivenom.png", 1, bDefault); // 해독제
	CreateNewTexture(L"Item_UI_Shovel", TEX_NORMAL, 
		L"../Bin/Resource/Image/UI/Item/supply/inv_supply+shovel.png", 1, bDefault); //삽
	CreateNewTexture(L"Item_UI_Bandage", TEX_NORMAL, 
		L"../Bin/Resource/Image/UI/Item/supply/inv_supply+bandage.png", 1, bDefault); //붕대

	CreateNewTexture(L"Item_UI_Heirloom", TEX_NORMAL, 
		L"../Bin/Resource/Image/UI/Item/heirloom/inv_heirloom+crest.png", 1, bDefault); //가보

	CreateNewTexture(L"Item_UI_RedGem", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Item/gem/inv_gem+ruby.png", 1, bDefault);
	CreateNewTexture(L"Item_UI_BlueGem", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Item/gem/inv_gem+sapphire.png", 1, bDefault);
	CreateNewTexture(L"Item_UI_GreenGem", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Item/gem/inv_gem+emerald.png", 1, bDefault);

	CreateNewTexture(L"Item_UI_MagicRing", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Item/quest_item/Item_MagicRing_inven.png", 1, bDefault);

	//재화 ----------------------------------------------------------------------
	CreateNewTexture(L"Item_UI_Gold0", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Item/gold/inv_gold+_0.png", 1, bDefault);

	CreateNewTexture(L"Item_UI_Gold1", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Item/gold/inv_gold+_1.png", 1, bDefault);

	CreateNewTexture(L"Item_UI_Gold2", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Item/gold/inv_gold+_2.png", 1, bDefault);

	CreateNewTexture(L"Item_UI_Gold3", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Item/gold/inv_gold+_3.png", 1, bDefault);
	//------------------------------------------------------------------------------

	CreateNewTexture(L"Item_UI_Bandage", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Item/supply/inv_supply+door_key.png", 1, bDefault); // 열쇠

	CreateNewTexture(L"Item_UI_Torch", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Item/supply/inv_supply+torch.png", 1, bDefault); // 횃불
	

	//BattleUI
	CreateNewTexture(L"UI_Battle_HeroPanel", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Background/skill_panel.png", 1, bDefault);

	CreateNewTexture(L"UI_ability_move", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/select/ability_move.png", 1, bDefault);

	CreateNewTexture(L"UI_ability_pass", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/Battle/select/ability_pass.png", 1, bDefault);

	CreateNewTexture(L"Item_UI_Key", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Item/supply/inv_supply+door_key.png", 1, bDefault);
	CreateNewTexture(L"Item_UI_Torch", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Item/supply/inv_supply+torch.png", 1, bDefault);
	CreateNewTexture(L"Item_UI_Food", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Item/supply/inv_provision+_1.png", 1, bDefault);

	//재화 INFO
	CreateNewTexture(L"UI_Goods", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Village/UI_Goods.png", 1, bDefault);

	//나레이션 그라데이션
	CreateNewTexture(L"UI_Narration_BackGround", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Background/UI_Narration_Grad.png", 1, bDefault);


	// Battle UI
	{
		// Damage Number Font 
		{
			CreateNewTexture(L"UI_Damage0", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Damage/0.png", 1, bDefault);
			CreateNewTexture(L"UI_Damage1", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Damage/1.png", 1, bDefault);
			CreateNewTexture(L"UI_Damage2", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Damage/2.png", 1, bDefault);
			CreateNewTexture(L"UI_Damage3", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Damage/3.png", 1, bDefault);
			CreateNewTexture(L"UI_Damage4", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Damage/4.png", 1, bDefault);
			CreateNewTexture(L"UI_Damage5", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Damage/5.png", 1, bDefault);
			CreateNewTexture(L"UI_Damage6", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Damage/6.png", 1, bDefault);
			CreateNewTexture(L"UI_Damage7", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Damage/7.png", 1, bDefault);
			CreateNewTexture(L"UI_Damage8", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Damage/8.png", 1, bDefault);
			CreateNewTexture(L"UI_Damage9", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Damage/9.png", 1, bDefault);
		}

		// Heal Number Font
		{
			CreateNewTexture(L"UI_Heal0", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Heal/0.png", 1, bDefault);
			CreateNewTexture(L"UI_Heal1", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Heal/1.png", 1, bDefault);
			CreateNewTexture(L"UI_Heal2", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Heal/2.png", 1, bDefault);
			CreateNewTexture(L"UI_Heal3", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Heal/3.png", 1, bDefault);
			CreateNewTexture(L"UI_Heal4", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Heal/4.png", 1, bDefault);
			CreateNewTexture(L"UI_Heal5", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Heal/5.png", 1, bDefault);
			CreateNewTexture(L"UI_Heal6", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Heal/6.png", 1, bDefault);
			CreateNewTexture(L"UI_Heal7", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Heal/7.png", 1, bDefault);
			CreateNewTexture(L"UI_Heal8", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Heal/8.png", 1, bDefault);
			CreateNewTexture(L"UI_Heal9", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Heal/9.png", 1, bDefault);
		}

		// Stress Number Font
		{
			CreateNewTexture(L"UI_Stress0", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Stress/0.png", 1, bDefault);
			CreateNewTexture(L"UI_Stress1", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Stress/1.png", 1, bDefault);
			CreateNewTexture(L"UI_Stress2", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Stress/2.png", 1, bDefault);
			CreateNewTexture(L"UI_Stress3", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Stress/3.png", 1, bDefault);
			CreateNewTexture(L"UI_Stress4", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Stress/4.png", 1, bDefault);
			CreateNewTexture(L"UI_Stress5", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Stress/5.png", 1, bDefault);
			CreateNewTexture(L"UI_Stress6", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Stress/6.png", 1, bDefault);
			CreateNewTexture(L"UI_Stress7", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Stress/7.png", 1, bDefault);
			CreateNewTexture(L"UI_Stress8", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Stress/8.png", 1, bDefault);
			CreateNewTexture(L"UI_Stress9", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Number/Stress/9.png", 1, bDefault);
		}

		// Font Effect
		{
			CreateNewTexture(L"UI_Dodge", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Letter/Dodge.png", 1, bDefault);
			CreateNewTexture(L"UI_Blight", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Letter/Blight.png", 1, bDefault);
			CreateNewTexture(L"UI_Blood", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Letter/Blood.png", 1, bDefault);
			CreateNewTexture(L"UI_Buff", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Letter/Buff.png", 1, bDefault);
			CreateNewTexture(L"UI_Damage_Critical", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Letter/Damage_Critical.png", 1, bDefault);
			CreateNewTexture(L"UI_Debuff", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Letter/Debuff.png", 1, bDefault);
			CreateNewTexture(L"UI_Heal_Critical", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Letter/Heal_Critical.png", 1, bDefault);
			CreateNewTexture(L"UI_Stun", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/Letter/Stun.png", 1, bDefault);
		}

		// Head Effect
		{
			CreateNewTexture(L"UI_Head_StressUp", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/HeadEffect/armatureName_afflicted_%d.png", 54, bDefault);
			CreateNewTexture(L"UI_Head_StressDown", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/HeadEffect/armatureName_heroic_%d.png", 52, bDefault);
			CreateNewTexture(L"UI_Head_Stun", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Battle/HeadEffect/armatureName_stun_loop_%d.png", 31, bDefault);
		}

		// ETC
		{
			CreateNewTexture(L"Alpha_Black", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/BackGround/AlphaBlack.png", 1, bDefault);
		}
	}


	//Loading Scene

	CreateNewTexture(L"UI_Loading_Village", TEX_NORMAL,
		L"../Bin/Resource/Image/Loading_Screen/loading_screen_town.png", 1, bDefault);

	CreateNewTexture(L"UI_Loading_Ruin", TEX_NORMAL,
		L"../Bin/Resource/Image/Loading_Screen/loading_screen_tutorial_crypts.png", 1, bDefault);

	CreateNewTexture(L"UI_Loading_Boss", TEX_NORMAL,
		L"../Bin/Resource/Image/Loading_Screen/loading_screen_Boss.png", 1, bDefault);


	CreateNewTexture(L"UI_Loading_TipOverlay", TEX_NORMAL,
		L"../Bin/Resource/Image/Loading_Screen/loading_screen_tipoverlay.png", 1, bDefault);

	CreateNewTexture(L"UI_Loading_TitleOverlay", TEX_NORMAL,
		L"../Bin/Resource/Image/Loading_Screen/loading_screen_titleoverlay.png", 1, bDefault);

	//Interaction Info

	CreateNewTexture(L"UI_Interaction", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/UI_InteractionInfo.png", 1, bDefault);


	//Hero Portrait

	CreateNewTexture(L"UI_HeroPortrait", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/Dungeon/HeroStatus/UI_HeroPortrait_%d.png", 4, bDefault);

	//ScreenEffect

	CreateNewTexture(L"UI_ScreenEffect", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/ScreenEffect/UI_ScreenEffect_%d.png", 3, bDefault);

}

void CResourceMgr::WealdDungeonTextureLoad()
{
	_bool bDefault = true;

	// Environment
	CreateNewTexture(L"Com_Weald_FloorTexture", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Weald/forest_Tiles.png", 1, bDefault);

	CreateNewTexture(L"Com_Weald_WallTexture", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Weald/weald.corridor_wall.%d.png", 9, bDefault);
	CreateNewTexture(L"Com_Weald_BackWallTexture", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Weald/weald.corridor_mid.png", 1, bDefault);


	// 문
	CreateNewTexture(L"Weald_Door_Closed", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Weald/weald.corridor_door.basic.png", 1, bDefault);
	CreateNewTexture(L"Weald_Door_Opened", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Weald/weald.corridor_door.Opened.png", 1, bDefault);

	CreateNewTexture(L"Weald_Door", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Doors/Weald/closed.png", 1, bDefault);

	// 골동품
	CreateNewTexture(L"Weald_Spiderweb", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Weald/cosmic_spiderweb/idle/armatureName_idle_0.png", 1, bDefault);

	CreateNewTexture(L"Weald_Spiderweb_Finish", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Weald/cosmic_spiderweb/investigate/armatureName_investigate_0.png", 1, bDefault);

	CreateNewTexture(L"Weald_heirloom_chest", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Weald/heirloom_chest/idle/armatureName_idle_0.png", 1, bDefault);

	CreateNewTexture(L"Weald_heirloom_chest_Finish", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Weald/heirloom_chest/investigate/armatureName_investigate_0.png", 1, bDefault);

	CreateNewTexture(L"Weald_lost_luggage", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Weald/lost_luggage/idle/armatureName_idle_0.png", 1, bDefault);

	CreateNewTexture(L"Weald_lost_luggage_Finish", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Weald/lost_luggage/investigate/armatureName_investigate_0.png", 1, bDefault);

	CreateNewTexture(L"Weald_mummified_remains", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Weald/mummified_remains/idle/armatureName_idle_0.png", 1, bDefault);

	CreateNewTexture(L"Weald_mummified_remains_Finish", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Weald/mummified_remains/investigate/armatureName_investigate_0.png", 1, bDefault);

	CreateNewTexture(L"Weald_travellers_tent_tutorial", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Weald/travellers_tent_tutorial/idle/armatureName_idle_0.png", 1, bDefault);

	CreateNewTexture(L"Weald_travellers_tent_tutorial_Finish", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Weald/travellers_tent_tutorial/investigate/armatureName_investigate_0.png", 1, bDefault);

	// 함정
	CreateNewTexture(L"Weald_Traps_Remains", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Traps/Weald/idle/armatureName_idle_0.png", 1, bDefault);

	CreateNewTexture(L"Weald_Traps_Sprung", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Traps/Weald/sprung/armatureName_sprung_%d.png", 67, bDefault);

	// 장애물
	CreateNewTexture(L"Weald_Obstacles_Remains", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Obstacles/Weald/armatureName_clear_00.png", 1, bDefault);

	CreateNewTexture(L"Weald_Obstacles", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Obstacles/Weald/armatureName_clear_%d.png", 60, bDefault);

	// 배경이미지
	CreateNewTexture(L"Weald_Image_Wagon", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Weald/weald.tutorial.room_wall.effigy_0.png", 1, bDefault);

	CreateNewTexture(L"Weald_Image_Village", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Weald/weald.tutorial.room_wall.effigy_1.png", 1, bDefault);

	CreateNewTexture(L"Weald_Image_PoisonRiver", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Weald/weald.room_wall.poisonriver.png", 1, bDefault);

	// SkyBox
	CreateNewTexture(L"Com_Weald_SkyBoxTexture", TEX_CUBE,
		L"../Bin/Resource/Image/SkyBox/WealdSkyBox.dds", 1, bDefault);

	//Brigand Bloodletter
	CreateNewTexture(L"Brigand Bloodletter_Combat", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Bloodletter/Combat/armatureName_combat_%d.png", 36, bDefault);
	CreateNewTexture(L"Brigand Bloodletter_Attack1", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Bloodletter/1.png", 1, bDefault);
	CreateNewTexture(L"Brigand Bloodletter_Attack2", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Bloodletter/2.png", 1, bDefault);
	CreateNewTexture(L"Brigand Bloodletter_Dead", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Bloodletter/3.png", 1, bDefault);
	CreateNewTexture(L"Brigand Bloodletter_Hitted", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Bloodletter/4.png", 1, bDefault);
	CreateNewTexture(L"Brigand Bloodletter_Death", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Cutthroat/5.png", 1, bDefault);

	// Brigand Cutthroat
	CreateNewTexture(L"Brigand Cutthroat_Combat", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Cutthroat/Combat/armatureName_combat_%d.png", 33, bDefault);
	CreateNewTexture(L"Brigand Cutthroat_Attack1", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Cutthroat/1.png", 1, bDefault);
	CreateNewTexture(L"Brigand Cutthroat_Attack2", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Cutthroat/2.png", 1, bDefault);
	CreateNewTexture(L"Brigand Cutthroat_Dead", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Cutthroat/3.png", 1, bDefault);
	CreateNewTexture(L"Brigand Cutthroat_Hitted", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Cutthroat/4.png", 1, bDefault);
	CreateNewTexture(L"Brigand Cutthroat_Death", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Cutthroat/5.png", 1, bDefault);

	// Brigand Fusilier
	CreateNewTexture(L"Brigand Fusilier_Combat", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Fusilier/Combat/armatureName_combat_%d.png", 34, bDefault);
	CreateNewTexture(L"Brigand Fusilier_Attack1", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Fusilier/1.png", 1, bDefault);
	CreateNewTexture(L"Brigand Fusilier_Dead", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Fusilier/2.png", 1, bDefault);
	CreateNewTexture(L"Brigand Fusilier_Hitted", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Fusilier/3.png", 1, bDefault);
	CreateNewTexture(L"Brigand Fusilier_Death", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Cutthroat/5.png", 1, bDefault);
}

void CResourceMgr::VillageTextureLoad()
{
	_bool bDefault = true;

	CreateNewTexture(L"Village_Door_Open", TEX_NORMAL,
		L"../Bin/Resource/Image/Village/Interaction/Door/open.png", 1, bDefault);

	CreateNewTexture(L"Village_Door_Close", TEX_NORMAL,
		L"../Bin/Resource/Image/Village/Interaction/Door/closed.png", 1, bDefault);

	// 마을
	{
		//바닥
		CreateNewTexture(L"Village_FloorTexture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Floor/WholeTerrain2.jpg", 1, bDefault);

		// 갈림길
		CreateNewTexture(L"Village_CrossSign", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Environment/Sign.png", 1, bDefault);

		// 대각선 길
		CreateNewTexture(L"Village_ㅇiagonalRoadTexture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Floor/farm_roads_custom_04.png", 1, bDefault);

		//외곽 나무 그림자
		CreateNewTexture(L"Com_Village_Wall_Texture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/BackGround/forest_skirt_%d.png", 3, bDefault);

		//외곽 건물 그림자
		CreateNewTexture(L"Com_Village_Back_Texture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/BackGround/city_back_%d.png", 3, bDefault);

		//플레이어 양옆 건물
		CreateNewTexture(L"Com_Village_Left_Buildings_Texture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Wall/BG_Buildings_Left.png", 1, bDefault);
		CreateNewTexture(L"Com_Village_Right_Buildings_Texture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Wall/BG_Buildings_Right.png", 1, bDefault);

		// SkyBox
		CreateNewTexture(L"Village_SkyBoxTexture", TEX_CUBE,
			L"../Bin/Resource/Image/SkyBox/VillageSkyBox.dds", 1, bDefault);
	}

	// 상호작용 건물 외부
	{
		// 여관
		CreateNewTexture(L"Tervarn_Outside", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Facilities/OutSide/Tavern/town_tavern_level03/armatureName_active_000.png", 1, bDefault);

		// 역마차
		CreateNewTexture(L"StageCoach_Outside", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Facilities/OutSide/Stagecoach/town_stage_coach_level01/idle.png", 1, bDefault);

		// 훈련소
		CreateNewTexture(L"Guild_Outside", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Facilities/OutSide/Guild/town_guild_level03/idle.png", 1, bDefault);

		// 상점
		CreateNewTexture(L"Store_Outside", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Facilities/OutSide/Nomad Wagon/town_nomad_wagon_level03/idle.png", 1, bDefault);

		// 상점 테이블
		CreateNewTexture(L"Store_Table", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Facilities/OutSide/Nomad Wagon//Table.png", 1, bDefault);


	}

	// 상호작용 건물 내부
	{
		// 여관
		{
			CreateNewTexture(L"Inside_FloorTexture", TEX_NORMAL,
				L"../Bin/Resource/Image/Village/Floor/city_gate_floor_tile.png", 1, bDefault);

			CreateNewTexture(L"Inside_CeilingTexture", TEX_NORMAL,
				L"../Bin/Resource/Image/Village/Floor/ceiling_tile.png", 1, bDefault);

			CreateNewTexture(L"Wood_Wall_Texture", TEX_NORMAL,
				L"../Bin/Resource/Image/Village/Wall/wood_horizontal.jpg", 1, bDefault);

			CreateNewTexture(L"Gambling_Table_Texture", TEX_NORMAL,
				L"../Bin/Resource/Image/Village/Facilities/tavern/Gambling.png", 1, bDefault);

			CreateNewTexture(L"Tavern_NPC_Texture", TEX_NORMAL,
				L"../Bin/Resource/Image/Village/Facilities/tavern/Inside/NPC.png", 1, bDefault);

			CreateNewTexture(L"Tavern_Window_Texture", TEX_NORMAL,
				L"../Bin/Resource/Image/Village/Facilities/tavern/Inside/Window.png", 1, bDefault);

			CreateNewTexture(L"Tavern_Barrel_Texture", TEX_NORMAL,
				L"../Bin/Resource/Image/Village/Facilities/tavern/Inside/Barrel.png", 1, bDefault);

			CreateNewTexture(L"Tavern_Stove_Texture", TEX_NORMAL,
				L"../Bin/Resource/Image/Village/Facilities/tavern/Inside/Stove.png", 1, bDefault);

			CreateNewTexture(L"Tavern_Bar_Table_Texture", TEX_NORMAL,
				L"../Bin/Resource/Image/Village/Facilities/tavern/Inside/Bar_Table.png", 1, bDefault);

			CreateNewTexture(L"Tavern_Bar_Back_Texture", TEX_NORMAL,
				L"../Bin/Resource/Image/Village/Facilities/tavern/Inside/Bar_Back.png", 1, bDefault);
		}

		// 훈련소
		{
			CreateNewTexture(L"Guild_NPC_Texture", TEX_NORMAL,
				L"../Bin/Resource/Image/Village/Facilities/guild/Inside/NPC.png", 1, bDefault);

			CreateNewTexture(L"Guild_SandBack_Texture", TEX_NORMAL,
				L"../Bin/Resource/Image/Village/Facilities/guild/Inside/SandBack.png", 1, bDefault);

			CreateNewTexture(L"Guild_WallSword_Texture", TEX_NORMAL,
				L"../Bin/Resource/Image/Village/Facilities/guild/Inside/WallSword.png", 1, bDefault);
		}
	}

	// 마을 장식
	{
		CreateNewTexture(L"Village_Entrance_Texture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Environment/Entrance.png", 1, bDefault);
		CreateNewTexture(L"Village_Entrance2_Texture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Environment/Entrance2.png", 1, bDefault);
		CreateNewTexture(L"Village_Entrance3_Texture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Environment/Entrance3.png", 1, bDefault);
		CreateNewTexture(L"Village_Entrance4_Texture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Environment/Entrance4.png", 1, bDefault);

		CreateNewTexture(L"Village_Wall_Texture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Environment/Wall.png", 1, bDefault);
		CreateNewTexture(L"Village_Wall1_Texture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Environment/Wall1.png", 1, bDefault);
		CreateNewTexture(L"Village_Wall2_Texture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Environment/Wall2.png", 1, bDefault);
		CreateNewTexture(L"Village_Wall3_Texture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Environment/Wall3.png", 1, bDefault);

		CreateNewTexture(L"Village_Portal_Texture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Environment/Portal.png", 1, bDefault);
		CreateNewTexture(L"Village_Portal2_Texture", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Environment/Portal2.png", 1, bDefault);
	}

	// UI
	{
		// 훈련소
		{
			// 기본 스킬 UI
			CreateNewTexture(L"UI_HeroSkill_Base", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Guild/base_guild.png", 1, bDefault);
			// 성녀 스킬 UI
			CreateNewTexture(L"UI_HeroSkill_Vestal", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Guild/vestal_guild.png", 1, bDefault);
			// 방패파괴자 스킬 UI
			CreateNewTexture(L"UI_HeroSkill_Shieldbreaker", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Guild/shieldbreaker_guild.png", 1, bDefault);
			// 광대 스킬 UI
			CreateNewTexture(L"UI_HeroSkill_Jester", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Guild/jester_guild.png", 1, bDefault);
			// 노상강도 스킬 UI
			CreateNewTexture(L"UI_HeroSkill_Highwayman", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Guild/highwayman_guild.png", 1, bDefault);
		}

		// 주점
		{
			// 기본 UI
			CreateNewTexture(L"UI_HeroTavern_Base", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Tavern/base_tavern.png", 1, bDefault);
			// 성녀 UI
			CreateNewTexture(L"UI_HeroTavern_Vestal", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Tavern/vestal_tavern_button.png", 1, bDefault);
			// 방패파괴자 UI
			CreateNewTexture(L"UI_HeroTavern_ShieldBreaker", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Tavern/shieldbreaker_tavern_button.png", 1, bDefault);
			// 광대 UI
			CreateNewTexture(L"UI_HeroTavern_Jester", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Tavern/jester_tavern_button.png", 1, bDefault);
			// 노상강도 UI
			CreateNewTexture(L"UI_HeroTavern_Highwayman", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/Hero/Tavern/highwayman_tavern_button.png", 1, bDefault);

		}

		// 아이콘
		{
			// 골드 아이콘
			CreateNewTexture(L"UI_Icon_Gold", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/UI/currency.gold.icon.png", 1, bDefault);
			// 가보 아이콘
			CreateNewTexture(L"UI_Icon_Crest", TEX_NORMAL,
				L"../Bin/Resource/Image/UI/UI/currency.crest.icon_1.png", 1, bDefault);
		}
	}


	//미니게임

	{
		//UI_ClickEffect_06
		CreateNewTexture(L"Minigame_Card", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Minigame/Minigame_Card%d.png", 5, bDefault);

		CreateNewTexture(L"Minigame_CardBack", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Minigame/Minigame_CardBack.png", 1, bDefault);

		CreateNewTexture(L"Minigame_ClickEffect", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Minigame/ClickEffect/UI_ClickEffect_%d.png", 7, bDefault);

		CreateNewTexture(L"Minigame_BackGround", TEX_NORMAL,
			L"../Bin/Resource/Image/Village/Minigame/BackGround/Minigame_BackGround.png", 1, bDefault);

	}

}

void CResourceMgr::RuinDungeonTextureLoad()
{
	_bool bDefault = true;

	// Environment
	CreateNewTexture(L"Com_Ruin_FloorTexture", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Ruin/bricks.png", 1, bDefault);

	CreateNewTexture(L"Com_Ruin_RoofTexture", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Ruin/bricks_gradient_02.png", 1, bDefault);


	CreateNewTexture(L"Com_Ruin_WallTexture", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Ruin/crypts.corridor_wall.%d.png", 7, bDefault);
	CreateNewTexture(L"Com_Ruin_BackWallTexture", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Ruin/crypts.corridor_mid.png", 1, bDefault);

	CreateNewTexture(L"Com_SecretWallTexture", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Ruin/crypts.endhall.01.png", 1, bDefault);

	// 문
	CreateNewTexture(L"Ruin_Door_Closed", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Ruin/crypts.corridor_door.Closed.png", 1, bDefault);
	CreateNewTexture(L"Ruin_Door_Opened", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Ruin/crypts.corridor_door.Opened.png", 1, bDefault);

	CreateNewTexture(L"Ruin_Door_Right", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Doors/Ruins/Right.png", 1, bDefault);

	CreateNewTexture(L"Ruin_Door_Left", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Doors/Ruins/Left.png", 1, bDefault);

	// 골동품
	CreateNewTexture(L"Ruin_Armor", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Ruin/suit_of_armor/idle/armatureName_idle_0.png", 1, bDefault);

	CreateNewTexture(L"Ruin_Armor_Finish", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Ruin/suit_of_armor/investigate/armatureName_investigate_0.png", 1, bDefault);

	CreateNewTexture(L"Ruin_Fountain", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Ruin/holy_fountain/idle/armatureName_idle_%d.png", 35, bDefault);

	CreateNewTexture(L"Ruin_Fountain_Finish", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Ruin/holy_fountain/investigate/armatureName_investigate_0.png", 1, bDefault);

	CreateNewTexture(L"Ruin_locked_sarcophagus", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Ruin/locked_sarcophagus/idle/armatureName_idle_0.png", 1, bDefault);

	CreateNewTexture(L"Ruin_locked_sarcophagus_Finish", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Ruin/locked_sarcophagus/investigate/armatureName_investigate_0.png", 1, bDefault);

	CreateNewTexture(L"Ruin_Sconce", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Ruin/sconce/idle/armatureName_idle_0.png", 1, bDefault);

	CreateNewTexture(L"Ruin_Sconce_Fire", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Ruin/sconce/Fire/armatureName_campfire_loop_0%d.png", 14, bDefault);


	CreateNewTexture(L"Ruin_Sconce_Finish", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Ruin/sconce/investigate/armatureName_investigate_0.png", 1, bDefault);

	//퍼즐방 스태츄
	CreateNewTexture(L"Ruin_OldOne_Statue", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Curios/Ruin/Old_One_Statue/oldone_statue_%d.png", 4, bDefault);

	// 함정
	CreateNewTexture(L"Ruin_Traps_Remains", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Traps/Ruin/idle/armatureName_idle_0.png", 1, bDefault);

	CreateNewTexture(L"Ruin_Traps_Sprung", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Traps/Ruin/sprung/armatureName_sprung_%d.png", 67, bDefault);

	// 장애물
	CreateNewTexture(L"Ruin_Obstacles_Remains", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Obstacles/Ruin/armatureName_clear_00.png", 1, bDefault);

	CreateNewTexture(L"Ruin_Obstacles", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Obstacles/Ruin/armatureName_clear_%d.png", 30, bDefault);

	// 배경이미지
	CreateNewTexture(L"Ruin_Image_Altar", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/Ruin/crypts.room_wall.altar.png", 1, bDefault);

	// SkyBox
	CreateNewTexture(L"Com_Weald_SkyBoxTexture", TEX_CUBE,
		L"../Bin/Resource/Image/SkyBox/WealdSkyBox.dds", 1, bDefault);

	// Bone Soldier
	CreateNewTexture(L"Bone Soldier_Combat", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Ruin/Bone Soldier/Bone Soldier/combat/armatureName_combat_%d.png", 28, bDefault);
	CreateNewTexture(L"Bone Soldier_Attack1", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Ruin/Bone Soldier/1.png", 1, bDefault);
	CreateNewTexture(L"Bone Soldier_Dead", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Ruin/Bone Soldier/2.png", 1, bDefault);
	CreateNewTexture(L"Bone Soldier_Hitted", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Ruin/Bone Soldier/3.png", 1, bDefault);
	CreateNewTexture(L"Bone Soldier_Death", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Cutthroat/5.png", 1, bDefault);

	// Bone Defender
	CreateNewTexture(L"Bone Defender_Combat", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Ruin/Bone Defender/Combat/armatureName_combat_%d.png", 30, bDefault);
	CreateNewTexture(L"Bone Defender_Attack1", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Ruin/Bone Defender/1.png", 1, bDefault);
	CreateNewTexture(L"Bone Defender_Attack2", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Ruin/Bone Defender/2.png", 1, bDefault);
	CreateNewTexture(L"Bone Defender_Dead", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Ruin/Bone Defender/3.png", 1, bDefault);
	CreateNewTexture(L"Bone Defender_Hitted", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Ruin/Bone Defender/4.png", 1, bDefault);
	CreateNewTexture(L"Bone Defender_Death", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Cutthroat/5.png", 1, bDefault);

	// Bone Courtier
	CreateNewTexture(L"Bone Courtier_Combat", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Ruin/Bone Courtier/Combat/armatureName_combat_%d.png", 26, bDefault);
	CreateNewTexture(L"Bone Courtier_Attack1", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Ruin/Bone Courtier/1.png", 1, bDefault);
	CreateNewTexture(L"Bone Courtier_Attack2", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Ruin/Bone Courtier/2.png", 1, bDefault);
	CreateNewTexture(L"Bone Courtier_Dead", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Ruin/Bone Courtier/3.png", 1, bDefault);
	CreateNewTexture(L"Bone Courtier_Hitted", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Ruin/Bone Courtier/4.png", 1, bDefault);
	CreateNewTexture(L"Bone Courtier_Death", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Cutthroat/5.png", 1, bDefault);

	//Puzzle Hint

	CreateNewTexture(L"UI_Puzzle_Hint", TEX_NORMAL,
		L"../Bin/Resource/Image/UI/UI_Puzzle_Hint.png", 1, bDefault);

	CreateNewTexture(L"Door_BossRoom", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/Doors/BossRoom/armatureName_closed_%d.png", 25, bDefault);


}

void CResourceMgr::BossMapTextureLoad()
{
	_bool bDefault = true;
	
	//Brigand Bloodletter
	CreateNewTexture(L"Brigand Bloodletter_Combat", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Bloodletter/Combat/armatureName_combat_%d.png", 36, bDefault);
	CreateNewTexture(L"Brigand Bloodletter_Attack1", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Bloodletter/1.png", 1, bDefault);
	CreateNewTexture(L"Brigand Bloodletter_Attack2", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Bloodletter/2.png", 1, bDefault);
	CreateNewTexture(L"Brigand Bloodletter_Dead", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Bloodletter/3.png", 1, bDefault);
	CreateNewTexture(L"Brigand Bloodletter_Hitted", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Bloodletter/4.png", 1, bDefault);
	CreateNewTexture(L"Brigand Bloodletter_Death", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Weald/Brigand Cutthroat/5.png", 1, bDefault);

	// Environment
	CreateNewTexture(L"Com_Boss_FloorTexture", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/BossMap/texture_hell_rock_01a.png", 1, bDefault);

	CreateNewTexture(L"Com_Boss_WallTexture", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/BossMap/darkestdungeon.corridor_wall.%d.png", 6, bDefault);
	

	// 부서지는 벽
	CreateNewTexture(L"Com_Boss_ObjectWallTexture", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/BossMap/TEXTURE_SEWER_WALL_2.png", 1, bDefault);
	CreateNewTexture(L"Com_Boss_ObjectWallTopTexture", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/BossMap/texture_egypt_rock06a.png", 1, bDefault);
	// 부서지는 애니메이션
	CreateNewTexture(L"Com_Boss_ObjectWallTexture", TEX_NORMAL,
		L"../Bin/Resource/Image/Dungeons/BackGround/BossMap/sewer_wall_broken00%d.png", 4, bDefault);

	// SkyBox
	CreateNewTexture(L"Com_BossMap_SkyBoxTexture", TEX_CUBE,
		L"../Bin/Resource/Image/SkyBox/BossSkybox.dds", 1, bDefault);

	// Mob
	CreateNewTexture(L"Mob_Idle", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Mob/Idle/cthulhu_tentacle_Idle.png", 1, bDefault);
	CreateNewTexture(L"Mob_Out", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Mob/In/cthulhu_tentacle_in00%d.png", 7, bDefault);
	CreateNewTexture(L"Mob_Death", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Mob/Death/cthulhu_tentacle_death00%d.png", 11, bDefault);
	CreateNewTexture(L"Mob_In", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Mob/Out/cthulhu_tentacle_out00%d.png", 9, bDefault);
	CreateNewTexture(L"Mob_Shoot", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Mob/Shoot/cthulhu_tentacle_shoot00%d.png", 11, bDefault);
	

	CreateNewTexture(L"Mob_Shooting", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Mob/Shooting/cthulhu_tentacle_shoot0007.png", 1, bDefault);



	// Boss
	// Phase1
	CreateNewTexture(L"Boss_Phase1_Idle", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Phase1/Idle/hellboss_caco_float00%d.png", 9, bDefault);
	CreateNewTexture(L"Boss_Phase1_Attack", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Phase1/Attack/hellboss_caco_attack00%d.png", 12, bDefault);
	CreateNewTexture(L"Boss_Phase1_Laser1", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Phase1/Attack_Laser/hellboss_caco_attack_eyelasers00%d.png", 7, bDefault);
	CreateNewTexture(L"Boss_Phase1_Laser2", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Phase1/Attack_Laser/hellboss_caco_attack_eyelasers01%d.png", 4, bDefault);
	CreateNewTexture(L"Boss_Phase1_Dash", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Phase1/Attack_Nova/hellboss_caco_attack_nova00%d.png", 11, bDefault);
	CreateNewTexture(L"Boss_Phase1_Change", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Phase1/Change/hellboss_caco_morph00%d.png", 43, bDefault);
	CreateNewTexture(L"Boss_Phase1_Dash_Ready", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Phase1/Dash_Ready/hellboss_caco_attack_nova00%d.png", 4, bDefault);
	CreateNewTexture(L"Boss_Phase1_Dash_Charge", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Phase1/Dash_Charge/hellboss_caco_attack_nova00%d.png", 4, bDefault);
	CreateNewTexture(L"Boss_Phase1_Dashes", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Phase1/Dash/hellboss_caco_attack_nova00%d.png", 3, bDefault);
	CreateNewTexture(L"Boss_Phase1_Groggy", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Phase1/Groggy/hellboss_caco_morph00%d.png", 6, bDefault);


	// Phase2
	CreateNewTexture(L"Boss_Phase2_Idle", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Phase2/Idle/hellboss_final_idle00%d.png", 21, bDefault);
	CreateNewTexture(L"Boss_Phase2_Death", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Phase2/Death/hellboss_final_death00%d.png", 26, bDefault);

	//
	CreateNewTexture(L"Boss_Phase1_Ultimate", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Phase1/Ultimate/hellboss_caco_attack0004.png", 1, bDefault);

	// Projectile
	CreateNewTexture(L"Boss_Projectile1", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Projectile/Bullet1/hell_bullet00%d.png", 4, bDefault);
	CreateNewTexture(L"Boss_Projectile2", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Projectile/Bullet2/ice_bullet00%d.png", 9, bDefault);
	CreateNewTexture(L"Boss_Projectile3", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Projectile/Bullet3/stormball00%d.png", 8, bDefault);
	CreateNewTexture(L"Boss_Laser", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Projectile/Laser/Laser.png", 1, bDefault);
	CreateNewTexture(L"Mob_Missile", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Mob/Missile/cthulhu_tentacle_missile00%d.png", 15, bDefault);

	// Spike
	CreateNewTexture(L"Sunken", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Projectile/Final_Spike1/hellboss_final_spike_02_00%d.png", 13, bDefault);
	CreateNewTexture(L"Spike_Attack", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Projectile/Final_Spike2/Attack/hellboss_final_tentacle00%d.png", 17, bDefault);
	CreateNewTexture(L"Spike_Ready", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Projectile/Final_Spike2/Ready/hellboss_final_tentacle0000.png", 1, bDefault);

	// Effect
	CreateNewTexture(L"Teleport_Effect", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Effect/Teleport/death_magic00%d.png", 10, bDefault);
	CreateNewTexture(L"Bullet_Hitted", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Effect/Bullet_Hitted/bullet_hit_blue00%d.png", 9, bDefault);

	CreateNewTexture(L"Green_Blood", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Effect/Hitted1/bloodhit_big_green00%d.png", 8, bDefault);
	CreateNewTexture(L"Yellow_Blood", TEX_NORMAL,
		L"../Bin/Resource/Image/Creatures/Monsters/Boss/Effect/Hitted2/bloodhit_big_yellow00%d.png", 8, bDefault);
}
