import QtQuick 1.0

Rectangle {
 property int frameSize: 300
 property int leftMargin: 10
 property int topMargin: 25
 property int bottomMargin: 65
 property int fontSize: 20
 property color frameColor: "#FFF8DC" // cornsilk
 
width: 3 * frameSize; height: frameSize
 
// Photo 1
 Rectangle {
 x: 0; y: 0
 width: frameSize; height: frameSize
 color: frameColor
 
Image {
 x: leftMargin; y: topMargin
 source: "/home/ubuntu/Desktop/Capture.PNG"
 }
 
Text {
 x: 0; y: frameSize - bottomMargin
 text: "AAAAAAAAAAAAAAAAAAAAAAA"
 font.pixelSize: fontSize
 width: frameSize; horizontalAlignment: Text.AlignHCenter
 height: bottomMargin; verticalAlignment: Text.AlignVCenter
 }
 }
 
// Photo 2
 Rectangle {
 x: frameSize; y: 0
 width: frameSize; height: frameSize
 color: frameColor
 
Image {
 x: leftMargin; y: topMargin
 source: "/home/ubuntu/Desktop/a.jpg"
 }
 
Text {
 x: 0; y: frameSize - bottomMargin
 text: "BBBBBBBBBBBBBBBBBBBBBB"
 font.pixelSize: fontSize
 width: frameSize; horizontalAlignment: Text.AlignHCenter
 height: bottomMargin; verticalAlignment: Text.AlignVCenter
 }
 }
 
// Photo 3
 Rectangle {
 x: 2 * frameSize; y: 0
 width: frameSize; height: frameSize
 color: frameColor
 
Image {
 x: leftMargin; y: topMargin
 source: "/home/ubuntu/Desktop/b.jpg"
 }
 
Text {
 x: 0; y: frameSize - bottomMargin
 text: "CCCCCCCCCCCCCCCCCCCC"
 font.pixelSize: fontSize
 width: frameSize; horizontalAlignment: Text.AlignHCenter
 height: bottomMargin; verticalAlignment: Text.AlignVCenter
 }
 }
}

