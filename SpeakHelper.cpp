#include "SpeakHelper.h"

SpeakHelper::SpeakHelper()
{
    if(initSpeech())//初始化语音
    {
        qDebug()<<"初始化成功";
    }
    else
    {
        qDebug()<<"初始化失败";
    }
}
bool SpeakHelper::initSpeech()
{
    if(_binit)
        return true;
    _binit = this->_voice.setControl("96749377-3391-11D2-9EE3-00C04F797396");//设置COM的名称用来初始化COM的模型,返回是否加载成功
    if(_binit)
    {
        connect(&this->_voice,SIGNAL(signal(QString, int, void*)), this, SLOT(dealevent(QString, int, void*)));

    }
    return _binit;
}

bool SpeakHelper::speak(QString txt)
{
    if(!_binit)
        return false;
    qDebug()<<"开始朗读";
    int result = this->_voice.dynamicCall("Speak(QString, SpeechVoiceSpeakFlags)", txt ,1).toInt();//执行第一个参数的方法名，以第二个参数返回是否执行成功
    _bReading = true;
    return result;//返回是否执行阅读函数
}

void SpeakHelper::setRate(int rate)
{
    if(!_binit)
        return;
    this->_voice.dynamicCall("SetRate(int)", rate);
}

void SpeakHelper::setVolume(int value)
{
    if(!_binit)
        return;
    this->_voice.dynamicCall("SetVolume(int)", value);
}

bool SpeakHelper::isSpeaking()
{
    return _bReading;
}

void SpeakHelper::dealevent(QString name, int arc, void *argv)
{
    if(name == "EndStream(int,QVariant)")
    {
       _bReading = false;
       qDebug()<<"朗读完毕";
       emit speakComplete();
    }
}

SpeakHelper::~SpeakHelper()
{

}

