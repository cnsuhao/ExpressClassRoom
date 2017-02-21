#pragma once
#include "../expresshead.h"
#include "LivePlayer.h"
#ifdef _DEBUG
#pragma comment(lib,"../lib/LivePlayerD.lib")
#else
#pragma comment(lib,"../lib/LivePlayer.lib")
#endif // DEBUG

class Ido
{
public:
	virtual void work()=0;
};

class tmp_thread
{
public:
	tmp_thread():m_work(NULL){}
	void setWork(Ido* work);
	void run();
	void do_work();
private:
	Ido *m_work;
};

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

class CVideoUI :public CContainerUI ,public Ido
{
	friend class CVideoWnd;
public:
	CVideoUI(void);
	~CVideoUI(void);
	void SetVisible(bool bVisible=true );
	void SetPos(RECT rc);
	HWND getHwnd();
	void Init();
	bool play(std::string url);
	void stop();
	void setScale(enScale eScale = Scale_Full);
	void setVolume(int volume);
	void setMute(bool mute=true);
	ILivePlayer *MediaPlayer;
private:
	CVideoWnd *m_pwindows;
	std::string playurl;
public:
	void work();

};
