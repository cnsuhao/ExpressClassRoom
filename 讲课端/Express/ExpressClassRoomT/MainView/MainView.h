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
	std::string dev_name;//�豸��
	std::string play_url;//���ŵ�ַ(������)
	std::string picture_path;//ͷ��·��
	_classUnit  class_ctrl;//���ɵĿؼ�
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

	//TCP�ͻ���
	ITCPClient* client;
	void Init();
	//��ʼ���·���Ƶ�ؼ�
	void initSubVideo();
	//��ʼ���Ҳ�����б�ؼ�
	void initClassRoom();
	void DisplayDateTime();
	//��������
	void updateConfigure();

	//���½����б�����һ��������
	void delete_classUI(_classinfo info,int current_mount);
	void add_classUI(_classinfo info);
private:
	std::string just_join;//�ոռ����IP
	std::map<std::string, _classinfo>class_list;
	//�������γ�Ա�����߳�
	HANDLE updateMenberJoin_thread;
	//����ͷ���߳�
	HANDLE updatePicture_thread;
	//�л����߳�
	HANDLE switch_view_thread;
	friend DWORD WINAPI updateJoin_Proc(_In_ LPVOID paramer);
	friend DWORD WINAPI updatePicture_Proc(_In_ LPVOID paramer);
	friend DWORD WINAPI LiveViewSwitch(_In_ LPVOID paramer);
private:
	//ѧ��-��ʦ �����л�
	int current_index;
	int current_view;
	void cut_view(int index, int view);

};


