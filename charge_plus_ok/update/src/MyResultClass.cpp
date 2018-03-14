#include "MyResultClass.h"
MyResultClass::MyResultClass() : m_request_finished(false) {  }
MyResultClass::~MyResultClass() {  }

void MyResultClass::MyRequestResultCallback(int id, bool success, const std::string& data)
{
    /*
    if (success)
    {
        std::ofstream outfile;
        outfile.open("baidu.html", std::ios_base::binary | std::ios_base::trunc);
        if (outfile.good()) outfile.write(data.c_str(), data.size());

    }
    */
    m_request_finished = true;

}
bool MyResultClass::IsRequestFinish(void) { return m_request_finished;   }
