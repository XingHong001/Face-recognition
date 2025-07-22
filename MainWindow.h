#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QDebug>
#include<opencv2/opencv.hpp>
using namespace cv;
#include<opencv2/face.hpp>
using namespace cv::face;
#include<vector>
using namespace std;
#include<QTimerEvent>
#include<QMessageBox>
#include<QFile>
#include"UserDao.h"
#include<map>
#include<ctime>
#include<cstdlib>
#include"SpeakHelper.h"
#include"excelhelper.h"
#include<QFileDialog>

struct PointAndName
{
    int m_x;//行
    int m_y;//列
    QString name;//姓名
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString c,int n, QWidget *parent = nullptr);
    ~MainWindow();
    //初始化界面 显示考试名称 初始化网格布局
    void init();
    //定时器事件处理函数
    void timerEvent(QTimerEvent *);
    //查找人脸
    void findFace();
    //label显示人脸
    void showImg();
    //bool 判断是否要生成新的坐标
    bool changeLabelImg(Mat face,PointAndName &v,int flag);

private slots:
    void on_pushButton_choose_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_export_clicked();

private:
    Ui::MainWindow *ui;
    QString course;//考试名称
    int num;//参考人数
    VideoCapture vc;//视频类对象
    Mat src;
    CascadeClassifier classifier;//级联分类器对象
    vector<Rect> find_faces;//保存查找到人脸的矩形
    UserDao *userDao;//创建数据库类的指针
    //人脸识别器对象
    Ptr<FaceRecognizer> recognizer;
    map<int,PointAndName> map_users;
    SpeakHelper helper;//创建语音类对象
    ExcelHelper ex_helper;//表格操作类对象
};
#endif // MAINWINDOW_H
