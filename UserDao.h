#ifndef USERDAO_H
#define USERDAO_H
#include<QSqlDatabase>//数据库
#include<QSqlQuery>//执行数据库操作语句
#include<QDebug>
#include<QDateTime>//表示时间的
#include<vector>
using namespace std;

struct UserData
{
    int id;
    QString name;
    QDateTime start;
    QDateTime end;
};

class UserDao
{
    //1.私有化构造函数和拷贝构造
    UserDao();
    UserDao(const UserDao&);
    //2.私有静态类指针
    static UserDao* userdao;
    //创建数据库类的对象
    QSqlDatabase db;
    //创建表
    void createTable();
public:
    //3.公有成员函数 实例化类指针
    static UserDao* getUser();
    ~UserDao();
    //插入数据 学生姓名
    void insertUser(QString name);
    //返回id
    int getUserId();
    //根据id获取姓名
    QString getName(int id);
    //更新考试结束的时间
    void updateEndTime(int id);
    //提取所有的数据
    void selectInfo(vector<UserData> &v);
};

#endif // USERDAO_H
