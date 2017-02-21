#pragma once
#include <map>
#include <set>
#include "../expresshead.h"
#include "../Video/Video.h"
#include "../HttpRequst/HttpRequest.h"
#include "../ConfigFile/ConfigFile.h"
#include "../TCP/TCPSocket.h"
#include "../Thread/CrossThread.h"

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

struct remote_class_info
{
	std::string class_name;
	std::string ico_path;
};

class CICOControlUI : public CLabelUI
{
public:
	CICOControlUI(){}
	~CICOControlUI(){}
	void PaintBkImage(HDC hDC)
	{
		if (m_sBkImage.IsEmpty()) return;
		if (!DrawImage(hDC, (LPCTSTR)m_sBkImage)) m_sBkImage.Empty(); 
		if (m_stranImage.IsEmpty()) return;
		if (!DrawImage(hDC, (LPCTSTR)m_stranImage)) m_stranImage.Empty();
	}
	void SetTranImage(LPCTSTR pStrImage)
	{
		if (pStrImage == m_stranImage)
			return;
		else
			m_stranImage = pStrImage;
		Invalidate();
	}
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("TranImage")) == 0)
			SetTranImage(pstrValue);
		else
			__super::SetAttribute(pstrName, pstrValue);
	}
private:
	CDuiString m_stranImage;

};


class MainView :public WindowImplBase ,public IListener
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
	void Recv(SOCKET sock, const char* ip, const int port, char* data, int dataLength);
private:
protected:
	void Notify(TNotifyUI& msg);
	map<std::string, std::string> paraseCommand(const char* cmd);
private:
	//TCP 客户端
	ITCPClient *client;

	//讲课端IP
	string class_ip;

	//教室列表
	map<string, remote_class_info>class_room_list;

	CLabelUI *lab_date, *lab_time,*lab_notice;


	_classUnit classRoom[5];

	// 所有在线客户端的IP列表
	std::vector<std::string>ip_table;

	void Init();

	//初始化右侧教室列表控件
	void initClassRoom();

	void DisplayDateTime();

	//更新配置
	void updateConfigure();

	//获取本地高清播放地址
	void getLoalUrl();
	//获取IP列表
	void update_ipTable();

	std::string localUrl;

	std::string remoteUrl;
private:
	HANDLE initdata_thread;
	int update_state;
	std::string update_devname;
	std::string update_ip;
	HANDLE update_thread;
	//初始化数据线程
	friend DWORD WINAPI initProc(_In_ LPVOID paramer);
	//更新新数据线程
	friend DWORD WINAPI updateProc(_In_ LPVOID paramer);
};

