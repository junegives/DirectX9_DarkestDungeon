#include "pch.h"
#include "Description.h"
#include "Skill.h"

#include"Export_Utility.h"
#include"Export_System.h"

CDescription::CDescription(LPDIRECT3DDEVICE9 _pGraphicDev)
	: Engine::CUIObj(_pGraphicDev)
{
	m_bReady = false;
}

CDescription::~CDescription()
{
}

HRESULT CDescription::ReadyGameObject()
{
	if (m_bReady) return S_OK;

	m_vSize = { 130.f, 30.f, 0.f };
	m_vAngle = { 0.f, 0.f, 0.f };
	m_vPos = { -1000.f, -1000.f, 0.f };

	m_bVisible = false;
	m_bEnable = true;
	m_bActive = true;

	m_pTransCom->SetScale(m_vSize.x, m_vSize.y, m_vSize.z);
	m_pTransCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransCom->SetAngle({ 0.f, 0.f, 0.f });

	m_pActiveTransform->SetScale(5.f, 5.f, 5.f);
	m_pActiveTransform->SetAngle({ 0.f, 0.f, 0.f });

	m_tDescription = make_shared<tagDescription>();

	CUIObj::ReadyGameObject();

	m_bReady = true;

	return S_OK;
}

_int CDescription::UpdateGameObject(const _float& fTimeDelta)
{
	if (!m_bVisible)
		return 0;

	int iExit(0);
	CGameObject::UpdateGameObject(fTimeDelta);
	AddRenderGroup(RENDER_LAST, shared_from_this());

	m_bVisible = false;

	return iExit;
}

void CDescription::LateUpdateGameObject()
{
	if (!m_bVisible)
		return;
}

void CDescription::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	PrintDescription();

	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CDescription::ActiveUI(tstring _strText, _vec3 _vPos)
{
	m_bVisible = true;
	m_pHoverObject = nullptr;
	m_bSimpleContent = true;
	m_eDescriptionType = EDescriptionType::CONTENT;
	m_strMainContent = _strText;
	m_vPos = _vPos;
}

void CDescription::ActiveUI(shared_ptr<CGameObject> _pGameObject)
{
	m_bVisible = true;
	m_bSimpleContent = false;
	m_pHoverObject = _pGameObject;
	m_tDescription = _pGameObject->GetDescription();
	m_vPos = _pGameObject->GetPos();
}

void CDescription::ActiveUI(shared_ptr<CGameObject> _pGameObject, _vec3 _vPos)
{
	m_bVisible = true;
	m_bSimpleContent = false;
	m_pHoverObject = _pGameObject;
	m_tDescription = _pGameObject->GetDescription();
	m_vPos = _vPos;
}

//void CDescription::SetDescription(EDescriptionType _eDescription, tstring _mainContent, tstring _title, tstring _subtitle,
//	tstring _strColorTitle1, D3DCOLOR _tColor1, tstring _strColorContent1,
//	tstring _strColorTitle2, D3DCOLOR _tColor2, tstring _strColorContent2)
//{
//
//
//	m_tDescription->m_eDescription = _eDescription;
//
//	m_strMainContent = _mainContent;
//	m_strTitle = _title;
//	m_strSubTitle = _subtitle;
//
//	m_tColor1 = _tColor1;
//	m_strColorTitle1 = _strColorTitle1;
//	m_strColorContent1 = _strColorContent1;
//
//	m_tColor2 = _tColor2;
//	m_strColorTitle2 = _strColorTitle2;
//	m_strColorContent2 = _strColorContent2;
//}

void CDescription::PrintDescription()
{
	// �� ���� ����ؼ� ������ ����� �����ϴ� �Լ�
	CalcLine(m_bSimpleContent);

	if (m_bSimpleContent)
	{
		PrintText(m_strMainContent);
	}

	else
	{
		switch (m_tDescription->m_eDescriptionType)
		{
		case EDescriptionType::CONTENT:

			// ����
			m_strNewline = L"";
			PrintContent();

			break;
		case EDescriptionType::ITEM:

			// ����
			PrintText(m_tDescription->m_strTitle, YELLOW);
			// ����
			m_strNewline = L"\n";
			PrintContent();

			break;
		case EDescriptionType::SKILL1:

			// ��ġ, Ÿ�� �ؽ�ó
			PrintPosInfo();
			// ����
			m_strNewline = L"\n";
			PrintText(m_tDescription->m_strTitle, YELLOW, true);
			// ������
			m_strNewline += L"\n";
			PrintText(m_tDescription->m_strSubTitle, BROWN, true);
			// ����
			m_strNewline += L"\n";
			PrintContent();

			break;
		case EDescriptionType::SKILL2:

			// ��ġ, Ÿ�� �ؽ�ó
			PrintPosInfo();
			// ����
			m_strNewline = L"\n";
			PrintText(m_tDescription->m_strTitle, YELLOW, true);
			// ������
			m_strNewline += L"\n";
			PrintText(m_tDescription->m_strSubTitle, BROWN, true);
			// ����
			m_strNewline += L"\n";
			PrintContent();
			// �÷�����1
			PrintText(m_tDescription->m_strColorTitle1, m_tDescription->m_tColor1, true);
			// �÷�����1
			m_strNewline += L"\n";
			PrintText(m_tDescription->m_strColorContent1, WHITE, true);

			break;
		case EDescriptionType::SKILL3:

			// ��ġ, Ÿ�� �ؽ�ó
			PrintPosInfo();
			// ����
			m_strNewline = L"\n";
			PrintText(m_tDescription->m_strTitle, YELLOW, true);
			// ������
			m_strNewline += L"\n";
			PrintText(m_tDescription->m_strSubTitle, BROWN, true);
			// ����
			m_strNewline = L"\n\n\n";
			PrintContent();
			// �÷�����1
			PrintText(m_tDescription->m_strColorTitle1, m_tDescription->m_tColor1, true);
			// �÷�����1
			m_strNewline += L"\n";
			PrintText(m_tDescription->m_strColorContent1, WHITE, true);
			// �÷�����2
			m_strNewline += L"\n";
			PrintText(m_tDescription->m_strColorTitle2, m_tDescription->m_tColor2, true);
			// �÷�����2
			m_strNewline += L"\n";
			PrintText(m_tDescription->m_strColorContent2, WHITE, true);
			break;
		case EDescriptionType::ENUM_END:
			break;
		default:
			break;
		}
	}
}

void CDescription::CalcCenterPos(tstring _str, _float& _fWidthPos, _float& fHeightPos)
{
	_fWidthPos = // (�� ���ڼ� - ���� ����(���� ����� �����ϴ� ���� ����) / 2) / 2 * ���� �� ���α���
		(_str.size()
			- (count(_str.begin(), _str.end(), ' ') / 2.f)
			/*- (count(m_strMainContent.begin(), m_strMainContent.end(), '.') / 2.f)
			- (count(m_strMainContent.begin(), m_strMainContent.end(), ':') / 2.f)*/
			) / 2.f * m_fWidthLetter;

	fHeightPos =
		m_fLine / 2.f * m_fHeightLetter;
}

void CDescription::CalcLine(_bool _bSimpleContent)
{
	if (_bSimpleContent)
	{
		m_fLine = static_cast<_float>(std::count(m_strMainContent.begin(), m_strMainContent.end(), '\n') + 1);
	}

	else
	{
		_int _iSize = (_int)m_tDescription->m_strMainContent.size();

		m_fLine = std::count(m_tDescription->m_strMainContent.begin(), m_tDescription->m_strMainContent.end(), '\n');
		m_fLine += 1;

		/*m_fLine = _iSize / 13;

		if (_iSize % 13 > 0) m_fLine += 1;*/

		m_fContentLine = m_fLine;

		switch (m_tDescription->m_eDescriptionType)
		{
		case EDescriptionType::ITEM:
			m_fLine += 1;
			break;
		case EDescriptionType::SKILL1:
			m_fLine += 3;
			break;
		case EDescriptionType::SKILL2:
			m_fLine += 5;
			break;
		case EDescriptionType::SKILL3:
			m_fLine += 7;
			break;
			break;
		case EDescriptionType::ENUM_END:
			break;
		default:
			break;
		}
	}

	// ���� �� ���� ������� �������� ����ϴ� �Լ�
	FitSize(_bSimpleContent);
}

void CDescription::FitSize(_bool _bSimpleContent)
{
	if (_bSimpleContent)
		m_vSize.x = 130.f;
	else if (m_tDescription->m_eDescriptionType == EDescriptionType::SKILL1 || m_tDescription->m_eDescriptionType == EDescriptionType::SKILL2 || m_tDescription->m_eDescriptionType == EDescriptionType::SKILL3)
		m_vSize.x = 160.f;
	
	m_vSize.y = (m_fLine) * 15.f;
	m_pTransCom->SetScale(m_vSize.x, m_vSize.y, m_vSize.z);

	// ���ΰ� ȭ���� ����� ���
	if (m_vPos.x + m_vSize.x + m_vSize.x / 2.f > WINCX / 2.f)
		m_vPos.x -= m_vSize.x;
	else
		m_vPos.x += m_vSize.x;

	// ���ΰ� ȭ���� ����� ���
	if (fabs(m_vPos.y) + m_vSize.y > WINCY / 2.f)
	{
		// ȭ�� �Ʒ��� �������� ���
		if (m_vPos.y < 0.f)
		{
			m_vPos.y += fabs(m_vPos.y) + m_vSize.y - WINCY / 2.f;
		}

		else if (m_vPos.y > 0.f)
		{

		}
	}

	m_pTransCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);

	m_pTextureCom->SetTexture(0);
	m_pRcTexCom->RenderBuffer();
}

void CDescription::PrintPosInfo()
{
	_matrix _matTemp;
	_matTemp = *m_pActiveTransform->GetWorld();

	// Ȱ�� ��ġ
	for (int i = 0; i < 4; ++i)
	{
		m_pActiveTransform->SetPosition(m_vPos.x, m_vPos.y + (m_fLine * m_fHeightLetter) / 2, m_vPos.z);


		_matTemp = *m_pActiveTransform->GetWorld();
		_matTemp.m[3][0] -= (i + 3) * 13.f;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &_matTemp);

		// Ȱ�� ��ġ��
		if(dynamic_pointer_cast<CSkill>(m_pHoverObject)->IsActivatePos(i))
			m_pFillPosTexture->SetTexture(0);

		// ��Ȱ�� ��ġ��
		else
			m_pEmptyPosTexture->SetTexture(0);

		m_pRcTexCom->RenderBuffer();
	}

	// Ȱ�� Ÿ��
	for (int i = 0; i < 4; ++i)
	{
		m_pActiveTransform->SetPosition(m_vPos.x, m_vPos.y + (m_fLine * m_fHeightLetter) / 2 - 1.f, m_vPos.z);


		_matTemp = *m_pActiveTransform->GetWorld();
		_matTemp.m[3][0] += (i + 3) * 13.f;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &_matTemp);

		// Ȱ�� ��ġ��
		if (dynamic_pointer_cast<CSkill>(m_pHoverObject)->IsTargetPos(i) || dynamic_pointer_cast<CSkill>(m_pHoverObject)->IsTargetAll())
			m_pFillTargetTexture->SetTexture(0);

		// ��Ȱ�� ��ġ��
		else
			m_pEmptyPosTexture->SetTexture(0);

		m_pRcTexCom->RenderBuffer();
	}

	// Ÿ���� ��ü���
	if(dynamic_pointer_cast<CSkill>(m_pHoverObject)->IsTargetAll())
	{
		_matTemp.m[3][0] -= 15.f;
		_matTemp.m[0][0] = 20.f;
		_matTemp.m[1][1] = 1.f;
		
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &_matTemp);

		m_pLinkTargetTexture->SetTexture(0);
		m_pRcTexCom->RenderBuffer();
	}
}

void CDescription::PrintContent()
{
	_int _iSize;
	_int _iIdx;

	tstring _copyText = m_tDescription->m_strMainContent;

	for (int i = 0; i < m_fContentLine; ++i)
	{
		_iSize = (_int)_copyText.size();
		// ù ��° ���� ���ڰ� ���� �� �ٹٲ����� ����

		_iIdx = static_cast<_int>(_copyText.find('\n'));
		if (_iIdx < 0) PrintText(_copyText, WHITE, true);
		else PrintText(_copyText.substr(0, _iIdx), WHITE, true);
		_copyText = _copyText.substr(_iIdx + 1);
		m_strNewline += L"\n";
	}
}

void CDescription::PrintText(tstring _strText, D3DXCOLOR _tColor, _bool _bNewline)
{
	CalcCenterPos(_strText, m_fCalcWidth, m_fCalcHeight);
	m_vTextPos = { m_vPos.x + WINCX / 2 - m_fCalcWidth - 1.f, -m_vPos.y + WINCY / 2 - m_fCalcHeight };

	if (_bNewline)
		_stprintf_s(m_buf, (m_strNewline + _strText).c_str());

	else
		_stprintf_s(m_buf, _strText.c_str());

	Engine::Render_Font(L"Font_Default_Light", m_buf, &m_vTextPos, _tColor);
}

void CDescription::AddComponent()
{
	if (m_bReady) return;

	__super::AddComponent();

	shared_ptr<CComponent> pComponent;

	// Ʋ �ؽ�ó
	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_pTextureCom->SetTextureKey(L"UI_Description", TEX_NORMAL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// ��ų�� �ؽ�ó (if������ ������ �����)
	{
		pComponent = m_pActiveTransform = make_shared<CTransform>();
		m_pActiveTransform->ReadyTransform();
		m_mapComponent[ID_DYNAMIC].insert({ L"Com_ActiveTransform",pComponent });

		// ��ų ��ġ On �ؽ�ó
		pComponent = m_pFillPosTexture = make_shared<CTexture>(m_pGraphicDev);
		m_pFillPosTexture->SetTextureKey(L"UI_Active_Pos", TEX_NORMAL);
		m_mapComponent[ID_STATIC].insert({ L"Com_TexturePosOn", pComponent });

		// ��ų ��ġ&Ÿ�� off �ؽ�ó
		pComponent = m_pEmptyPosTexture = make_shared<CTexture>(m_pGraphicDev);
		m_pEmptyPosTexture->SetTextureKey(L"UI_Deactive", TEX_NORMAL);
		m_mapComponent[ID_STATIC].insert({ L"Com_TexturePosOff", pComponent });

		// ��ų Ÿ�� On �ؽ�ó
		pComponent = m_pFillTargetTexture = make_shared<CTexture>(m_pGraphicDev);
		m_pFillTargetTexture->SetTextureKey(L"UI_Active_Target", TEX_NORMAL);
		m_mapComponent[ID_STATIC].insert({ L"Com_TextureTargetOn", pComponent });

		// ��ų ��ġ Link �ؽ�ó
		pComponent = m_pLinkPosTexture = make_shared<CTexture>(m_pGraphicDev);
		m_pLinkPosTexture->SetTextureKey(L"UI_Active_Pos_Link", TEX_NORMAL);
		m_mapComponent[ID_STATIC].insert({ L"Com_TexturePosLink", pComponent });

		// ��ų Ÿ�� Link �ؽ�ó
		pComponent = m_pLinkTargetTexture = make_shared<CTexture>(m_pGraphicDev);
		m_pLinkTargetTexture->SetTextureKey(L"UI_Active_Target_Link", TEX_NORMAL);
		m_mapComponent[ID_STATIC].insert({ L"Com_TextureTargetLink", pComponent });
	}
}
