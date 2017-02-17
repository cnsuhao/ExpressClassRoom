#ifndef __EXPRESS_HEARD__
#define __EXPRESS_HEARD__

#include <stdio.h>
#include <WinSock2.h>
#include <windows.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <mmsystem.h>
#pragma comment(lib, "WINMM.LIB")
#include "../Duilib/UIlib.h"
#ifdef _DEBUG
#pragma  comment(lib,"../lib/DuiLib_d.lib")
#else
#pragma  comment(lib,"../lib/DuiLib.lib")
#endif
using namespace DuiLib;
using namespace std;
#endif

extern  std::string  login_token;
extern  std::string  login_ip;
extern  std::string  login_cgi;
extern  std::string  admin_user;
extern  std::string  admin_passwd;
#define  CLOUD_IP_FILE	"express.conf"
#define  DB_FILE		"user.db"