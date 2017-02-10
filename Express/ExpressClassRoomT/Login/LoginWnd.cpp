#include <time.h>
#include "LoginWnd.h"
#include "../HttpRequst/HttpRequest.h"
#include "../ConfigFile/ConfigFile.h"
#include "../sqlite3/db.h"
#include "../xml/tinystr.h"
#include "../xml/tinyxml.h"

#define  TIMER_ID_LOGIN	1000
#define  CLOUD_IP_FILE	"login.conf"
#define  DB_FILE		"user.db"

std::string global_user;
std::string globale_passwd;
bool		global_rem=false;
bool		global_auto=false;

static std::string XorEncrypt(std::string src, std::string key)
{
	std::string tmp = src;
	for (int i = 0; i < src.length(); i++)
	{
		src[i] ^= key[i%key.length()];
	}
	return src;
}

LoginWnd::LoginWnd() :is_rem(false), is_auto(false), userName(""), Password(""), login_error(false)
{

}


LoginWnd::~LoginWnd()
{
}

LPCTSTR LoginWnd::GetWindowClassName()const
{
	return _T("LoginWnd");
}


int callForDB(void* data, int argc, char** argv, char** colname)
{
	if (argc == 5)
	{
		global_user = string(argv[0]);
		globale_passwd = string(argv[1]);
		global_rem = !strcmp(argv[2], "1");
		global_auto = !strcmp(argv[3], "1");
	}
	return 0;
}
void LoginWnd::Init()
{
	//--[init configure]
	db::Open(DB_FILE);
	db::Exec("create table user (username TEXT  PRIMARY KEY NOT NULL ,password TEXT NOT NULL, rem BOOLEAN,auto BOOLEAN,recentlogin DATETIME)", NULL);
	db::Exec("select * from user where recentlogin in (select max(recentlogin) from user)",callForDB);
	db::Close();
	LoadLocalData();
	is_rem = global_rem;
	is_auto = global_auto;
	if (is_auto)
	{
		StartLogin();
	}
}
CDuiString LoginWnd::GetSkinFile()
{
	return _T("LoginWnd.xml");
}

void LoginWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btn_close"))
		{
			this->Close(0);
		}
		else if (msg.pSender->GetName() == _T("btn_login"))
		{
			StartLogin();
		}
		else if (msg.pSender->GetName() == _T("btn_cancel"))
		{
			CTabLayoutUI *tab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("TabLay")));
			tab->SelectItem(0);
			KillTimer(*this, TIMER_ID_LOGIN);
		}
		else if (msg.pSender->GetName() == _T("btn_ok"))
		{
			CTabLayoutUI *tab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("TabLay")));
			tab->SelectItem(0);
			KillTimer(*this, TIMER_ID_LOGIN);
		}
		else if (msg.pSender->GetName() == _T("btn_rem"))
		{
			COptionUI* optauto = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_auto")));
			COptionUI* optrem = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_rem")));
			optrem->Selected(!optrem->IsSelected());
			is_rem = optrem->IsSelected();
			if (!is_rem)
			{
				optauto->Selected(false);
				is_auto = false;
			}
		}
		else if (msg.pSender->GetName() == _T("btn_auto"))
		{
			COptionUI* optrem = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_rem")));
			COptionUI* optauto = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_auto")));
			optauto->Selected(!optauto->IsSelected());
			is_auto = optauto->IsSelected();
			if (is_auto)
			{
				optrem->Selected(true);
				is_rem = true;
			}
		}
	}
	else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
	{
		COptionUI* optauto = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_auto")));
		COptionUI* optrem = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_rem")));
		if (msg.pSender->GetName() == _T("opt_auto"))
		{
			is_auto = optauto->IsSelected();
			if (is_auto)
			{
				optrem->Selected(true);
				is_rem = true;
			}
		}
		else if (msg.pSender->GetName() == _T("opt_rem"))
		{
			is_rem = optrem->IsSelected();
			if (!is_rem)
			{
				optauto->Selected(false);
				is_auto = false;
			}
		}
		

		
	}
}


CDuiString LoginWnd::GetSkinFolder()
{
	return _T("Skin");
}

void LoginWnd::LoadLocalData()
{
	// load configure file && load login detail from database
	CEditUI* IPEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("ip")));
	ConfigFile cfg(CLOUD_IP_FILE);
	IPEdit->SetText(cfg.getValue("ip").c_str());

	cloudIP = cfg.getValue("ip");
	cgi = cfg.getValue("cgi");

	CEditUI* userNameEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("user")));
	userNameEdit->SetText(global_user.c_str());

	CEditUI* passwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("passwd")));
	passwdEdit->SetText(globale_passwd.c_str());

	COptionUI* optrem = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_rem")));
	optrem->Selected(global_rem);

	COptionUI* optauto = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_auto")));
	optauto->Selected(global_auto);

}

void LoginWnd::StartLogin()
{
	CTabLayoutUI *tab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("TabLay")));
	tab->SelectItem(1);
	SetTimer(*this, TIMER_ID_LOGIN, 3000, NULL);
	CEditUI* userNameEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("user")));
	userName = userNameEdit->GetText().GetData();

	CEditUI* passwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("passwd")));
	Password = passwdEdit->GetText().GetData();

	COptionUI* optrem = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_rem")));
	COptionUI* optauto = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_auto")));

}
void LoginWnd::LoginError(const char* msg)
{
	CLabelUI *lab = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lab_tip")));
	lab->SetText(msg);
	CTabLayoutUI *tab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("TabLay")));
	tab->SelectItem(2);

}

LRESULT LoginWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_TIMER)
		return OnTimer(uMsg, wParam, lParam);
	else
		return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT LoginWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (wParam == TIMER_ID_LOGIN)
	{
		try
		{
			std::string requestUrl = "http://" + cloudIP + "/" + cgi+"type=login&userName="+userName+"&password="+Password;
			std::string res=HttpRequest::request(requestUrl);
			TiXmlDocument xml;
			xml.Parse(res.c_str());
			std::string code, msg;
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
			}
			if (code != "1")
			{
				KillTimer(*this, TIMER_ID_LOGIN);
				LoginError(msg.c_str());
				login_error = true;
			}
		}
		catch (exception &e)
		{
			KillTimer(*this, TIMER_ID_LOGIN);
			LoginError(e.what());
			login_error = true;
		}
		KillTimer(*this, TIMER_ID_LOGIN);
		if (!login_error)
		{
			db::Open(DB_FILE);
			char sqlcmd[500];
			time_t timer;
			time(&timer);
			sprintf(sqlcmd, "insert into user values(\"%s\",\"%s\",%d,%d,%ld)", userName.c_str(), is_rem ? Password.c_str() : "", is_rem, is_auto, timer);
			db::Exec(sqlcmd, NULL);
			sprintf(sqlcmd, "update user set password=\"%s\",rem=%d,auto=%d,recentlogin=%d where username=\"%s\"", is_rem ? Password.c_str() : "", is_rem, is_auto, timer, userName.c_str());
			db::Exec(sqlcmd, NULL);
			db::Close();
			ConfigFile cfg(CLOUD_IP_FILE);
			cfg.addValue("ip", m_PaintManager.FindControl(_T("ip"))->GetText().GetData());
			cfg.save();
			this->Close();
		}
	}
	return 0;
}



