#include "ParticleSystem.h"
#include "Renderer.h"

#include "GameMgr.h"
#include "Transform.h"
#include "Texture.h"
#include "Animator.h"

CParticleSystem::CParticleSystem()
{
    m_pGraphicDev   = 0;
    m_pVB = 0;
    m_pTex = 0;
}

CParticleSystem::~CParticleSystem()
{
    Safe_Release(m_pVB);
    Safe_Release(m_pTex);
}

void CParticleSystem::Init(LPDIRECT3DDEVICE9 _pGraphicDev, tstring strTexFileName)
{
    m_pGraphicDev = _pGraphicDev;

    HRESULT hr = 0;

    hr = m_pGraphicDev->CreateVertexBuffer(
        m_vbSize * sizeof(PARTICLE),
        D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
        FVF_PARTICLE,
        D3DPOOL_DEFAULT,
        &m_pVB,
        0);

    if (FAILED(hr))
    {
        ::MessageBox(0, L"CreateVertexBuffer() - FAILED", L"ParticleSystem", 0);
        return;
    }

    TCHAR		szFileName[128] = L"";
    wsprintf(szFileName, strTexFileName.c_str());

    hr = D3DXCreateTextureFromFile(
        m_pGraphicDev,
        szFileName,
        &m_pTex);

    if (FAILED(hr))
    {
        ::MessageBox(0, L"D3DXCreateTextureFromFile() - FAILED", L"ParticleSystem", 0);
        return;
    }

    m_bActive = false;
}

_int CParticleSystem::UpdateGameObject(const _float& fTimeDelta)
{
    return _int();
}

void CParticleSystem::LateUpdateGameObject()
{
    if (IsDead())
    {
        Reset();
        SetActive(false);
        RemoveDeadParticles();
        RemoveAllParticles();
        return;
    }
        
    CRenderer::GetInstance()->AddRenderGroup(RENDER_ALPHA, shared_from_this());
}

void CParticleSystem::PreRender()
// PreRender
{
    //m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, false);
    m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
    m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, true);
    m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE, FtoDw(m_fSize));
    m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));


    /*m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MIN, (DWORD)0.2f);
    m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MAX, (DWORD)5.f);*/

    // �Ÿ��� ���� ��ƼŬ�� ũ�⸦ �����Ѵ�.
    m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
    m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
    m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));

    // �ؽ�ó�� ���ĸ� �̿��Ѵ�.
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void CParticleSystem::RenderGameObject()
{
    D3DXMATRIX matTriangleWorld;
    matTriangleWorld = *m_pTransformCom->GetWorld();

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());

        if (!m_listParticles.empty())
        {
            PreRender();

            m_pGraphicDev->SetTexture(0, m_pTex);
            m_pGraphicDev->SetFVF(FVF_PARTICLE);
            m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(PARTICLE));

            // ���ؽ� ���۸� ��� ��� ó������ ����
            if (m_vbOffset >= m_vbSize)
                m_vbOffset = 0;

            PARTICLE* v = 0;

            m_pVB->Lock(
                m_vbOffset * sizeof(PARTICLE),
                m_vbBatchSize * sizeof(PARTICLE),
                (void**)&v,
                m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

            DWORD   numParticlesInBatch = 0;

            // ��� ��ƼŬ�� �������� ������
            for (auto& iter : m_listParticles)
            {
                if (iter.bIsAlive)
                {
                    // ������ ��ƼŬ�� ���� ���ؽ� ���� ���׸�Ʈ�� �����Ѵ�.
                    v->vPosition = iter.vPosition;
                    v->tColor = (D3DXCOLOR)iter.tColor;
                    v++;

                    numParticlesInBatch++;

                    // ���� �ܰ谡 ��� ä���� �ִ���?
                    if (numParticlesInBatch == m_vbBatchSize)
                    {
                        // ���ؽ� ���۷� ����� ������ �ܰ��� ��ƼŬ���� �׸���.
                        m_pVB->Unlock();

                        m_pGraphicDev->DrawPrimitive(
                            D3DPT_POINTLIST,
                            m_vbOffset,
                            m_vbBatchSize);

                        // �ܰ谡 �׷����� ���� ���� �ܰ踦 ��ƼŬ�� ä���.

                        // ���� �ܰ��� ó�� ���������� �̵�
                        m_vbOffset += m_vbBatchSize;

                        // ���ؽ� ������ ��踦 �Ѵ� �޸𸮷� �������� �������� �ʴ´�.
                        // ��踦 ���� ��� ó������
                        if (m_vbOffset >= m_vbSize)
                            m_vbOffset = 0;

                        m_pVB->Lock(
                            m_vbOffset * sizeof(PARTICLE),
                            m_vbBatchSize * sizeof(PARTICLE),
                            (void**)&v,
                            m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

                        numParticlesInBatch = 0; // ���� �ܰ踦 ���� ����
                    }
                }
            }

            m_pVB->Unlock();

            // �� ���׸�Ʈ �� ��ä��� ���� �ֵ� �׸���
            if (numParticlesInBatch)
            {
                m_pGraphicDev->DrawPrimitive(
                    D3DPT_POINTLIST,
                    m_vbOffset,
                    numParticlesInBatch);
            }

            m_vbOffset += m_vbBatchSize;

            PostRender();
        }
}

void CParticleSystem::PostRender()
// PostRender
{
    //m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, true);
    m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
    m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, false);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

void CParticleSystem::AddComponent()
{
    shared_ptr<CComponent> pComponent;

    // Transform
    pComponent = m_pTransformCom = make_shared<CTransform>();
    m_pTransformCom->ReadyTransform();
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform",pComponent });

    // Animator
    pComponent = m_pAnimatorCom = make_shared<CAnimator>(m_pGraphicDev);
    m_pAnimatorCom->SetAnimKey(L"", 0.05f);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

    // Texture
    pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
    m_pTextureCom->SetTextureKey(L"", TEX_NORMAL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
}

// �ý��� ���� ��� ��ƼŬ �Ӽ��� �����ϴ� �Լ�
void CParticleSystem::Reset()
{
    std::list<PARTICLE_ATTRIBUTE>::iterator i;
    for (i = m_listParticles.begin(); i != m_listParticles.end(); i++)
    {
        ResetParticle(&(*i));
    }
}

void CParticleSystem::SettingOrigin(_vec3* _vOrigin, _int _iNumParticles)
{
    m_vOrigin = *_vOrigin;
    m_pTransformCom->SetPosition(m_vOrigin.x, m_vOrigin.y, m_vOrigin.z);
    m_pTransformCom->SetScale(1.f, 1.f, 1.f);

    for (int i = 0; i < _iNumParticles; ++i)
        AddParticle();
}

void CParticleSystem::SettingBounding(BOUNDING_BOX* _tBoundingBox, _vec3* _vPos, _int _iNumParticles)
{
    m_tBoundingBox = *_tBoundingBox;
    
    m_pTransformCom->SetPosition(_vPos->x, _vPos->y, _vPos->z);
    m_pTransformCom->SetScale(10.f, 10.f, 10.f);

    for (int i = 0; i < _iNumParticles; ++i)
        AddParticle();
}

void CParticleSystem::SettingBoundingSphere(BOUNDING_SPHERE* _tBoundingSphere, _vec3 _vPos, _int _iNumParticles)
{
    m_tBoundingShpere = *_tBoundingSphere;

    m_vOrigin = _vPos;
    m_pTransformCom->SetPosition(m_vOrigin.x, m_vOrigin.y, m_vOrigin.z);
    m_pTransformCom->SetScale(1.f, 1.f, 1.f);

    for (int i = 0; i < _iNumParticles; ++i)
        AddParticle();

    m_bOrthogonal = true;
}

// �� ��ƼŬ�� �Ӽ��� �����ϴ� �Լ�
void CParticleSystem::ResetParticle(PARTICLE_ATTRIBUTE* attribute)
{
}

// �ý��ۿ� ��ƼŬ�� �߰��ϴ� �Լ�
void CParticleSystem::AddParticle()
{
    PARTICLE_ATTRIBUTE _pAttribute;

    ResetParticle(&_pAttribute);

    m_listParticles.push_back(_pAttribute);
}

_bool CParticleSystem::IsDead()
{
    for (auto& iter : m_listParticles)
    {
        if (iter.bIsAlive)
            return false;
    }

    return true;
}

_bool CParticleSystem::IsEmpty()
{
    return m_listParticles.empty();
}

void CParticleSystem::RemoveDeadParticles()
{
    auto iter = m_listParticles.begin();

    while (iter != m_listParticles.end())
    {
        if (iter->bIsAlive == false)
        {
            iter = m_listParticles.erase(iter);
            continue;
        }

        iter++;
    }
}

void CParticleSystem::RemoveAllParticles()
{
    auto iter = m_listParticles.begin();

    while (iter != m_listParticles.end())
    {
        iter = m_listParticles.erase(iter);
    }
}

_float CParticleSystem::GetRandomFloat(_float _fLowBound, _float _fHighBound)
{
    if (_fLowBound >= _fHighBound)
        return _fLowBound;

    // 0~1 ������ float ����
    _float f = (rand() % 10000) * 0.0001f;

    return (f * (_fHighBound - _fLowBound)) + _fLowBound;
}

void CParticleSystem::GetRandomVector(_vec3* out, _vec3* min, _vec3* max)
{
    out->x = GetRandomFloat(min->x, max->x);
    out->y = GetRandomFloat(min->y, max->y);
    out->z = GetRandomFloat(min->z, max->z);
}
