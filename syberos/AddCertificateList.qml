import QtQuick 2.1
import QtQuick.Controls 1.1

Rectangle {
    id: pg_AddCertList
    anchors.fill: parent

    WarnDlg { id: warnDlg }


    Text {
        id: nameLabel
        text: qsTr("Certificate:")
        x: nameLabel.width
        y: pg_AddCertList.height / 5
        color: "green"
        anchors.topMargin: 18
        font.pixelSize: 15
    }

    TextField {
        id: nameLabelText
        x: (pg_AddCertList.width - nameLabel.width  - nameLabelText.width  ) / 2
        //anchors.left: nameLabel.right
        anchors.verticalCenter: nameLabel.verticalCenter
        width: 270
        //width: 400
        height: 35
        focus: true
        //echoMode: TextInput.Password
        font.pixelSize: 15
    }

//    Text {
//        id: keyLabel
//        text: qsTr("Certificate file:")
//        anchors.left: nameLabel.left
//        anchors.top: nameLabelText.bottom
//        color: "green"
//        anchors.topMargin: 50
//        font.pixelSize: 15
//    }
//    TextField {
//        id: keyText
//        anchors.left: nameLabelText.left
//        width: 400
//        anchors.verticalCenter: keyLabel.verticalCenter
//        height: nameLabelText.height
//        focus: true
//        font.pixelSize: 15
//    }
//    SideButton {
//        id: keyBtn
//        text: qsTr("Browse...")
//        anchors.left: keyText.right
//        anchors.leftMargin: 5
//        anchors.verticalCenter: keyText.verticalCenter


//        onClicked: {
//            keyText.text = develmanager.getKeyFile();
//        }
//    }

    Text {
        id: pemLabel
        text: qsTr("Private key:")
        color: "green"
        anchors.left: nameLabel.left
        anchors.top: nameLabel.bottom
        anchors.topMargin: 22
        font.pixelSize: 15
    }
    TextField {
        id: pemText
        width: 400
        anchors.left: nameLabelText.left
//        anchors.right: keyText.right
        anchors.verticalCenter: pemLabel.verticalCenter
        height: nameLabelText.height
        focus: true
        font.pixelSize: 15
    }
    SideButton {
        id: pemBtn
        text: qsTr("Browse...")
        anchors.left: pemText.right
        anchors.leftMargin: 5
        anchors.verticalCenter: pemText.verticalCenter
//        height: keyBtn.height
//        width: keyBtn.width

        onClicked: {
            pemText.text = develmanager.getPemFile();
        }
    }

    Text {
        id: passwdLabel
        text: qsTr("Password:")
        color: "green"
        anchors.left: nameLabel.left
        anchors.top: pemLabel.bottom
        anchors.topMargin: 22
        font.pixelSize: 15
    }
    TextField {
        id: passwdText
        anchors.left: nameLabelText.left
        anchors.verticalCenter: passwdLabel.verticalCenter
        width: 270
        height: nameLabelText.height
        echoMode: TextInput.Password
        font.pixelSize: 15
    }


    SideButton {
        id: signBtn

        text: qsTr("  Save")
        anchors.left: nameLabelText.left
        width: 150
        anchors.top: passwdLabel.bottom
        anchors.topMargin: 50
        height: 50
        side: false

        onClicked: {
            warnDlg.visible = false;

            if (nameLabelText.text === "") {
                warnDlg.warnString = qsTr("The certificate name can't be empty!");
                warnDlg.x =  nameLabelText.x + nameLabelText.width + 10;
                warnDlg.y = nameLabelText.y;
                warnDlg.visible = true;
            }
//            else if (keyText.text === "") {
//                warnDlg.warnString = qsTr("Please select the public key");
//                warnDlg.x =  keyBtn.x + keyBtn.width + 10;
//                warnDlg.y = keyBtn.y;
//                warnDlg.visible = true;
//            }
            else if (pemText.text === "") {
                warnDlg.warnString = qsTr("Please select the keystore file");
                warnDlg.x =  pemBtn.x + pemBtn.width + 10;
                warnDlg.y = pemBtn.y;
                warnDlg.visible = true;
            }
            else if (passwdText.text === "") {
                warnDlg.warnString = qsTr("The password can't be empty!");
                warnDlg.x =  passwdText.x + passwdText.width + 10;
                warnDlg.y = passwdText.y;
                warnDlg.visible = true;
            }

            else {
                    var r = develmanager.savecertificatelist(nameLabelText.text, pemText.text, passwdText.text, "ThisFileIsNotUsed");
                    if (r === -1) {
                        warnDlg.warnString = qsTr("file!");
                        warnDlg.x =  signBtn.x;
                        warnDlg.y = signBtn.y - 40;
                        warnDlg.visible = true;
                    }
                    else if (r === -2) {
                        warnDlg.warnString = qsTr("list name existed, please write it again!");
                        warnDlg.x =  signBtn.x;
                        warnDlg.y = signBtn.y - 40;
                        warnDlg.visible = true;
                    }
                    else {
                        warnDlg.warnString = qsTr("Sign succeed!");
                        pg_CertList.reloadCertList();
                        warnDlg.x =  signBtn.x;
                        warnDlg.y = signBtn.y - 40;
                        warnDlg.visible = true;
                        pg_AddCertList.destroy();
                    }
            }

        }
    }

    SideButton {
        id: destroy
        text: qsTr("    Back")
        anchors.left: signBtn.right
        anchors.leftMargin: 40
        anchors.verticalCenter: signBtn.verticalCenter
        height: signBtn.height
        width: signBtn.width

        onClicked: {
            pg_AddCertList.destroy()
        }
    }
}
