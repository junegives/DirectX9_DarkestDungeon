#include"pch.h"
#include "DynamicCamera.h"
#include"Export_System.h"


CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
    :Engine::CCamera(pGraphicDev)
{
}

CDynamicCamera::~CDynamicCamera()
{
}

HRESULT CDynamicCamera::ReadyGameObject()
{
	m_vEye = _vec3(3.f, 0.f, -5.f);
	m_vAt = _vec3(0.f, 0.f, 1.f);
	m_vUp = _vec3(0.f, 1.f, 0.f);

	//m_fFov = D3DXToRadian(60.f);
	//m_fAspect = (_float)WINCX / WINCY;
	//m_fNear = 0.1f;
	//m_fFar = 1000.f;

	CCamera::ReadyGameObject();

	return S_OK;
}

_int CDynamicCamera::UpdateGameObject(const _float& fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;

	_int	iExit = CCamera::UpdateGameObject(fTimeDelta);


	return iExit;
}

void CDynamicCamera::LateUpdateGameObject()
{
	Key_Input(m_fTimeDelta);

	if (false == m_bFix)
	{
		Mouse_Move();
		Mouse_Fix();
	}
}

void CDynamicCamera::Key_Input(const _float& fTimeDelta)
{
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, 0, &m_matView);

	if (Engine::Get_DIKeyState(DIK_W) & 0X80)
	{
		_vec3		vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * fTimeDelta * m_fSpeed;

		m_vEye += vLength;
		m_vAt += vLength;

	}

	if (Engine::Get_DIKeyState(DIK_S) & 0X80)
	{
		_vec3		vLook;
		memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * fTimeDelta * m_fSpeed;

		m_vEye -= vLength;
		m_vAt -= vLength;

	}

	if (Engine::Get_DIKeyState(DIK_A) & 0X80)
	{
		_vec3		vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * fTimeDelta * m_fSpeed;

		m_vEye -= vLength;
		m_vAt -= vLength;

	}

	if (Engine::Get_DIKeyState(DIK_D) & 0X80)
	{
		_vec3		vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * fTimeDelta * m_fSpeed;

		m_vEye += vLength;
		m_vAt += vLength;
	}

	if (Engine::Get_DIKeyState(DIK_TAB) & 0x80)
	{
		if (m_bCheck)
			return;

		m_bCheck = true;

		if (m_bFix)
			m_bFix = false;

		else
			m_bFix = true;
	}
	else
		m_bCheck = false;


	if (false == m_bFix)
		return;
}

void CDynamicCamera::Mouse_Move()
{
	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, 0, &m_matView);

	_long		dwMouseMove(0);

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
	{
		_vec3		vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3		vLook = m_vAt - m_vEye;

		_matrix		matRot;
		D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.f));

		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;
	}

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
	{
		_vec3		vUp{ 0.f, 1.f, 0.f };

		_vec3		vLook = m_vAt - m_vEye;

		_matrix		matRot;
		D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove / 10.f));

		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;
	}
}

void CDynamicCamera::Mouse_Fix()
{
	POINT	ptMouse{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}
