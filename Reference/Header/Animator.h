#pragma once


#include"Component.h"


BEGIN(Engine)

struct tagFrame {
	_uint iCurrentFrame;
	_uint iMaxFrame;
	_float fTime;
};

class ENGINE_DLL CAnimator : public CComponent
{
public:
	explicit CAnimator(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAnimator();

public:

	//Animation Key 변경 혹은 초기 세팅 때 사용
	HRESULT	SetAnimKey(const tstring& _keyName, float _fAnimTime);

	//매 프레임마다 이미지 프레임을 넘길지 확인해야하기 때문에 Texture와 다르게 Update를 갖고 있음
	virtual		_int	UpdateComponent(const _float& fTimeDelta);
	virtual		void	LateUpdateComponent();

	void		SetAnimTexture();

public:



public:
	//애니메이션을 loop로 돌리지 않고 한번만 돌릴때 
	_bool	CheckFinish() { return m_bFinishCount; }

	_vec2 GetTextureSize() { 
		if (m_vecTexture)
			return m_vecTexture->front()->vImgSize;
		
		_vec2 vpos = _vec2(0.f, 0.f);
		return vpos;
	};


private:
	vector<shared_ptr<tagTextureInfo>>* m_vecTexture;

private:

	tagFrame m_tAnimFrame;

	_float m_fActTime= 0.f;
	_bool m_bFinishCount = false;

};

END