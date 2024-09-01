#pragma once

#include"Camera.h"
#include<queue>

BEGIN(Engine)
class CTransform;
END

enum class ECameraMode {
	IDLE,
	FPS,
	ROTATION,
	ORBIT,
	ZOOMIN,
	ZOOMOUT,
	LOOKBACK,
	BATTLE,
	VILLAGE,
	INTERACTION,
	ENUM_END
};

enum class EEffectState {
	SHAKING,
	FADEIN,
	FADEOUT,
	ENUM_END
};

enum class EShakingType {
	CROSS,
	VERTICAL,
	HORIZON,
	RANDOM,
	ENUM_END
};

struct tagEffectInfo {
	EEffectState eEffectType;
	_float		 fAmplitude;
	_float		 fDir;
	_float		 MoveDistance;
	_float		 fTime;
	_float		 fActTime;
	EShakingType eShakingType;
};

class CStaticCamera : public CCamera
{
public:
	explicit CStaticCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStaticCamera();

public:
	virtual HRESULT			ReadyGameObject();

	virtual _int		UpdateGameObject(const _float& fTimeDelta);
	virtual void		LateUpdateGameObject();

public:
//�ܺο��� ȣ���ؼ� ����ϴ� �Լ�

	void SetState(ECameraMode _eCamType) { 	
		m_eCurrentState = _eCamType; 
		if (ECameraMode::VILLAGE == _eCamType)
			m_bInVillage = true;
		else
			m_bInVillage = false;	
	}


	ECameraMode GetState() { return m_eCurrentState; }
	//���� ������� ī�޶� ������ : ����
	void ChangeCameraWithDegree(ECameraMode _eCamType, _float _fDegree, _float _fTime = 0.5f);
	//������(_vCenter)�� ��� ȣ��(ȣ�� ũ��: _fXSpeed, _fZSpeed)�� �׸��� ī�޶� ������(_vDst) �ű�� : ����
	void ChangeCameraWithPoint(ECameraMode _eCamType, _vec3 _vDst, _vec3 _vCenter, _float _fTime = 0.5f);
	//���� �������� ��� ������ �׸��� ī�޶� ������ �ű�� : ����, �ܾƿ�, ī�޶� ����
	void MovingLineCamera(ECameraMode _eCamType, _vec3 _vDst, _float _fTime = 1.f);
	//ī�޶� ����Ʈ ����
	void AddCameraEffect(EEffectState _eEffect, _float _fTime, _float _fAmplitude = 0.5f, EShakingType _eShaking = EShakingType::CROSS);
	//������ �������θ� �̵��ϴ� �Լ�
	void MovingRightVec(_int _iDir);
	
	void SlopeCamera(_int _iDir, _int _iSlope);

	void CalcAngle(_vec3 _Dst);
	void LineCameraNoLerp();

	void SetCamMovingStateOnOff(_bool _bValue, _float _fDir) {
		m_bCamMovingHorizontal = _bValue;
		m_fRightVecDir = _fDir;
	}

	void SetCameraPos(_vec3 _vPos);

	_float GetYAngle() { return m_fYAngle; }

public:
	//�̴ϰ���, �ޱ� ���� �� �������� ���������� ��쿡 ���
	void SetAngleReset();

private:
//Moving Function

	//ī�޶� ����, ī�޶��� ���� ��(At)�� �����̰� Position(Eye)�� ��ȭ, �⺻������ ���� �׸��� ȸ��
	void ChangeCamEyePos();
	//ī�޶� ����, ī�޶��� Position(Eye)�� �����̰� ���� ��(At)�� ��ȭ
	void ChangeCamAtPos();
	//Point to Point. ������ �׸��� �̵� 
	void MovingDirect();
	//Camrea Parameter Reset
	void CamReset();

private:
//Camera Effect
	void CameraEffectProcess();
	void ShakingCamera();

private:
//Village Camera Function
	void CameraMove();
	void Mouse_Fix();
	void InteractionTervarn();
	void KeyInput();
private:
	shared_ptr<CTransform> m_pPlrTransCom = nullptr;
	ECameraMode m_eCurrentState = ECameraMode::IDLE;
	
//Moving Range
	//ȸ���� ����
	_float m_fAngle = 0.f;
	_float m_fTotalAngle = 0.f;
	_float m_fCurrentAngle = 0.f;
	//������ ����
	_vec3 m_vDstVec;
	_vec3 m_vOrigin;

	_matrix m_matOrigin;

	D3DXQUATERNION m_qPrev;
	_vec3 m_vLook;

	//Delta Time ������
	_float m_deltaTime = 0.f;
	//ī�޶� ������ ��ü �ð�
	_float m_fTotalTime = 0.f; // For Rotation
	_float m_fTotalTime2 = 0.f; //For MoveLineCam

	//Ư�� ������ ��ɹ��� ���� ī�޶� ������ �ð�
	_float m_fActTime = 0.f;
	_float m_fActTime2 = 0.f;
	_float m_fActTime3 = 0.f;

	_vec3 m_vSpeed;
	_vec3 m_vDir;

	_float m_fLerp = 0.f;
	_float m_fDir = 1.f;
	_float m_fRightVecDir = 1.f;
//State
	_bool m_bIsLookBack = false;
	_bool m_bCursorLock = true;
	_bool m_bInVillage = false;

	_bool m_bInteraction = false;

	//ȸ���� �������� ���� state
	_bool m_bCamMoving = false;
	_bool m_bCamMovingHorizontal = false;
	_bool m_bCamRotationBattle = false;

//Village ------------------------------------------------

	_float m_fYAngle = 0.f;
	
private:
	queue<unique_ptr<tagEffectInfo>> m_qEffectQueue;

};
