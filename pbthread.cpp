
#include "pbthread.h"

//���캯��
PBThread::PBThread(void) :m_prun(NULL), m_brun(false)
{

}

PBThread::PBThread(Runnable* p_run) : m_prun(p_run), m_brun(false),
m_threadname("")
{

}

PBThread::PBThread(const char* name, Runnable* p_run) : m_threadname(name),
m_prun(p_run), m_brun(false)
{

}

PBThread::PBThread(unsigned int id, const char* name, Runnable* p_run) :
m_prun(p_run), m_threadid(id), m_threadname(name), m_brun(false)
{

}

PBThread::PBThread(std::string name, unsigned int id, Runnable* p_run) :
m_prun(p_run), m_threadid(id), m_threadname(name), m_brun(false)
{

}

//��������
PBThread::~PBThread(void)
{

}

//start

void PBThread::Run()
{
	//�߳�������̬
	if (!m_brun)
	{
		return;
	}
	//����Ϊ��
	if (NULL != m_prun)
	{
		m_prun->Run();
	}
	//��������
	m_brun = false;
}

//enter the waiting Queue
void PBThread::Join(int timeout)
{
	//
	//�߳��ڵȴ����л���û�ڽ����д���
	if ((NULL == m_handle) || (!m_brun))
	{
		return;
	}
	if (timeout <= 0)
	{
		timeout = INFINITE;
	}
	//�ȴ�����
	::WaitForSingleObject(m_handle,timeout);
}

//start the Thread
bool PBThread::Start(bool issuspend)
{
	//run or waiting
	//creat the Thread
	//�Ѿ�������
	if (m_brun)
	{
		return true;
	}
	//�类����
	//waiting queue�ȴ�����
	if (issuspend)
	{
		//param:��ȫ���ã�NULL��,��ջ��С���ص�ָ�룬ʵ��ָ�룬��ʶ��idָ��
		m_handle = (HANDLE)_beginthreadex(NULL, 0, StaticThreadFunc, this, CREATE_SUSPENDED, &m_threadid);
	}
	else
	//�̱߳�����
	{
		m_handle = (HANDLE)_beginthreadex(NULL, 0, StaticThreadFunc, this, 0, &m_threadid);
	}
	//run
	m_brun = (NULL != m_handle);
	return m_brun;
}

//���������лص�����̬
void PBThread::Resume()
{
	//���������
	if ((m_brun) || (NULL == m_handle))
	{
		return;
	}
	::ResumeThread(m_handle);
}

//suspend ����
void PBThread::Suspend()
{
	if ((!m_brun) || (NULL == m_handle))
	{
		return;
	}
	::SuspendThread(m_handle);
	//suspend,join destroy:m_brun = false
	m_brun = false;
}

//��ֹ�߳�
bool PBThread::Terminate(unsigned long ExitCode)
{
	//only for the running Thread
	if ((!m_brun) || (NULL == m_handle))
	{
		return true;
	}
	//�ر��߳�
	if (::TerminateThread(m_handle, ExitCode))
	{
		//�ر��ڽ�����ע��ľ��
		::CloseHandle(m_handle);
		return true;
	}
	std::cout << "failed to close the Thread" << "\n\n" << std::endl;
	Sleep(1000);
	return false;
}

unsigned int PBThread::GetThreadID()
{
	return m_threadid;
}

std::string PBThread::GetThreadName()
{
	if ("" == m_threadname)
	{
		return "no name";
	}
	else
	{
		return m_threadname;
	}
}

void PBThread::SetThreadname(const char* name)
{
	if (NULL == name)
	{
		m_threadname = "";
	}
	else
	{
		m_threadname = name;
	}
}

void PBThread::SetThreadID(unsigned int id)
{
	m_threadid = id;
}

unsigned int PBThread::StaticThreadFunc(void* prt)
{
	//�߳�ָ��
	PBThread* pthread = (PBThread*)prt;
	pthread->Run();
	return 0;
}






