#pragma once

#include "UIObj.h"
#include "Enums.h"

BEGIN(Engine)

class CTexture;

END


class CDungeonStatus : public CUIObj
{
public:
	explicit CDungeonStatus(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CDungeonStatus();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:

	void GetEvent(_bool _bHPEvent, EHeroType _eHero);

	void GetBuff(EHeroType _eHero);

protected:
	void			AddComponent();

private:
	vector<EHeroType> m_vHeroVec;

	_int m_iCurrentHeroNum = 0;

	shared_ptr<CTransform> m_pTransformCom[4];
	shared_ptr<CTexture> m_pTextureCom = nullptr;
	shared_ptr<CRcTex> m_pRCTexCom2[4];


};

