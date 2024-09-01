#pragma once
#include "UIObj.h"

BEGIN(Engine)

class CTexture;
class CTransform;

END

class CDescription : public CUIObj
{
public:
	explicit CDescription(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CDescription();

public:
	virtual HRESULT		ReadyGameObject() override;
	virtual _int		UpdateGameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdateGameObject() override;
	virtual void		RenderGameObject() override;

public:
	// ��ü�� �Ѱܹ޾Ƽ� �����ϴ� �Լ�
	virtual void	ActiveUI(tstring _strText, _vec3 _vPos) override;
	virtual void	ActiveUI(shared_ptr<CGameObject> _pGameObject) override;
	virtual void	ActiveUI(shared_ptr<CGameObject> _pGameObject, _vec3 _vPos) override;


	// ���ڿ��� �����ϴ� �Լ�
	/*void	SetDescription(EDescriptionType _eDescription, tstring _mainContent, tstring _title = L"", tstring _subtitle = L"",
		tstring _strColorTitle1 = L"", D3DCOLOR _tColor1 = D3DCOLOR_ARGB(255, 255, 255, 255), tstring _strColorContent1 = L"",
		tstring _strColorTitle2 = L"", D3DCOLOR _tColor2 = D3DCOLOR_ARGB(255, 255, 255, 255), tstring _strColorContent2 = L"");*/

	// ������ ����ϴ� �Լ�
	void	PrintDescription();

	// ���ڿ��� ���� ���� ���� ���ͷ� �ű�� �Լ�
	void	CalcCenterPos(tstring _str, _float& _fWidthPos, _float& fHeightPos);

	// �� ������ �������� ����ϴ� �Լ�
	void	CalcLine(_bool _bSimpleContent);

	// ��� �ؽ�ó�� ũ�⸦ ���ڿ� ���� �ø��� �Լ�
	void	FitSize(_bool _bSimpleContent);

	// ��ġ, Ÿ�� �ؽ�ó�� ����ϴ� �Լ�
	void	PrintPosInfo();

	// �� ���� �þ�� Content�� ����ؼ� ����ϴ� �Լ�
	void	PrintContent();

	// ���ڿ��� ����ϴ� �Լ�
	void	PrintText(tstring _strText, D3DXCOLOR _tColor = WHITE, _bool _bNewline = false);

	void				AddComponent();

public:

protected:
	EDescriptionType m_eDescriptionType = EDescriptionType::ENUM_END;

	shared_ptr<CTexture> m_pTextureCom = nullptr;

	shared_ptr<tagDescription>	m_tDescription;

	tstring m_strMainContent = L"";
	tstring	m_strTitle = L"";
	tstring m_strSubTitle = L"";

	_float	m_fWidthLetter = 15.f;
	_float	m_fHeightLetter = 20.f;
	_float	m_fCountPerLine = 13.f;
	_float	m_fContentLine = 1.f;
	_float	m_fLine = 1.f;
	tstring	m_strNewline = L"\n";

	_bool	m_bSimpleContent = false;

	// ��¿� �ʿ��� ����
	_float m_fCalcWidth;
	_float m_fCalcHeight;

	TCHAR m_buf[64];
	_vec2 m_vTextPos;

	// ��ų�� ���Ǵ� ������Ʈ��
	shared_ptr<CTexture> m_pEmptyPosTexture = nullptr;
	shared_ptr<CTexture> m_pFillPosTexture = nullptr;
	shared_ptr<CTexture> m_pFillTargetTexture = nullptr;
	shared_ptr<CTexture> m_pLinkPosTexture = nullptr;
	shared_ptr<CTexture> m_pLinkTargetTexture = nullptr;

	shared_ptr<CTransform> m_pActiveTransform = nullptr;

	shared_ptr<CGameObject> m_pHoverObject = nullptr;
};