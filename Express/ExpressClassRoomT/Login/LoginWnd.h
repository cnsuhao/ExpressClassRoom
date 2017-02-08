#pragma once
#include "../expresshead.h"


class LoginWnd :public WindowImplBase
{
public:
	LoginWnd();
	~LoginWnd();
public:
	LPCTSTR GetWindowClassName()const;
	CDuiString GetSkinFile();
	CDuiString GetSkinFolder();
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	bool is_rem, is_auto;
	std::string userName, Password;
protected:
	void Notify(TNotifyUI& msg);
private:
	void Init();
	void LoadLocalData();
	void StartLogin();
	void LoginError(const char* msg);
	void LoginSuccess();

};


