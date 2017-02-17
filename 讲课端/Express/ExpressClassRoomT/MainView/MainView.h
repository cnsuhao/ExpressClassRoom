#pragma once
#include "../expresshead.h"
#include "../Video/Video.h"
#include "../HttpRequst/HttpRequest.h"
#include "../ConfigFile/ConfigFile.h"

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

struct _classUnit
{
	CButtonUI* btn_ico;
	CLabelUI* lab_title;
	CLabelUI* lab_ip;
	CButtonUI* btn_connect;
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
	void loadAllClassRoom();
	void loadClassRoom(std::string ip);
protected:
	void Notify(TNotifyUI& msg);
private:
	CLabelUI *lab_date, *lab_time;
	_SmallVideoCtrl subVideo[6];
	_classUnit classRoom[4];
	void Init();
	//初始化下方视频控件
	void initSubVideo();
	//初始化右侧教室列表控件
	void initClassRoom();
	void DisplayDateTime();
	//更新配置
	void updateConfigure();

};


