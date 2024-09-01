#pragma once

#include"Engine_Define.h"
#include"UIObj.h"

enum class ELoadingSceneType {
	VILLAGE,
	RUIN,
	BOSS,
	ENUM_END
};


BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;

END

class CLoadingBack : public CUIObj
{
public:
	explicit CLoadingBack(LPDIRECT3DDEVICE9 _pGraphicDev , ELoadingSceneType _eType);
	virtual ~CLoadingBack();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

public:

	void LoadingComplete(_bool _bValue) {
		m_bLoadingDone = _bValue;
	}

protected:
	virtual void AddComponent();

private:

	shared_ptr<CTexture> m_pBackGroundTex = nullptr;

	shared_ptr<CTexture> m_pTextureCom[2];
	shared_ptr<CTransform> m_pLayoutTransCom[3];
	shared_ptr<CRcTex> m_pLayoutRcTexCom[3];
	shared_ptr<CAnimator> m_pAnimatorCom;

private:
	ELoadingSceneType m_eLoadingType;
	_bool m_bLoadingDone = false;

};

