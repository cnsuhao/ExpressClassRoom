#include <windows.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <process.h>
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
int main()
{
	
	P p;
	Q q;
	p.doU();
	test_thread th;
	th.setWork(&p);
	th.run();

	th.setWork(&q);
	th.run();

	getchar();
	return EXIT_SUCCESS;
}