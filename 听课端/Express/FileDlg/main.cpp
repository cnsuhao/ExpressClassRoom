#include <windows.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <process.h>
#include <string>

using namespace std;

void __cdecl proc(void* param);


class IWork
{
public:
	virtual void exe() = 0;
};

class test_thread
{
public:
	test_thread() :m_work(NULL)
	{

	}
	void run()
	{
		_beginthread(proc, NULL, this);
	}
	void setWork(IWork* work)
	{
		m_work = work;
	}
	void do_work()
	{
		if (m_work)
			m_work->exe();
	}
private:
	IWork *m_work;
	
};

void __cdecl proc(void* param)
{
	test_thread*th = (test_thread*)(param);
	th->do_work();
}

class P : public IWork
{
public:
	void exe()
	{
		printf("P::exe===>\n");
	}
	void doU()
	{
		th.setWork(this);
		th.run();
	}
private:
	test_thread th;

};

class Q : public IWork
{
public:
	void exe()
	{
		printf("Q::exe===>\n");
	}
};

#include <map>


map<std::string,std::string> paraseCommand(const char* cmd)
{
	map<std::string, std::string> res_map;
	string scmd = string(cmd);
	int pos = -1;
	while ((pos = scmd.find_first_of('&'))!=-1)
	{
		string percmd = scmd.substr(0, pos);
		string key = percmd.substr(0, percmd.find_first_of('='));
		string values = percmd.substr(percmd.find_first_of('=') + 1, percmd.length());
		res_map[key] = values;
		scmd.erase(0, pos+1);
	}
	return res_map;
}

static wstring ANSIToUnicode(const string str)
{
	int  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar(CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		0);
	wchar_t *  pUnicode;
	pUnicode = new  wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		unicodeLen);
	wstring  rt;
	rt = (wchar_t*)pUnicode;
	delete  pUnicode;

	return  rt;
};

wstring FromAnsiToUnicode(const string AnsiStr)
{
	int length = AnsiStr.length();
	int unicodeLen = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, AnsiStr.c_str(), -1, NULL, 0);
	wchar_t *punicode;
	punicode = new wchar_t[unicodeLen + 1];
	memset(punicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, AnsiStr.c_str(), -1, (LPWSTR)punicode, unicodeLen);
	wstring unicodeStr;
	unicodeStr = (wchar_t*)punicode;
	delete[] punicode;
	return unicodeStr;
}



int main()
{
	

	string a = "ÄãºÃ°¡";
	cout << a << endl;
	std::locale l = wcout.getloc();
	wcout << FromAnsiToUnicode(a) << endl;
	getchar();
	return EXIT_SUCCESS;
}