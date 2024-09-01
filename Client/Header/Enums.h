#pragma once

enum class EAttackType {
	ATTACK,			// 단순 공격
	BLIGHT,			// 중독
	BLEED,			// 출혈
	STUN,			// 기절
	MOVE,			// 이동
	HEAL,			// 회복
	STRESS,			// 스트레스
	ENUM_END
};

enum class EFacilityType
{
	TERVARN,
	GUILD,
	STAGECOACH,
	STORE,

	ENUM_END
};

enum class ETerrainType
{
	VILLAGE,
	VILLAGE_INSIDE,
	DUNGEON,
	DUNGEON2,
	DUNGEON2_ROOF,

	ENUM_END
};

enum class EHeroType
{
	VESTAL,
	HIGHWAYMAN,
	SHILEDBREAKER,
	JESTER,

	ENUM_END
};

enum class EWealdDungeonDoorType
{
	ROOM1,
	ROOM2,
	ROOM3,
	ROOM4,
	ROOM5,
	ROOM6,

	ENUM_END
};

enum class ERuinDungeonDoorType
{
	ROOM1,
	ROOM2,
	ROOM3,
	ROOM4,
	ROOM5,
	ROOM6,

	ENUM_END
};

enum class EState
{
	IDLE,
	ACTIVE,
	FINISH
};

enum class EHandItem {
	SHOVEL,				// 삽
	ANTI_VENOM,			// 해독제
	KEYS,				// 열쇠
	BANDAGE,			// 붕대
	GOLD,				// 골드
	HEIRLOOM,			// 가보
	FOOD,				// 음식
	TORCH,				// 횃불
	SPELLHAND_FIRE,		//보스전 마법
	SPELLHAND_STORM ,	//보스전 마법
	RED_GEM,			//퍼즐방 보석
	BLUE_GEM,
	GREEN_GEM,
	RING,
	ENUM_END	
};

enum class EScreenEffect {
	BLOOD,
	ATTACKED,
	FADEIN_WHITE,
	FADEIN_BLACK,
	ENUM_END
};