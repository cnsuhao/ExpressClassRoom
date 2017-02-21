#include "MainView.h"
#include "../Video/Video.h"
#include "../SettingView/SettingView.h"
#include "../NotifyView/NotifyWnd.h"
#include "../xml/tinyxml.h"
#include "../xml/tinystr.h"
#include "../HttpRequst/HttpRequest.h"
#include "../CMyCharConver.h"
#include "../CjrCurl/IMyCurl.h"
#include <fstream>
#define  TIME_ID_UPDATE_TIME	1001
#define  TIME_ID_NOTIFY			1002
MainView::MainView() :lab_date(NULL), lab_time(NULL), client(NULL),
initdata_thread(NULL), update_thread(NULL), update_state(0)
{
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


void MainView::Recv(SOCKET sock, const char* ip, const int port, char* data, int dataLength)
{
	if (dataLength > 0)
	{
		map<string, string> res = paraseCommand(data);
		if (res["type"] == "GetIpList")
		{
			string iplist=res["ip"];
			int pos = -1;
			while ((pos = iplist.find_first_of(';'))!=-1)
			{
				ip_table.push_back(iplist.substr(0, pos));
				iplist.erase(0, pos + 1);
			}
			initdata_thread = CreateThread(NULL, 0, initProc, (void*)this, NULL, 0);
			
		}
		else if (res["type"] == "UpdateDevName")
		{
			update_state = 1;
			if (update_thread)
			{
				WaitForSingleObject(update_thread,3000);
				CloseHandle(update_thread);
			}
			update_ip = res["ip"];
			update_devname = res["name"];
			update_thread = CreateThread(NULL, 0, updateProc, (void*)this, NULL, 0);
		}
		else if (res["type"] == "UpdatePicture")
		{
			update_state = 2;
			if (update_thread)
			{
				WaitForSingleObject(update_thread, 3000);
				CloseHandle(update_thread);
			}
			update_ip = res["ip"];
			update_thread = CreateThread(NULL, 0, updateProc, (void*)this, NULL, 0);
		}
		else if (res["type"] == "JoinMeeting")
		{
			//加入听课
			ip_table.push_back(res["ip"]);
			if (initdata_thread)
			{
				WaitForSingleObject(initdata_thread, 3000);
				CloseHandle(initdata_thread);
			}
			PostMessageA(WM_USER + 125);
			initdata_thread = CreateThread(NULL, 0, initProc, (void*)this, NULL, 0);
		}
		else if (res["type"] == "QuitMeeting")
		{
			//退出听课
			for (int i = 0; i < ip_table.size(); i++)
			{
				if (ip_table[i] == res["ip"])
				{
					ip_table.erase(ip_table.begin() + i);
					break;
				}
			}
			if (initdata_thread)
			{
				WaitForSingleObject(initdata_thread, 3000);
				CloseHandle(initdata_thread);
			}
			initdata_thread = CreateThread(NULL, 0, initProc, (void*)this, NULL, 0);
		}
		else if (res["type"] == "SelChat")
		{
			//当前选择的发言

		}
		else if (res["type"] == "RequestJoin")
		{
			//听课端不会收到此消息
		}
		else if (res["type"] == "RequestSpeak")
		{
			//听课端不会收到此消息
		}
	}
}

map<std::string, std::string> MainView::paraseCommand(const char* cmd)
{
	map<std::string, std::string> res_map;
	string scmd = string(cmd);
	int pos = -1;
	while ((pos = scmd.find_first_of('&')) != -1 || scmd.length()>=1)
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
			if (IDOK == TipMsg::ShowMsgWindow(*this, _T("是否退出"), _T("提示")))
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
		else if(msg.pSender->GetName()==_T("btn_request_connect"))
		{
			//请求连接
			string data = "type=RequestJoin&ip="+login_ip;
			char cdata[100];
			strcpy(cdata, data.c_str());
			client->sendData(cdata, strlen(cdata));
		}
		else if (msg.pSender->GetName() == _T("btn_request_interact"))
		{
			//请求互动发言
			string data = "type=RequestSpeak&ip=" + login_ip;
			char cdata[100];
			strcpy(cdata, data.c_str());
			client->sendData(cdata, strlen(cdata));
		}

	}
	else if (msg.sType==_T("online"))
	{
		CHorizontalLayoutUI* hor_notify = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("NotifyLay")));
		hor_notify->SetVisible(true);
		PlaySound(_T("mgg.wav"), NULL, SND_ASYNC);
		SetTimer(*this, TIME_ID_NOTIFY, 400, NULL);
	}
	else if (msg.sType == _T("initclassroom"))
	{
		// update class UI
		classRoom[0].lab_title->SetText(class_room_list[class_ip].class_name.c_str());
		classRoom[0].btn_ico->SetBkImage((class_ip + "/" + class_room_list[class_ip].ico_path).c_str());
		int named = 2;
		for (map<string, remote_class_info>::iterator it = class_room_list.begin(); it != class_room_list.end(); it++)
		{
			if (it->first != class_ip)
			{
				char ver_name[20];
				sprintf(ver_name, "ver%d", named);
				CVerticalLayoutUI* ver = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(ver_name));
				ver->SetVisible(true);
				classRoom[named - 1].lab_ip->SetText(it->first.c_str());
				classRoom[named - 1].lab_title->SetText(it->second.class_name.c_str());
				if (it->second.ico_path.length()>2)
					classRoom[named - 1].btn_ico->SetBkImage((it->first + "/" + it->second.ico_path).c_str());
				++named;
			}
		}

	}
	else if (msg.sType == _T("update_dev"))
	{
		CHorizontalLayoutUI* hor_notify = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("NotifyLay")));
		hor_notify->SetVisible(true);
		lab_notice->SetText(_T("名称有了新的更新"));
		PlaySound(_T("mgg.wav"), NULL, SND_ASYNC);
		SetTimer(*this, TIME_ID_NOTIFY, 400, NULL);
	}
	else if (msg.sType == _T("update_ico"))
	{
		CHorizontalLayoutUI* hor_notify = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("NotifyLay")));
		hor_notify->SetVisible(true);
		lab_notice->SetText(_T("头像有了新的更新"));
		PlaySound(_T("mgg.wav"), NULL, SND_ASYNC);
		SetTimer(*this, TIME_ID_NOTIFY, 400, NULL);
	}
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
	if (uMsg == WM_TIMER)
		return OnTimer(uMsg, wParam, lParam);
	else if (uMsg == WM_USER + 123)
	{
		//更新设备名称
		ConfigFile c(CLOUD_IP_FILE);
		string data = "type=UpdateDevName&ip=" + login_ip + "&name=" + c.getValue("name");
		char cdata[200];
		strcpy(cdata, data.c_str());
		client->sendData(cdata,strlen(cdata));
	}
	else if (uMsg == WM_USER + 124)
	{
		//更新头像消息
		string data = "type=UpdatePicture&ip=" + login_ip;
		char cdata[200];
		strcpy(cdata, data.c_str());
		client->sendData(cdata, strlen(cdata));
	}
	else if (uMsg == WM_USER + 125)
	{
		//加入听课成功
		CButtonUI* btn_request = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_request_connect")));
		btn_request->SetText(_T("断开连接"));
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
		if (!localUrl.empty())
		{
			video->MediaPlayer->Load(localUrl);
			//video->play(localUrl);
		}
		CLabelUI *lab = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_notify")));
		lab->SetText(_T("您已经登录到听课端"));
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
	ConfigFile cfg(CLOUD_IP_FILE);
	class_ip = cfg.getValue("cloudip", "remote");
	initClassRoom();
	getLoalUrl();
	lab_date = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_date")));
	lab_time = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_time")));
	SetTimer(*this, TIME_ID_UPDATE_TIME, 999, NULL);
	SetTimer(*this, 1111, 500, NULL);
	// hide notify
	CHorizontalLayoutUI* hor_notify = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("NotifyLay")));
	hor_notify->SetVisible(false);
	classRoom[0].lab_ip->SetText(class_ip.c_str());
	ip_table.push_back(class_ip);
	update_ipTable();
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


void MainView::initClassRoom()
{
	for (int i = 0; i < 5; i++)
	{
		char ico_name[MAX_PATH];
		sprintf(ico_name, "school_ico%d", i + 1);
		classRoom[i].btn_ico = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T(ico_name)));
		char school_name[MAX_PATH];
		sprintf(school_name, "lab_school_name%d", i + 1);
		classRoom[i].lab_title = static_cast<CLabelUI*>(m_PaintManager.FindControl(school_name));
		char school_ip[MAX_PATH];
		sprintf(school_ip,"lab_school_ip%d",i+1);
		classRoom[i].lab_ip = static_cast<CLabelUI*>(m_PaintManager.FindControl(school_ip));
		char school_connect[MAX_PATH];
		sprintf(school_connect, "btn_connect%d", i + 1);
		classRoom[i].btn_connect = static_cast<CButtonUI*>(m_PaintManager.FindControl(school_connect));
		char ver_name[MAX_PATH];
		sprintf(ver_name, "ver%d", i + 1);
		CVerticalLayoutUI* ver = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T(ver_name)));
		if (i!=0)
			ver->SetVisible(false);

	}
}

void MainView::getLoalUrl()
{
	std::string code, msg,tmpurl;
	std::string requestUrl = "http://" + login_ip + "/" + login_cgi + "type=queryurl&name=lurl&token=" + login_token;
	std::string res = HttpRequest::request(requestUrl);
	TiXmlDocument xml;
	xml.Parse(res.c_str());
	TiXmlNode *root = xml.RootElement();
	for (TiXmlNode *i = root->FirstChildElement(); i; i = i->NextSiblingElement())
	{
		if (strcmp(i->Value(), "code") == 0)
		{
			code = string(i->FirstChild()->Value());
		}
		else if (strcmp(i->Value(), "msg") == 0)
		{
			msg = string(i->FirstChild()->Value());
		}
		else if (msg == "successed" && strcmp(i->Value(), "data") == 0)
		{
			i = i->FirstChildElement()->FirstChildElement();
			tmpurl = string(i->FirstChild()->Value());
		}
	}
	localUrl = tmpurl.replace(7, 9, login_ip);
}

void MainView::update_ipTable()
{
	client = ITCPClient::getInstance();
	client->open(class_ip.c_str(), 4507);
	client->setListener(this);
	char data[] = "type=GetIpList";
	client->sendData(data, strlen(data));
}

DWORD WINAPI initProc(_In_ LPVOID paramer)
{
	MainView* p = (MainView*)paramer;

	for (int ii = 0; ii < p->ip_table.size(); ii++)
	{
		CreateDirectoryA(p->ip_table[ii].c_str(),NULL);
		std::string logincode,namecode,icocode, mmsg,token,pmsg;
		std::string requestUrl = "http://" + p->ip_table[ii] + "/" + login_cgi + "type=login&userName=" + admin_user + "&password=" + admin_passwd;
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
			remote_class_info info;
			//获取名称
			requestUrl = "http://" + p->ip_table[ii] + "/" + login_cgi + "type=getdevname&token=" + token;
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
					info.class_name = CMyCharConver::UTF8ToANSI(mmsg);
				}
			}

			//获取头像
			requestUrl = "http://" + p->ip_table[ii] + "/" + login_cgi + "type=getpicture&token=" + token;
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
					info.ico_path =  CMyCharConver::UTF8ToANSI(pmsg);
				}
			}
			p->class_room_list[p->ip_table[ii]] = info;
			//download picture
			if (!info.ico_path.empty())
			{
				ICjrCurl *cjrcurl = ICjrCurl::GetInstance();
				cjrcurl->Download("http://" + p->ip_table[ii] + "/" + info.ico_path, p->ip_table[ii] + "/" + info.ico_path, "");

			}
		}
		p->m_PaintManager.SendNotify(p->lab_notice, _T("initclassroom"));
	}
	return 0;
}

DWORD WINAPI updateProc(_In_ LPVOID paramer)
{
	MainView *p = (MainView*)paramer;
	if (p->update_state == 1)
	{
		//更新设备名称

		p->m_PaintManager.SendNotify(p->lab_notice, _T("update_dev"));
	}
	else if (p->update_state == 2)
	{
		//更新头像
		p->m_PaintManager.SendNotify(p->lab_notice, _T("update_ico"));
	}
	p->update_state = 0;
	return 0;
}