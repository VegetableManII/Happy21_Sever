#include "threadhandle.h"
#include <QDebug>

#include "eventdispatcher_libev.h"

ThreadHandle::ThreadHandle()
{
    initfirst = false;
    this->threadNumber=1;
}

ThreadHandle::~ThreadHandle() //停止所有线程，并释放资源
{
    QThread * tmp;
    for (auto it = threadSize.begin(); it != threadSize.end(); ++it)
    {
        tmp = it.key();
        tmp->exit();
        tmp->wait(3000);
        delete tmp;
    }
}

ThreadHandle & ThreadHandle::getClass()
{
    static ThreadHandle th;
    return th;
}

QThread *ThreadHandle::getThread()
{
    if (!initfirst)
    {
        initThreadType(HANDLESIZE, 0);
    }
    if (type == THREADSIZE)
        return findThreadSize();
    else
        return findHandleSize();
}

void ThreadHandle::removeThread(const QThread * thread)
{
    QThread *tmp=const_cast<QThread *>(thread);
    auto t = threadSize.find(tmp);
    if (t != threadSize.end())
    {
        t.value() --;
        if (type == HANDLESIZE && t.value() == 0 && threadSize.size() > 1)
        {
            threadSize.remove(tmp);
            tmp->exit();
            tmp->wait(3000);
            delete thread;
            threadNumber--;
        }
    }
}

void ThreadHandle::initThreadType(ThreadType type, unsigned int max)
{
    if (!initfirst)
    {
        this->type = type;
        this->size = max;
        if(this->size == 0)
        {
            if(type == THREADSIZE)
                this->size = 30;
            else
                this->size = 7;
        }

        if (type == THREADSIZE)
            initThreadSize();
        else
        {
            QThread * tmp = new QThread;
#ifndef Q_OS_WIN
            tmp->setEventDispatcher(new EventDispatcherLibEv());
#endif
            threadSize.insert(tmp,0);
            tmp->start();
        }
    }
    initfirst = true;
}

void ThreadHandle::initThreadSize() //建立好线程并启动，
{
    QThread * tmp;
    for (unsigned int i = 0; i < size;++i)
    {
        tmp = new QThread;
#ifndef Q_OS_WIN
        tmp->setEventDispatcher(new EventDispatcherLibEv());
#endif
        threadSize.insert(tmp,1);
        tmp->start();
    }
}

QThread * ThreadHandle::findHandleSize() //查找 到线程里的连接数 小于最大值 就返回查找到的，找不到就新建一个线程
{
    for (auto it  = threadSize.begin();it != threadSize.end() ;++it)
    {
        if (it.value() < size)
        {
            it.value() ++;
            return it.key();
        }
    }
    QThread * tmp = new QThread;
#ifndef Q_OS_WIN
    tmp->setEventDispatcher(new EventDispatcherLibEv());
#endif
    threadSize.insert(tmp,threadNumber);
    tmp->start();
    threadNumber++;
    return tmp;
}

QThread *ThreadHandle::findThreadSize() //遍历查找所有线程  找出value值为0的线程 1==在线  0==离线
{
    auto it = threadSize.begin();
    auto ite = threadSize.begin();
    for (++it ; it != threadSize.end(); ++it)
    {
        if (it.value() < 1)
        {
            ite = it;
        }
    }
    ite.value() ++;
    return ite.key();
}

void ThreadHandle::clear()//仅仅清空计数，线程不释放
{
    for (auto it  = threadSize.begin();it != threadSize.end() ;++it)
    {
        it.value()  = 0;
    }
}
