#pragma once
#include "../expresshead.h"


class NotifyWnd :public WindowImplBase
{
public:
	NotifyWnd(std::string msg,std::string title);
	~NotifyWnd();
public:
	LPCTSTR GetWindowClassName()const;
	CDuiString GetSkinFile();
	CDuiString GetSkinFolder();
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Init();
protected:
	std::string _msg, _title;
	void Notify(TNotifyUI& msg);

};

class TipMsg
{
public:
	static int ShowMsgWindow(HWND parentHwnd,std::string msg="提示信息",std::string title="提示");
};
