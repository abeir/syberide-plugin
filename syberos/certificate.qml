import QtQuick 2.1
import QtQuick.Controls 1.1

Rectangle {
    id: root

    anchors.fill: parent

    function setEnabled() {
        table.enabled = true;
        refreshBtn.enabled = true;
        deleteBtn.enabled = true;
        addBtn.enabled = true;
        saveBtn.enabled = true;
        loginBtn.enabled = true;
    }

    function setDisabled() {
        table.enabled = false;
        refreshBtn.enabled = false;
        deleteBtn.enabled = false;
        addBtn.enabled = false;
        saveBtn.enabled = false;
        loginBtn.enabled = false;
    }

    WarnDlg { id: warnDlg }

    TableView {
        id: table
        width: parent.width
        height: parent.height - 90

        TableViewColumn { role: "deviceId"; title: qsTr("DeviceID"); width: 240 }
        TableViewColumn { role: "appId"; title: qsTr("AppID"); width: 240 }
        TableViewColumn { role: "expTime"; title: qsTr("ExpTime"); width: parent.width - 485 }

        model: develmanager.develListModel
    }

    Row {
        id: row

        property int __width: 120

        anchors.top: table.bottom
        anchors.topMargin: 20
        x: 20
        spacing: 20

        SideButton {
            id: refreshBtn
            text: qsTr("Refresh")
            width: row.__width
            side: false

            onClicked: {
                warnDlg.visible = false;
                /*if (!develmanager.isLanding()) {
                    warnDlg.warnString = qsTr("Please log in first!");
                    warnDlg.x = 5;
                    warnDlg.y = root.height - 30;
                    warnDlg.visible = true;
                }
                else {*/
                    develmanager.updateDeveloper();
            //    }
            }
        }

        SideButton {
            id: deleteBtn
            text: qsTr("Delete")
            width: row.__width
            side: false

            onClicked: {
                warnDlg.visible = false;
                if (!develmanager.isLanding()) {
                    warnDlg.warnString = qsTr("Please log in first!");
                    warnDlg.x = 5;
                    warnDlg.y = root.height - 30;
                    warnDlg.visible = true;
                }
                else if (table.currentRow < 0
                            || table.currentRow >= table.rowCount) {
                    warnDlg.warnString = qsTr("Please select one first!");
                    warnDlg.x = 5;
                    warnDlg.y = root.height - 30;
                    warnDlg.visible = true;
                }
                else {
                    develmanager.deleteDeveloper(table.currentRow);
                }
            }
        }

        SideButton {
            id: addBtn
            text: qsTr("Add")
            width: row.__width
            side: false

            onClicked: {
                warnDlg.visible = false;
               /* if (!develmanager.isLanding()) {
                    warnDlg.warnString = qsTr("Please log in first!");
                    warnDlg.x = 5;
                    warnDlg.y = root.height - 30;
                    warnDlg.visible = true;
                }
                else {*/
                    var component = Qt.createComponent("AddDevice.qml")
                    if (component.status === Component.Ready) {
                        var dialog = component.createObject(root);
                        setDisabled();
                        dialog.exited.connect(setEnabled);
                        dialog.visible = true;
                    }
               // }
            }
        }

        SideButton {
            id: saveBtn
            text: qsTr("Save")
            width: row.__width
            side: false

            onClicked: {
                warnDlg.visible = false;
                var c = develmanager.developerCount();
                if (c===0) {
                    warnDlg.warnString = qsTr("No certifications!");
                    warnDlg.y = root.height - 30;
                    warnDlg.visible = true;
                }
                else {
                    var r = develmanager.saveDeveloper();
                    if (r === false) {
                        warnDlg.warnString = qsTr("Save failed!");
                        warnDlg.x = 5;
                        warnDlg.y = root.height - 30;
                        warnDlg.visible = true;
                    }
                }
            }
        }
    }

    SideButton {
        id: loginBtn

        text: qsTr("Login")
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.verticalCenter: row.verticalCenter
        width: row.__width
        side: false

        onClicked: {
            var component = Qt.createComponent("LoginDlg.qml")
            if (component.status === Component.Ready) {
                var dialog = component.createObject(root);
                setDisabled();
                dialog.exited.connect(setEnabled);
                dialog.visible = true;
            }
        }
    }

}
