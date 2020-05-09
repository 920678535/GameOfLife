import QtQuick 2.9
import QtQuick.Controls 2.2

import net.phoneyou.LULU.BB 1.0 as BB

ApplicationWindow {
    id: window
    visible: true
    width: 640
    height: 480
    title: qsTr("GAME OF LIFE")
    font: Qt.application.font

    Button {
        text: qsTr("arguments")
        font.pixelSize: 22
        font.bold: true
        onClicked: drawer.open()
    }

    Drawer {
        id: drawer
        width: 0.66 * window.width
        height: window.height

        Label {
            text: qsTr("parameter configuration。")
            anchors.centerIn: parent
        }

        Column {
            anchors.fill: parent

            ItemDelegate {
                text: qsTr("Page 1")
                width: parent.width
                onClicked: {
                    drawer.close()
                }
            }
            ItemDelegate {
                text: qsTr("Page 2")
                width: parent.width
                onClicked: {
                    drawer.close()
                }
            }
        }
    }
}
