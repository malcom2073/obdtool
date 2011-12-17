import Qt 4.7


import "../Button"

//OSK
Rectangle {
	width:1100
	height:500
	color:"grey"
	property string text:""
	property bool shift:false
	property bool alt:false
	property bool caps:false
        TextInput {
		id: finaltext
		x:0
		y:0
		width:1000
		height:100
		color:"white"
		font.pixelSize:40
		text: parent.text
	}
	Component.onCompleted: {
	}
	ListModel {
		id: buttonmodel
	}
	Button {
		id:buttonq
		x:0
		y:100
		width:100
		height:100
		property string maintext:"q"
		property string alttext:"1"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		Component.onCompleted: {
			buttonmodel.append({"button": buttonq});
		}
	}
	Button {
		id:buttonw
		x:100
		y:100
		width:100
		height:100
		property string maintext:"w"
		property string alttext:"2"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		Component.onCompleted: {
			buttonmodel.append({"button": buttonw});
		}
	}
	Button {
		id:buttone
		x:200
		y:100
		width:100
		height:100
		property string maintext:"e"
		property string alttext:"3"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		Component.onCompleted: {
			buttonmodel.append({"button": buttone});
		}
	}
	Button {
		x:300
		id:buttonr
		y:100
		width:100
		height:100
		property string maintext:"r"
		property string alttext:"4"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		Component.onCompleted: {
			buttonmodel.append({"button": buttonr});
		}
	}
	Button {
		id:buttont
		x:400
		y:100
		width:100
		height:100
		property string maintext:"t"
		property string alttext:"5"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		
		Component.onCompleted: {
			buttonmodel.append({"button": buttont});
		}
	}
	Button {
		id:buttony
		x:500
		y:100
		width:100
		height:100
		property string maintext:"y"
		property string alttext:"6"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		Component.onCompleted: {
			buttonmodel.append({"button": buttony});
		}
	}
	Button {
		id:buttonu
		x:600
		y:100
		width:100
		height:100
		property string maintext:"u"
		property string alttext:"7"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		Component.onCompleted: {
			buttonmodel.append({"button": buttonu});
		}
	}
	Button {
		id:buttoni
		x:700
		y:100
		width:100
		height:100
		property string maintext:"i"
		property string alttext:"8"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		Component.onCompleted: {
			buttonmodel.append({"button": buttoni});
		}
	}
	Button {
		id:buttono
		x:800
		y:100
		width:100
		height:100
		property string maintext:"o"
		property string alttext:"9"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		Component.onCompleted: {
			buttonmodel.append({"button": buttono});
		}
	}
	Button {
		id:buttonp
		x:900
		y:100
		width:100
		height:100
		property string maintext:"p"
		property string alttext:"0"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		Component.onCompleted: {
			buttonmodel.append({"button": buttonp});
		}
	}
	Button {
		id:buttonbk
		x:1000
		y:100
		width:100
		height:100
		property string maintext:"Back"
		property string alttext:"Del"
		text: maintext
		onClicked: {
			finaltext.text=window.mid(finaltext.text,0,finaltext.text.length-1);
		}
		Component.onCompleted: {
			buttonmodel.append({"button": buttonbk});
		}
	}
	Button {
		x:30
		y:200
		width:100
		height:100
		property string maintext:"a"
		property string alttext:"!"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttona
		Component.onCompleted: {
			buttonmodel.append({"button": buttona});
		}
	}
	Button {
		x:130
		y:200
		width:100
		height:100
		property string maintext:"s"
		property string alttext:"@"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttons
		Component.onCompleted: {
			buttonmodel.append({"button": buttons});
		}
	}
	Button {
		x:230
		y:200
		width:100
		height:100
		property string maintext:"d"
		property string alttext:"#"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttond
		Component.onCompleted: {
			buttonmodel.append({"button": buttond});
		}
	}
	Button {
		x:330
		y:200
		width:100
		height:100
		property string maintext:"f"
		property string alttext:"$"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttonf
		Component.onCompleted: {
			buttonmodel.append({"button": buttonf});
		}
	}
	Button {
		x:430
		y:200
		width:100
		height:100
		property string maintext:"g"
		property string alttext:"%"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttong
		Component.onCompleted: {
			buttonmodel.append({"button": buttong});
		}
	}
	Button {
		x:530
		y:200
		width:100
		height:100
		property string maintext:"h"
		property string alttext:"^"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttonh
		Component.onCompleted: {
			buttonmodel.append({"button": buttonh});
		}
	}
	Button {
		x:630
		y:200
		width:100
		height:100
		property string maintext:"j"
		property string alttext:"&"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttonj
		Component.onCompleted: {
			buttonmodel.append({"button": buttonj});
		}
	}
	Button {
		x:730
		y:200
		width:100
		height:100
		property string maintext:"k"
		property string alttext:"*"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttonk
		Component.onCompleted: {
			buttonmodel.append({"button": buttonk});
		}
	}
	Button {
		x:830
		y:200
		width:100
		height:100
		property string maintext:"l"
		property string alttext:"("
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttonl
		Component.onCompleted: {
			buttonmodel.append({"button": buttonl});
		}
	}
	Button {
		x:60
		y:300
		width:100
		height:100
		property string maintext:"z"
		property string alttext:")"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttonz
		Component.onCompleted: {
			buttonmodel.append({"button": buttonz});
		}
	}
	Button {
		x:160
		y:300
		width:100
		height:100
		property string maintext:"x"
		property string alttext:"-"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttonx
		Component.onCompleted: {
			buttonmodel.append({"button": buttonx});
		}
	}
	Button {
		x:260
		y:300
		width:100
		height:100
		property string maintext:"c"
		property string alttext:"_"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttonc
		Component.onCompleted: {
			buttonmodel.append({"button": buttonc});
		}
	}
	Button {
		x:360
		y:300
		width:100
		height:100
		property string maintext:"v"
		property string alttext:"="
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttonv
		Component.onCompleted: {
			buttonmodel.append({"button": buttonv});
		}
	}
	Button {
		x:460
		y:300
		width:100
		height:100
		property string maintext:"b"
		property string alttext:"+"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttonb
		Component.onCompleted: {
			buttonmodel.append({"button": buttonb});
		}
	}

	Button {
		x:560
		y:300
		width:100
		height:100
		property string maintext:"n"
		property string alttext:"|"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttonn
		Component.onCompleted: {
			buttonmodel.append({"button": buttonn});
		}
	}
	Button {
		x:660
		y:300
		width:100
		height:100
		property string maintext:"m"
		property string alttext:"\\"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttonm
		Component.onCompleted: {
			buttonmodel.append({"button": buttonm});
		}
	}
	Button {
		x:760
		y:300
		width:100
		height:100
		property string maintext:"["
		property string alttext:"{"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttonb1
		Component.onCompleted: {
			buttonmodel.append({"button": buttonb1});
		}
	}
	Button {
		x:860
		y:300
		width:100
		height:100
		property string maintext:"]"
		property string alttext:"}"
		text: maintext
		onClicked: {
			finaltext.text+=text
		}
		id:buttonb2
		Component.onCompleted: {
			buttonmodel.append({"button": buttonb2});
		}
	}
	Button {
		x:200
		y:400
		width:400
		height:100
		text: "Space"
		onClicked: {
			finaltext.text+=" "
		}
	}
	function capsHit()
	{
	}
	function shiftHit()
	{
			for (var i=0;i<buttonmodel.count;i++)
			{
				if (shift)
				{
					buttonmodel.get(i).button.text = window.toLower(buttonmodel.get(i).button.text);
				}
				else
				{
					buttonmodel.get(i).button.text = window.toUpper(buttonmodel.get(i).button.text);
				}
			}
			if (shift)
			{
				shift = false;
			}
			else
			{
				shift = true;
			}
	}
	function altHit()
	{
		for (var i=0;i<buttonmodel.count;i++)
		{
			if (alt)
			{
				buttonmodel.get(i).button.text = buttonmodel.get(i).button.alttext;
			}
			else
			{
				buttonmodel.get(i).button.text = buttonmodel.get(i).button.maintext;
			}
		}
		if (alt)
		{
			alt = false;
		}
		else
		{
			alt = true;
		}
	}
	Button {
		x:0
		y:400
		width:200
		height:100
		text: "Shift"
		onClicked: {
			parent.shiftHit();
		}
	}
	Button {
		x:200
		y:400
		width:400
		height:100
		text: "Space"
		onClicked: {
			finaltext.text+=" "
		}
	}
	Button {
		x:600
		y:400
		width:200
		height:100
		text: "Alt"
		onClicked: {
			//finaltext.text+=" "
			parent.altHit();
		}
	}
	signal okHit(string text)
	Button {
		x:700
		y:400
		width:200
		height:100
		text: "Ok"
		onClicked: {
			parent.okHit(finaltext.text);
			finaltext.text = "";
		}
	}
}
