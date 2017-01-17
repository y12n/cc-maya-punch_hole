//
//  AETemplates.h
//  pounchHole
//
//  Created by Hunyadi J�nos on 2015. 01. 03..
//  Copyright (c) 2015. Janos Hunyadi. All rights reserved.
//

#ifndef MELSCRIPTS_H
#define MELSCRIPTS_H

#include <maya/MString.h>


MString mel_AETemplate()
{
	MString s_aeTemplate = MString() + "global proc AEpunchHoleTemplate( string $nodeName )" +
		"{" +
		"	editorTemplate -beginScrollLayout;" +
		"	editorTemplate -beginLayout \"Main control\" -collapse 0;" +
		"	editorTemplate -addControl \"offset\";" +
		"	editorTemplate -addControl \"rotate\";" +
		"	editorTemplate -addControl \"reverseOrder\";" +
		"	editorTemplate -endLayout;" +
		"	editorTemplate -beginLayout \"Profile\" -collapse 0;" +
		"	editorTemplate -addControl \"profilePresets\";" +
		"	editorTemplate -addControl \"extrusionCount\";" +
		"	editorTemplate -addControl \"extrusionOffset\";" +
		"	editorTemplate -addControl \"normalOffset\";" +
		"	" +
		"	AEaddRampControl( $nodeName + \".extrudeRamp\" );" +
		"	" +
		"	editorTemplate -endLayout;" +
		"	editorTemplate -suppress \"inMesh\";" +
		"	editorTemplate -suppress \"outMesh\";" +
		"	editorTemplate -suppress \"vertNum\";" +
		"	AEdependNodeTemplate $nodeName;" +
		"	editorTemplate -addExtraControls;" +
		"	editorTemplate -endScrollLayout;" +
		"}";


	return s_aeTemplate;

}

MString mel_createShelf()
{

	MString s_aeTemplate = MString() + 
		"int $cc_doesShelfExist = `shelfLayout -query -ex \"CreativeCase\"`;\r\n"
		"if ($cc_doesShelfExist == 1)\r\n"
		"{\r\n"
		"	string $shelfButtons[] = `shelfLayout -q -ca \"CreativeCase\"`;\r\n"
		"	int $ex_b01,$ex_b02 = 0;\r\n"
		"	for( $i=0; $i<size($shelfButtons); ++$i )\r\n"
		"	{\r\n"
		"		if( `control -exists $shelfButtons[$i]` == true)\r\n"
		"		{\r\n"
		"			if (`control -q -docTag $shelfButtons[$i]` == \"ph_createPhButton\") {$ex_b01 = 1;}\r\n"
		"		}\r\n"
		"	}\r\n"
		"	if ($ex_b01 == 0) {shelfButton -p \"CreativeCase\" -dtg \"ph_createPhButton\" -annotation \"Apply a Puncher modifier on the selected verts\" -image1 \"puncher.png\" -command \"punchHoleCommand\";}\r\n"
		"}\r\n"
		"	\r\n"
		"if ($cc_doesShelfExist == false)\r\n"
		"{\r\n"
		"		shelfLayout -cellWidth 33 -cellHeight 33 -p $gShelfTopLevel CreativeCase;\r\n"
		"		shelfButton -p \"CreativeCase\" -dtg \"ph_createPhButton\" -annotation \"Apply a Puncher modifier on the selected verts\" -image1 \"puncher.png\" -command \"punchHoleCommand\";\r\n"
		"}\r\n";


	return s_aeTemplate;
}


#endif