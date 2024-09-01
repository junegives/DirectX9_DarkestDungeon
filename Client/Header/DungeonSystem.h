#pragma once
#include"GameObject.h"

class CDungeonRoom;

class CDungeonSystem
{
public:
	explicit CDungeonSystem();
	virtual ~CDungeonSystem();

public:
	void	AbleRoom(int _iNum);
	void	DisableRoom(int _iNum);
	void	DisableAllRoom();

public:
	vector<shared_ptr<CDungeonRoom>>	GetDungeonRoomVector() { return m_vDungeonRoom; }

public:
	void	PushDungeonRoomVector(vector<shared_ptr<CDungeonRoom>>& _vVec)
	{
		m_vDungeonRoom = _vVec;
	}



private:
	vector<shared_ptr<CDungeonRoom>>	m_vDungeonRoom = {};

};

