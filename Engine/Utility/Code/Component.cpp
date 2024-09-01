#include "Component.h"

CComponent::CComponent()
{
}

CComponent::CComponent(LPDIRECT3DDEVICE9 _pGraphic)
	:m_pGraphicDev(_pGraphic)
	
{
	//_pGraphic->AddRef();
}

CComponent::~CComponent()
{
	//m_pGraphicDev->Release();

}
