#pragma once
#include "../expresshead.h"
#include "../Video/Video.h"

struct _SmallVideoCtrl
{
	CLabelUI*	lab_title;
	CButtonUI*	btn_sound;
	CVideoUI*	video;
	union 
	{
		CButtonUI* btn_teacher;
		CButtonUI* btn_ppt;
	};
	CButtonUI* btn_student;
};

class MainView :public WindowImplBase
{
public:
	MainView();
	~MainView();
public:
	LPCTSTR GetWindowClassName()const;
	CDuiString GetSkinFile();
	CDuiString GetSkinFolder();
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam);
	CControlUI* CreateControl(LPCTSTR pstrClass);
private:
protected:
	void Notify(TNotifyUI& msg);
private:
	CLabelUI *lab_date, *lab_time;
	_SmallVideoCtrl subVideo[6];
	void Init();
	void initSubVideo();
	void DisplayDateTime();

};


