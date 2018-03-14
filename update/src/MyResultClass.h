#ifndef _MYRESULTCLASS__H_
#define _MYRESULTCLASS__H_
#include <string>

class MyResultClass
{
    public:
        MyResultClass();
        ~MyResultClass();

    public:
        void MyRequestResultCallback(int id, bool success, const std::string& data);
        bool IsRequestFinish(void);
    private:
        bool m_request_finished;

};
#endif // _MYRESULTCLASS__H_
