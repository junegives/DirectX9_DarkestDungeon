#include "Collider.h"
#include "CollisionMgr.h"

CCollider::CCollider()
{
}

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev)
{
}

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev, ECollideID _eCollideID)
    : CComponent(pGraphicDev), m_eCollideID(_eCollideID)
{
}

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev, ECollideID _eCollideID, _vec3* _vPos, const _vec3& _vScale)
    : CComponent(pGraphicDev), m_eCollideID(_eCollideID), m_vPos(_vPos), m_vScale(_vScale)
{
}

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev, ECollideID _eCollideID, _vec3* _vPos, const _float& _fRadius)
    : CComponent(pGraphicDev), m_eCollideID(_eCollideID), m_vPos(_vPos), m_fRadius(_fRadius)
{
}

CCollider::~CCollider()
{
}

void CCollider::GetMinMax(_vec3& vMin, _vec3& vMax)
{
    vMin = { m_vPos->x - m_vScale.x / 2.f  ,m_vPos->y - m_vScale.y / 2.f  ,m_vPos->z - m_vScale.z / 2.f };
    vMax = { m_vPos->x + m_vScale.x / 2.f  ,m_vPos->y + m_vScale.y / 2.f ,m_vPos->z + m_vScale.z / 2.f };
}

void CCollider::GetSphere(_vec3& _vCenter, _float& _fRadius)
{
    _vCenter = *m_vPos;
    _fRadius = m_vScale.x / 2.f;
}

void CCollider::RenderCollider(void)
{
    if (CCollisionMgr::GetInstance()->GetVisible())
    {
        // Z -
        m_vPoint[0][0] = { m_vPos->x - m_vScale.x / 2.f, m_vPos->y + m_vScale.y / 2.f, m_vPos->z - m_vScale.z / 2.f };
        m_vPoint[0][1] = { m_vPos->x + m_vScale.x / 2.f, m_vPos->y + m_vScale.y / 2.f, m_vPos->z - m_vScale.z / 2.f };
        m_vPoint[0][2] = { m_vPos->x + m_vScale.x / 2.f, m_vPos->y - m_vScale.y / 2.f, m_vPos->z - m_vScale.z / 2.f };
        m_vPoint[0][3] = { m_vPos->x - m_vScale.x / 2.f, m_vPos->y - m_vScale.y / 2.f, m_vPos->z - m_vScale.z / 2.f };
        m_vPoint[0][4] = { m_vPos->x - m_vScale.x / 2.f, m_vPos->y + m_vScale.y / 2.f, m_vPos->z - m_vScale.z / 2.f };

        // X +
        m_vPoint[0][5] = { m_vPos->x + m_vScale.x / 2.f, m_vPos->y - m_vScale.y / 2.f, m_vPos->z - m_vScale.z / 2.f };
        m_vPoint[0][6] = { m_vPos->x + m_vScale.x / 2.f, m_vPos->y - m_vScale.y / 2.f, m_vPos->z + m_vScale.z / 2.f };
        m_vPoint[0][7] = { m_vPos->x + m_vScale.x / 2.f, m_vPos->y + m_vScale.y / 2.f, m_vPos->z + m_vScale.z / 2.f };
        m_vPoint[0][8] = { m_vPos->x + m_vScale.x / 2.f, m_vPos->y + m_vScale.y / 2.f, m_vPos->z - m_vScale.z / 2.f };

        // Z +                                                                                                
        m_vPoint[1][0] = { m_vPos->x + m_vScale.x / 2.f, m_vPos->y + m_vScale.y / 2.f, m_vPos->z + m_vScale.z / 2.f };
        m_vPoint[1][1] = { m_vPos->x - m_vScale.x / 2.f, m_vPos->y + m_vScale.y / 2.f, m_vPos->z + m_vScale.z / 2.f };
        m_vPoint[1][2] = { m_vPos->x - m_vScale.x / 2.f, m_vPos->y - m_vScale.y / 2.f, m_vPos->z + m_vScale.z / 2.f };
        m_vPoint[1][3] = { m_vPos->x + m_vScale.x / 2.f, m_vPos->y - m_vScale.y / 2.f, m_vPos->z + m_vScale.z / 2.f };
        m_vPoint[1][4] = { m_vPos->x + m_vScale.x / 2.f, m_vPos->y + m_vScale.y / 2.f, m_vPos->z + m_vScale.z / 2.f };

        // X -                                                                                               
        m_vPoint[1][5] = { m_vPos->x - m_vScale.x / 2.f, m_vPos->y - m_vScale.y / 2.f, m_vPos->z + m_vScale.z / 2.f };
        m_vPoint[1][6] = { m_vPos->x - m_vScale.x / 2.f, m_vPos->y - m_vScale.y / 2.f, m_vPos->z - m_vScale.z / 2.f };
        m_vPoint[1][7] = { m_vPos->x - m_vScale.x / 2.f, m_vPos->y + m_vScale.y / 2.f, m_vPos->z - m_vScale.z / 2.f };
        m_vPoint[1][8] = { m_vPos->x - m_vScale.x / 2.f, m_vPos->y + m_vScale.y / 2.f, m_vPos->z + m_vScale.z / 2.f };

        D3DXMATRIX matAll;
        D3DXMATRIX matWorld;
        D3DXMATRIX matView;
        D3DXMATRIX matProj;

        D3DXMatrixIdentity(&matWorld);

        //m_pGraphicDev->GetTransform(D3DTS_WORLD, &matWorld);
        m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
        m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

        _float epsilon = 0.00001f; // ¾Û½Ç·Ð °ª

        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 9; ++j)
            {
                D3DXVec3TransformCoord(&m_vPoint[i][j], &m_vPoint[i][j], &matView);
                _float adjustedZ = (m_vPoint[i][j].z < epsilon) ? epsilon : m_vPoint[i][j].z;
                m_vPoint[i][j].z = adjustedZ;
            }
        }

        D3DXCreateLine(m_pGraphicDev, &m_pLine);

        m_pLine->SetWidth(2);
        m_pLine->SetAntialias(true);

        m_pLine->Begin();

        m_pLine->DrawTransform(m_vPoint[0], 9, &matProj, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
        m_pLine->DrawTransform(m_vPoint[1], 9, &matProj, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

        m_pLine->End();

        m_pLine->Release();
    }

}