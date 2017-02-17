#pragma once

/*
* =====================================================================================
*
*					^    ^
*					------
*					(0  0)\________________
*					(____)                 )\/\
*						  \     Thread     |
*						   ||-------------W|
*						   ||	    	  ||
*
*         Author:  Yoohoo	wand_niu@live.com
*
* =====================================================================================
*/

#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>  
#include <unistd.h>  
#endif
#include <iostream>
#include <time.h>


class IWork
{
public:
	virtual void runProc() = 0;
};

class CrossThread
{
public:
	CrossThread();
	~CrossThread();
	void run();
	void join();
	void setWork(IWork* _wk);
	static void sleep(unsigned int msecond);
	void work();
private:
	IWork* cross_work;
#ifdef WIN32
	HANDLE	thread_instance;
#else
	pthread_t thread_instance;
#endif 
};

class CrossMutex
{
#ifdef WIN32
	typedef CRITICAL_SECTION _type_mutex;
#else
	typedef pthread_mutex_t _type_mutex;
#endif 

public:
	typedef _type_mutex type_mutex;
	CrossMutex(CrossMutex::type_mutex _mutex);
	void lock();
	void unlock();
private:
	CrossMutex::type_mutex m_mutex;

};