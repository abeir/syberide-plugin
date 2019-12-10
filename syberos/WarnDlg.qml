import QtQuick 2.1

Rectangle {
    id: warnNotify

    property string warnString: ""

    height: 30
    width: warnText.width
    color:  "#00000000"
    visible: false
    x: 5
    y: 5
    z: 100

    Text {
        id: warnText
        color: "red"
        text: warnString
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 15
    }
    Timer {
        id: showTimer
        interval: 3000
        running: warnNotify.visible
        onTriggered: {
            warnNotify.visible = false;
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            warnNotify.visible = false;
        }
    }
}
