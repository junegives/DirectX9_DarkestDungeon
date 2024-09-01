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
	
	//�⺻ 1��Ī ȭ������ ��ȯ
	void SetFPSMode();
	//���������� ����ϴ� ī�޶�(Ŀ�� ����+ī�޶� ȸ�� ����)�� ��ȯ
	void SetVillageMode();
	void SetState(ECameraMode _eState);
	//ZoomIn, ZoomOut, Basic Moving
	void MovingStraight(ECameraMode _eType, _vec3 _vPos);
	//����
	void CameraOrbit(ECameraMode _eType, _vec3 _vDst, _vec3 _vCenter);
	//����
	void CameraRotation(ECameraMode _eType, _float _fDegree,_float _fTime = 1.5f);
	//���� ī�޶��� Right ���͸� �������� ������ , �������� �̵�(_fDir 1.f = ������ -1.f = ����)
	void MovingRightVec(_bool _bValue, _float _fDir);

	//���� ���� ī�޶󿡼� ȸ���� Y����ŭ�� �����´�
	_float GetYAngle();
public: 

	void AddEffectInfo(EEffectState _eEffect, _float _fTime, _float _fAmplitude = 0.1f , EShakingType _eShakeType = EShakingType::CROSS);
	void CamAngleReset();
	void CamSetPos(_vec3 _vPos);

private:

	shared_ptr<CStaticCamera> m_pMainCam = nullptr;


};

