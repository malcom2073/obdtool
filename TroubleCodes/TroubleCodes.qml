import Qt 4.7
import "../Button"

	Rectangle {
		id:troublecodewindow
		x:0
		y:0
		width:500
		height:500
		radius:10
		color:"orange"
		Behavior on x {  PropertyAnimation { properties: "x"; duration: 500 } }
		Rectangle {
			x:5
			y:5
			width:490
			height:490
			color:"black"
			Component {
				id: tcDelegate
				Item {
					width:300
					height:40
					Text {
						color:"white"
						anchors.fill:  parent
						font.pixelSize: 40
						text: modelData;
					}
				}
			}
			ListModel {
				id:testmodel
				ListElement {
					modelData: "P0214"
				}
				ListElement {
					modelData: "P0104"
				}
				ListElement {
					modelData: "B0003"
				}
				ListElement {
					modelData: "U0823"
				}
				
			}
			ListView {
				x:0
				y:0
				width:500
				height:400
				//model: propertyMap["troublemodel"];
				model: testmodel
				delegate: tcDelegate
				clip:true
			}
			Rectangle {
				x:0
				y:390
				width:100
				height:100
				radius:10
				color:"orange"
				Rectangle {
					x:5
					y:5
					width:90
					height:90
					radius: 10
					color:"black"
					MouseArea {
						x: 0
						y: 1
						anchors.rightMargin: 0
						anchors.bottomMargin: -1
						anchors.leftMargin: 0
						anchors.topMargin: 1
						anchors.fill: parent
						onClicked: {
							window.getTroubleCodes();
						}
						Text {
							font.pixelSize:20
							color:"white"
							anchors.fill: parent
							horizontalAlignment:Text.AlignHCenter
							verticalAlignment:Text.AlignVCenter
							text: "Get Trouble Codes"
							wrapMode: Text.WordWrap
						}
					}
				}
			}
		
			Rectangle {
				x:100
				y:390
				width:100
				height:100
				radius:10
				color:"orange"
				Rectangle {
					x:5
					y:5
					width:90
					height:90
					radius:10
					color:"black"
					MouseArea {
						anchors.fill: parent
						onClicked: {
							window.clearTroubleCodes();
						}
						Text {
							color:"white"
							font.pixelSize:20
							anchors.fill: parent
							horizontalAlignment:Text.AlignHCenter
							verticalAlignment:Text.AlignVCenter
							text: "Clear Trouble Codes"
							wrapMode: Text.WordWrap
						}
					}
				}
			}
			Rectangle {
				x:200
				y:390
				width:100
				height:100
				radius:10
				color:"orange"
				Rectangle {
					x:5
					y:5
					width:90
					height:90
					radius:10
					color:"black"
					MouseArea {
						anchors.fill: parent
						onClicked: {
						troublecodewindow.x = -500
						}
						Text {
							color:"white"
							font.pixelSize:20
							anchors.fill: parent
							horizontalAlignment:Text.AlignHCenter
							verticalAlignment:Text.AlignVCenter
							text: "Close Trouble Codes"
							wrapMode: Text.WordWrap
						}
					}
				}
			}
		}
	}
