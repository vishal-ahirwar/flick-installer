import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

ApplicationWindow {
    id: loginPage
    visible: true
    width: 400
    height: 529
    color: "transparent"
    flags: Qt.FramelessWindowHint
    property var btnAbortAction: function () {
        Qt.quit()
    }
    Connections {
        target: DownloadManager
        onComplete: {
            btnAbort.text = "Restart System"
            btnAbortAction = function () {
                //TODO
                DownloadManager.reboot()
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        radius: 25
        color: "white"

        Rectangle {
            anchors.left: parent.left
            color: "#c2dfe3"
            width: parent.width * 0.5
            height: width
            radius: 999
            opacity: 0.4
            anchors.margins: 15
        }
        Rectangle {
            anchors.right: parent.right
            color: "#c2dfe3"
            width: parent.width * 0.5
            height: width
            radius: 999
            opacity: 0.4
            anchors.margins: 15
        }
        Rectangle {
            anchors.top: parent.top
            color: "#c2dfe3"
            width: parent.width * 0.5
            height: width
            radius: 999
            opacity: 0.4
            anchors.margins: 15
        }
        Rectangle {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            color: "#c2dfe3"
            width: parent.width * 0.5
            height: width
            radius: 999
            opacity: 0.4
            anchors.margins: 15
        }
        Rectangle {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            color: "#c2dfe3"
            width: parent.width * 0.5
            height: width
            radius: 999
            opacity: 0.4
            anchors.margins: 15
        }
        Text {
            id: info
            visible: !btnStart.visible
            text: DownloadManager.file_name
            anchors.fill: btnStart
            horizontalAlignment: Text.AlignHCenter
            color: "#253237"
        }
        ProgressBar {

            id: progressBar
            anchors.centerIn: parent
            visible: false
            width: parent.width * 0.75
            height: 15
            value: DownloadManager.download_progress

            background: Rectangle {
                radius: 18
                color: "#253237"
                width: parent.width
                height: parent.height
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
                }
            }
        }

        Text {
            font.bold: true
            font.pixelSize: 52
            text: "Solix Installer"
            color: "#253237"
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            anchors.top: parent.top
            anchors.topMargin: 55
        }
        Text {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 55
            font.pixelSize: 14
            text: "Copyright ©2025 Vishal Ahirwar.All rights reseved."
            horizontalAlignment: Text.AlignHCenter
            width: parent.width
            color: "#253237"
            font.bold: true
        }

        Button {
            id: btnStart
            text: "Start Installation"
            anchors.centerIn: parent
            width: 145
            height: 55
            visible: DownloadManager.can_procceed
            background: Rectangle {
                width: parent.width
                height: parent.height
                radius: 10
                color: "#c2dfe3"
            }
            contentItem: Text {

                text: parent.text
                color: "#253237"
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
            }
            onClicked: {
                visible = false
                progressBar.visible = true
                DownloadManager.start()
            }
        }
        Button {
            id: btnAbort
            text: "Abort"
            anchors {
                top: btnStart.bottom
                left: btnStart.left
                right: btnStart.right
                topMargin: 25
            }

            width: 145
            height: 55
            background: Rectangle {
                width: parent.width
                height: parent.height
                radius: 10
                color: "#9db4c0"
            }
            contentItem: Text {
                font.pixelSize: 16
                text: parent.text
                color: "#253237"
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            onClicked: {
                btnAbortAction()
            }
        }
    }
}
