#include "%ProjectName:l%_plugin.%CppHeaderSuffix%"
#include <QDebug>


%ProjectName:s%Plugin::%ProjectName:s%Plugin()
{
    p_progressCBFunc = NULL;
}

void %ProjectName:s%Plugin::setProgressCBFunc(SetProgressFunc progressCBFunc)
{
    p_progressCBFunc = progressCBFunc;
}

void %ProjectName:s%Plugin::setProgress(int progress) {
    if(p_progressCBFunc){
        p_progressCBFunc(progress);
    }
}

bool %ProjectName:s%Plugin::onBackup(QDataStream* pStream)
{
    //toDo
    //input context to pStream
    //*pStream << QString("hahahahhahahah---------------conect----");
    return true;
}

bool %ProjectName:s%Plugin::onRestore(QDataStream* pStream)
{
    //toDo
    //output from pStream
    //QString s;
    //*pStream >> s;
    return true;
}
