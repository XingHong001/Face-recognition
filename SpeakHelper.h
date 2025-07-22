#ifndef SPEAKHELPER_H
#define SPEAKHELPER_H

#include<QAxObject>
#include <QDebug>

class SpeakHelper : public QAxObject
{
    Q_OBJECT
private:

    QAxObject _voice;
    bool _binit = false;
    bool _bReading = false;

signals:
    void speakComplete();
private slots:
    void dealevent(QString name, int arc , void* argv);

public:
    SpeakHelper();
    ~SpeakHelper();
    bool initSpeech();//初始化函数
    bool speak(QString txt);//文本转语音函数
    void setRate(int rate);//设置语速，范围-10到10
    void setVolume(int value);//设置音量，范围 0到100
    bool isSpeaking();
};

#endif // SPEAKHELPER_H
