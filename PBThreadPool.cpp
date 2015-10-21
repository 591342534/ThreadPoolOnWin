
#include "PBThreadPool.h"



//��������
ThreadPoolExcute::CTWorker::~CTWorker()
{

}

//���캯��
//ThreadPoolExcute::CTWorker::CTWorker(Runnable* p_run, int _num)
//{
//	work_prun = p_run;
//	t_workernum = _num;
//}

ThreadPoolExcute::CTWorker::CTWorker(Runnable* p_run, ThreadPoolExcute* pool) :
w_pool_prt(pool), work_prun(p_run), enabledo(true)
{

}


void ThreadPoolExcute::CTWorker::Run()
{
	//�̴߳�������̬
	//to work ,to handle the task in the tasklist
	Runnable* mytask = NULL;
	if (NULL == w_pool_prt)
	{
		std::cout << "pool init error!" << "\n\n" << std::endl;
	}
	//����ѭ��
	while (enabledo)
	{
		//free
		if (NULL == work_prun)
		{
			//�����������з���һ������
			mytask = w_pool_prt->getTask();
		}
		else
		{
			mytask = work_prun;
		}
		//���������
		if (NULL == mytask)
		{
			EnterCriticalSection(&(w_pool_prt->t_csThreadPool));
			std::cout << "error to excute my task!!" << "\n\n" << std::endl;
			//to do
			//tasklist is empty
			if (w_pool_prt->GetPoolSize() > w_pool_prt->min_thread_num)
			{
				//�����߳̽���ȴ�����
				//������ݹ�,����������
				//iterator
				_Threaditer iter = w_pool_prt->t_threadpool.find(this);
				//����,destroy
				if (iter != w_pool_prt->t_threadpool.end())
				{
					//RB Tree �ع�
					w_pool_prt->t_threadpool.erase(iter);
					//�������մ�worker
					w_pool_prt->t_trashthread.insert(this);
					//enabledo = false;
				}
				enabledo = false;
			}
			//����������
			else
			{
				_Threaditer itr = w_pool_prt->t_trashthread.begin();
				while (itr!=w_pool_prt->t_trashthread.end())
				{
					//�߳��ͷ���Դ������������Queue
					(*itr)->Join();
					delete (*itr);
					//�������ݹ��ع�RB Tree
					w_pool_prt->t_trashthread.erase(itr);
					//����ʵ�ֱ���
					itr = w_pool_prt->t_trashthread.begin();
				}
			}
			//����ѭ��
			LeaveCriticalSection(&(w_pool_prt->t_csThreadPool));
			continue;
		}
		//LeaveCriticalSection(&(w_pool_prt->t_csThreadPool));
		else
		{
			//ִ������
			mytask->Run();
			//ָ������
			mytask = NULL;
		}
		
	}

}

ThreadPoolExcute::ThreadPoolExcute() : m_brun(true), enableInsertTask(true)
{
	InitializeCriticalSection(&t_csThreadPool);
	InitializeCriticalSection(&t_csTasklist);
}

//ThreadPoolExcute::ThreadPoolExcute(unsigned int max,unsigned int min)
//{
//	if ((max <= 0) || (min <= 0) || (max < min))
//	{
//		std::cout << "error" << "\n\n" << std::endl;
//	}
//
//	max_thread_num = max;
//	min_thread_num = min;
//}


ThreadPoolExcute::ThreadPoolExcute(Runnable* _task) :type_task(_task), m_brun(true), enableInsertTask(true)
{
	InitializeCriticalSection(&t_csThreadPool);
	InitializeCriticalSection(&t_csTasklist);
}

//��������
ThreadPoolExcute::~ThreadPoolExcute()
{
	Terminate();
	DeleteCriticalSection(&t_csTasklist);
	DeleteCriticalSection(&t_csThreadPool);
}


//�ս��̳߳�
void ThreadPoolExcute::Terminate()
{
	m_brun = false;
	enableInsertTask = false;
	min_thread_num = 0;
	max_thread_num = 0;
	_t_pending_num = 0;
	EnterCriticalSection(&t_csThreadPool);
	//����������
	_Threaditer iter = t_trashthread.begin();
	while (iter!=t_trashthread.end())
	{
		(*iter)->Join();
		//free the memory
		delete (*iter);
		//reconstruct
		t_trashthread.erase(iter);
		iter = t_trashthread.begin();
	}
	LeaveCriticalSection(&t_csThreadPool);
}

bool ThreadPoolExcute::initThreadPool(unsigned int max, unsigned int min)
{
	if ((max <= 0) || (min <= 0) || (max < min))
	{
		return false;
	}

	max_thread_num = max;
	min_thread_num = min;

	////�̳߳�û��������
	//if (NULL==t_task)
	//{
	//	std::cout << "no task for the pool" << "\n\n" << std::endl;
	//	return false;
	//}
	//������
	int curr = t_threadpool.size();
	if (curr<=min_thread_num)
	{
		int t_extra = min_thread_num - curr;
		for (int i = 0; i < t_extra; i++)
		{
			//Ԥ�����߳�
			CTWorker* p_worker = new CTWorker(NULL,this);
			_t_pending_num++;
			if (NULL == p_worker)
			{
				std::cout << "failed to allocate the memory" << "\n\n" << std::endl;
				return false;
			}
			EnterCriticalSection(&t_csThreadPool);
			//�����̵߳Ĵ���Σ������̳߳ع�����Դ
			t_threadpool.insert(p_worker);
			//worker start to work
			p_worker->Start();
			//�˳������ٽ���
			LeaveCriticalSection(&t_csTasklist);
		}
	}
	//���Բ���������
	enableInsertTask = true;
	m_brun = true;
	return true;
}

unsigned int ThreadPoolExcute::GetPoolSize()
{
	return t_threadpool.size();
}

Runnable* ThreadPoolExcute::getTask()
{
	if (t_task.empty())
	{
		std::cout << "no tesk in list" << "\n\n" << std::endl;
		return 0;
	}
	Runnable* p_task = NULL;
	EnterCriticalSection(&t_csThreadPool);
	p_task = t_task.front();
	t_task.pop_front();
	LeaveCriticalSection(&t_csThreadPool);
	return p_task;
}

bool ThreadPoolExcute::Execute(Runnable* p_runwork)
{
	if (!enableInsertTask)
	{
		std::cout << "can't insert the task\n\n" << std::endl;
		return false;
	}
	if (NULL == p_runwork)
	{
		std::cout << "error task\n\n" << std::endl;
		return false;
	}
	//�̲߳���
	if (t_threadpool.empty())
	{
		std::cout << "craeting the worker...\n\n" << std::endl;
		Sleep(1000);
		initThreadPool();
	}
	//no thread can be used
	if (t_task.size()>=_t_pending_num)
	{
		if (t_threadpool.size() < max_thread_num)
		{
			CTWorker* ex_worker = new CTWorker(p_runwork,this);
			if (NULL == ex_worker)
			{
				std::cout << "failed to allocate the memory" << "\n\n" << std::endl;
				Sleep(1000);
				return false;
			}
			//insert to the RB Tree
			//data protection
			EnterCriticalSection(&t_csThreadPool);
			t_threadpool.insert(ex_worker);
			LeaveCriticalSection(&t_csThreadPool);
			//start the thread
			ex_worker->Start();
		}
		else
		{
			std::cout << "no more thread in pool" << "\n\n" << std::endl;
			return false;
		}
	}
	//enough
	//main Thread
	EnterCriticalSection(&t_csTasklist);
	t_task.push_back(p_runwork);
	LeaveCriticalSection(&t_csTasklist);
	return true;
}