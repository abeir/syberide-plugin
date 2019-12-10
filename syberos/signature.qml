import QtQuick 2.1
import QtQuick.Controls 1.1

Rectangle {
    id: pg_signature
    anchors.fill: parent
    z:1
    WarnDlg { id: warnDlg }

    Text {
        id: prompt1
        text: qsTr("Please select the package to be signed:")
        color: "green"
        font.pixelSize: 16
        x: (pg_signature.width - appLabel.width - 30 - appText.width - 5 - appBtn.width) / 2
        y: pg_signature.height / 5
    }

    Text {
        id: appLabel
        text: qsTr("Package:")
        anchors.left: prompt1.left
        anchors.top: prompt1.bottom
        anchors.topMargin: 18
	    font.pixelSize: 15
    }
    TextField {
        id: appText
        anchors.left: appLabel.right
        anchors.leftMargin: 30
        anchors.verticalCenter: appLabel.verticalCenter
        width: 400
        height: 35
        focus: true
	    font.pixelSize: 15
    }
    SideButton {
        id: appBtn
        text: qsTr("Browse...")
        anchors.left: appText.right
        anchors.leftMargin: 5
        anchors.verticalCenter: appText.verticalCenter
        height: 34
        width: 100

        onClicked: {
                appText.text = develmanager.getSopPackage();
        }
    }

    SideButton {
        id: destroyBtn
        text: qsTr("Back")
        x: appBtn.x
        y: appBtn.y + appBtn.height + 30
        height: 34
        width: 100

        onClicked: {
                pg_signature.destroy();
        }
    }

    SideButton {
        id: signBtn
        text: qsTr("Start sign")
        x: destroyBtn.x - width - 30
        y: destroyBtn.y
        height: 34
        width: 100
        //side: false

        onClicked: {
            warnDlg.visible = false;

                    var r = develmanager.signPackage(appText.text);
                    if (r === -100) {
                        warnDlg.warnString = qsTr("The password is not right!");
                        warnDlg.x =  signBtn.x;
                        warnDlg.y = signBtn.y + signBtn.height + 10;
                        warnDlg.visible = true;
                    }
                    else if (r === -101) {
                        warnDlg.warnString = qsTr("Sign failed!");
                        warnDlg.x =  signBtn.x;
                        warnDlg.y = signBtn.y + signBtn.height + 10;
                        warnDlg.visible = true;
                    }

        }
    }

    Connections {
        target: develmanager
        onSopSignSucceed: {
            warnDlg.warnString = qsTr("Sign succeed!");
            warnDlg.x =  signBtn.x;
            warnDlg.y = signBtn.y + signBtn.height + 10;
            warnDlg.visible = true;
        }
    }
}
