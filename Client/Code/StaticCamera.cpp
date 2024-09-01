#include"pch.h"
#include "StaticCamera.h"
#include"Export_Utility.h"
#include"Export_System.h"

CStaticCamera::CStaticCamera(LPDIRECT3DDEVICE9 pGraphicDev)
    :CCamera(pGraphicDev)
{
}

CStaticCamera::~CStaticCamera()
{
}

HRESULT CStaticCamera::ReadyGameObject()
{
	m_pPlrTransCom = dynamic_pointer_cast<CTransform>(Engine::Get_Component(L"Layer_4_GameObj", L"Obj_Player", L"Com_Transform", COMPONENTID::ID_DYNAMIC));
	HRESULT result = CCamera::ReadyGameObject();

	m_matView = *(m_pPlrTransCom->GetWorld());
	D3DXMatrixInverse(&m_matView,0, &m_matView);


	if (ECameraMode::VILLAGE == m_eCurrentState)
		m_bInVillage = true;

	return result;
}

_int CStaticCamera::UpdateGameObject(const _float& fTimeDelta)
{

	if (!m_pPlrTransCom)
		m_pPlrTransCom = dynamic_pointer_cast<CTransform>(Engine::Get_Component(L"Layer_4_GameObj", L"Obj_Player", L"Com_Transform", COMPONENTID::ID_DYNAMIC));

	if (m_bInVillage)
		KeyInput();

	_vec3 pos = *m_pPlrTransCom->GetPos();
	pos;

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);
	m_deltaTime = fTimeDelta;
	return 0;
}

void CStaticCamera::LateUpdateGameObject(){
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);


	_vec3 pos = *m_pPlrTransCom->GetPos();
	pos;

	m_vUp = _vec3(0.f, 1.f, 0.f);
	_vec3 vLook, vPos;
	_vec3 test1, test2;

	_matrix matYpos, matCamWorld;

	switch (m_eCurrentState)
	{
	case ECameraMode::IDLE:

		break;
	case ECameraMode::FPS:

		m_matView = *(m_pPlrTransCom->GetWorld());
		m_matView.m[3][1] += 3.f;
		D3DXMatrixInverse(&m_matView, 0, &m_matView);

		break;
	case ECameraMode::VILLAGE:
		
		if (m_bCursorLock) {
			CameraMove();
			Mouse_Fix();
		}

		matCamWorld = *(m_pPlrTransCom->GetWorld());
		matCamWorld.m[3][1] += 3.f;
		D3DXMatrixInverse(&m_matView, 0, &m_matView);
		memcpy(&m_matView.m[3][0], &matCamWorld.m[3][0], sizeof(_vec3));
		D3DXMatrixInverse(&m_matView, 0, &m_matView);

		break;
	case ECameraMode::ROTATION:
		ChangeCamEyePos();
		break;

	case ECameraMode::ORBIT:
		ChangeCamAtPos();
		break;

	case ECameraMode::ZOOMIN:
		MovingDirect();
		break;

	case ECameraMode::ZOOMOUT:
		MovingDirect();
		break;

	case ECameraMode::LOOKBACK:
		ChangeCamEyePos();
		break;

	case ECameraMode::INTERACTION:
		InteractionTervarn();
		break;
	case ECameraMode::BATTLE:
		break;
	case ECameraMode::ENUM_END:

		break;

	default:
		break;
	}


	//동시에 여러 상태를 갖기 위한 State------------------------------------------------------------------
	if (m_bCamRotationBattle) {
		ChangeCamEyePos();
	}
	
	if (m_bCamMovingHorizontal)
		MovingRightVec(1);

	if (m_bCamMoving) {
		MovingDirect();
	}
	//----------------------------------------------------------------------------------------------------

	if (!m_qEffectQueue.empty())
		CameraEffectProcess();


	pos = *m_pPlrTransCom->GetPos();
	pos;

	int a = 0;

}

void CStaticCamera::ChangeCameraWithDegree(ECameraMode _eCamType, _float _fDegree, _float _fTime)
{
	CamReset();

	if (_eCamType == ECameraMode::BATTLE) {
		m_bCamRotationBattle = true;
	}

	if (_eCamType == ECameraMode::ROTATION || _eCamType == ECameraMode::LOOKBACK || _eCamType == ECameraMode::BATTLE) {
		m_eCurrentState = _eCamType;
		m_fAngle = _fDegree / _fTime;
		m_fTotalTime = _fTime;
		m_fTotalAngle = _fDegree;

		D3DXQUATERNION q;
		_matrix matYpos, matView;
		_vec3 vUpvec = { 0.f, 1.f , 0.f };
		m_fLerp = 0.1f;
		D3DXQuaternionRotationAxis(&q, &vUpvec, D3DXToRadian(_fDegree));

		m_qPrev = q;
		//쿼터니언을 행렬로 변환
		D3DXMatrixRotationQuaternion(&matYpos, &q);

		m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
		D3DXMatrixInverse(&matView, 0, &matView);

		m_matOrigin = matYpos * matView;
		D3DXMatrixInverse(&m_matOrigin, 0, &m_matOrigin);

	}
}

void CStaticCamera::ChangeCameraWithPoint(ECameraMode _eCamType, _vec3 _vDst, _vec3 _vCenter, _float _fTime)
{

	if (_eCamType == ECameraMode::ROTATION)
		return;

	CamReset();

	m_eCurrentState = _eCamType;
	m_vDstVec = _vDst;
	m_fTotalTime = _fTime;


	_vec3 vCurrentPos;
	_matrix matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0 , &matView);

	memcpy(&vCurrentPos, &matView.m[3][0], sizeof(_vec3));
	
	_vCenter.y = vCurrentPos.y;
	_vDst.y = vCurrentPos.y;

	_float fXLen = fabsf(vCurrentPos.x - _vDst.x);
	_float fZLen = fabsf(vCurrentPos.z - _vDst.z);

	m_vSpeed.x = fXLen / _fTime;
	m_vSpeed.z = fZLen / _fTime;

	_vec3 vDirVec1 = _vCenter - _vDst;
	_vec3 vDirVec2 = _vCenter - vCurrentPos;

	D3DXVec3Normalize(&vDirVec1, &vDirVec1);
	D3DXVec3Normalize(&vDirVec2, &vDirVec2);

	_float fAngle = D3DXVec3Dot(&vDirVec1, &vDirVec2);
	m_fAngle = acosf(fAngle);

	m_fTotalAngle = m_fAngle;
	_float fDegree = D3DXToDegree(m_fAngle);
	m_vOrigin = vCurrentPos;

	m_fAngle = m_fAngle / _fTime;

}

void CStaticCamera::MovingLineCamera(ECameraMode _eCamType, _vec3 _vDst, _float _fTime)
{
	
	//m_eCurrentState = _eCamType;
	m_bCamMoving = true;

	CamReset();

	_vec3 vCurrentPos;

	_matrix matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);
	memcpy(&vCurrentPos, &matView.m[3][0], sizeof(_vec3));

	m_fTotalTime2 = _fTime;
	_vec3 vDir = _vDst - vCurrentPos;
	m_vSpeed.x =  D3DXVec3Length(&vDir);
	m_vSpeed.x /= _fTime;

	D3DXVec3Normalize(&vDir, &vDir);

	m_vDir = vDir;
	m_vDir.y = 0.f;
	m_vDstVec = _vDst;


}

void CStaticCamera::SetCameraPos(_vec3 _vPos)
{
	D3DXMatrixInverse(&m_matView, 0, &m_matView);

	memcpy(&m_matView.m[3][0], &_vPos, sizeof(_vec3));

	D3DXMatrixInverse(&m_matView, 0, &m_matView);

}

void CStaticCamera::SetAngleReset()
{

	m_fYAngle = 0.f;


	D3DXMatrixInverse(&m_matView, 0, &m_matView);


	_vec3 vRight = { 1.f , 0.f , 0.f };
	_vec3 vUp = { 0.f , 1.f , 0.f };
	_vec3 vLook = { 0.f, 0.f, 1.f };

	memcpy(&m_matView.m[0][0], &vRight, sizeof(_vec3));
	memcpy(&m_matView.m[1][0], &vUp, sizeof(_vec3));
	memcpy(&m_matView.m[2][0], &vLook, sizeof(_vec3));

	D3DXMatrixInverse(&m_matView, 0, &m_matView);


}

void CStaticCamera::ChangeCamEyePos()
{
	D3DXQUATERNION q;
	_matrix matYpos, matView;
	_vec3 vUpvec = { 0.f, 1.f , 0.f };

	_float fCalAngle = D3DXToRadian(m_fAngle) * m_deltaTime;

	D3DXQuaternionRotationAxis(&q, &vUpvec, fCalAngle);
	//쿼터니언을 행렬로 변환
	D3DXMatrixRotationQuaternion(&matYpos, &q);

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);

	m_matView = matYpos * matView;
	D3DXMatrixInverse(&m_matView, 0, &m_matView);

	m_fCurrentAngle += D3DXToRadian(m_fAngle) * m_deltaTime;
	m_fActTime += m_deltaTime;

	//Battle
	if (m_fActTime >= m_fTotalTime && m_bCamRotationBattle) {
		m_eCurrentState = ECameraMode::IDLE;
		m_bCamRotationBattle = false;
		return;
	}

	//Rotation
	if (m_fActTime >= m_fTotalTime && m_eCurrentState == ECameraMode::ROTATION) {

		if (m_bInteraction) {
			if (m_bIsLookBack) {
				m_eCurrentState = ECameraMode::VILLAGE;
				m_bInVillage = true;
				m_fYAngle = 0.f;
				m_bIsLookBack = false;
				m_bInteraction = false;

			}
			else {
				m_eCurrentState = ECameraMode::IDLE;
				m_bIsLookBack = true;
			}


		}
		else {

			const _vec3* vAngle = m_pPlrTransCom->GetAngles();
			m_pPlrTransCom->SetAngle({ vAngle->x, vAngle->y + D3DXToRadian(m_fTotalAngle) , vAngle->z });
			m_eCurrentState = ECameraMode::FPS;
		}
	
		return;

	}

	//Look Back
	if (m_fActTime >= m_fTotalTime && m_eCurrentState == ECameraMode::LOOKBACK) {

		if (m_bIsLookBack == false) {
			m_bIsLookBack = true;
			m_matView = m_matOrigin;
			m_eCurrentState = ECameraMode::IDLE;

		}
		else {
			m_bIsLookBack = false;
			m_eCurrentState = ECameraMode::FPS;
		}

		return;
	}

}

void CStaticCamera::ChangeCamAtPos()
{
	D3DXQUATERNION q,qRot; 
	_matrix matYpos, matRot, matTrans, matRot2, matView , matParent , matWorld;
	_vec3 vUpvec = { 0.f, 1.f , 0.f };
	_vec3 vPos;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);

	//D3DXQuaternionSlerp

	D3DXMatrixIdentity(&matWorld);
	//특정 vector 축으로 회전하는 쿼터니언 생성
	D3DXQuaternionRotationAxis(&q, &vUpvec, -1.f * m_fAngle * m_deltaTime);

	memcpy(&vPos, &matView.m[3][0], sizeof(_vec3));
	

	//쿼터니언을 행렬로 변환
	D3DXMatrixRotationQuaternion(&matYpos, &q);
	D3DXMatrixTranslation(&matTrans, m_vSpeed.x * m_deltaTime, 0.f, m_vSpeed.z * 0.2f* m_deltaTime);
	D3DXMatrixTranslation(&matParent, m_vDstVec.x * m_deltaTime, 0.f , m_vDstVec.z * m_deltaTime);

	//스 * 자 * 이 * 공 * 부
	matView *= matTrans * matYpos;
	m_matView = matView;
	D3DXMatrixInverse(&m_matView, 0, &m_matView);

	m_fActTime += m_deltaTime;
	m_fCurrentAngle += m_fAngle * m_deltaTime;

	if (m_fTotalTime <= m_fActTime) {
		m_eCurrentState = ECameraMode::IDLE;
	}

}

void CStaticCamera::MovingDirect()
{
	_vec3 vCurrentPos, vCurPos2;
	_matrix matView;

	m_fActTime3 += m_deltaTime;

	if (m_fTotalTime2 <= m_fActTime3) {
		m_bCamMoving = false;
		m_fActTime3 = 0.f;
		m_eCurrentState = ECameraMode::IDLE;
	}

	matView = m_matView;

//	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);
	memcpy(&vCurrentPos, &matView.m[3][0], sizeof(_vec3));
	vCurPos2 = vCurrentPos;

	m_fLerp += (_float)0.3 * m_deltaTime;


	if (m_fLerp > 0.8f) 
		m_fLerp = 0.8f;

	D3DXVec3Lerp(&vCurrentPos, &vCurrentPos, &m_vDstVec, m_fLerp);

	memcpy(&matView.m[3][0] , &vCurrentPos , sizeof(_vec3));
	D3DXMatrixInverse(&matView, 0, &matView);

	m_matView = matView;


}

void CStaticCamera::CamReset()
{
	m_fLerp = 0.f;
	m_fAngle = 0.f;
	m_fTotalAngle = 0.f;
	m_fCurrentAngle = 0.f;
	m_fTotalTime = 0.f;
	m_fActTime = 0.f;
	m_fDir = 1.f;
}

void CStaticCamera::MovingRightVec(_int _iDir)
{
	_matrix matView;
	_vec3 vRight, vPos;
	
	matView = m_matView;

	D3DXMatrixInverse(&matView, 0, &matView);

	memcpy(&vRight, &matView.m[0][0], sizeof(_vec3));
	memcpy(&vPos, &matView.m[3][0], sizeof(_vec3));

	vPos += vRight * m_deltaTime *2.f * m_fRightVecDir;

	memcpy(&matView.m[3][0], &vPos, sizeof(_vec3));
	D3DXMatrixInverse(&matView, 0, &matView);

	m_matView = matView;

	if(m_bCamMoving)
		m_vDstVec.x += vRight.x * m_deltaTime * 2.f;

}

void CStaticCamera::SlopeCamera(_int _iDir, _int _iSlope)
{
	


}

void CStaticCamera::CalcAngle(_vec3 _Dst)
{




}

void CStaticCamera::LineCameraNoLerp()
{

	_vec3 vCurrentPos, vCurPos2;
	_matrix matView;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);
	memcpy(&vCurrentPos, &matView.m[3][0], sizeof(_vec3));

	vCurrentPos += m_vDir * m_vSpeed.x * 2.f * m_deltaTime;
	
	memcpy(&matView.m[3][0], &vCurrentPos, sizeof(_vec3));
	D3DXMatrixInverse(&matView, 0, &matView);

	m_matView = matView;

	m_fActTime2 += m_deltaTime;

	if (m_fTotalTime <= m_fActTime2) {
		m_bCamMoving = false;
		m_fActTime2 = 0.f;
	}



}


void CStaticCamera::AddCameraEffect(EEffectState _eEffect, _float _fTime, _float _fAmplitude , EShakingType _eShaking)
{

	unique_ptr<tagEffectInfo> pEffectInfo = make_unique<tagEffectInfo>();
	
	pEffectInfo->eEffectType = _eEffect;
	pEffectInfo->fTime = _fTime;
	pEffectInfo->fAmplitude = _fAmplitude;
	pEffectInfo->MoveDistance = 0.f;
	pEffectInfo->fActTime = 0.f;
	pEffectInfo->fDir = -1.f;
	pEffectInfo->eShakingType = _eShaking;

	m_qEffectQueue.push(std::move(pEffectInfo));

}

void CStaticCamera::CameraEffectProcess()
{
	switch (m_qEffectQueue.front()->eEffectType)
	{
	case EEffectState::SHAKING:
		ShakingCamera();
		break;
	case EEffectState::FADEIN:
		break;
	case EEffectState::FADEOUT:
		break;
	case EEffectState::ENUM_END:
		break;
	default:
		break;
	}
	

}

//방향 Vertical 고정
void CStaticCamera::ShakingCamera()
{
	_vec3 vCurrentPos;                                                                                                                                                                                            
	_matrix matView;

	_float fDir = m_qEffectQueue.front()->fDir;

	//이동거리 구하기
	_float fDistance = fDir *10.f * m_deltaTime;

	//한 방향으로 진폭만큼 이동했을시 방향 전환

	printf("%f\n", m_qEffectQueue.front()->MoveDistance + fDistance);
	printf("%f\n", fDir);

	if (fabsf(m_qEffectQueue.front()->MoveDistance + fDistance) >= m_qEffectQueue.front()->fAmplitude) {
		fDistance = m_qEffectQueue.front()->fAmplitude * fDir - m_qEffectQueue.front()->MoveDistance;
		m_qEffectQueue.front()->MoveDistance = m_qEffectQueue.front()->fAmplitude * fDir;
		printf("위치 출력!! %f\n", m_qEffectQueue.front()->fAmplitude * fDir);

		m_qEffectQueue.front()->fDir *= -1.f;
	}
	else {
		m_qEffectQueue.front()->MoveDistance += fDistance;
	}

	D3DXMatrixInverse(&m_matView, 0, &m_matView);

	_vec3 vRight, vUp;
	//현재 카메라 position에 더해주기
	memcpy(&vCurrentPos, &m_matView.m[3][0], sizeof(_vec3));

	memcpy(&vRight, &m_matView.m[0][0], sizeof(_vec3));
	memcpy(&vUp, &m_matView.m[1][0], sizeof(_vec3));


	if (m_eCurrentState == ECameraMode::FPS || m_eCurrentState == ECameraMode::VILLAGE) {

		if(EShakingType::CROSS == m_qEffectQueue.front()->eShakingType){
			vCurrentPos += m_qEffectQueue.front()->MoveDistance * vRight;
			vCurrentPos += m_qEffectQueue.front()->MoveDistance * vUp;
		}
		else if (EShakingType::HORIZON == m_qEffectQueue.front()->eShakingType) {
			vCurrentPos += m_qEffectQueue.front()->MoveDistance * vRight;
		}
		else if (EShakingType::HORIZON == m_qEffectQueue.front()->eShakingType) {
			vCurrentPos += m_qEffectQueue.front()->MoveDistance * vUp;

		}
		else if (EShakingType::RANDOM == m_qEffectQueue.front()->eShakingType) {
		
		
		}

	}
	else {

		if (EShakingType::CROSS == m_qEffectQueue.front()->eShakingType) {
			vCurrentPos += m_qEffectQueue.front()->MoveDistance * vRight;
			vCurrentPos += m_qEffectQueue.front()->MoveDistance * vUp;
		}
		else if (EShakingType::HORIZON == m_qEffectQueue.front()->eShakingType) {
			vCurrentPos += m_qEffectQueue.front()->MoveDistance * vRight;
		}
		else if (EShakingType::HORIZON == m_qEffectQueue.front()->eShakingType) {
			vCurrentPos += m_qEffectQueue.front()->MoveDistance * vUp;

		}
		else if (EShakingType::RANDOM == m_qEffectQueue.front()->eShakingType) {


		}
	}

	memcpy(&m_matView.m[3][0], &vCurrentPos, sizeof(_vec3));


	D3DXMatrixInverse(&m_matView, 0, &m_matView);

	//지속시간 측정 후, 지정한 시간에 도달하면 pop을 해 없애준다
	m_qEffectQueue.front()->fActTime += m_deltaTime;
	
	if (m_qEffectQueue.front()->fActTime >= m_qEffectQueue.front()->fTime)
		m_qEffectQueue.pop();
	

}




//------------------------------------Village-----------------------------------------------


void CStaticCamera::CameraMove()
{

	_matrix		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, 0, &m_matView);

	_long		dwMouseMove(0);

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
	{
		D3DXMatrixInverse(&m_matView, 0, &m_matView);
		_vec3		vRight;
		memcpy(&vRight, &m_matView.m[0][0], sizeof(_vec3));

		_float fAngle = dwMouseMove / 10.f;

		if (fabsf(m_fYAngle + fAngle) > 60.f) {
			if (fAngle < 0) {
				fAngle = 60.f - m_fYAngle;
				m_fYAngle = -60.f;
				fAngle = 0.f;
			}
			else {
				fAngle = 60.f - m_fYAngle;
				m_fYAngle = 60.f;
				fAngle = 0.f;
			}
		}
		else {
			m_fYAngle += fAngle;
		}

		_matrix		matRot;
		D3DXQUATERNION q;

		D3DXQuaternionRotationAxis(&q, &vRight, D3DXToRadian(fAngle));
		D3DXMatrixRotationQuaternion(&matRot, &q);

		//	D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(fAngle));
		m_matView = m_matView * matRot;
		D3DXMatrixInverse(&m_matView, 0, &m_matView);
	}

	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
	{

		D3DXMatrixInverse(&m_matView, 0, &m_matView);

		_vec3		vUp{ 0.f, 1.f, 0.f };
		_matrix		matRot;

		D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove / 10.f));

		const _vec3* vAngle = m_pPlrTransCom->GetAngles();
		m_pPlrTransCom->SetAngle({ 0.f, vAngle->y + D3DXToRadian(dwMouseMove / 10.f) , 0.f });
		m_matView = m_matView * matRot;
		D3DXMatrixInverse(&m_matView, 0, &m_matView);
	}

}

void CStaticCamera::Mouse_Fix()
{
	POINT	ptMouse{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CStaticCamera::InteractionTervarn()
{
	m_bInteraction = true;
	_vec3 vCamLook;
	_vec3 vLook = { 0.f , 0.f ,1.f };

	D3DXMatrixInverse(&m_matView, 0, &m_matView);

	memcpy(&vCamLook, &m_matView.m[2][0], sizeof(_vec3));
	D3DXVec3Normalize(&vCamLook, &vCamLook);

	_float fAngle = D3DXVec3Dot(&vLook, &vCamLook);
	fAngle = acosf(fAngle) * 2;

	_matrix		matRot;

	if (m_fActTime + m_deltaTime > 0.5f) {

		_vec3 vLook = { 0.f, 0.f, 1.f };
		_vec3 vRight = { 1.f , 0.f, 0.f };

		memcpy(&m_matView.m[2][0], &vLook, sizeof(_vec3));
		memcpy(&m_matView.m[0][0], &vRight, sizeof(_vec3));
		D3DXMatrixInverse(&m_matView, 0, &m_matView);
	}
	else {

		D3DXMatrixRotationX(&matRot, fAngle * m_deltaTime);
		m_matView = matRot * m_matView;
		D3DXMatrixInverse(&m_matView, 0, &m_matView);
	}

	m_fActTime += m_deltaTime;

	if (m_fActTime > 0.5f) {
		ChangeCameraWithDegree(ECameraMode::ROTATION, 180.f);
	}
	
}

void CStaticCamera::KeyInput()
{
	if (GetAsyncKeyState(VK_TAB) & 0x8000) {
		m_bCursorLock = !m_bCursorLock;

	}
}
