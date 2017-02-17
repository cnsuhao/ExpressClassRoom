#include "MainView.h"
#include "../Video/Video.h"
#include "../SettingView/SettingView.h"
#include "../NotifyView/NotifyWnd.h"
#define  TIME_ID_UPDATE_TIME	1001
#define  TIME_ID_NOTIFY			1002
MainView::MainView() :lab_date(NULL), lab_time(NULL)
{
}
MainView::~MainView()
{
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
			Close();
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

	}
	if (msg.sType == DUI_MSGTYPE_DBCLICK)
	{
		if (msg.pSender->GetName() == _T("mainVideo"))
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
		}
	}
	else if (msg.sType==_T("online"))
	{
		CHorizontalLayoutUI* hor_notify = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("NotifyLay")));
		hor_notify->SetVisible(true);
		PlaySound(_T("mgg.wav"), NULL, SND_ASYNC);
		SetTimer(*this, TIME_ID_NOTIFY, 400, NULL);
	}
}

CControlUI* MainView::CreateControl(LPCTSTR pstrClass)
{
	if (_tcscmp(pstrClass, _T("Video")) == 0)
		return new CVideoUI();
	else
		return __super::CreateControl(pstrClass);
}
LRESULT MainView::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_TIMER)
		return OnTimer(uMsg, wParam, lParam);
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
		video->play("rtmp://192.168.8.237:1935/live/slive");
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

	}
}