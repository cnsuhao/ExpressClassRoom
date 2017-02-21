#include "MainView.h"
#include "../Video/Video.h"
#include "../SettingView/SettingView.h"
#include "../NotifyView/NotifyWnd.h"
#include "../Login/LoginWnd.h"
#include "../xml/tinyxml.h"
#include "../xml/tinystr.h"
#include "../CMyCharConver.h"
#include "../CjrCurl/IMyCurl.h"
#define  TIME_ID_UPDATE_TIME	1001
#define  TIME_ID_NOTIFY			1002


#define WM_CLIENT_ADDED		WM_USER+100



MainView::MainView() :lab_date(NULL), lab_time(NULL), updateMenberJoin_thread(NULL), updatePicture_thread(NULL),
switch_view_thread(NULL), current_view(-1), current_index(-1), client(NULL)
{
	if (!client)
	{
		client = ITCPClient::getInstance();
		client->open(login_ip.c_str(), 4507);
		client->setListener(this);
	}
}
MainView::~MainView()
{
	if (client)
	{
		client->close();
		ITCPClient::releaseInstance(client);
	}
}

LPCSTR  MainView::GetWindowClassName() const
{
	return _T("MainView");
}

CDuiString MainView::GetSkinFolder()
{
	return _T("Skin");
}

CDuiString MainView::GetSkinFile()
{
	return _T("MainView.xml");
}
map<std::string, std::string> MainView::paraseCommand(const char* cmd)
{
	map<std::string, std::string> res_map;
	string scmd = string(cmd);
	int pos = -1;
	while ((pos = scmd.find_first_of('&')) != -1 || scmd.length() >= 1)
	{
		if (pos == -1)
			pos = scmd.length();
		string percmd = scmd.substr(0, pos);
		string key = percmd.substr(0, percmd.find_first_of('='));
		string values = percmd.substr(percmd.find_first_of('=') + 1, percmd.length());
		res_map[key] = values;
		scmd.erase(0, pos + 1);
	}
	return res_map;
}

void MainView::Recv(SOCKET sock, const char* ip, const int port, char* data, int dataLength)
{
	if (dataLength > 0)
	{
		map<string, string> res = paraseCommand(data);
		if (res["type"] == "GetIpList")
		{
			//讲课端不需要
		}
		else if (res["type"] == "UpdateDevName")
		{
			//更新设备名称
			class_list[res["ip"]].dev_name = res["name"];
			class_list[res["ip"]].class_ctrl.lab_title->SetText(res["name"].c_str());
			class_list[res["ip"]].class_ctrl.lab_title->Invalidate();
		}
		else if (res["type"] == "UpdatePicture")
		{
			if (!updatePicture_thread)
			{
				updatePicture_thread = CreateThread(NULL, 0, updatePicture_Proc, this, NULL, NULL);
			}
			else
			{
				WaitForSingleObject(updatePicture_thread, 3000);
				CloseHandle(updatePicture_thread);
				updatePicture_thread = CreateThread(NULL, 0, updatePicture_Proc, this, NULL, NULL);
			}
		}
		else if (res["type"] == "JoinMeeting")
		{
			//讲课端无此消息
		}
		else if (res["type"] == "QuitMeeting")
		{
			//收到听课端退出的消息
			delete_classUI(class_list[res["ip"]], class_list.size());
		}
		else if (res["type"] == "SelChat")
		{
			//讲课端 不需要
		}
		else if (res["type"] == "RequestJoin")
		{
			just_join = res["ip"];
			if (!updateMenberJoin_thread)
			{
				updateMenberJoin_thread = CreateThread(NULL, 0, updateJoin_Proc, this, NULL, NULL);
			}
			else
			{
				WaitForSingleObject(updateMenberJoin_thread,3000);
				CloseHandle(updateMenberJoin_thread);
				updateMenberJoin_thread = CreateThread(NULL, 0, updateJoin_Proc, this, NULL, NULL);
			}
		}
		else if (res["type"] == "RequestSpeak")
		{
			
		}
	}
}

DWORD WINAPI updateJoin_Proc(_In_ LPVOID paramer)
{
	MainView *p = (MainView*)paramer;
	p->class_list[p->just_join];
	CreateDirectoryA(p->just_join.c_str(), NULL);
	std::string logincode, namecode, icocode, mmsg, token, pmsg;
	// 登陆获得token
	std::string requestUrl = "http://" + p->just_join + "/" + login_cgi + "type=login&userName=" + admin_user + "&password=" + admin_passwd;
	std::string res = HttpRequest::request(requestUrl);
	TiXmlDocument xml;
	xml.Parse(res.c_str());
	TiXmlNode *root = xml.RootElement();
	for (TiXmlNode *i = root->FirstChildElement(); i; i = i->NextSiblingElement())
	{
		if (strcmp(i->Value(), "code") == 0)
		{
			logincode = string(i->FirstChild()->Value());
		}
		else if (strcmp(i->Value(), "msg") == 0)
		{
			token = string(i->FirstChild()->Value());
		}
	}
	if (logincode == "1")
	{
		//获取名称
		requestUrl = "http://" + p->just_join + "/" + login_cgi + "type=getdevname&token=" + token;
		res = HttpRequest::request(requestUrl);
		TiXmlDocument xmll;
		xmll.Parse(res.c_str());
		TiXmlNode *root = xmll.RootElement();
		for (TiXmlNode *i = root->FirstChildElement(); i; i = i->NextSiblingElement())
		{
			if (strcmp(i->Value(), "code") == 0)
			{
				namecode = string(i->FirstChild()->Value());
			}
			else if (strcmp(i->Value(), "msg") == 0 && namecode == "1")
			{
				mmsg = string(i->FirstChild()->Value());
				p->class_list[p->just_join].dev_name = CMyCharConver::UTF8ToANSI(mmsg);
			}
		}

		//获取头像
		requestUrl = "http://" + p->just_join + "/" + login_cgi + "type=getpicture&token=" + token;
		string pres = HttpRequest::request(requestUrl);
		TiXmlDocument xmlp;
		xmlp.Parse(pres.c_str());
		TiXmlNode *rootp = xmlp.RootElement();
		for (TiXmlNode *i = rootp->FirstChildElement(); i; i = i->NextSiblingElement())
		{
			if (strcmp(i->Value(), "code") == 0)
			{
				icocode = string(i->FirstChild()->Value());
			}
			else if (strcmp(i->Value(), "msg") == 0 && icocode == "1")
			{
				pmsg = string(i->FirstChild()->Value());
				p->class_list[p->just_join].picture_path = CMyCharConver::UTF8ToANSI(pmsg);
			}
		}
		//download picture
		if (!p->class_list[p->just_join].picture_path.empty())
		{
			ICjrCurl *cjrcurl = ICjrCurl::GetInstance();
			cjrcurl->Download("http://" + p->just_join + "/" + p->class_list[p->just_join].picture_path, p->just_join + "/" + p->class_list[p->just_join].picture_path, "");

		}
		//获取播放地址
		string rcode, rmsg, rtmp_url;
		string rurl = "http://" + p->just_join + "/" + login_cgi + "type=queryurl&name=sublurl&token=" + token;
		string rres = HttpRequest::request(rurl);
		TiXmlDocument xmlr;
		xmlr.Parse(pres.c_str());
		TiXmlNode *rootr = xmlr.RootElement();
		for (TiXmlNode *ir = root->FirstChildElement(); ir; ir = ir->NextSiblingElement())
		{
			if (strcmp(ir->Value(), "code") == 0)
			{
				rcode = string(ir->FirstChild()->Value());
			}
			else if (strcmp(ir->Value(), "msg") == 0)
			{
				rmsg = string(ir->FirstChild()->Value());
			}
			else if (rmsg == "successed" && strcmp(ir->Value(), "data") == 0)
			{
				ir = ir->FirstChildElement()->FirstChildElement();
				rtmp_url = string(ir->FirstChild()->Value());
			}
		}
		p->class_list[p->just_join].play_url = rtmp_url.replace(7, 9, login_ip);

	}
	//控件关联
	p->class_list[p->just_join].class_ctrl.btn_connect = p->classRoom[p->class_list.size() - 1].btn_connect;
	p->class_list[p->just_join].class_ctrl.btn_ico = p->classRoom[p->class_list.size() - 1].btn_ico;
	p->class_list[p->just_join].class_ctrl.lab_ip = p->classRoom[p->class_list.size() - 1].lab_ip;
	p->class_list[p->just_join].class_ctrl.lab_title = p->classRoom[p->class_list.size() - 1].lab_title;
	p->class_list[p->just_join].class_ctrl.lay = p->classRoom[p->class_list.size() - 1].lay;

	p->class_list[p->just_join].class_video.btn_sound = p->subVideo[p->class_list.size() - 1].btn_sound;
	p->class_list[p->just_join].class_video.btn_student = p->subVideo[p->class_list.size() - 1].btn_student;
	p->class_list[p->just_join].class_video.btn_teacher = p->subVideo[p->class_list.size() - 1].btn_teacher;
	p->class_list[p->just_join].class_video.lab_title = p->subVideo[p->class_list.size() - 1].lab_title;
	p->class_list[p->just_join].class_video.video = p->subVideo[p->class_list.size() - 1].video;

	p->PostMessageA(WM_CLIENT_ADDED, 0, 0);

	return 0;
}
DWORD WINAPI updatePicture_Proc(_In_ LPVOID paramer)
{
	MainView *p = (MainView*)paramer;
	return 0;
}
DWORD WINAPI LiveViewSwitch(_In_ LPVOID paramer)
{
	MainView *p = (MainView*)paramer;
	//std::string logincode, namecode, icocode, mmsg, token, pmsg;
	//// 登陆获得token
	//std::string requestUrl = "http://" + p->just_join + "/" + login_cgi + "type=login&userName=" + admin_user + "&password=" + admin_passwd;
	//std::string res = HttpRequest::request(requestUrl);
	//TiXmlDocument xml;
	//xml.Parse(res.c_str());
	//TiXmlNode *root = xml.RootElement();
	//for (TiXmlNode *i = root->FirstChildElement(); i; i = i->NextSiblingElement())
	//{
	//	if (strcmp(i->Value(), "code") == 0)
	//	{
	//		logincode = string(i->FirstChild()->Value());
	//	}
	//	else if (strcmp(i->Value(), "msg") == 0)
	//	{
	//		token = string(i->FirstChild()->Value());
	//	}
	//}
	//if (logincode == "1")
	//{
	//	//获取名称
	//	requestUrl = "http://" + p->just_join + "/" + login_cgi + "type=getdevname&token=" + token;
	//	res = HttpRequest::request(requestUrl);
	//	TiXmlDocument xmll;
	//	xmll.Parse(res.c_str());
	//	TiXmlNode *root = xmll.RootElement();
	//	for (TiXmlNode *i = root->FirstChildElement(); i; i = i->NextSiblingElement())
	//	{
	//		if (strcmp(i->Value(), "code") == 0)
	//		{
	//			namecode = string(i->FirstChild()->Value());
	//		}
	//		else if (strcmp(i->Value(), "msg") == 0 && namecode == "1")
	//		{
	//			mmsg = string(i->FirstChild()->Value());
	//			p->class_list[p->just_join].dev_name = CMyCharConver::UTF8ToANSI(mmsg);
	//		}
	//	}
	//}
	return 0;
}

void MainView::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btn_min"))
		{
			::ShowWindow(*this, SW_MINIMIZE);
		}
		else if (msg.pSender->GetName() == _T("btn_max"))
		{
			static bool record = false;
			if (!record)
			{
				::ShowWindow(*this, SW_MAXIMIZE);
				record = !record;
			}
			else
			{
				::ShowWindow(*this, SW_RESTORE);
				record = !record;
			}
		}
		else if (msg.pSender->GetName() == _T("btn_close"))
		{
			if (IDOK == TipMsg::ShowMsgWindow(*this, _T("确实要退出？"), _T("提示")))
			{
				Close();
			}
		}
		else if (msg.pSender->GetName() == _T("btn_expend"))
		{
			CVerticalLayoutUI *ver = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("ClassRoomLay")));
			ver->SetVisible(!ver->IsVisible());
			msg.pSender->SetVisible(false);
			CButtonUI *btn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_unexpend")));
			btn->SetVisible(true);
		}
		else if (msg.pSender->GetName() == _T("btn_unexpend"))
		{
			CVerticalLayoutUI *ver = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("ClassRoomLay")));
			ver->SetVisible(!ver->IsVisible());
			msg.pSender->SetVisible(false);
			CButtonUI *btn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_expend")));
			btn->SetVisible(true);
		}
		else if (msg.pSender->GetName()==_T("btn_setting"))
		{
			SettingView * setview = new SettingView;
			setview->Create(*this, _T("Setting"), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);
			setview->CenterWindow();
			setview->ShowModal();			
		}
		
		char teacher_name[30];
		for (int i = 1; i <= 5; i++)
		{
			//switch live stream to director teacher
			sprintf(teacher_name,"teacher%d",i);
			if (msg.pSender->GetName() == teacher_name)
			{
				cut_view(i, 0);
				break;
			}
		}
		char student_name[30];
		for (int i = 1; i <= 5; i++)
		{
			//switch live stream to play student
			sprintf(student_name, "student%d", i);
			if (msg.pSender->GetName() == student_name)
			{
				cut_view(i, 1);
				break;
			}
		}

	}
	if (msg.sType == DUI_MSGTYPE_DBCLICK)
	{
	/*	if (msg.pSender->GetName() == _T("mainVideo"))
		{
			CHorizontalLayoutUI* hor = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("SubVideoLay")));
			hor->SetVisible(!hor->IsVisible());
			for (int i = 1; i <= 6; i++)
			{
				char name[123] = "video";
				sprintf(name,"video%d",i);
				CVideoUI * video = static_cast<CVideoUI*>(m_PaintManager.FindControl(name));
				video->SetVisible(hor->IsVisible());
			}	
		}
		else if (msg.pSender->GetName() == _T("video1"))
		{
			static bool is_play = false;
			if (!is_play)
				subVideo[0].video->play("rtmp://192.168.8.236:1935/live/slive");
			else
				subVideo[0].video->stop();
			is_play=!is_play;
		}
		else if (msg.pSender->GetName() == _T("video2"))
		{
			static bool is_play = false;
			if (!is_play)
				subVideo[1].video->play("rtmp://192.168.8.236:1935/live/slive");
			else
				subVideo[1].video->stop();
			is_play=!is_play;
		}
		else if (msg.pSender->GetName() == _T("video3"))
		{
			static bool is_play = false;
			if (!is_play)
				subVideo[2].video->play("rtmp://192.168.8.236:1935/live/slive");
			else
				subVideo[2].video->stop();
			is_play=!is_play;
		}
		else if (msg.pSender->GetName() == _T("video4"))
		{
			static bool is_play = false;
			if (!is_play)
				subVideo[3].video->play("rtmp://192.168.8.236:1935/live/slive");
			else
				subVideo[3].video->stop();
			is_play=!is_play;
		}
		else if (msg.pSender->GetName() == _T("video5"))
		{
			static bool is_play = false;
			if (!is_play)
				subVideo[4].video->play("rtmp://192.168.8.236:1935/live/slive");
			else
				subVideo[4].video->stop();
			is_play=!is_play;
		}*/
	}
	else if (msg.sType==_T("online"))
	{
		CHorizontalLayoutUI* hor_notify = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("NotifyLay")));
		hor_notify->SetVisible(true);
		PlaySound(_T("mgg.wav"), NULL, SND_ASYNC);
		SetTimer(*this, TIME_ID_NOTIFY, 400, NULL);
	}
}

void MainView::delete_classUI(_classinfo info, int current_mount)
{
	CDuiString ver_name = info.class_ctrl.lay->GetName();
	int index = -1;
	if (_tcscmp(ver_name, _T("vv1")))
	{
		index = 1;
	}
	else if (_tcscmp(ver_name, _T("vv2")))
	{
		index = 2;
	}
	else if (_tcscmp(ver_name, _T("vv3")))
	{
		index = 3;
	}
	else if (_tcscmp(ver_name, _T("vv4")))
	{
		index = 4;
	}

	for (int i = index; i != -1 && i < current_mount; i++)
	{
		classRoom[i].btn_connect->SetText(classRoom[i+1].btn_connect->GetText());
		classRoom[i].btn_ico->SetBkImage(classRoom[i+1].btn_ico->GetBkImage());
		classRoom[i].lab_ip->SetText(classRoom[i+1].lab_ip->GetText());
		classRoom[i].lab_title->SetText(classRoom[i+1].lab_title->GetText());
	}
	char hide_verName[20];
	sprintf(hide_verName, "vv%d", current_mount);
	CVerticalLayoutUI* hide_ver = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(hide_verName));
	hide_ver->SetVisible(false);
}
void MainView::add_classUI(_classinfo info)
{
	info.class_ctrl.lay->SetVisible(true);
}

CControlUI* MainView::CreateControl(LPCTSTR pstrClass)
{
	if (_tcscmp(pstrClass, _T("Video")) == 0)
		return new CVideoUI();
	else  if (_tcscmp(pstrClass, _T("ICOImage")) == 0)
		return new CICOControlUI();
	else
		return __super::CreateControl(pstrClass);
}
LRESULT MainView::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/*if (uMsg == WM_TIMER)
		return OnTimer(uMsg, wParam, lParam);
	else*/ if (uMsg == WM_CLIENT_ADDED)
	{
		if (IDOK == TipMsg::ShowMsgWindow(*this, (class_list[just_join].dev_name+"申请加入听课").c_str(), _T("提示")))
		{
			class_list[just_join].class_ctrl.lay->SetVisible(true);
			lab_notice->SetText((class_list[just_join].dev_name+"上线啦").c_str());
			m_PaintManager.SendNotify(lab_notice, _T("online"));
		}
	}
	else if (WM_UPDATE_DEVNAME)
	{
		string s = "type=UpdateDevName&ip="+login_ip+"&name="+dev_name;
		char data[50];
		strcpy(data, s.c_str());
		if (!client)
		{
			client = ITCPClient::getInstance();
			client->open(login_ip.c_str(), 4507);
			client->setListener(this);
		}
		client->sendData(data,strlen(data));
	}
	else if (WM_UPDATE_ICO)
	{
		string s = "type=UpdatePicture&ip=" + login_ip;
		char data[50];
		strcpy(data, s.c_str());
		if (!client)
		{
			client = ITCPClient::getInstance();
			client->open(login_ip.c_str(), 4507);
			client->setListener(this);
		}
		client->sendData(data, strlen(data));
	}
	else
		return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT MainView::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (wParam == TIME_ID_UPDATE_TIME)
	{
		DisplayDateTime();
	}
	else if (wParam == 1111)
	{
		CVideoUI *video = static_cast<CVideoUI*>(m_PaintManager.FindControl(_T("mainVideo")));
		//video->play("rtmp://192.168.8.237:1935/live/slive");
		CLabelUI*lab = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_notify")));
		m_PaintManager.SendNotify(lab, _T("online"));
		KillTimer(*this, 1111);
	}
	else if (wParam == TIME_ID_NOTIFY)
	{
		CLabelUI*lab = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_notify")));
		LPCTSTR s=lab->GetText().GetData();
		TCHAR title[200];
		if (lab->GetText().GetLength() <= 2)
		{
			KillTimer(*this, TIME_ID_NOTIFY);
			CHorizontalLayoutUI* hor_notify = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("NotifyLay")));
			hor_notify->SetVisible(false);

		}
		_tcscpy(title, (s + 2));
		lab->SetText(title);
	}

	return 0;
}
void MainView::Init()
{
	
	lab_notice = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_notify")));
	initSubVideo();
	initClassRoom();
	lab_date = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_date")));
	lab_time = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_time")));
	SetTimer(*this, TIME_ID_UPDATE_TIME, 999, NULL);
	SetTimer(*this, 1111, 500, NULL);
	// hide notify
	CHorizontalLayoutUI* hor_notify = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("NotifyLay")));
	hor_notify->SetVisible(false);
}
void MainView::DisplayDateTime()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	static char cdate[250];
	static char ctime[250];
	sprintf(cdate, "%d-%d-%d", st.wYear, st.wMonth, st.wDay);
	if (lab_date)
		lab_date->SetText(cdate);

	sprintf(ctime, "%d:%02d:%02d",st.wHour,st.wMinute,st.wSecond);

	if (lab_time)
		lab_time->SetText(ctime);

}

void MainView::initSubVideo()
{
	for (int i = 0; i < 6; i++)
	{
		char title_name[20];
		sprintf(title_name,"lab_title%d",i+1);
		subVideo[i].lab_title = static_cast<CLabelUI*>(m_PaintManager.FindControl(title_name));
		char snd_name[20];
		sprintf(snd_name, "btn_sound%d", i + 1);
		subVideo[i].btn_sound = static_cast<CButtonUI*>(m_PaintManager.FindControl(snd_name));
		char video_name[20];
		sprintf(video_name, "video%d", i + 1);
		subVideo[i].video = static_cast<CVideoUI*>(m_PaintManager.FindControl(video_name));
		char teacher_name[20];
		sprintf(teacher_name,"teacher%d",i+1);
		subVideo[i].btn_teacher = static_cast<CButtonUI*>(m_PaintManager.FindControl(teacher_name));
		if (i != 5)
		{
			char student_name[20];
			sprintf(student_name, "student%d", i + 1);
			subVideo[i].btn_student = static_cast<CButtonUI*>(m_PaintManager.FindControl(student_name));
		}
		else
		{
			subVideo[i].btn_student = NULL;
		}

	}
}

void MainView::initClassRoom()
{
	for (int i = 0; i < 4; i++)
	{
		char ico_name[MAX_PATH];
		sprintf(ico_name, "school_ico%d", i + 1);
		classRoom[i].btn_ico = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T(ico_name)));
		char school_name[MAX_PATH];
		sprintf(school_name, "lab_school_name%d", i + 1);
		classRoom[i].lab_ip = static_cast<CLabelUI*>(m_PaintManager.FindControl(school_name));
		char school_ip[MAX_PATH];
		sprintf(school_ip,"lab_school_ip%d",i+1);
		classRoom[i].lab_ip = static_cast<CLabelUI*>(m_PaintManager.FindControl(school_ip));
		char school_connect[MAX_PATH];
		sprintf(school_connect, "btn_connect%d", i + 1);
		classRoom[i].btn_connect = static_cast<CButtonUI*>(m_PaintManager.FindControl(school_connect));
		char ver_name[MAX_PATH];
		sprintf(ver_name, "vv%d", i + 1);
		classRoom[i].lay = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(ver_name));
		//classRoom[i].lay->SetVisible(false);

	}
}

/*
index:子视频控件的序号
view:直播画面切换
*/
void MainView::cut_view(int index, int view)
{
	current_index = index;
	current_view = view;
	if (!switch_view_thread)
	{
		switch_view_thread = CreateThread(NULL, 0, LiveViewSwitch, this, NULL, NULL);
	}
	else
	{
		WaitForSingleObject(switch_view_thread, 3000);
		CloseHandle(switch_view_thread);
		switch_view_thread = CreateThread(NULL, 0, LiveViewSwitch, this, NULL, NULL);
	}
}