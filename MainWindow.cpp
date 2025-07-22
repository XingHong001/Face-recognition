#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QString c,int n,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),course(c),num(n)
{
    ui->setupUi(this);
    qDebug()<<course<<" "<<num;
    init();
    //打开摄像头
    if(vc.open(0))
    {
        //开启定时器
        startTimer(50);
    }
    //配置数据模型
    classifier = CascadeClassifier("D:/Opencv/opencv3.4-install/install/etc/haarcascades/haarcascade_frontalface_alt2.xml");
    //调用getUser
    userDao = UserDao::getUser();
    QFile file("face.xml");
    if(file.exists())//存在
    {
        //加载
        recognizer = FaceRecognizer::load<LBPHFaceRecognizer>("face.xml");
    }
    else
    {
        //创建
        recognizer = LBPHFaceRecognizer::create();
    }
    //初始化随机种子
    srand(time(0));
}

MainWindow::~MainWindow()
{
    delete ui;
}
//初始化界面
void MainWindow::init()
{
    //1.显示考试名称
    ui->label->setText(course+"考试");
    //2.动态的向网格布局中添加label控件
    int row = 0;//代表行 不固定的  列是固定的6
    int index = 0;
    while(true)
    {
        for(int i=0;i<6;i++)//i代表列
        {
            if(index==num)
                return;
            //1.创建一个label
            QLabel *label = new QLabel(this);//传参this方便回收
            //2.显示图片
            QPixmap p("D:/Opencv/picture/avatar.png");
            //3.调整图片大小 60*60
            p = p.scaled(60,60);
            label->setPixmap(p);
            //设置所有的objectname为0
            label->setObjectName("0");
            //4.设置外圈线
            label->setFrameShape(QFrame::Box);
            //5.网格布局中添加label控件  控件    行   列  这个控件占几行几列
            ui->gridLayout->addWidget(label,  row, i, 1,1);
            index++;
        }
        row++;
    }
}

//定时器事件处理函数
void MainWindow::timerEvent(QTimerEvent *)
{
    //1.一帧一帧的提取图片
    vc>>src;
    //2.判空
    if(src.empty())
        return;
    //3.翻转
    flip(src,src,1);
    //4.查找人脸并画框  显示在label上
    findFace();
    showImg();
}

//查找人脸的函数
void MainWindow::findFace()
{
    Mat gray;
    cvtColor(src,gray,CV_BGR2GRAY);
    classifier.detectMultiScale(gray,find_faces);
    //判断是否查找到人脸
    if(find_faces.size())//找到人了 摄像头前有人
    {
        //画框
        rectangle(src,find_faces[0],Scalar(0,0,255),2);
    }
}
//显示在label上
void MainWindow::showImg()
{
    //1.转色彩空间
    Mat dst;
    cvtColor(src,dst,CV_BGR2RGB);
    //2.转QImage
    QImage img(dst.data,
               dst.cols,
               dst.rows,
               dst.cols*dst.channels(),
               QImage::Format_RGB888);
    //3.转QPixmap
    QPixmap p = QPixmap::fromImage(img);
    //4.调整大小 显示
    p = p.scaled(QSize(ui->label_show->size()));
    ui->label_show->setPixmap(p);
}

//改变label的图像  bool 代表要不要继续生成新坐标
//参数1：Mat  显示在label上的图像
//参数2：结构体
//参数3：flag == 1 选座   flag ==0  离开
bool MainWindow::changeLabelImg(Mat face, PointAndName &v, int flag)
{
    //1.在网格布局中 找到对应的label控件
    QLayoutItem *item = ui->gridLayout->itemAtPosition(v.m_x,v.m_y);
    QWidget *w = item->widget();
    //父子类指针的转换
    QLabel *label = dynamic_cast<QLabel*>(w);
    //2.判断flag  要走还是要留
    //走
    if(flag == 0)
    {
        label->setText(v.name+"结束考试");
        return false;
    }
    //留
    //如果是留 3. 判断当前位置可用不可用  不可用--> return true
    QString objectName = label->objectName();
    if(objectName=="1")
        return true;
    //可用  修改当前的objectName为1
    label->setObjectName("1");
    //4.label显示图像
    Mat rgb;
    cvtColor(face,rgb,CV_BGR2RGB);
    QImage img(rgb.data,
               rgb.cols,
               rgb.rows,
               rgb.cols*rgb.channels(),
               QImage::Format_RGB888);
    QPixmap p = QPixmap::fromImage(img);
    p = p.scaled(QSize(60,60));
    label->setPixmap(p);

    return false;
}

//选座
void MainWindow::on_pushButton_choose_clicked()
{
    //1.获取到考生姓名 判空 为空给弹窗提示
    QString name = ui->lineEdit_name->text();
    if(name.isEmpty())
    {
        QMessageBox::information(this,"","请输入姓名");
        return;
    }
    //2.获取到人脸 保存到容器中
    vector<Mat> studyFaces;
    Mat dst;
    if(find_faces.size()==0)
    {
        QMessageBox::information(this,"","未找到考试人员");
        return;
    }
    //截图
    dst = src(find_faces[0]);
    //创建一个副本
    Mat temp = dst.clone();
    cvtColor(dst,dst,CV_BGR2GRAY);
    cv::resize(dst,dst,Size(80,80));
    studyFaces.push_back(dst);
    //3.插入数据库中 插入成功返回一个id（label标签）
    userDao->insertUser(name);
    //获取id
    int id = userDao->getUserId();
    vector<int> studyLabels;//保存id到容器（给人脸识别器用）
    studyLabels.push_back(id);
    //4.人脸识别器更新 保存到xml中
    recognizer->update(studyFaces,studyLabels);
    recognizer->save("face.xml");
    //5.随机抽取一个座位，并显示人脸
    PointAndName value;//声明一个结构体变量
    do
    {
        int count = rand()%num;//0~29
        int row = count/6;//行
        int col = count%6;//列
        value.m_x = row;
        value.m_y = col;
        value.name = name;
    }while(changeLabelImg(temp,value,1));
    //添加语音播报
    helper.speak(name+"请开始考试");
    QString tips = QString("请在第%1行，%2列开始考试").arg(value.m_x+1).arg(value.m_y+1);
    QMessageBox::information(this,"",tips);

        //保存到map容器中
    map_users[id] = value;
}



void MainWindow::on_pushButton_stop_clicked()
{
    //1.待预测的人脸照片
    if(find_faces.size()==0)
    {
        QMessageBox::information(this,"","未找到正在考试的人员");
        return;
    }
    Mat dst = src(find_faces[0]);
    //2.转灰度重置大小
    cvtColor(dst,dst,CV_BGR2GRAY);
    cv::resize(dst,dst,Size(60,60));
    //3.人脸识别器预测  label  confidence
    int label = -1;
    double confi = 0;//可信度
    recognizer->predict(dst,label,confi);
    qDebug()<<"label: "<<label<<"confi: "<<confi;
    //4.根据label获取姓名
    QString name = userDao->getName(label);
    //5.弹窗询问是否要离开   离开--》修改label 更新结束考试的时间
    int choose = QMessageBox::question(this,"提示",name+"确定结束考试吗？",QMessageBox::Ok|QMessageBox::Cancel,//消息框出现的按钮
                          QMessageBox::Ok);//当前默认焦点所在的按钮
    switch(choose){
    case QMessageBox::Ok :
    {
        //1.更新考试结束的时间  数据库
        userDao->updateEndTime(label);
        //2.找到网格布局中 对应的座位 修改成XXX结束考试
        PointAndName value = map_users[label];
        changeLabelImg(Mat(),value,0);
    }
    case QMessageBox::Cancel:
        break;
    default:
        break;
    }

}

void MainWindow::on_pushButton_export_clicked()
{
    //1.选择一个要写入的表格
    QString filename = QFileDialog::getOpenFileName(this,"","D:/QT/Project/build-Test0303_kaochang-Desktop_Qt_5_14_2_MinGW_32_bit-Debug");
    //2.打开表格
    ex_helper.open(filename);
    //3.写入标题
    ex_helper.addTitle(filename);
    //4.写入数据
    vector<UserData> v;
    userDao->selectInfo(v);
    ex_helper.addInfos(filename,v);
    //5.关闭表格
    ex_helper.close();
    //提示
    QMessageBox::information(this,"","导出成功");
}
