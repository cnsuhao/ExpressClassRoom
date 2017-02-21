#pragma once
#include "../expresshead.h"
#include "../Video/Video.h"
#include "../HttpRequst/HttpRequest.h"
#include "../ConfigFile/ConfigFile.h"
#include "../TCP/TCPSocket.h"
#include <map>

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
	CVerticalLayoutUI *lay;
};

struct _classinfo
{
	std::string dev_name;//设备名
	std::string play_url;//播放地址(标清流)
	std::string picture_path;//头像路径
	_classUnit  class_ctrl;//容纳的控件
	_SmallVideoCtrl class_video;
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
class MainView :public WindowImplBase, public IListener
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
	map<std::string, std::string> paraseCommand(const char* cmd);
private:
	void loadAllClassRoom();
	void loadClassRoom(std::string ip);
protected:
	void Notify(TNotifyUI& msg);
private:
	CLabelUI *lab_date, *lab_time,*lab_notice;;
	_SmallVideoCtrl subVideo[6];
	_classUnit classRoom[4];

	//TCP客户端
	ITCPClient* client;
	void Init();
	//初始化下方视频控件
	void initSubVideo();
	//初始化右侧教室列表控件
	void initClassRoom();
	void DisplayDateTime();
	//更新配置
	void updateConfigure();

	//更新教室列表，保持一定的有序
	void delete_classUI(_classinfo info,int current_mount);
	void add_classUI(_classinfo info);
private:
	std::string just_join;//刚刚加入的IP
	std::map<std::string, _classinfo>class_list;
	//更新听课成员加入线程
	HANDLE updateMenberJoin_thread;
	//更新头像线程
	HANDLE updatePicture_thread;
	//切换流线程
	HANDLE switch_view_thread;
	friend DWORD WINAPI updateJoin_Proc(_In_ LPVOID paramer);
	friend DWORD WINAPI updatePicture_Proc(_In_ LPVOID paramer);
	friend DWORD WINAPI LiveViewSwitch(_In_ LPVOID paramer);
private:
	//学生-老师 画面切换
	int current_index;
	int current_view;
	void cut_view(int index, int view);

};


