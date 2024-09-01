#pragma once

#include"Engine_Define.h"
#include"StaticCamera.h"

class CCameraMgr
{
	DECLARE_SINGLETON(CCameraMgr)

private:
	explicit CCameraMgr();
	virtual ~CCameraMgr();

public:
	void SetMainCamera(shared_ptr<CStaticCamera> _pCam) { m_pMainCam = _pCam; }
	void RemoveMainCamera() { m_pMainCam = nullptr; }
	shared_ptr<CGameObject> GetMainCamera() { return m_pMainCam; }

public: //Camera Moving
	
	//기본 1인칭 화면으로 전환
	void SetFPSMode();
	//빌리지에서 사용하는 카메라(커서 고정+카메라 회전 자유)로 전환
	void SetVillageMode();
	void SetState(ECameraMode _eState);
	//ZoomIn, ZoomOut, Basic Moving
	void MovingStraight(ECameraMode _eType, _vec3 _vPos);
	//공전
	void CameraOrbit(ECameraMode _eType, _vec3 _vDst, _vec3 _vCenter);
	//자전
	void CameraRotation(ECameraMode _eType, _float _fDegree,_float _fTime = 1.5f);
	//현재 카메라의 Right 벡터를 기준으로 오른쪽 , 왼쪽으로 이동(_fDir 1.f = 오른쪽 -1.f = 왼쪽)
	void MovingRightVec(_bool _bValue, _float _fDir);

	//시점 자유 카메라에서 회전한 Y값만큼을 가져온다
	_float GetYAngle();
public: 

	void AddEffectInfo(EEffectState _eEffect, _float _fTime, _float _fAmplitude = 0.1f , EShakingType _eShakeType = EShakingType::CROSS);
	void CamAngleReset();
	void CamSetPos(_vec3 _vPos);

private:

	shared_ptr<CStaticCamera> m_pMainCam = nullptr;


};

