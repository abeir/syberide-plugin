import QtQuick 2.1
import QtQuick.Controls 1.2

Rectangle {
    id: root

    anchors.fill: parent
    color: "#212121"
    opacity: 0.8
    z: 1

    signal exited

    WarnDlg { id: warnDlg }

    Rectangle {
        id: rect

        color: "aliceblue"
        radius: 10
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height / 4
        width: devIdLabel.width + 10 + devIdText.width + 5 + queryBtn.width + 20 * 2
        height: 170

        Text {
            id: devIdLabel
            text: qsTr("DeviceID:")
            x: 20
            y: 30
	    font.pixelSize: 15
        }
        TextField {
            id: devIdText
            anchors.left: devIdLabel.right
            anchors.leftMargin: 10
            anchors.verticalCenter: devIdLabel.verticalCenter
            width: 280
            height: 30
            font.family: "Helvetica"
	    font.pixelSize: 15
            focus: true
        }
        SideButton {
            id: queryBtn

            text: qsTr("Query")
            anchors.left: devIdText.right
            anchors.leftMargin: 5
            anchors.verticalCenter: devIdText.verticalCenter
            width: 70
            height: 29

            onClicked: {
                warnDlg.visible = false;
                devIdText.text = develmanager.getDeviceID();
                if (devIdText.text === "") {
                    warnDlg.warnString = qsTr("Query failed, Please check whether your device is connected normally!");
                    warnDlg.x = rect.x + 5;
                    warnDlg.y = rect.y + rect.height - 30;
                    warnDlg.visible = true;
                }
            }
        }

        Text {
            id: appIdLabel
            text: qsTr("AppID:")
            anchors.left: devIdLabel.left
            anchors.top: devIdLabel.bottom
            anchors.topMargin: 15
	    font.pixelSize: 15
        }
        TextField {
            id: appIdText
            anchors.left: devIdText.left
            anchors.right: devIdText.right
            anchors.verticalCenter: appIdLabel.verticalCenter
            height: devIdText.height
            font.family: "Helvetica"
	    font.pixelSize: 15
            focus: true
        }

        Row {
            id: row

            property int __width: 120

            anchors.left: parent.left
            anchors.leftMargin: (rect.width - __width * 2 - spacing) / 2
            anchors.top: appIdLabel.bottom
            anchors.topMargin: 25
            spacing: 80

            SideButton {
                id: okBtn
                text: qsTr("  OK")
                width: row.__width
                side: false

                onClicked: {
                    warnDlg.visible = false;
                    if (devIdText.text === "") {
                        warnDlg.warnString = qsTr("Please enter or click the \"Query\" button to query the device id!");
                        warnDlg.x = rect.x + 5;
                        warnDlg.y = rect.y + rect.height - 30;
                        warnDlg.visible = true;
                    }
                    else if (appIdText.text === "") {
                        warnDlg.warnString = qsTr("Please enter the AppID!");
                        warnDlg.x = rect.x + 5;
                        warnDlg.y = rect.y + rect.height - 30;
                        warnDlg.visible = true;
                    }
                    else {
                        develmanager.addDeveloper(devIdText.text, appIdText.text);
                        root.exited();
                        root.destroy();
                    }
                }
            }
            SideButton {
                id: cancelBtn
                text: qsTr("Cancel")
                width: row.__width
                side: false

                onClicked: {
                    root.exited();
                    root.destroy();
                }
            }
        }
    }

}
