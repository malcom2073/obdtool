import Qt 4.7
import GaugeImage 0.1

Rectangle {
	x:0
	y:0
	width: 1280
	height: 800
	color: "black"

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
		y:150
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
		x: 550
		y: 150
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
		width:250
		height: 250
		minimum:0
		maximum: 140
		numLabels: 7
		startAngle: 45
		endAngle: 315
		Behavior on m_value { PropertyAnimation { properties: "m_value"; duration: (propertyMap["010F_DURATION"] ? propertyMap["010F_DURATION"] : 500) } }
		m_value: (propertyMap["010F"] ? propertyMap["010F"] : 0)
	  
	}

}
