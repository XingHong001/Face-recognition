#include "excelhelper.h"
#include <QDebug>
#include <QDir>

void ExcelHelper::open(QString file)
{
    //动Excel进程，获取Excel工作簿集
    excel = new QAxObject("Excel.Application");
    excel->dynamicCall("SetVisible(bool)", false); //true 表示操作文件时可见，false表示为不可见

    excel->setProperty("DisplayAlerts",false);

    //通过进程获取Excel工作簿集
    workbooks = excel->querySubObject("WorkBooks");
    //添加一个工作簿
    workbooks->dynamicCall("Add");

    workbook = workbooks->querySubObject("Open(QString&)", file);
    // 获取打开的excel文件中所有的工作sheet
    worksheets = workbook->querySubObject("WorkSheets");
    // 获得第一个sheet
    worksheet = worksheets->querySubObject("Item(int)", 1);
    
    //获得数据范围
    QAxObject * usedrange = worksheet->querySubObject("UsedRange");
    //获得行数
    QAxObject * rowsObj = usedrange->querySubObject("Rows");
    rows = rowsObj->property("Count").toInt();
    //获得列数
    QAxObject * columnsObj = usedrange->querySubObject("Columns");
    cols = columnsObj->property("Count").toInt();
    qDebug()<<rows<<" "<<cols;

}

void ExcelHelper::close()
{
   // workbook->dynamicCall("Save()");    //保存文件
    workbook->dynamicCall("Close(Boolean)", false);
    excel->dynamicCall("Quit(void)");  //EXE结束前需要关闭EXCEL
    delete excel;
}

void ExcelHelper::addInfos(QString fileName, vector<UserData> &v)
{

    int index = 0;
    for(int i = 2;i <= v.size()+1;i++)//行
    {
        for(int j = 1;j <= 4;j++)//列
        {
            QAxObject *cell = worksheet->querySubObject("Cells(int,int)", i, j);

            if(j==1)
                cell->setProperty("ColumnWidth", 15);
            else
                cell->setProperty("ColumnWidth", 30);

            cell->setProperty("RowHeight", 60);
            //水平居中
            cell->setProperty("HorizontalAlignment", -4108);

            switch (j) {
            case 1:
                cell->dynamicCall("SetValue(const int&)",v[index].id);
                break;
            case 2:
                cell->dynamicCall("SetValue(const QString&)",v[index].name);
                break;
            case 3:                                             //2024-04-15 11:30:06
                cell->dynamicCall("SetValue(const QString&)",v[index].start.toString("yyyy-MM-dd hh:mm:ss") );
                break;
            case 4:
                cell->dynamicCall("SetValue(const QString&)",v[index].end.toString("yyyy-MM-dd hh:mm:ss") );
                break;
            default:
                break;
            }
        }
        index++;
    }
    workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(fileName));
}

void ExcelHelper::addTitle(QString fileName)
{
    QString titles[4] = {"id","学生姓名","考试开始时间","考试结束时间"};
    for(int j = 1;j <= 4;j++)
    {
        QAxObject *cell = worksheet->querySubObject("Cells(int,int)", 1, j);
        QAxObject *font = cell->querySubObject("Font");  //获取单元格字体
        font->setProperty("Bold", true);  //字体加粗
        //水平居中
        cell->setProperty("HorizontalAlignment", -4108);

        if(j==1)//id
            cell->setProperty("ColumnWidth", 15);
        else
            cell->setProperty("ColumnWidth", 30);
        //高度
        cell->setProperty("RowHeight", 40);
        cell->dynamicCall("SetValue(const QString&)", titles[j-1]);
    }
    workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(fileName));

}


