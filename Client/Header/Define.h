#pragma once

// Color
#define WHITE	D3DXCOLOR(1.f, 1.f, 1.f, 1.f)
#define YELLOW	D3DXCOLOR(0.878f, 0.807f, 0.6f, 1.f)
#define BROWN	D3DXCOLOR(0.549f, 0.482f, 0.4f, 1.f)
#define ORANGE	D3DXCOLOR(0.737f, 0.443f, 0.196f, 1.f)

#define RED		D3DXCOLOR(0.694f, 0.0f, 0.0f, 1.f)
#define LIGHTGREEN	D3DXCOLOR(0.529f, 0.760f, 0.254f, 1.f)
#define SKYBLUE	D3DXCOLOR(0.368f, 0.788f, 0.839f, 1.f)
#define LIME	D3DXCOLOR(0.741f, 0.76f, 0.254f, 1.f)


// Weald Dungeon
#define WEALD_WALLSIZEX 16.f
#define WEALD_WALLSIZEUPY 5.f
#define WEALD_PATHSIZEX 18.f

#define	WEALD_BACKWALLSIZEY 

// Ruin Dungeon
#define RUIN_WALLSIZEX 16.f
#define RUIN_WALLSIZEUPY 5.f
#define RUIN_PATHSIZEX 16.f

// BossMap
#define BOSS_WALLSIZEX 16.f
#define BOSS_WALLSIZEUPY 14.f
#define BOSS_PATHSIZEX 16.f

// Time
#define BATTLEINTERVEL 4.f					// �� ������ �ð�
#define CORPSEINTERVEL 0.f					// ��ü �� �ð�
#define DEATHMOVINGINTERVEL 0.5f			// �׾ ��ü �����̴� �ð�
#define SKILLMOVINGINTERVEL 0.5f			// ��� ��ų �¾Ƽ� �����̴� �ð�
#define SKILLMOVEINTERVEL	0.5f			// ��ų �Ἥ �����̴� �ð�
#define SWITCHINGINTERVEL	0.5f			// �ڸ���ü�ð�
#define WHILEATTACKINTERVEL	2.f				// ���缭 �����ϴ� �ð�
#define MOVINGBACKINTERVEL	0.2f			// ���ư��� �ð�
#define ATTACKSKILLMOVINGINTERVEL 0.3f		// ���� ��ų�� ����ϸ� �����̴� �ð�
#define STRESSEVENTINTERVEL		4.f			// ��Ʈ���� �̺�Ʈ ó�� �߻����� �ð�
#define STRESSOUTPUTINTERVEL	1.f			// ��Ʈ������ ���� ���� ��ȣ�ۿ� �߻����� �ð�
#define KEYINPUTINTERVEL 1.f				// key input intervel

#define TRAPACTIVETIME 1.f
#define OBSTACLECLEARTIME 1.2f
#define CURIOACTIVETIME 1.f

#define	HITTEDTIME	2.3f
#define	ATTACKTIME	2.3f
#define CHANGETIME	1.f
#define DELAYTIME	1.f

#define DOTDAMEFFECTTIME 1.3f
#define STRESSEFFECTTIME 3.f

// Village
#define VILLAGE_WALLSIZE	6.f
#define VILLAGE_WALLSIZEUPY 5.f
#define VILLAGE_TILECNT		10
#define VILLAGE_TILESIZE	12.f

// ����
#define CRIRATE	30
#define DEATHRATE	0