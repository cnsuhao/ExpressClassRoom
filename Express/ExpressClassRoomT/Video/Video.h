#pragma once
#include "../expresshead.h"
#include "LivePlayer.h"

#ifdef _DEBUG
#pragma comment(lib,"../lib/LivePlayerD.lib")
#else
#pragma comment(lib,"../lib/LivePlayer.lib")
#endif
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
