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

	//Animation Key ���� Ȥ�� �ʱ� ���� �� ���
	HRESULT	SetAnimKey(const tstring& _keyName, float _fAnimTime);

	//�� �����Ӹ��� �̹��� �������� �ѱ��� Ȯ���ؾ��ϱ� ������ Texture�� �ٸ��� Update�� ���� ����
	virtual		_int	UpdateComponent(const _float& fTimeDelta);
	virtual		void	LateUpdateComponent();

	void		SetAnimTexture();

public:



public:
	//�ִϸ��̼��� loop�� ������ �ʰ� �ѹ��� ������ 
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