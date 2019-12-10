import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import Chooselist 1.0
import Chooselist_forbuild 1.0

Rectangle {
    id: pg_CertList
    anchors.fill: parent
    property  string strlistname
    property string  strlistname_fb
    property string effecflag

    Component.onCompleted: {
            mymodel.getDataofList();
            mymodel_fb.getDataofList();
            console.log("certificatelist.qml construction complete==========");
        }

    WarnDlg { id: warnDlg }

    function showChild1(){
           var component = Qt.createComponent("AddCertificateList.qml");
           if (component.status === Component.Ready) {
               var bQml = component.createObject(pg_CertList);
           }
       }

    function reloadCertList_fb(){
         mymodel_fb.getDataofList();
    }
    function reloadCertList(){
         mymodel.getDataofList();
    }

    Chooselist{
        id:mymodel
        Component.onCompleted: {
            console.log("mymodel count ===========================", mymodel.rowCount());
        }
    }

    Chooselist_forbuild{
        id:mymodel_fb
        Component.onCompleted: {
            console.log("mymodel_fb count ===========================", mymodel_fb.rowCount());
        }
    }

    Dialog{
        id:dialog_delete1
        title: qsTr("delete")
        width: 410
        height: 160
        standardButtons:  StandardButton.Yes | StandardButton.No

        Text {
            id: dialogtext1
            x: 10
            y: 10
            color: "red"
            text: qsTr("Would you want to delete the certificate with name of [ ")+ strlistname +" ] ?" ;
        }

         onYes:{
             develmanager.deletecertificatelist(strlistname);
              console.log("strlistname-----------delete:", strlistname);
             mymodel.getDataofList();
             listview_lr.currentIndex = -1;
             dialog_delete1.close();
         }
    }

    Dialog{
        id:dialog_delete2
        title:qsTr( "delete" )
        width: 410
        height: 160
        standardButtons:  StandardButton.Yes | StandardButton.No

         onYes:{
             develmanager.deletecertificatelist_fb(strlistname_fb);
              console.log("strlistname-----------delete:", strlistname_fb);
             mymodel_fb.getDataofList();
             listview_fb.currentIndex = -1;
             dialog_delete2.close();
         }

        Text {
            id: dialogtext2
            x: 10
            y: 10
            color: "red"
            text:qsTr( "Would you want to delete the certificate with name of [ ")+ strlistname_fb +" ] ?" ;
        }
    }

    // Hint information for situations that no item selected.
    Dialog{
        id:dialog_delete3
        title:qsTr( "Error" )
        width: 400
        height: 150
        standardButtons:  StandardButton.Ok

         onAccepted: {
             dialog_delete3.close();
         }

        Text {
            id: dialogtext3
            x: 10
            y: 10
            color: "red"
            text:qsTr( "No item had been selected. Please select one certificate to delete. ") ;
            wrapMode: Text.Wrap
        }
    }


 Text {
        id: prompt1
        text: qsTr("Certificates for SOP debug:")
        color: "green"
        x: 50
        y: 50
//        anchors.top:loginrow.bottom
//        anchors.left:loginrow.left
        font.pixelSize: 16
    }

    Rectangle {
        id:listwiewrec_fb
        anchors.left: prompt1.left
        y: prompt1.y + prompt1.height + 10
        width: pg_CertList.width*5/8
        height: pg_CertList.height/2 - y - 50
        border {
                id: list_fb_bd
                color: "#888888"
                width: 5
         }

        ListView {
                    id: listview_fb
                    anchors.fill: parent
                    anchors.topMargin: list_fb_bd.width
                    anchors.bottomMargin: list_fb_bd.width
                    anchors.leftMargin: list_fb_bd.width
                    anchors.rightMargin: list_fb_bd.width
                    model: mymodel_fb
                    clip: true

                    ExclusiveGroup{
                        id: listview_fb_group
                    }

                    delegate:  Rectangle {
                        id:delegateRec_fb
                        width: listview_fb.width
                        height:listview_fb.height/4
                        color:listview_fb.currentIndex == index ? "#a3a3a3" : "white"

                        border {
                                color: "green"
                                width: 1
                         }

                        Row {
                            Column {
                                id:column0
                                width: listview_fb.width*4/5
                                Text {
                                    id:contentTextId_fb
                                    x: 5
                                    font.pointSize: 16
                                    width: listview_fb.width
                                    wrapMode: Text.WordWrap
                                    text: qsTr("  Certificate Name: ") + "<span style=\"color:'red'\">" + listName + "</span>"

                                }
                                Text {
                                    x: contentTextId_fb.x
                                    font.pointSize: 9
                                    color: "green"
                                    text: "<br/>pemfile: <span style=\"color:'green'\">" + pemfile + "</span>"
                                }

                            }
                            CheckBox{
                                //style: ButtonStyle
                                id: checkBox_fb
                                text:  effectiveflag=="1" ? "生效中" : "未生效"
                                checked: effectiveflag=="1" ? true : false
                                exclusiveGroup: listview_fb_group
                            }


//                            Text{
//                                id:effectivefligtext_fb
//                                color: "red"

//                                //anchors.right: listview_fb.right
//                                width: listview_fb.width/5

//                                text : effectiveflag=="1" ? "生效" : ""

//                            }
                        }

                        MouseArea {
                            id:mousearea_fb
                            anchors.fill: parent
                            onClicked: {
                                listview_fb.currentIndex = index;

                                console.log("index------------------", listName);
                                strlistname_fb  = listName;
                                console.log("msarea.pressed------------------", mousearea_fb.pressed);
                                console.log("listview.currentItem.y------------------", listview_fb.currentItem.y);

                            }
                        }
                        Component.onCompleted: {
                            console.log("mymodel count ===========================", mymodel_fb.rowCount());
                        }
                    }
                }
    }

    // Buttons for sop building certificates.
    Column{
        id:buttonrow_fb
        x: listwiewrec_fb.width + listwiewrec_fb.x + 20
        y:listwiewrec_fb.y
        //x:pg_CertList.width*13/18
        height:listwiewrec_fb.height
        spacing: buttonrow_fb.height/9
        width:pg_CertList.width/9

        Button{
            id:button1_fb
            width: pg_CertList.width/9
            height:buttonrow_fb.height/6
            text:qsTr("Add")

            onClicked: {

                warnDlg.visible = false;

                    var component = Qt.createComponent("AddCertificateList_fb.qml")
                    if (component.status === Component.Ready) {
                        var dialog = component.createObject(pg_CertList);
                        //setDisabled();
                        //dialog.exited.connect(setEnabled);
                        //dialog.visible = true;
                    }else{
                        console.log("Error in certificatelist.qml...", component.errorString());
                    }

                    //mymodel_fb.getDataofList()
            }
        }
        Button{
            id:button2_fb
            text:qsTr("Delete")
            width: pg_CertList.width/9
            height:buttonrow_fb.height/6
            onClicked: {
                if (listview_fb.currentIndex >= 0){
                    dialog_delete2.open();
                }else{
                    dialog_delete3.open();
                }
            }
        }

        Button{
            id:button4_fb
            text:qsTr("Available")
            width: pg_CertList.width/9
            height:buttonrow_fb.height/6
            onClicked: {

                develmanager.availablecertificatelist_fb(strlistname_fb)
                mymodel_fb.getDataofList()
            }
        }
    }

    Text {
           id: prompt2
           text: qsTr("Certificates for SOP publish:")
           color: "green"
           y: pg_CertList.height / 2
           anchors.left: prompt1.left
           font.pixelSize: 16
       }

    Rectangle {
        id:listwiewrec
        y:pg_CertList.height / 2 + 40
        anchors.left: prompt1.left
        width: pg_CertList.width*5/8
        height: pg_CertList.height - y - 20
        border {
                id: list_bd
                color: "#888888"
                width: 5
         }

        ListView {
                    id: listview_lr
                    anchors.fill: parent
                    anchors.topMargin: list_bd.width
                    anchors.bottomMargin: list_bd.width
                    anchors.leftMargin: list_bd.width
                    anchors.rightMargin: list_bd.width
                    model: mymodel
                    clip: true

                    ExclusiveGroup{
                        id: listview_lr_group
                    }

                    delegate:  Rectangle {
                        id:delegateRec
                        width: listview_lr.width
                        height: listview_lr.height/4
                        //color: mousearea.pressed ?  "gray" : "white"
                        color:listview_lr.currentIndex == index ? "#a3a3a3" : "white"

                        border {
                                color: "green"
                                width: 1
                         }
                        Row{
                            Column {
                                id:column1
                                width: listview_lr.width*4/5
                                Text {
                                    id:contentTextId
                                    x: 5
                                    font.pointSize: 16
                                    width: parent.width
                                    wrapMode: Text.WordWrap
                                    text: qsTr("  Certificate Name: ") + "<span style=\"color:'red'\">" + listName + "</span>"

                                }
                                Text {
                                    x: contentTextId.x
                                    font.pointSize: 9
                                    color: "green"
                                    text:  "<br/>pemfile: <span style=\"color:'green'\">" + pemfile + "</span>"
//                                       + "<br/>keyfile: <span style=\"color:'green'\">" +keyfile + "</span>"
                                       /*   + "<br/>password: <span style=\"color:'green\">" +password + "</span>"*/
                                }
                            }

                            CheckBox{
                                //style: ButtonStyle
                                id: checkBox_rl
                                text:  effectiveflag=="1" ? "生效中" : "未生效"
                                checked: effectiveflag=="1" ? true : false
                                exclusiveGroup: listview_lr_group
                            }

//                            Text{
//                                id:effectivefligtext
//                                color: "red"

//                                anchors.right: listview.right
//                                width: listview.width/5

//                                text : effectiveflag=="1" ? "生效" : ""

//                            }
                        }

                        MouseArea {
                            id:mousearea
                            anchors.fill: parent

                            onClicked: {
                                listview_lr.currentIndex = index;

                                console.log("index------------------", listName);
                                strlistname  = listName;

                                console.log("msarea.pressed------------------", mousearea.pressed);

                                console.log("listview_lr.currentItem.y------------------", listview_lr.currentItem.y);
                                //effectivefligtext.text = effecflag
                            }
                        }
                        Component.onCompleted: {
                            console.log("mymodel count ===========================", mymodel.rowCount());
                        }
                    }
                }
    }

    //Buttons for sop release sign.
    Column{
        id:buttonrow
        x: listwiewrec.x + listwiewrec.width + 20
        y: listwiewrec.y
        height:listwiewrec.height
        spacing: buttonrow.height/9
        width:pg_CertList.width/9
        Button{
            id:button1
            text:qsTr("Add")
            width: pg_CertList.width/9
            height:buttonrow.height/7
            onClicked: {

                console.log("aaaaaaaaaaa")
                 showChild1()
                mymodel.getDataofList()
            }
        }
        Button{
            id:button2
            text:qsTr("Delete")
            width: pg_CertList.width/9
            height:buttonrow.height/7
            onClicked: {
                if (listview_lr.currentIndex >= 0)
                {
                    dialog_delete1.open();
                    mymodel.getDataofList();
                }else{
                    dialog_delete3.open();
                }

               // develmanager.deletecertificatelist(strlistname)
            }
        }


        Button{
            id:button3
            text:qsTr("Available")
            width: pg_CertList.width/9
            height:buttonrow.height/7
            onClicked: {

                develmanager.availablecertificatelist(strlistname)
                mymodel.getDataofList()
            }
        }

        Button{
               id:signaturebutton
               width: pg_CertList.width/9
               height:buttonrow.height/7
               text:qsTr("signature")
               onClicked: {
                   warnDlg.visible = false;

                       var component = Qt.createComponent("signature.qml")
                       if (component.status === Component.Ready) {
                           var dialog = component.createObject(pg_CertList);
                           //setDisabled();
                           //dialog.exited.connect(setEnabled);
                           //dialog.visible = true;
                      }
               }
            }
    }


}


