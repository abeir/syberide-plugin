import QtQuick 2.1
import QtQuick.Controls 1.1

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
        width: userNameLabel.width + 10 + userNameText.width + 30 * 2
        height: 230

        Text {
            id: userNameLabel
            text: qsTr("UserName:")
            x: 30
            y: 30
	    font.pixelSize: 15
        }
        TextField {
            id: userNameText
            anchors.left: userNameLabel.right
            anchors.leftMargin: 10
            anchors.verticalCenter: userNameLabel.verticalCenter
            width: 240
            height: 30
            font.family: "Helvetica"
	    font.pixelSize: 15
            focus: true
        }

        Text {
            id: passwordLabel
            text: qsTr("Password:")
            anchors.left: userNameLabel.left
            anchors.top: userNameLabel.bottom
            anchors.topMargin: 15
	    font.pixelSize: 15
        }
        TextField {
            id: passwordText
            anchors.left: userNameText.left
            anchors.verticalCenter: passwordLabel.verticalCenter
            width: userNameText.width
            height: userNameText.height
            echoMode: TextInput.Password
            font.family: "Helvetica"
	    font.pixelSize: 15
            focus: true
        }

        Row {
            id: row

            property int  __width: 120

            anchors.left: parent.left
            anchors.leftMargin: (rect.width - __width * 2 - spacing) / 2
            anchors.top: passwordLabel.bottom
            anchors.topMargin: 30
            spacing: 60

            SideButton {
                id: loginBtn
                text: qsTr("Login")
                width: row.__width
                side: false

                onClicked: {
                    warnDlg.visible = false;
                    if (userNameText.text === "") {
                        warnDlg.warnString = qsTr("Please enter the username");
                        warnDlg.x = rect.x + 5;
                        warnDlg.y = rect.y + rect.height - 30;
                        warnDlg.visible = true;
                    }
                    else if (passwordText.text === "") {
                        warnDlg.warnString = qsTr("Please enter the password");
                        warnDlg.x = rect.x + 5;
                        warnDlg.y = rect.y + rect.height - 30;
                        warnDlg.visible = true;
                    }
                    else {
                        develmanager.land(userNameText.text, passwordText.text);
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

        Text {
            id: tip
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.top: row.bottom
            anchors.topMargin: 15
            width: parent.width - 5
            text: qsTr("Tips: If you don't have an account for developing, please visit \"https://appstore.syberos.com\" to register first!")
            color: "green"
            wrapMode: Text.WrapAnywhere //Text.WordWrap
	    font.pixelSize: 13
        }

        Connections {
            target: develmanager

            onLandSuccess: {
                develmanager.updateDeveloper();
                root.exited();
                root.destroy();
            }
            onOffLanding: {
                warnDlg.visible = false;
                warnDlg.warnString = qsTr("Login failed, Please check your username and password!");
                warnDlg.x = rect.x + 5;
                warnDlg.y = rect.y + rect.height - 30;
                warnDlg.visible = true;
            }
        }
    }

}
