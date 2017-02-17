#include "CrossThread.h"
#include <stdio.h>
#include <math.h>
CrossThread::CrossThread() :cross_work(NULL)
{
}

CrossThread::~CrossThread()
{

}

void CrossThread::setWork(IWork* _wk)
{
	if (cross_work)
	{
		delete cross_work;
		cross_work = NULL;
	}
	cross_work = _wk;
}

void CrossThread::work()
{
	if (cross_work)
	{
		cross_work->runProc();
	}
}

/* 
	windows platform
*/
#ifdef WIN32 

static unsigned int __stdcall proc(void* param)
{
	CrossThread* th = (CrossThread*)param;
	th->work();
	return 0;
}
void CrossThread::run()
{
	unsigned int thrdaddr;
	thread_instance = (HANDLE)_beginthreadex(NULL, 0, proc, this, 0, &thrdaddr);
	if (!thread_instance)
	{
		CloseHandle(thread_instance);
		thread_instance = NULL;
		throw ("create thread error!");
	}
}

void CrossThread::join()
{
	WaitForSingleObject(thread_instance, INFINITE);
	CloseHandle(thread_instance);
	thread_instance = NULL;
}

void CrossThread::sleep(unsigned int msecond)
{
	::Sleep(msecond);
}

/*
	like unix platform
*/
#else

static void* proc(void* param)
{
	CrossThread* th = (CrossThread*)param;
	th->work();
	return NULL;
}

void CrossThread::run()
{
	if (pthread_create(&thread_instance, NULL, proc, this) != 0)
	{
		throw ("create thread error");
	}
}

void CrossThread::join()
{
	pthread_join(thread_instance, NULL);
	thread_instance = NULL;
}

void CrossThread::sleep(unsigned int msecond)
{
	timespec ts;
	ts.tv_sec = msecond / 1000;
	ts.tv_nsec = (msecond % 1000) * 1000000;
	nanosleep(&ts, NULL);
}

#endif 

CrossMutex::CrossMutex(CrossMutex::type_mutex _mutex)
{
	m_mutex = _mutex;
#ifdef WIN32
	InitializeCriticalSection(&m_mutex);
#else
	pthread_mutex_init(&m_mutex, NULL);
#endif 

}

void CrossMutex::lock()
{
#ifdef WIN32
	EnterCriticalSection(&m_mutex);
#else
	pthread_mutex_lock(&m_mutex);
#endif 
}

void CrossMutex::unlock()
{
#ifdef WIN32
	LeaveCriticalSection(&m_mutex);
	DeleteCriticalSection(&m_mutex);
#else
	pthread_mutex_unlock(&m_mutex);
#endif 
}
