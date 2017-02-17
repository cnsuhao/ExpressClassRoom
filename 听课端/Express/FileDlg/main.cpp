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


int main()
{
	
	map<std::string, std::string> res_map = paraseCommand("a=b&c=&d=E&f");

	getchar();
	return EXIT_SUCCESS;
}