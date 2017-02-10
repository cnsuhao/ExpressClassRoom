#pragma once
#include "../expresshead.h"


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
	void Init();
	void DisplayDateTime();

};


