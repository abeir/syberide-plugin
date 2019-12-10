/*
    \author maqidi
    \brief it is used to Automated Testing.
    by far you can use it to
        capture Syberos phone's screen
        simulate touch event with mouse.
*/


#include <QClipboard>
#include <QToolTip>

#include "monitorwidget.h"
#include "ui_monitorwidget.h"
#include "sshhandler.h"

#include "sshhandler.h"
//#include <remotelinux/syberdevice/syberdevice.h>

using namespace ProjectExplorer;

namespace SyberOSDevice {
namespace Internal {


MonitorWidget::MonitorWidget(QWidget *parent) :
    QWidget(parent)
    ,ui(new Ui::MonitorWidget)
    ,m_deviceManager(DeviceManager::instance())
    ,m_deviceManagerModel(new DeviceManagerModel(m_deviceManager, this))
{
    m_posList.clear();
    m_strFormat.clear();
    ui->setupUi(this);
    ui->coorLbl->adjustSize();
    QSize sSize = ui->splitter->size();
    ui->imageLabel->resize(sSize.width()*0.75, ui->imageLabel->height());
}

MonitorWidget::~MonitorWidget()
{
    destroySsh();
    delete ui;
}

void MonitorWidget::destroySsh()
{
    if(m_sshHandler)
    {
        delete m_sshHandler;
        m_sshHandler = NULL;
    }
}

void MonitorWidget::addTab(QWidget *page, QString pageName)
{
    if(page && !pageName.isEmpty())
        ui->tabWidget->addTab(page, pageName);
}

void MonitorWidget::init()
{
    ui->deviceCombo->setModel(m_deviceManagerModel);
    connect(ui->deviceCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(initSsh(int)) );
    connect(ui->imageLabel, &ImageLabel::mousePressed, [this](QPoint p, bool isMouseGrabed){
            if(isMouseGrabed)
            {
                m_posList.append(p);
                if(!m_strFormat.isNull())
                {
                    ui->listWidget->addItem(m_strFormat.arg(p.rx()).arg(p.ry()));
                }
            }
    });

    connect(ui->clearBtn, &QAbstractButton::clicked, [this](){
       m_sshHandler->clearImage();
       ui->imageLabel->setImage(QStringLiteral(""));
       m_posList.clear();
       ui->listWidget->clear();
    });

    ui->listWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->listWidget->setSelectionMode(QAbstractItemView::QAbstractItemView::ContiguousSelection);
    connect(ui->listWidget, &QListWidget::itemSelectionChanged, [this](){
        QClipboard *clipBorad = QApplication::clipboard();
        QStringList strclip;
        Q_FOREACH(QListWidgetItem *item,ui->listWidget->selectedItems())
        {
            strclip.append(item->text());
        }
        clipBorad->setText(strclip.join(m_strSeperator));
        QRect rect(geometry().center(), QSize(100, 100));
        QToolTip::showText(mapToGlobal(rect.topLeft()), tr("the coordinations has been copied to clipboard"), this, rect);
    });
}

void MonitorWidget::initSsh(int index)
{
    ui->coorLbl->clear();
    ui->listWidget->clear();
    ui->connectBtn->setEnabled(false);
    ui->connectBtn->disconnect();
    ui->captureButton->setEnabled(false);
    ui->captureButton->disconnect();


    const IDevice::ConstPtr device = m_deviceManagerModel->device(index);
    if (!device.isNull()) {

        if (device->hasDeviceTester()) {
            DeviceTester *tester = device->createDeviceTester();
            if(tester)
            {
                connect(tester, &DeviceTester::finished, [this](ProjectExplorer::DeviceTester::TestResult result){
                    switch(result)
                    {
                    case DeviceTester::TestSuccess:
                        ui->connectBtn->setEnabled(true);
                        connect(ui->connectBtn, &QAbstractButton::clicked, this, &MonitorWidget::connectSsh);
                        ui->coorLbl->setText(tr("<font color=\"#0000ff\">%1</font>").arg(tr("设备可用")));
                        break;
                    case DeviceTester::TestFailure:
                        ui->connectBtn->setEnabled(false);
                        ui->captureButton->setEnabled(false);
                        ui->clearBtn->setEnabled(false);
                        ui->coorLbl->setText(tr("<font color=\"#ff0000\">%1</font>").arg(tr("设备不可用")));
                        break;
                    }
                });
                connect(tester, &DeviceTester::errorMessage, [this](const QString &message){
                    ui->coorLbl->setText(tr("<font color=\"#ff0000\">%1</font>").arg(message));
                });
                tester->testDevice(device);
            }
            else
                ui->coorLbl->setText(tr("<font color=\"#ff0000\">%1 : %2</font>").arg(tr("设备不可用")).arg(tr("请检查手机是否连接")));
        }
        else
        {
            ui->coorLbl->setText(tr("<font color=\"#ff0000\">%1 : %2</font>").arg(tr("该设备没有ssh连接")).arg(tr("请使用其它方式检测设备状态")));
        }
    }
}

void MonitorWidget::printScreen()
{
    m_strFormat = ui->lineEdit->text();
    if(m_strFormat.contains(tr("\\")))
    {
        QStringList strlist = m_strFormat.split(tr("\\"));
        m_strFormat = strlist[0];
        if(strlist.size() >= 2)
        {
            m_strSeperator = strlist[1];
        }
        else
        {
            m_strSeperator = tr(" ");
        }
    }
    if(!(m_strFormat.contains(tr("%1")) && m_strFormat.contains(tr("%2"))))
        m_strFormat = QStringLiteral("%1 %2");

    if(m_sshHandler->getState() != SshHandler::SftpCorrect)
    {
        ui->coorLbl->setText( tr("<font color=\"#ff0000\">%1</font>").arg(tr("sftp connection failed :  state num: %1").arg(m_sshHandler->getState())));
        return ;
    }

    m_sshHandler->sshExecProcess();
}

void MonitorWidget::connectSsh()
{
    if(m_sshHandler)
    {
        delete m_sshHandler;
        m_sshHandler = NULL;
    }
    m_sshHandler = new SshHandler(this);
    Q_ASSERT(m_sshHandler);



    connect(m_sshHandler, &SshHandler::error, [this](QString errMsg){
        ui->coorLbl->setText(tr("<font color=\"#ff0000\">%1  %2</font>").arg(tr("设备不可用")).arg(errMsg));
        ui->captureButton->setEnabled(false);
        delete m_sshHandler;
        m_sshHandler = NULL;
    });

    connect(m_sshHandler, &SshHandler::finishConn, [this](int connectionType){
        switch(connectionType)
        {
            case SshHandler::SftpCorrect:
                if(ui->captureButton->isEnabled())
                {
                    connect(ui->captureButton, &QAbstractButton::clicked, this, &MonitorWidget::printScreen);
                    ui->connectBtn->setEnabled(false);
                }
                break;
            case SshHandler::ConnectionCorrect:
                ui->captureButton->setEnabled(true);
            break;
        case SshHandler::Disconnect:
            ui->captureButton->setEnabled(false);
            break;
        }
    });

    connect(m_sshHandler, &SshHandler::sftpFinished, [this](QString localPath){
        ui->imageLabel->setImage(localPath);
    });

    connect(m_sshHandler, &SshHandler::remoteProcessReturned, [this](QString returnVal){
       m_sshHandler->sshPullFile(returnVal.trimmed());
    });

    m_sshHandler->sshInit();


}

} // namespace Internal
} // namespace SyberOSDevice

