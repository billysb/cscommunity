"Resource/UI/BuySubMachineguns_TER.res"
{
	"BuySubMenu"
	{
		"ControlName"		"WizardSubPanel"
		"fieldName"		"BuySubMenu"
		"xpos"			"0"
		"ypos"			"0"
		"wide"			"640"
		"tall"			"480"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
	}

	"Title"
	{
		"ControlName"		"Label"
		"fieldName"		"Title"
				"xpos"		"52"
		"ypos"		"22"
		"wide"		"500"
		"tall"		"48"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
		"labelText"		"#Cstrike_SubmachinegunsLabel"
		"textAlignment"		"west"
		"dulltext"		"0"
		"brighttext"		"0"
		"font"		"MenuTitle"
		"wrap"		"0"
	}

	"ItemInfo"
	{
		"ControlName"		"Panel"
		"fieldName"		"ItemInfo"
		"xpos"		"244"
		"ypos"		"116"
		"wide"		"400"
		"tall"		"380"
		"autoResize"		"3"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
	}

	"mac10"
	{
		"ControlName"		"MouseOverPanelButton"
		"fieldName"		"mac10"
				"xpos"		"52"
		"ypos"		"116"
		"wide"		"170"
		"tall"		"20"
		"autoResize"		"0"
		"pinCorner"		"2"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
		"labelText"		"#Cstrike_MAC10"
		"textAlignment"		"west"
		"dulltext"		"0"
		"brighttext"		"0"
		"command"		"buy mac10"
		"cost"			"1400"
	}

	"mp5"
	{
		"ControlName"		"MouseOverPanelButton"
		"fieldName"		"mp5"
				"xpos"		"52"
		"xpos"		"0"
		"ypos"		"148"
		"wide"		"170"
		"tall"		"20"
		"pinCorner"		"2"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
		"labelText"		"#Cstrike_MP5"
		"textAlignment"		"west"
		"dulltext"		"0"
		"brighttext"		"0"
		"command"		"buy mp5navy"
		"cost"			"1500"
		"as_restrict"		"1"
	}

	"ump45"
	{
		"ControlName"		"MouseOverPanelButton"
		"fieldName"		"ump45"
				"xpos"		"52"
		"ypos"		"180"
		"wide"		"170"
		"tall"		"20"
		"autoResize"		"0"
		"pinCorner"		"2"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
		"labelText"		"#Cstrike_UMP45"
		"textAlignment"		"west"
		"dulltext"		"0"
		"brighttext"		"0"
		"command"		"buy ump45"
		"cost"			"1700"
	}

	"p90"
	{
		"ControlName"		"MouseOverPanelButton"
		"fieldName"		"p90"
				"xpos"		"52"
		"ypos"		"212"
		"wide"		"170"
		"tall"		"20"
		"autoResize"		"0"
		"pinCorner"		"2"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
		"labelText"		"#Cstrike_P90"
		"textAlignment"		"west"
		"dulltext"		"0"
		"brighttext"		"0"
		"command"		"buy p90"
		"cost"			"2350"
		"as_restrict"		"1"
	}

	"CancelButton"
	{
		"ControlName"		"MouseOverPanelButton"
		"fieldName"		"CancelButton"
				"xpos"		"52"
		"ypos"		"380"
		"wide"		"170"
		"tall"		"20"
		"autoResize"		"0"
		"pinCorner"		"2"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
		"labelText"		"#Cstrike_Cancel"
		"textAlignment"		"west"
		"dulltext"		"0"
		"brighttext"		"0"
		"Command"		"vguicancel"
		"Default"		"1"
	}
	"MarketSticker"
	{
		"ControlName"	"ImagePanel"
		"fieldName"	"MarketSticker"
		"image"			"gfx/vgui/market_sticker_category"
		"xpos"	"508"
		"ypos"	"250"
		"wide"	"84"
		"tall"	"84"
		"autoresize"	"2"
		"pinCorner"	"0"
		"visible"	"1"
		"enabled"	"1"
		"tabPosition"	"0"
		"vpos"	"-1"
		"scaleImage"	"1"
	}
}
