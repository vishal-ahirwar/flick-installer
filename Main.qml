import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
ApplicationWindow {
    id: loginPage
    visible: true
    width: 456; height: 529
    color: "transparent"
    flags: Qt.FramelessWindowHint
    property var btnAbortAction:function(){
                    Qt.quit()
    }
    Connections{
        target: DownloadManager
        onComplete:{
            btnAbort.text="Restart System"
            btnAbortAction=function(){
                //TODO
                DownloadManager.reboot()
            }
        }
    }

    Rectangle{
        anchors.fill: parent
        radius: 25
        color: "#0C0C0C"
        ProgressBar{

            Text{
                id:progressBarText
                text: DownloadManager.file_name
                anchors{
                    top:parent.bottom
                    left: parent.left
                    right:parent.right
                    topMargin: 15
                }
                horizontalAlignment: Text.AlignHCenter
                color: "#FFFFFF"
            }

            id:progressBar
            anchors.centerIn: parent
            visible: false
            width: parent.width*0.75
            height: 15
            value:DownloadManager.download_progress

            background: Rectangle{
                radius: 18
                color: "#FFFFFF"
                width: parent.width
                height:parent.height
            }
            contentItem: Item {
                     implicitWidth: 200
                     implicitHeight: 4

                     // Progress indicator for determinate state.
                     Rectangle {
                         width: progressBar.visualPosition * parent.width
                         height: parent.height
                         radius: 18
                         color: "#69DC9E"
                         visible: !progressBar.indeterminate
                     }

                     // Scrolling animation for indeterminate state.
                     Item {
                         anchors.fill: parent
                         visible: progressBar.indeterminate
                         clip: true

                         Row {
                             spacing: 20

                             Repeater {
                                 model: progressBar.width / 40 + 1

                                 Rectangle {
                                     color: "#17a81a"
                                     width: 20
                                     height: progressBar.height
                                 }
                             }
                             XAnimator on x {
                                 from: 0
                                 to: -40
                                 loops: Animation.Infinite
                                 running: progressBar.indeterminate
                             }
                         }
                     }}
        }

        Text{
            font.bold: true
            font.pixelSize: 52
            text: "Aura Installer"
            color: "#FFFFFF"
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            anchors.top: parent.top
            anchors.topMargin: 55
        }
        Text{
            font.pixelSize: 14
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 55
            text:"Copyright ©2025 Vishal Ahirwar.All rights reseved."
            horizontalAlignment: Text.AlignHCenter
            width: parent.width
            color: "#BFFFFFFF"
            font.bold: true
        }

        Button{
            id:btnStart
            text: "Start Installation"
            anchors.centerIn: parent
            width: 145
            height: 55
            background: Rectangle{
                width: parent.width
                height:parent.height
                radius: 18
                color:  "#69DC9E"}
            contentItem: Text {

                text:parent.text
                color: "#FFFFFF"
                font.bold: false
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
            }
            onClicked: {
                visible=false
                progressBar.visible=true
                DownloadManager.start()
            }
        }
        Button{
            id:btnAbort
            text: "Abort"
            anchors{
                top:btnStart.bottom
                left:btnStart.left
                right: btnStart.right
                topMargin: 25
            }

            width: 145
            height: 55
            background: Rectangle{
                width: parent.width
                height:parent.height
                radius: 18
                color: "#E59F71"

            }
            contentItem: Text {
                font.pixelSize: 16
                text:parent.text
                color: "#FFFFFF"
                font.bold: false
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

            }
            onClicked: {
                btnAbortAction()
            }
        }

    }
}
