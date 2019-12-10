//#include "mainwindow.h"
#include "pdkinstallwizard.h"
#include "kdrunoncechecker.h"

#include <QApplication>
#include <qtsingleapplication.h>
#include<QFile>
#include<QWizard>
#include<QTranslator>
#include<QMessageBox>

int main(int argc, char *argv[])
{
   SharedTools::QtSingleApplication app((QLatin1String("交叉编译环境管理工具")), argc, argv);//SharedTools::QtSingleApplication app((QLatin1String("sdk-manager")), argc, argv);
   KDRunOnceChecker runCheck(qApp->applicationDirPath()+QLatin1String("lockmyApp123456.lock"));

   if(runCheck.isRunning(KDRunOnceChecker::ConditionFlag::Lockfile)){

       if(runCheck.isRunning(KDRunOnceChecker::ConditionFlag::ProcessList)){
                QMessageBox::warning(0,QObject::tr("该功能正在运行"),QObject::tr("交叉编译环境管理工具正在运行,请勿重复操作.").arg(qAppName()));
                /*QMessageBox::warning(0,QObject::tr("AlreadyRunning"),QObject::tr("Another %1 instance is already running. Wait "
                                                                                           "until it finishes, close it, or restart your system.").arg(qAppName()));*/
                return EXIT_SUCCESS;
       }

   }

   QTranslator qtTranslator;
   qtTranslator.load(QString(QLatin1String(":/translate/sdk-manager.qm")));
   app.installTranslator(&qtTranslator);
   PDKInstallWizard pdkInstallwizard;
   pdkInstallwizard.show();

    return app.exec();
}
