#pragma once

#include"Engine_Define.h"

BEGIN(Engine)


class ENGINE_DLL CResourceMgr
{
	DECLARE_SINGLETON(CResourceMgr)

private:
	explicit CResourceMgr();
	~CResourceMgr();
		
public:
	//초기 세팅
	void ReadyResource(LPDIRECT3DDEVICE9 _pGraphicDev);

public:
	//텍스쳐를 세팅해서 저장하는 함수, 큐브와 일반 텍스쳐 동일하게 사용
	HRESULT CreateNewTexture(const tstring& _KeyName, TEXTUREID _eType, wstring _pPath, const _uint& _iCnt , _bool _bDelete);
	//텍스쳐를 불러오는 함수
	vector<shared_ptr<tagTextureInfo>>* GetTexture(const tstring& _keyName, TEXTUREID _eType);

public:
	//전체 텍스쳐 삭제
	void RemoveAllTexture();
	//더 이상 사용하지 않는(특정 scene에서만 사용)텍스쳐 삭제
	void RemoveSceneTexture();
	void Free();


public:
	//Create Texture Function
	void MainTitleTextureLoad();
	void BaseTextureLoad();
	void UITextureLoad();
	void WealdDungeonTextureLoad();
	void VillageTextureLoad();
	void RuinDungeonTextureLoad();
	void BossMapTextureLoad();


private:


	LPDIRECT3DDEVICE9 m_pGraphicDev;

	map<tstring, vector<shared_ptr<tagTextureInfo>> > m_TextureMap;
	map<tstring, vector<shared_ptr<tagTextureInfo>>> m_CubeMap;

//	map<tstring, vector<IDirect3DBaseTexture9*> > m_TextureMap;
	//map<tstring, vector<IDirect3DBaseTexture9*>> m_CubeMap;

};

END

