//
//  AETemplates.h
//  pounchHole
//
//  Created by Hunyadi János on 2015. 01. 03..
//  Copyright (c) 2015. Janos Hunyadi. All rights reserved.
//

#ifndef MELSCRIPTS_H
#define MELSCRIPTS_H

#include <maya/MString.h>


MString mel_AETemplate()
{
	MString s_aeTemplate = MString() + "global proc AEpunchHoleTemplate( string $nodeName )\r\n"
		"{\r\n"
		"	editorTemplate -beginScrollLayout;\r\n"
		"	editorTemplate -beginLayout \"Main control\" -collapse 0;\r\n"
		"	editorTemplate -addControl \"offset\";\r\n"
		"	editorTemplate -addControl \"rotate\";\r\n"
		"	editorTemplate -addControl \"reverseOrder\";\r\n"
		"	editorTemplate -endLayout;\r\n"
		"	editorTemplate -beginLayout \"Profile\" -collapse 0;\r\n"
		"	editorTemplate -addControl \"profilePresets\";\r\n"
		"	editorTemplate -addControl \"extrusionCount\";\r\n"
		"	editorTemplate -addControl \"extrusionOffset\";\r\n"
		"	editorTemplate -addControl \"normalOffset\";\r\n"
		"	editorTemplate -endLayout;\r\n"
		"	editorTemplate -suppress \"inMesh\";\r\n"
		"	editorTemplate -suppress \"outMesh\";\r\n"
		"	editorTemplate -suppress \"vertNum\";\r\n"
		"	AEdependNodeTemplate $nodeName;\r\n"
		"	editorTemplate -addExtraControls;\r\n"
		"	editorTemplate -endScrollLayout;\r\n"
		"}\r\n";





	return s_aeTemplate;

}

MString mel_createShelf()
{

	MString s_aeTemplate = MString() + "int $cc_doesShelfExist = `shelfLayout -query -ex \"CreativeCase\"`;\n" +
		"if ($cc_doesShelfExist == 1)\n" +
		"{\n" +
		"    \n" +
		"    string $shelfButtons[] = `shelfLayout -q -ca \"CreativeCase\"`;\n" +
		"    \n" +
		"    int $ex_b01,$ex_b02,$ex_b03 = 0;\n" +
		"    \n" +
		"    for( $i=0; $i<size($shelfButtons); ++$i )\n" +
		"{\n" +
		"    if( `control -exists $shelfButtons[$i]` == true)\n" +
		"    {\n" +
		"         if (`control -q -docTag $shelfButtons[$i]` == \"sm_createSmButton\") {$ex_b01 = 1;}\n" +
		"         if (`control -q -docTag $shelfButtons[$i]` == \"sm_addSmButton\") {$ex_b02 = 1;}\n" +
		"         if (`control -q -docTag $shelfButtons[$i]` == \"sm_createPlaneSmButton\") {$ex_b03 = 1;}\n" +
		"    }\n" +
		"    \n" +
		"}\n" +
		"if ($ex_b01 == 0) {shelfButton -p \"CreativeCase\" -dtg \"sm_createSmButton\" -annotation \"Add a ShellMod modifier to the selected objects\" -image1 \"shellModNode.png\" -command \"shellModCommand\";}\n" +
		"if ($ex_b02 == 0) {shelfButton -p \"CreativeCase\" -dtg \"sm_addSmButton\" -annotation \"Add the objects to the first selected ShellMod node\" -image1 \"shellModNode_Add.png\" -command \"shellModCommand -a\";}\n" +
		"if ($ex_b03 == 0) {shelfButton -p \"CreativeCase\" -dtg \"sm_createPlaneSmButton\" -annotation \"Creates a polygon plane with a ShellMod\" -image1 \"shellModNode_Plane.png\" -command \"polyPlane -sx 1 -sy 1 -w 5 -h 5;shellModCommand;move -y 1\";}\n" +
		"    \n" +
		"}\n" +
		"if ($cc_doesShelfExist == false)\n" +
		"{\n" +
		"    shelfLayout -cellWidth 33 -cellHeight 33 -p $gShelfTopLevel CreativeCase;\n" +
		"    shelfButton -p \"CreativeCase\" -dtg \"sm_createSmButton\" -annotation \"Add a ShellMod modifier to the selected objects\" -image1 \"shellModNode.png\" -command \"shellModCommand\";\n" +
		"    shelfButton -p \"CreativeCase\" -dtg \"sm_addSmButton\" -annotation \"Add the objects to the first selected ShellMod node\" -image1 \"shellModNode_Add.png\" -command \"shellModCommand -a\";\n" +
		"    shelfButton -p \"CreativeCase\" -dtg \"sm_createPlaneSmButton\" -annotation \"Creates a polygon plane with a ShellMod\" -image1 \"shellModNode_Plane.png\" -command \"polyPlane -sx 1 -sy 1 -w 5 -h 5;shellModCommand;move -y 1\";\n" +
		"};\n";

	return s_aeTemplate;
}

MString mel_deleteShelf()
{

	MString s_shelf = 
		"int $cc_doesShelfExist;\
		$cc_doesShelfExist = `shelfLayout -query -ex \"CreativeCase\"`;\
		\
		if ($cc_doesShelfExist == true)\
		{\
		deleteUI CreativeCase;\
		}\
		";

	return s_shelf;
}

#endif