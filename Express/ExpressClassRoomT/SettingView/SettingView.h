#pragma once
#include "../expresshead.h"
//#include "../ConfigFile/ConfigFile.h"

class SettingView :public WindowImplBase
{
public:
	SettingView();
	~SettingView();
	LPCTSTR GetWindowClassName()const;
	CDuiString GetSkinFile();
	CDuiString GetSkinFolder();
private:
	void Notify(TNotifyUI& msg);
	//ConfigFile cfg;
};

