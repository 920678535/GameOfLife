import QtQuick 2.9
import QtQuick.Controls 2.2

ApplicationWindow {
    id: window
    visible: true
    width: 640
    height: 480
    title: qsTr("Stack")

    Label {
        text: qsTr("lulu")
        font.pixelSize: 22
        font.bold: true
        color: "red"
    }

//    Drawer {
//        id: drawer
//        width: 0.66 * window.width
//        height: window.height

//        Label {
//            text: "Content goes here!"
//            anchors.centerIn: parent
//        }

//        Column {
//            anchors.fill: parent

//            ItemDelegate {
//                text: qsTr("Page 1")
//                width: parent.width
//                onClicked: {
//                    stackView.push("Page1Form.ui.qml")
//                    drawer.close()
//                }
//            }
//            ItemDelegate {
//                text: qsTr("Page 2")
//                width: parent.width
//                onClicked: {
//                    stackView.push("Page2Form.ui.qml")
//                    drawer.close()
//                }
//            }
//        }
//    }
}
