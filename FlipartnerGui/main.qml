import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4

import "./components"

Window {
    width: 800
    height: 480
    visible: true
    title: qsTr("Hello World")
    color: "black"

//    FileDialog {
//        id: fileDialog
//        title: qsTr("Please choose a file")
//        folder: shortcuts.home

//        onAccepted: {
//            console.log("You chose: " + fileDialog.fileUrls)
//        }
//        onRejected: {
//            console.log("Canceled")
//        }

//        Component.onCompleted: visible = false
//    }

    ListView {
        id: deviceList
        model: flipperDetector.devices
        anchors.fill: parent

        anchors.leftMargin: 100
        anchors.rightMargin: 100
        anchors.topMargin: parent.height/4
        anchors.bottomMargin: parent.height/4

        spacing: 6

        delegate: FlipperListDelegate {
            onFlashRequested: {
                console.log(serialNumber)
//                fileDialog.open()
            }
        }
    }

    Text {
        id: titleLabel
        anchors.bottom: deviceList.top
        anchors.bottomMargin: 40
        anchors.horizontalCenter: deviceList.horizontalCenter
        text: "Flipartner"
        font.pointSize: 24
        font.capitalization: Font.AllUppercase
        color: "white"
    }

    Text {
        id: subtitleLabel
        anchors.top: titleLabel.bottom
        anchors.horizontalCenter: titleLabel.horizontalCenter
        text: "a Flipper companion app"
        color: "darkgray"
        font.capitalization: Font.AllUppercase
    }

    Text {
        text: "a cool bottom line"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        color: "#333"
        font.capitalization: Font.AllUppercase
    }
}
