#include "UserDao.h"
//静态指针在类外进行初始化
UserDao* UserDao::userdao = nullptr;

UserDao::UserDao()
{
    //1.加载数据库驱动
    db = QSqlDatabase::addDatabase("QSQLITE");
    //2.设置数据库的名称
    db.setDatabaseName("face.db");
    //3.打开数据库
    db.open();
    //4.创建表
    createTable();
}


UserDao::UserDao(const UserDao &)
{

}
//创建表
void UserDao::createTable()
{
    /*
         * 表名： user_info
         * 字段：id 整型（标签值）主键
         *      name 字符串
         *      start 数据库时间类型 datetime
         *      end 数据库时间类型 datetime
        */
    QString sql = "create table if not exists user_info("
                      "id integer primary key autoincrement,"
                      "name varchar(20),"
                      "start datetime,"
                      "end datetime)";
    QSqlQuery query;
    bool ok = query.exec(sql);
    if(ok)
        qDebug()<<"succ";
    else
        qDebug()<<"fail";
}

UserDao *UserDao::getUser()
{
    if(userdao==nullptr)
        userdao = new UserDao;
    return userdao;
}

UserDao::~UserDao()
{
    //关闭数据库
    db.close();
}

//插入数据
void UserDao::insertUser(QString name)
{
    QString sql = "INSERT INTO user_info(name,start,end) "
                  "VALUES(?,datetime('now','localtime'),datetime('now','localtime'));";
    //1.执行数据库操作的对象
    QSqlQuery query;
    //2.要修改数据库语句 prepare
    query.prepare(sql);
    //3.绑定值   第一个？的位置替换成name
    query.bindValue(0,name);
    //4.执行数据库操作语句
    bool ok = query.exec();// 不传递参数时，会自动执行之前prepare预编译好的sql语句
    if(ok)
        qDebug()<<"insert succ";
    else
        qDebug()<<"insert fail";
}

//插入成功返回id的
int UserDao::getUserId()
{
    QString sql = "SELECT last_insert_rowid();";
    QSqlQuery query;
    query.exec(sql);

    int id = -1;
    if(query.next())//移动到下一条记录 如果存在下一条记录就返回true
    {
        id = query.value(0).toInt();
        qDebug()<<"id is "<<id;
    }
    return id;
}

QString UserDao::getName(int id)
{
    QString sql = "SELECT name FROM user_info WHERE id = ?;";
    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(0,id);
    query.exec();
    QString name;
    if(query.next())
    {
        name = query.value(0).toString();
    }
    return  name;
}

void UserDao::updateEndTime(int id)
{
    QString sql = "UPDATE user_info SET end = datetime('now','localtime') WHERE id = ?;";
    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(0,id);
    bool ok = query.exec();
    if(ok)
        qDebug()<<"update succ";
    else
        qDebug()<<"update fail";
}

//提取所有的数据
void UserDao::selectInfo(vector<UserData> &v)
{
    QString sql = "SELECT * FROM user_info;";
    QSqlQuery query;
    query.exec(sql);
    while(query.next())
    {
        //提取一行  id  name  start end
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QDateTime start = query.value(2).toDateTime();
        QDateTime end = query.value(3).toDateTime();

        UserData user;//结构体变量
        user.id = id;
        user.name = name;
        user.start = start;
        user.end = end;
        //保存到容器当中
        v.push_back(user);
    }
}
