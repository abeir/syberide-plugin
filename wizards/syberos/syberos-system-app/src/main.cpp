#include <QCoreApplication>
#include <QThread>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int i;
    for(i = 1; i < 6; i++){
        qDebug()<<"Hello SyberOS. This loop index is "<< i;
        QThread::sleep(1);
    }

    return 0;

    // If you want to have a event loop to do something, use the following line instead.
    // return a.exec();
}
