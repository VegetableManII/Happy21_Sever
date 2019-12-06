#ifndef THREADHANDLE_H
#define THREADHANDLE_H

#include <QThread>
#include <QHash>

class ThreadHandle
{
public:
    ~ThreadHandle();//

    enum ThreadType
    {
        THREADSIZE,  //how many thread
        HANDLESIZE   //how many connection in each thread
    };

    static ThreadHandle & getClass();

    QThread * getThread();
    void initThreadType(ThreadType type = HANDLESIZE,unsigned int max = 1000);
    void removeThread(const QThread *thread);
    void clear();
protected:
    void initThreadSize();
    QThread * findThreadSize();
    QThread * findHandleSize();

private:
    explicit ThreadHandle();

    ThreadType type;
    unsigned int size;
    unsigned int threadNumber;
    QHash<QThread *,unsigned int> threadSize;
    bool initfirst;
};

#endif // THREADHANDLE_H
