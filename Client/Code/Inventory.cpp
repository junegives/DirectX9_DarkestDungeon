#include"pch.h"
#include "Inventory.h"
#include"Item.h"
#include "UIMgr.h"
#include"Export_Utility.h"
#include"Player.h"
#include"Export_System.h"

#include "SoundMgr.h"

CInventory::CInventory(LPDIRECT3DDEVICE9 _pGraphicDev)
    :Engine::CUIObj(_pGraphicDev)
{
}

CInventory::~CInventory()
{
}

HRESULT CInventory::ReadyGameObject()
{
    if (m_bReady) return S_OK;

    m_vSize = { 208.f , 40.f ,0.f };
    m_vAngle = { 0.f, 0.f, 0.f };
    m_vPos = { 0.f, -320.f, 0.5f };

    m_bVisible = true;
    m_bEnable = true;
    m_bActive = true;
    
    m_pTransCom->SetScale(m_vSize.x, m_vSize.y, m_vSize.z);
    m_pTransCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
    m_pTransCom->SetAngle({0.f, 0.f, 0.f});

    CUIObj::ReadyGameObject();

    m_bReady = true;

    return S_OK;
}

_int CInventory::UpdateGameObject(const _float& fTimeDelta)
{
    int iExit(0);
    CGameObject::UpdateGameObject(fTimeDelta);
    AddRenderGroup(RENDER_UI, shared_from_this());

    for (auto& iter : m_itemList) {
        iter->pItem->UpdateGameObject(fTimeDelta);
    }

    return iExit;
}

void CInventory::LateUpdateGameObject()
{

    for (auto& iter : m_itemList) {
        iter->pItem->LateUpdateGameObject();
    }

    __super::LateUpdateGameObject();
}

void CInventory::RenderGameObject()
{

    if (!m_bVisible)
        return;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->GetWorld());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

    m_pTextureCom->SetTexture(0);
    m_pRcTexCom->RenderBuffer();

    _int iCount = 0;

    for (auto& iter : m_itemList) {
        iter->pItem->RenderGameObject();

        if (iter->iCapacity >= 2) {
            TCHAR buf[64];
            _stprintf_s(buf, TEXT("%d"), iter->iCapacity);

            _vec2 vPos = { (WINCX * 0.5f) - 188.f + (iCount * 40.f) , 690.f };

            Engine::Render_Font(L"Font_Default_Small", buf, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
        }
        ++iCount;
    }


    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CInventory::ExchangeGoods(int* _iGold, int* _iHeirRoom)
{

    _bool bFindGold = false;
    _bool bFindHeirRoom = false;

    for (auto iter = m_itemList.begin(); iter != m_itemList.end(); ) {

        if (EHandItem::HEIRLOOM == (*iter)->pItem->GetItemTypeEnum()) {
            
            *_iHeirRoom += (*iter)->iCapacity;
           iter = m_itemList.erase(iter);
           --m_iTotalItemNum;
           bFindHeirRoom = true;
        }
        else if (EHandItem::GOLD == (*iter)->pItem->GetItemTypeEnum()) {

           *_iGold += (*iter)->iCapacity;
            iter = m_itemList.erase(iter);
            --m_iTotalItemNum;
            bFindGold = true;
        }
        else {
            ++iter;
        }

        if (bFindGold && bFindHeirRoom)
            break;

    }

    _int iCount = 0;

    for (auto& iter : m_itemList) {

        iter->pItem->UpdateInventory({ -182.f + (iCount * 42.f) , -320.f ,0.f });
        ++iCount;
    }



}

void CInventory::InsertItem(shared_ptr<CItem> _pItem)
{

    _bool bDuplicated = false;
    //Áßº¹°Ë»ç ¹× °¹¼ö ´Ã¸®±â 

    for (auto& iter : m_itemList) {

        if (iter->pItem->GetItemKeyName() == _pItem->GetItemKeyName()) {
            iter->iCapacity += iter->pItem->GetAmount();
            bDuplicated = true;
            break;
        }
    }

    if (!bDuplicated) {

        shared_ptr<tagInvenInfo> m_pInvenInfo = make_shared<tagInvenInfo>();
        m_pInvenInfo->pItem = _pItem;
        m_pInvenInfo->iCapacity = _pItem->GetAmount();

        if (m_itemList.size() <= 10) {
            m_itemList.push_back(m_pInvenInfo);
            _pItem->AwakeGameObject();
            _pItem->SetPos({ -182.f + (m_iTotalItemNum * 42.f) , -320.f ,0.f });
            _pItem->SetScale({ 18.f, 32.f, 1.f });
            _pItem->ReadyGameObject();
            ++m_iTotalItemNum;
        }
    }
}

void CInventory::DeleteItem(tstring _strItmeName)
{

    for (auto it = m_itemList.begin(); it != m_itemList.end(); it++) {
        if (_strItmeName == (*it)->pItem->GetItemKeyName()) {

            if ((*it)->iCapacity > 1) {

                --((*it)->iCapacity);

            }
            else {
                m_itemList.erase(it);
                --m_iTotalItemNum;

                _int iCount = 0;
                for (auto& iter : m_itemList) {

                    iter->pItem->UpdateInventory({ -182.f + (iCount * 42.f) , -320.f ,0.f });
                    ++iCount;
                }

                break;
            }
        }
    }

}

void CInventory::PickingUI(LONG _fX, LONG _fY, _bool _bLB)
{
    size_t iCurrentNum = -1;
    auto iter = m_itemList.begin();

    for (size_t i = 0; i < m_itemList.size(); ++i) {
        if (m_UIRect.left + 45.f * (i+1) >= _fX) {
            iCurrentNum = i;
            break;
        }
        ++iter;
    }


    if (iCurrentNum >= 0 && iCurrentNum < m_itemList.size()) {

       EHandItem eType = (*iter)->pItem->GetItemTypeEnum();
        
       if (eType != EHandItem::ENUM_END) {

           if(EHandItem::TORCH == eType && false == _bLB){
               auto pSharedPlayer = m_pPlayer.lock();
               pSharedPlayer->SpendTorchItem();

               CSoundMgr::GetInstance()->StopSound(CHANNELID::ITEM);
               CSoundMgr::GetInstance()->PlaySound(L"UI_Item_Torch_Start.wav", CHANNELID::ITEM, 1.f);
               DeleteItem(L"Item_UI_Torch");
           }
           else {
               auto pSharedPlayer = m_pPlayer.lock();
               pSharedPlayer->SetCurrentItem(eType);
           }
       }
    }
}

void CInventory::HoverUI(LONG _fX, LONG _fY)
{
    size_t iCurrentNum = -1;
    auto iter = m_itemList.begin();

    for (size_t i = 0; i < m_itemList.size(); ++i) {
        if (m_UIRect.left + 45.f * (i + 1) >= _fX) {
            iCurrentNum = i;
            break;
        }
        ++iter;
    }


    if (iCurrentNum >= 0 && iCurrentNum < m_itemList.size()) {

        CUIMgr::GetInstance()->SetDescription((*iter)->pItem);
    }
}



void CInventory::AddComponent()
{
    if (m_bReady) return;

    shared_ptr<CComponent> pComponent;

    CUIObj::AddComponent();

    pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
    m_pTextureCom->SetTextureKey(L"UI_Inventory", TEX_NORMAL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture",pComponent });
}
