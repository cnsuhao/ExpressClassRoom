#include "LoginWnd.h"
#include "../HttpRequst/HttpRequest.h"
#include "../ConfigFile/ConfigFile.h"
#include "../sqlite3/db.h"

#define  TIMER_ID_LOGIN	1000
#define  CLOUD_IP_FILE	"login.conf"
#define  DB_FILE		"user.db"
static std::string XorEncrypt(std::string src, std::string key)
{
	std::string tmp = src;
	for (int i = 0; i < src.length(); i++)
	{
		src[i] ^= key[i%key.length()];
	}
	return src;
}

LoginWnd::LoginWnd() :is_rem(false), is_auto(false), userName(""), Password("")
{

}


LoginWnd::~LoginWnd()
{
}

LPCTSTR LoginWnd::GetWindowClassName()const
{
	return _T("LoginWnd");
}

int call(void* data, int argc, char** argv, char** colname)
{
	for (int i = 0; i < argc; i++)
	{
		cout << colname[i] << argv[i] << endl;
	}
	return 0;
}
void LoginWnd::Init()
{
	//--[init configure]
	LoadLocalData();
	db::Open("test.db");
	db::Exec("create Table user (username text not null ,password text not null,boolean rem,bool auto,recentlogin datetime)", NULL);
	
	db::Exec("insert into user values(\"administrator\",\"password\",1,1,1231223123)", NULL);
	db::Exec("select * from user", call);
	db::Close();
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
			CTabLayoutUI *tab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("TabLay")));
			tab->SelectItem(1);
			StartLogin();
		}
		else if (msg.pSender->GetName() == _T("btn_cancel"))
		{
			CTabLayoutUI *tab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("TabLay")));
			tab->SelectItem(0);
			KillTimer(*this, TIMER_ID_LOGIN);
		}
		else if (msg.pSender->GetName() == _T("btn_rem"))
		{
			COptionUI* optrem = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_rem")));
			optrem->Selected(!optrem->IsSelected());
			is_rem = optrem->IsSelected();
		}
		else if (msg.pSender->GetName() == _T("btn_auto"))
		{
			COptionUI* optauto = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_auto")));
			optauto->Selected(!optauto->IsSelected());
			is_auto = optauto->IsSelected();
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
}

void LoginWnd::StartLogin()
{
	SetTimer(*this, TIMER_ID_LOGIN, 2000, NULL);
	CEditUI* userNameEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("user")));
	userName = userNameEdit->GetText().GetData();

	CEditUI* passwdEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("passwd")));
	Password = passwdEdit->GetText().GetData();
	ConfigFile cfg(CLOUD_IP_FILE);
	cfg.addValue("ip",m_PaintManager.FindControl(_T("ip"))->GetText().GetData());
	cfg.save();
}
void LoginWnd::LoginError(const char* msg)
{
	::MessageBoxA(NULL, msg, "tip", 0);
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
			HttpRequest::request("http://192.168.8.83 ");
		}
		catch (exception &e)
		{
			KillTimer(*this, TIMER_ID_LOGIN);
			LoginError(e.what());
			Close(STATUS_ERROR);
		}
		KillTimer(*this, TIMER_ID_LOGIN);
		Close(STATUS_OK);
	}
	return 0;
}



