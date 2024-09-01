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
	//�ʱ� ����
	void ReadyResource(LPDIRECT3DDEVICE9 _pGraphicDev);

public:
	//�ؽ��ĸ� �����ؼ� �����ϴ� �Լ�, ť��� �Ϲ� �ؽ��� �����ϰ� ���
	HRESULT CreateNewTexture(const tstring& _KeyName, TEXTUREID _eType, wstring _pPath, const _uint& _iCnt , _bool _bDelete);
	//�ؽ��ĸ� �ҷ����� �Լ�
	vector<shared_ptr<tagTextureInfo>>* GetTexture(const tstring& _keyName, TEXTUREID _eType);

public:
	//��ü �ؽ��� ����
	void RemoveAllTexture();
	//�� �̻� ������� �ʴ�(Ư�� scene������ ���)�ؽ��� ����
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

