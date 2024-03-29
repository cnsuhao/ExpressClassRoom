#pragma once
#include "../expresshead.h"


class LoginWnd :public WindowImplBase
{
public:
	LoginWnd();
	~LoginWnd();
public:
	static std::string getToken(std::string ip);
	LPCTSTR GetWindowClassName()const;
	CDuiString GetSkinFile();
	CDuiString GetSkinFolder();
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam);
	CControlUI* CreateControl(LPCTSTR pstrClass);
private:
	bool is_rem, is_auto;
	std::string userName, Password;
	std::string cloudIP, cgi;
	bool login_error;
protected:
	void Notify(TNotifyUI& msg);
private:
	void Init();
	void LoadLocalData();
	void StartLogin();
	void LoginError(const char* msg);
	void LoginSuccess();

};


