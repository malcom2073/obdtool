import Qt 4.7
//import "Gauge"
import "Button"
import "Osk"
import GaugeImage 0.1
import "Setup"
import "TroubleCodes"
Rectangle {
	x:0
	y:0
	width: 1280
	height: 800
	color: "black"
	Text {
		x: 100
		y: 100
		width: 300
		height: 50
		text: (propertyMap["010C"] ? propertyMap["010C"] : "");
	}
	Text {
		x: 500
		y: 600
		width: 300
		height: 50
		text: (propertyMap["consoleMessage"] ? propertyMap["consoleMessage"] : "");
	}
	Text {
		x: 500
		y: 650
		width: 300
		height: 50
		text: (propertyMap["connectedMessage"] ? propertyMap["connectedMessage"] : "");
	}


	GaugeImage {
		x:0
		y:0
		width:200
		height:200
		minimum:-40
		maximum:200
		numLabels:3
		startAngle:45
		endAngle:270
		Behavior on m_value {  PropertyAnimation { properties: "m_value"; duration: (propertyMap["0105_DURATION"] ? propertyMap["0105_DURATION"] : 500) } }
		m_value: (propertyMap["0105"] ? propertyMap["0105"] : 0)

	}

	GaugeImage {
		x:425
		y:0
		width:250
		height:250
		minimum:0
		maximum:655
		numLabels:3
		startAngle:90
		endAngle:270
		Behavior on m_value {  PropertyAnimation { properties: "m_value"; duration: (propertyMap["0110_DURATION"] ? propertyMap["0110_DURATION"] : 500) } }
                m_value: (propertyMap["0110"] ? propertyMap["0110"] : 0) * 10

	}

	GaugeImage {
		x:0
		y:250
		width:550
		height:550
		minimum: 0
		maximum: 7000
		numLabels: 7
		startAngle: 45
		endAngle: 315
		Behavior on m_value {  PropertyAnimation { properties: "m_value"; duration: (propertyMap["010C_DURATION"] ? propertyMap["010C_DURATION"] : 500) } }
		m_value: (propertyMap["010C"] ? propertyMap["010C"] : 0)
	}
	GaugeImage {
		x: 640
		y: 250
		width:550
		height: 550
		minimum:0
		maximum: 140
		numLabels: 7
		startAngle: 45
		endAngle: 315
		Behavior on m_value { PropertyAnimation { properties: "m_value"; duration: (propertyMap["010D_DURATION"] ? propertyMap["010D_DURATION"] : 500) } }
		m_value: (propertyMap["010D"] ? propertyMap["010D"] : 0)
	  
	}

	GaugeImage {
		x: 975
		y: 0
		width:300
		height: 300
		minimum:0
		maximum: 140
		numLabels: 7
		startAngle: 45
		endAngle: 315
		Behavior on m_value { PropertyAnimation { properties: "m_value"; duration: (propertyMap["010F_DURATION"] ? propertyMap["010F_DURATION"] : 500) } }
		m_value: (propertyMap["010F"] ? propertyMap["010F"] : 0)
	  
	}
	Rectangle {
		id:taskbar
		x:-1180
		y:700
		width:1280
		height:100
		Rectangle {
			x:0
			y:0
			width:200
			height:100
			color: "orange"
			radius: 10
			Rectangle {
				radius: 10
				x:5
				y:5
				width:190
				height:90
				color: "black"
				MouseArea {
					anchors.fill: parent
					onClicked: {
						Qt.quit()
				}
					Text {
						font.pixelSize: 30
						color: "white"
						anchors.fill: parent
						horizontalAlignment:Text.AlignHCenter
						verticalAlignment:Text.AlignVCenter
						text: "Quit"
					}
				}
			}
		}
		Rectangle {
			x:200
			y:0
			width:200
			height:100
			radius: 10
			color: "orange"
			Rectangle {
				radius: 10
				color: "black"
				x: 5
				y: 5
				width: 190
				height: 90
				MouseArea {
					x: 0
					y: 0
					anchors.rightMargin: 0
					anchors.bottomMargin: 0
					anchors.leftMargin: 0
					anchors.topMargin: 0
					anchors.fill: parent
					onClicked: {
						window.connect();
						//Qt.quit()
					}
					Text {
						font.pixelSize: 30
						color: "white"
						anchors.fill: parent
						horizontalAlignment:Text.AlignHCenter
						verticalAlignment:Text.AlignVCenter
						text: "Connect"
					}
				}
			}
		}
		Rectangle {
			x:400
			y:0
			width:200
			height:100
			color: "orange"
			radius: 10
			Rectangle {
				x:5
				y:5
				width:190
				height:90
				radius: 10
				color: "black"
				MouseArea {
					anchors.fill: parent
					onClicked: {
						window.disconnect();
					}
					Text {
						font.pixelSize:30
						color: "white"
						anchors.fill: parent
						horizontalAlignment:Text.AlignHCenter
						verticalAlignment:Text.AlignVCenter
						text: "Disconnect"
					}
				}
			}
		}	
		Button {
			x:600
			y:0
			width:200
			height:100
			outerColor:"orange"
			textColor:"orange"
			text:"Setup"
			onClicked: {
				setup.x = 200
			}
		}
		Rectangle {
			x:980
			y:0
			width:200
			height:100
			color: "orange"
			radius: 10
			Rectangle {
				x:5
				y:5
				width:190
				height:90
				radius: 10
				color: "black"
				MouseArea {
					anchors.fill: parent
					onClicked: {
						troublecodewindow.x = 300
					}
					Text {
						font.pixelSize:30
						color: "white"
						anchors.fill: parent
						horizontalAlignment:Text.AlignHCenter
						verticalAlignment:Text.AlignVCenter
						text: "Trouble Codes"
					}
				}
			}
		}
		property bool expanded:false
		Behavior on x {  PropertyAnimation { properties: "x"; duration: 500 } }
		Button {
			x:1180
			y:0
			width:100
			height:100
			onClicked: {
				if (parent.expanded)
				{
					parent.expanded = false;
					taskbar.x = -1180
				}
				else
				{
					parent.expanded = true;
					taskbar.x = 0
				}
			}
		}
		
	}
	TroubleCodes {
		id:troublecodewindow
		x:-500
		y:100
	}
	Osk {
		id:osk
		z:1
		x:100
		y:-500
		width:1100
		height:500
		property TextInput target;
		Behavior on y {  PropertyAnimation { properties: "y"; duration: 500 } }
		onOkHit: {
			target.text = text;
			osk.y = -500
		}
	}
	Setup {
		id: setup
		x:-900
		y:200
		width:600
		height: 400
	}
}
