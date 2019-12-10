#ifndef SELECTTARGETINSTALLPAGE_H
#define SELECTTARGETINSTALLPAGE_H

#include "installbasepage.h"
#include "common/downloader.h"
#include "common/node.h"
#include "common/qpdktargetitemmodel.h"
#include<QNetworkReply>
#include<QTreeView>
#include<QLabel>
#include<QModelIndex>
#include<QAbstractButton>
#include<QVariantMap>


class SelectTargetInstallPage : public InstallBasePage
{
      Q_OBJECT

public:
    SelectTargetInstallPage(QWidget *parent = 0);
    void initUI();
    void initConnect();
    void initializePage();
    void printf(Node *node);
    int	nextId() const;
    bool validatePage();
    bool getSelectData();
    void getTraversalData(Node*node);
    bool parseRepojson();

public slots:
    void updateModel(const QModelIndex &index);
    void initModel();


private:
    Downloader downloader;
    QTreeView *treeView;
    QPDKTargetItemModel* treePDKModel;
    QLabel *m_msgLabel;
    QAbstractButton *installbutton;
    QVariantMap repoMap;



};

#endif // SELECTTARGETINSTALLPAGE_H
