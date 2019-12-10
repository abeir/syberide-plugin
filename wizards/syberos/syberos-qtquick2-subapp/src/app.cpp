#include "%APPID%_workspace.h"
#include <QDebug>

%APPID%_Workspace::%APPID%_Workspace()
        : CWorkspace()
{
    m_view = SYBEROS::SyberosGuiCache::qQuickView();
    QObject::connect(m_view->engine(), SIGNAL(quit()), qApp, SLOT(quit()));
    m_view->setSource(QUrl("qrc:/qml/main.qml"));
    m_view->showFullScreen();
}

void %APPID%_Workspace::onLaunchComplete(Option option, const QStringList& params)
{
    switch (option) {
        case CWorkspace::HOME:
            qDebug()<< "Start by Home";
            break;
        case CWorkspace::URL:
            break;
        case CWorkspace::EVENT:
            break;
        case CWorkspace::DOCUMENT:
            break;
        default:
            break;
    }
}

