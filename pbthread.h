#ifndef _PBTHREAD_H
#define _PBTHREAD_H

#include <windows.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

//runnable

class Runnable
{
	//�߳�ִ�л���
public:
	Runnable(){};
	
	virtual ~Runnable(){};

//protected:
	virtual void Run() = 0;

};

class PBThread : public Runnable
{
private:
	explicit PBThread(const PBThread& prt);
public:
	PBThread(void);
	PBThread(Runnable* prun);
	PBThread(const char* threadname, Runnable* prun = NULL);
	PBThread(unsigned int thread_id, const char* threadname, Runnable* prun = NULL);
	PBThread(std::string threadname, unsigned int thread_id, Runnable* prun = NULL);

	~PBThread(void);

	//void Start();
	bool Start(bool m_issuspend = false);
	//�߳�����

	virtual void Run();
	//�߳��ͷ���Դ������ȴ�����
	void Join(int timeout = -1);

	//�̴߳ӹ����ù���������̬��RB_Tree��
	void Resume();

	//�̹߳���
	void Suspend();

	bool Terminate(unsigned long ExitCode);

	std::string GetThreadName();

	unsigned int GetThreadID();

	void SetThreadname(const char* name);
	void SetThreadID(unsigned int _id);

private:
	//�ص�
	static unsigned int WINAPI StaticThreadFunc(void* prt);
private:
	//�߳̾��
	HANDLE m_handle = NULL;
	//
	Runnable* const m_prun;
	//id
	unsigned int m_threadid;
	//name
	std::string  m_threadname;
	//run or not
	volatile bool m_brun;
	//volatile bool m_issuspend;
};
#endif 