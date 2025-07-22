#include "LoginDialog.h"
#include "ui_LoginDialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    //循环 使下拉菜单显示人数
    for(int i = 30;i<=50;i++)
    {
        ui->comboBox->addItem(QString::number(i));
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

//页面跳转
void LoginDialog::on_pushButton_clicked()
{
    //1.获取到考试名称
    QString Course = ui->lineEdit->text();
    //2.判空
    if(Course.isEmpty())
    {
        QMessageBox::information(this,"","请输入考试科目");
        return;
    }
    //3.获取下拉菜单显示的考试人数
    QString num = ui->comboBox->currentText();
    //4.创建MainWindow的窗口的同时 把考试科目和人数发送过去
    MainWindow *mw = new MainWindow(Course,num.toInt());
    //5.显示MainWindow
    mw->show();
    //6.关闭现在的窗口
    this->close();
}
