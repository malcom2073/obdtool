import Qt 4.7
import "../Button"

Rectangle {
	id:setup
	x:0
	y:0
	width:600
	height:400
	radius:10
	color:"orange"
	Behavior on x {  PropertyAnimation { properties: "x"; duration: 500 } }
	Rectangle {
		x:5
		y:5
		width:590
		height:390
		radius:10
		color:"black"
		Rectangle {
			x:0
			y:0
			width:120
			height:50
			Text {
				anchors.fill:parent
				horizontalAlignment:Text.AlignHCenter
				verticalAlignment:Text.AlignVCenter
				font.pixelSize:25
				text: "Com Port:"
			}
		}
		Rectangle {
			x:120
			y:0
			width:200
			height:50
			color:"grey"
			TextInput {
				id:setup_comport_textinput
				color:"white"
				font.pixelSize:25
				anchors.fill:parent
				MouseArea {
					anchors.fill:parent
					onClicked: {
						osk.y = 200
						osk.target = parent
					}
				}
				text: propertyMap["setup_comport_textinput"]
			}
		}
		Rectangle {
			x:0
			y:50
			width:120
			height:50
			Text {
				anchors.fill:parent
				horizontalAlignment:Text.AlignHCenter
				verticalAlignment:Text.AlignVCenter
				font.pixelSize:25
				text: "Baud Rate:"
			}
		}
		Rectangle {
			x:120
			y:50
			width:200
			height:50
			color:"grey"
			TextInput {
				id:setup_combaud_textinput
				color:"white"
				font.pixelSize:25
				anchors.fill:parent
				MouseArea {
					anchors.fill:parent
					onClicked: {
						osk.y = 200
						osk.target = parent
					}
				}
				text: propertyMap["setup_combaud_textinput"]
			}
		}
		Button {
			x:390
			y:290
			width:100
			height:100
			text: "Save"
			textSize:25
			onClicked: {
				setup.x = -900
				var comport = setup_comport_textinput.text;
				console.log("Saving com port:" + comport + " baud " + setup_combaud_textinput.text);
				window.saveComPort(setup_comport_textinput.text,setup_combaud_textinput.text);
			}
		}
		Button {
			x:490
			y:290
			width:100
			height:100
			text: "Cancel"
			textSize:25
			onClicked: {
				setup.x = -900
			}
		}
	}
}
