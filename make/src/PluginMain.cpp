//
//  PluginMain.h
//  punchHole
//
//  Created by Creative Case on 2016. 05. 1.
//  Copyright (c) 2014. Creative Case. All rights reserved.
//

#ifndef punchHole_PluginMain_h
#define punchHole_PluginMain_h

#include "punchHole.h"
#include "punchHoleCommand.h"
#include "AETemplate.h"
#include "icons.h"

#include <maya/MFnPlugin.h>

// Initalize Plugin


MStatus initializePlugin(MObject obj)
{
	MStatus status;

	icons_data_write();

	MFnPlugin fnPlugin(obj, "Creative Case", "0.2", "Any");

	MGlobal::executeCommand( mel_createShelf() );
	MGlobal::executeCommand( mel_AETemplate() );
	

	

	status = fnPlugin.registerNode("punchHole", punchHole::id, punchHole::creator, punchHole::initialize);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.registerCommand( "punchHoleCommand", punchHoleCommand::creator, punchHoleCommand::newSyntax );
    CHECK_MSTATUS_AND_RETURN_IT( status );


	return MS::kSuccess;
}


MStatus uninitializePlugin(MObject obj)
{
	MStatus status;

	MFnPlugin fnPlugin(obj);

	status = fnPlugin.deregisterNode(punchHole::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.deregisterCommand( "punchHoleCommand" );
    CHECK_MSTATUS_AND_RETURN_IT( status );

	return MS::kSuccess;
}

#endif