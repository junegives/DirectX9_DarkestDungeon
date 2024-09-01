#pragma once
#include "GameObject.h" 

BEGIN(Engine)

class CTerrainTex;
class CTexture;
class CTransform;

END



class CTerrain : public CGameObject
{
public :
	CTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	CTerrain(LPDIRECT3DDEVICE9 pGraphicDev, const tstring& _KeyName, ETerrainType _eType);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

private:
	virtual void			AddComponent();
	void			KeyInput(const _float& fTimeDelta);
	//HRESULT			SetUpMaterial(void);

private:
	shared_ptr<CTerrainTex> m_pBufferCom = nullptr;
	shared_ptr<CTexture> m_pTextureCom = nullptr;
	shared_ptr<CTransform> m_pTransformCom = nullptr;

	_float		m_fSpeed = 10.f;

	tstring		m_strKeyName;
	ETerrainType m_eTerrainType = ETerrainType::ENUM_END;
};
