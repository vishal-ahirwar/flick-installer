import QtQuick
import QtQuick.Window
import QtQuick.Controls

ApplicationWindow {
    id: loginPage
    visible: true
    width: 339
    height: 558
    color: "transparent"
    flags: Qt.FramelessWindowHint
    // visibility: "FullScreen"
    property var btnAbortAction: function () {
        Qt.quit()
    }
    Connections {
        target: DownloadManager
        onComplete: {
            btnAbort.text = "Reboot"
            btnQuit.visible=true
            btnAbortAction = function () {
                //TODO
                DownloadManager.reboot()
            }
        }
    }
    FontLoader {
        id: interFont
        source: "qrc:/font/Inter-VariableFont.ttf"
    }

    Rectangle {
        anchors.fill: parent
        radius: 14
        color: "#0C0D0D"
        Text {
            id: info
            visible: !btnStart.visible
            text: DownloadManager.file_name
            anchors.fill: btnStart
            horizontalAlignment: Text.AlignHCenter
            color: "#FFFFFF"
            font.family: interFont.name
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
                color: "#0033FD"
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
                    color: "#FFFFFF"
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
                                color: "#FFFFFF"
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
            font.pixelSize: 32
            text: "Flick\nInstaller"
            color: "#FFFFFF"
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            anchors.top: parent.top
            anchors.topMargin: 55
            font.family: interFont.name
        }
        Text {
            font.pixelSize: 12
            text: "BY Vishal Ahirwar"
            color: "gray"
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 55
            font.family: interFont.name
        }

        Button {
            id: btnStart
            text: "Install"
            anchors.centerIn: parent
            width: 217
            height: 50
            visible: DownloadManager.can_procceed
            background: Rectangle {
                width: parent.width
                height: parent.height
                radius: 14
                color: "#0033FD"
            }
            contentItem: Text {

                text: parent.text
                color: "#FFFFFF"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 32
                font.family: interFont.name
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

            width: 217
            height: 50
            background: Rectangle {
                width: parent.width
                height: parent.height
                radius: 14
                color: "#0033FD"
            }
            contentItem: Text {
                font.pixelSize: 32
                text: parent.text
                color: "#FFFFFF"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.family: interFont.name
            }
            onClicked: {
                btnAbortAction()
            }
        }
        Button {
            visible:false
            id: btnQuit
            text: "Quit"
            anchors {
                top: btnAbort.bottom
                left: btnAbort.left
                right: btnAbort.right
                topMargin: 25
            }

            width: 217
            height: 50
            background: Rectangle {
                width: parent.width
                height: parent.height
                radius: 14
                color: "#fd0000"
            }
            contentItem: Text {
                font.pixelSize: 32
                text: parent.text
                color: "#FFFFFF"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.family: interFont.name
            }
            onClicked: {
                Qt.quit()
            }
        }
    }
}
