import Qt 4.7



Rectangle {
	id:outerrect
	radius:10
	color:outerColor
	signal clicked;
	Rectangle {
		color:innerColor
		id:innerrect
		x: 5
		y:5
		width:parent.width-10
		height:parent.height-10
		radius:parent.radius
		MouseArea {
			anchors.fill:parent
			Text {
				anchors.fill:parent
				font.pixelSize: textSize
				color:textColor
				horizontalAlignment:Text.AlignHCenter
				verticalAlignment:Text.AlignVCenter
				text: outerrect.text
			}
			onClicked: {
				outerrect.clicked();
			}
		}
	}
	property color textColor:"white"
	property int textSize:40
	property string text:""
	property string outerColor:"red"
	property string innerColor:"black"
}