import QtQuick 1.1

Rectangle {
    id: window;
    width: 800;
    height: 480;

    Rectangle {
        id: rectYellow;
        width: 400;
        height: 300;
        x: 300;
        y: 200;
        color: "yellow";

        MouseArea {
            drag {
                target: rectYellow;
                minimumX: 0;
                minimumY: 0;
                maximumX: (rectYellow.parent.width - rectYellow.width);
                maximumY: (rectYellow.parent.height - rectYellow.height);
            }
            anchors.fill: parent;
        }
        Rectangle {
            id: rectGreen;
            x: 100;
            y: 100;
            width: 50;
            height: 50;
            color: "green";

            MouseArea {
                drag {
                    target: rectGreen;
                    minimumX: 0;
                    minimumY: 0;
                    maximumX: (rectGreen.parent.width - rectGreen.width);
                    maximumY: (rectGreen.parent.height - rectGreen.height);
                }
                anchors.fill: parent;
            }
        }
    }
}
