#ifndef __%APPID:u%_WORKSPACE__
#define __%APPID:u%_WORKSPACE__

#include <QQuickView>
#include <QQuickWindow>
#include <QQmlEngine>
#include <QQmlComponent>
#include <SyberosGuiCache>
#include <cgui_application.h>
#include <cworkspace.h>


using namespace SYBEROS;

class %APPID%_Workspace : public CWorkspace
{
    Q_OBJECT

    private:
    QQuickView *m_view;

    public:
    %APPID%_Workspace();

    // 应用重入回调接口，应用需要自己重写这些接口以处理重入流程
    // 当应用是由Home启动时，回调open
    //void open(const QStringList& params);
    // 当应用是由openurl接口启动时，回调openByUrl
    //void openByUrl(const QUrl& url);
    // 当应用是由某事件触发启动时，回调openByEvent，例如DBus触发
    //void openByEvent(const QStringList& params);
    // 当应用是由用户希望打开某种Mimetype而启动时，回调openByDocument
    //void openByDocument(const QString& exec, const QStringList& parameters);

    // 应用启动结束时，回调此函数。根据传入的option，应用可以区分启动的方式。
    void onLaunchComplete(Option option, const QStringList& params);
    // 应用在被调度到前台时回调此函数
    //void onActive();
    // 应用被放置到后台时回调此函数
    //void onDeactive();
};


#endif //__%APPID:u%_WORKSPACE__
