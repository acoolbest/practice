/****************************************************************************** 
Copyright by Javacode007, All rights reserved! 
Filename    : testsqlite3.c 
Author      : Javacode007 
Date        : 2012-8-11 
Version     : 1.0 
Description : SQLite3 基本功能性能测试 
******************************************************************************/  
/*
数据库事务性
*/
#include "sqlite3.h"  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <sys/time.h>  
#include <time.h>  
  
#define PRINT_TRACE(fmt, args...) do { \
    fprintf(stderr, fmt" File:%s, Line:%04d\r\n",##args, __FILE__, __LINE__);\
    \
}while(0)      
  
  
#ifdef TRANSACTION_ON  
#define START_TRANSACTION() sqlite3_exec(db, "begin transaction;", NULL, NULL, NULL)  
#define END_TRANSACTION()   sqlite3_exec(db, "commit transaction;", NULL, NULL, NULL)  
#else  
#define START_TRANSACTION()   
#define END_TRANSACTION()     
#endif  
  
  
  
//性别枚举类型  
typedef enum   
{  
    UNKNOWN,  
    MALE,  
    FEMALE,  
}SEX_E;  
  
//SQL 操作枚举类型  
typedef enum  
{  
    INSERT,  
    SELECT,  
    UPDATE,  
    DELETE  
}SQL_OPTYPE;  
  
typedef struct stEmployee  
{  
    unsigned int id;  
    unsigned int age;  
    SEX_E        sex;  
    char         registertime[26];  
    char         cellphone[12];  
    char         email[128];  
    char         resume[512];  
    float        salary;  
}Employee_S;  
  
//全局雇员ID  
static unsigned int g_employeeid;  
  
long diff_timeval(struct timeval tv1, struct timeval tv2)  
{  
    long timecost = 0;  
  
    timecost = 1000000*(tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec);  
  
    return timecost;  
}  
  
void init_employee(Employee_S* employee)  
{  
    time_t ttime;  
      
    if(NULL == employee)  
    {  
        PRINT_TRACE("Invalid parameter: NULL pointer!");  
        return ;  
    }  
      
    memset(employee, 0, sizeof(*employee));  
    g_employeeid ++;  
  
    employee->id  = g_employeeid;  
    employee->age = 0;  
    employee->sex = UNKNOWN;  
  
    time(&ttime);  
    sprintf(employee->registertime, "%s", ctime(&ttime));  
    sprintf(employee->cellphone, "%0*d", sizeof(employee->cellphone)-1, 0);
    sprintf(employee->email, "persion_%06d@sun.org", g_employeeid); 
    sprintf(employee->resume, "Resume:");  
      
    employee->salary = 12345.78;  
      
      
}  
  
void print_employee(Employee_S employee)  
{  
    printf("id=%d, age=%d, sex=%d, regtime=%s", employee.id, employee.age, employee.sex, employee.registertime);  
    printf("cellphone=%s, email=%s, resume=%s, salary=%6.2f\r\n",  employee.cellphone, employee.email, employee.resume, employee.salary);  
}  
  
  
sqlite3* open_db(const char* dbname)  
{  
    int ret = -1;  
    sqlite3 *db = NULL;  
  
    if(NULL == dbname)  
    {  
        PRINT_TRACE("Invalid parameter: null database name!");  
        return NULL;  
    }  
  
    ret = sqlite3_open(dbname, &db);  
    if(SQLITE_OK != ret)  
    {  
        PRINT_TRACE("Open database \"%s\" failed: %s.", dbname, sqlite3_errmsg(db));  
        sqlite3_close(db);  
        return NULL;  
    }  
  
    return db;  
}  
  
int close_db(sqlite3* db)  
{  
    int ret = -1;  
  
    if(NULL != db)  
    {  
        ret = sqlite3_close(db);  
    }  
  
    return ret;  
}  
  
int create_table(sqlite3* db, const char* tablename)  
{  
    int ret = -1;  
    int cmdlen = 0;  
      
    char *errmsg ;  
    char *sqlfmt = "CREATE TABLE %s (id INTEGER PRIMARY KEY, age INTEGER, sex "
                    "INTEGER, registertime VARCHAR(26), cellphone VARCHAR(12),"
                    "email TEXT, resume TEXT, salary REAL);";
    char *sqlcmd = NULL;  
      
  
    if(NULL == db || NULL == tablename)  
    {  
        PRINT_TRACE("Invalid parameter: &db=%p, &tablename=%p.", db, tablename);  
        return ret;  
    }  
  
    //必须用 strlen 获取字符长度，不能用 sizeof，sizeof 获取的是指针长度，为4.  
    cmdlen = strlen(sqlfmt) + strlen(tablename) + 1;  
    sqlcmd = (char*)malloc(cmdlen);  
  
    if(NULL == sqlcmd)  
    {  
        PRINT_TRACE("Not enough memory for sql command!");  
        return ret;  
    }  
      
    memset(sqlcmd, 0, cmdlen);  
    sprintf(sqlcmd, sqlfmt, tablename);  
  
    ret = sqlite3_exec(db, sqlcmd, NULL, NULL, &errmsg);  
    if(SQLITE_OK != ret)  
    {  
        PRINT_TRACE("Create table \"%s\" failed: %s.", tablename, errmsg);  
        sqlite3_free(errmsg);  
        free(sqlcmd);  
        return ret;  
    }  
  
    free(sqlcmd);  
  
    return ret;  
}  
  
char* get_sqlcmd(const char* tablename, SQL_OPTYPE optype, Employee_S employee)  
{  
    char* insertfmt = "INSERT INTO %s values(%d, %d, %d, '%s', '%s', '%s', '%s', %10.6f);";  
    char* selectfmt = "SELECT * FROM %s where id <= %d;";  
    char* updatefmt = "UPDATE %s set age=%d, sex=%d, registertime='%s',cellphone='%s',"\
                      "email='%s', resume='%s', salary=%10.6f where id=%d;";  
    char* deletefmt = "DELETE FROM %s where id=%d;";  
    char* sqlcmd = NULL;  
    int   cmdlen = 0;
  
    if(NULL == tablename)  
    {
        PRINT_TRACE("Invalid parameter: NULL pointer of tablename!");  
        return NULL;  
    }  
  
    //为了简化，给 sql 语句申请最大空间  
    cmdlen = strlen(updatefmt) + strlen(tablename) + sizeof(employee);  
    sqlcmd = (char*) malloc(cmdlen);  
    if(NULL == sqlcmd)
    {
        PRINT_TRACE("Not enough memory for sql command!"); 
        return NULL;  
    }  
      
    switch(optype)  
    {  
        case INSERT:  
            sprintf(sqlcmd, insertfmt, tablename, employee.id, employee.age, \
                employee.sex, employee.registertime, employee.cellphone, \
                employee.email, employee.resume, employee.salary);
            break;  
              
        case SELECT:  
            sprintf(sqlcmd, selectfmt, tablename, employee.id);  
            break;  
  
        case UPDATE:  
            sprintf(sqlcmd, updatefmt, tablename, employee.age, employee.sex, \
                employee.registertime, employee.cellphone, employee.email, \
                employee.resume, employee.salary, employee.id);  
            break;  
  
        case DELETE:  
            sprintf(sqlcmd, deletefmt, tablename, employee.id);
            break;  
  
        default:  
            PRINT_TRACE("Unknown operation type:%d\r\n", optype);  
            free(sqlcmd);  
            return NULL;  
     }  
  
    return sqlcmd;  
      
      
}  
  
int test(const char* dbname, const char* tablename, SQL_OPTYPE optype, int count, int istableexists)  
{  
    int  ret = -1;  
    int  i = 0;  
    int  failcount = 0;  
    long costtime = 0;  
  
    time_t  ttime;  
    struct timeval tvStart;  
    struct timeval tvEnd;  
      
    sqlite3*   db = NULL;  
    Employee_S employee;  
    char*      sqlcmd = NULL;  
    char*      errmsg = NULL;  
  
    time(&ttime);  
    printf("\r\nStart \"%s\" at: %s", __FUNCTION__, ctime(&ttime));  
      
    if(NULL == dbname || NULL == tablename || 0 >= count)  
    {  
        PRINT_TRACE("Invalid Parameter:dbname=%p, tablename=%p, count=%d.", dbname, tablename, count);  
        return ret;  
    }  
  
    //打开数据库  
    db = open_db(dbname);  
    if(NULL == db)  
    {  
        return ret;  
    }  
  
    //判断是否需要创建表  
    if( 0 == istableexists)  
    {  
        ret = create_table(db, tablename);  
        if(SQLITE_OK != ret)  
        {  
            close_db(db);  
            return ret;  
        }  
    }  
  
    //开启事务模式  
    START_TRANSACTION();  
  
    for(i = 0; i < count; i++)  
    {  
        init_employee(&employee);  
          
        sqlcmd = get_sqlcmd(tablename, optype, employee); 
          
        if(NULL == sqlcmd)  
        {  
            failcount ++;  
            continue;  
        }  
  
        //开始计时  
        gettimeofday(&tvStart, NULL);  
          
        ret = sqlite3_exec(db, sqlcmd, NULL, NULL, &errmsg);  
  
        //结束计时  
        gettimeofday(&tvEnd, NULL);  
  
        costtime += diff_timeval(tvStart, tvEnd);  
  
        if(SQLITE_OK != ret)  
        {  
            failcount ++;  
            PRINT_TRACE("Excecute sql: %s failed! Error info:%s.", sqlcmd, errmsg);  
        }  
          
        if(NULL != sqlcmd)  
        {  
            free(sqlcmd);  
        }  
              
        if(NULL != errmsg)  
        {  
            sqlite3_free(errmsg);  
        }  
          
    }  
  
    //关闭事务模式  
    END_TRANSACTION();  
  
    close_db(db);  
  
    //打印结果  
    printf("Operation Type: %d, Database name: %s,  Table name: %s. \r\n", optype, dbname, tablename);  
    printf("Counts\tTotalTime(us)\tAverageTime(us)\r\n");  
    printf("%-8d %-10ld    %-10.6f\r\n", count, costtime, (float)(costtime/count));  
  
    time(&ttime);  
    printf("Finish \"%s\" at: %s\r\n", __FUNCTION__, ctime(&ttime));  
  
    if(0 < failcount)  
    {  
        ret = -1;  
    }  
  
    return ret;  
      
}  
  
int main()  
{  
    int   count = 1000;  
    int   istableexists = 0;  
    int   ret = -1;  
      
    char* dbname = "employee.db";  
    char* tablename = "employee";  
  
    SQL_OPTYPE optype;  
      
    optype = INSERT;  
    ret = test(dbname, tablename, optype, count, istableexists);  
    if(SQLITE_OK != ret)  
    {  
        PRINT_TRACE("Test failed!");  
        return 0;  
    }  
      
    istableexists = 1;  
    g_employeeid = 0;  
    optype = SELECT;  
    ret = test(dbname, tablename, optype, count, istableexists);  
      
    if(SQLITE_OK != ret)  
    {  
        PRINT_TRACE("Test failed!");  
        return 0;  
    }  
  
    g_employeeid = 0;  
    optype = UPDATE;  
  
    ret = test(dbname, tablename, optype, count, istableexists);  
    if(SQLITE_OK != ret)  
    {  
        PRINT_TRACE("Test failed!");  
        return 0;  
    }  
  
    g_employeeid = 0;  
    optype = DELETE;  
  
    ret = test(dbname, tablename, optype, count, istableexists);  
    if(SQLITE_OK != ret)  
    {  
        PRINT_TRACE("Test failed!");  
        return 0;  
    }  
  
    return 0;  
}  