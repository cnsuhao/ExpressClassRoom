#pragma once
#include "../expresshead.h"
#include "LivePlayer.h"

#pragma comment(lib,"../lib/LivePlayer.lib")
class CVideoUI;
class CVideoWnd :public CWindowWnd
{
public:
	CVideoWnd();
	LPCTSTR GetWindowClassName() const;
	void Init(CVideoUI* pOwner);
	void PainBk();
	RECT CalPos();
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	CVideoUI*	    m_pOwner;
	int click_tick;
	clock_t pre_time, cur_time;
};

class CVideoUI :public CContainerUI
{
	friend class CVideoWnd;
public:
	CVideoUI(void);
	~CVideoUI(void);
	void SetVisible(bool bVisible=true );
	void SetPos(RECT rc);
	HWND getHwnd();
	void Init();
	ILivePlayer *MediaPlayer;
private:
	CVideoWnd *m_pwindows;
};
