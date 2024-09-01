#pragma once

#include"UIObj.h"
#include"Item.h"

BEGIN(Engine)

class CTexture;

END

class CPlayer;


struct tagInvenInfo {
	shared_ptr<CItem> pItem;
	_int			  iCapacity;
};

class CInventory : public CUIObj
{
public:
	explicit CInventory(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CInventory();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void LateUpdateGameObject() override;
	virtual void RenderGameObject() override;

	
	void SetPlayer(shared_ptr<CPlayer> _pPlayer) { m_pPlayer = _pPlayer; }

public:

	void ExchangeGoods(int* _iGold, int* _iHeirRoom);

public:
	void InsertItem(shared_ptr<CItem> _pItem);
	void DeleteItem(tstring _strItmeName);

	virtual void PickingUI(LONG _fX, LONG _fY, _bool _bLB) override;
	virtual void HoverUI(LONG _fX, LONG _fY) override;

protected:
	void	AddComponent();

private:
	shared_ptr<CTexture> m_pTextureCom;
	list<shared_ptr<tagInvenInfo>> m_itemList;

	_int m_iTotalItemNum = 0;
	_int m_iCurrentCursor = 0;


	weak_ptr<CPlayer> m_pPlayer;

};