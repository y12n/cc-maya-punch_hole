//	Example python command:
//
//	import maya.cmds as mc
//	locObj = mc.punchHoleCommand( n="ReName_Me", oy= 2, rx= -45.0, p=5, it=2, r=1.0, c=4)
//	cmds.move( 0, 3, 0, locObj, absolute=True )
//	cmds.makeIdentity( locObj, apply=True, t=1, r=1, s=1, n=2 )
//

#include "punchHoleCommand.h"

punchHoleCommand::punchHoleCommand()
{
}


void* punchHoleCommand::creator()
{
	return new punchHoleCommand;
}


bool punchHoleCommand::isUndoable() const
{
	return true;
}

MSyntax punchHoleCommand::newSyntax()
{
	MSyntax syntax;

	syntax.setObjectType( MSyntax::kSelectionList, 1, 1 );
	syntax.useSelectionAsDefault( true );

	syntax.enableEdit( false );
	syntax.enableQuery( false );

	return syntax;
}

MStatus punchHoleCommand::assignSameMaterial(MDagPath& inputShapeDagPath, MObject& outputShapeDagPath)
{

	MStatus status;

	MString sMaterial;

	if (inputShapeDagPath.hasFn(MFn::kMesh))
	{
		// Find the Shading Engines Connected to the SourceNode
		MFnMesh fnMesh(inputShapeDagPath.node());

		// A ShadingGroup will have a MFnSet
		MObjectArray sets, comps;
		fnMesh.getConnectedSetsAndMembers(inputShapeDagPath.instanceNumber(), sets, comps, true);

		// Each set is a Shading Group. Loop through them
		for (unsigned int i = 0; i < sets.length(); ++i)
		{
			MFnDependencyNode fnDepSGNode(sets[i]);

			sMaterial = fnDepSGNode.name();
		}
	}


	// MGlobal::displayInfo(MString() + "[punchHole] Initial SG: " + sMaterial);

	MSelectionList sList;
	MGlobal::getSelectionListByName(sMaterial, sList);
	MObject oInitialShadingGroup;
	status = sList.getDependNode(0, oInitialShadingGroup);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MFnSet fnShadingGroup(oInitialShadingGroup, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnShadingGroup.addMember(outputShapeDagPath);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;
}


MStatus punchHoleCommand::doIt( const MArgList& argList )
{
	MStatus status;

	MArgDatabase argData( syntax(), argList, &status );

	MSelectionList selection;
	argData.getObjects( selection );

	if (selection.length() == 0)
	{
		MGlobal::displayWarning("[punchHoleNode] Select 1 vertex component only...");
		return::MStatus::kSuccess;
	}


	// Global
	MItSelectionList iter(selection);


	MDagPath m_pathBaseMeshShape;
	MObject component;
	iter.getDagPath(m_pathBaseMeshShape, component);

	MGlobal::displayInfo(MString() + m_pathBaseMeshShape.partialPathName());


	MItMeshVertex pIter(m_pathBaseMeshShape, component, &status);

	int vertId = -1;

	int numConnectedEdges = 0;

	for ( pIter.reset() ; !pIter.isDone() ; pIter.next() )
	{
		vertId = pIter.index();
		status = pIter.numConnectedEdges(numConnectedEdges);


		if (vertId < 0)
		{
			MGlobal::displayWarning("[punchHoleNode] Only vertex component selection is allowed...");
			return::MStatus::kSuccess;

		}


	}

	MGlobal::displayInfo(MString() + "[punchHole] Vert selected - vtx[" + vertId + "]" );






	if (component.apiType() == MFn::kMeshVertComponent)
	{


		if (numConnectedEdges < 4)
		{

			MGlobal::displayWarning("[punchHoleNode] Minimum 4 connected edges needed...");
			return::MStatus::kSuccess;
		}



		// Create locator
		o_punchHoleNode = m_DEPNode.create("punchHole", &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		MFnDependencyNode fnDep( o_punchHoleNode );
		MPxCommand::setResult(fnDep.name());

		MPlug p_vertNum = fnDep.findPlug("vertNum", &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		p_vertNum.setInt( vertId );



		MPlug p_node_inMesh = fnDep.findPlug("inMesh", &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		MPlug p_node_output = fnDep.findPlug("output", &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);





		MFnDagNode fnDepSource( m_pathBaseMeshShape.node() );

		MPlug p_sourceMesh_worldMesh = fnDepSource.findPlug( "outMesh", &status );
		CHECK_MSTATUS_AND_RETURN_IT(status);
		MPlug p_sourceMesh_inMesh = fnDepSource.findPlug( "inMesh", &status );
		CHECK_MSTATUS_AND_RETURN_IT(status);;


		// MGlobal::displayInfo(MString() + p_sourceMesh_worldMesh.name());


		if (!p_sourceMesh_inMesh.isConnected())
		{


			// Need to generate construction history on the mesh
			MDagPath transform( m_pathBaseMeshShape );
			transform.pop();

			MFnDependencyNode fnDependNode( m_pathBaseMeshShape.node() );

			MFnMesh fnMesh;
			o_intermediate = fnMesh.copy ( m_pathBaseMeshShape.node(), transform.node(), &status );
			if ( status != MS::kSuccess ) { status.perror( "FAILED to copy mesh node" ); return status; }

			// Set the copy to be an intermediate object
			fnDependNode.setObject( o_intermediate );
			MPlug intermediatePlug = fnDependNode.findPlug( "intermediateObject" );
			intermediatePlug.setValue( true );

			// Assign the '.outMesh' plug from the intermediate node.
			MPlug p_inter_outMesh = fnDependNode.findPlug( "outMesh", &status  );
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// 
			status = m_DGMod.connect( p_inter_outMesh, p_node_inMesh );
			CHECK_MSTATUS_AND_RETURN_IT(status);

			status = m_DGMod.connect( p_node_output, p_sourceMesh_inMesh );
			CHECK_MSTATUS_AND_RETURN_IT(status);

			status = m_DGMod.doIt();
			CHECK_MSTATUS_AND_RETURN_IT(status);


		}



		else
		{



			MPlugArray inputs_plugArr;
			p_sourceMesh_inMesh.connectedTo(inputs_plugArr, true, false, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);




			if (inputs_plugArr.length() != 0)
			{



				MPlug p_source_inMesh = inputs_plugArr[0];

				MFnDependencyNode inMesh_dn(p_source_inMesh.node());

				MPlug p_outMesh_source = inMesh_dn.findPlug("output", &status);
				CHECK_MSTATUS_AND_RETURN_IT(status);


				MGlobal::displayWarning(MString() + "[punchHoleNode] " + p_outMesh_source.name());

				status = m_DGMod.disconnect( p_outMesh_source, p_sourceMesh_inMesh );
				CHECK_MSTATUS_AND_RETURN_IT(status);





				status = m_DGMod.connect( p_outMesh_source, p_node_inMesh );
				CHECK_MSTATUS_AND_RETURN_IT(status);


				status = m_DGMod.connect( p_node_output, p_sourceMesh_inMesh );
				CHECK_MSTATUS_AND_RETURN_IT(status);


				m_DGMod.doIt();
			}





		}




	}


	MSelectionList currSel;
	currSel.add(m_pathBaseMeshShape.partialPathName(&status));
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = MGlobal::setActiveSelectionList(currSel, MGlobal::kReplaceList);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return redoIt();
}
MStatus punchHoleCommand::redoIt()
{
	MStatus status;







	return MS::kSuccess;
}

MStatus punchHoleCommand::undoIt()
{
	MStatus status;

	// Restore the initial state
	status = m_DGMod.undoIt();
	CHECK_MSTATUS_AND_RETURN_IT( status );

	status = m_DAGMod.undoIt();
	CHECK_MSTATUS_AND_RETURN_IT(status);



	// Delete locator

	if ( !o_punchHoleNode.isNull() )
	{
		MGlobal::displayInfo(MString() + "[punchHoleNode] Deleting nodes");


		status = m_DAGMod.deleteNode(o_punchHoleNode);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		m_DAGMod.doIt();


	}

	if ( !o_intermediate.isNull() )
	{
		MGlobal::displayInfo(MString() + "[punchHoleNode] Deleting intermediate");


		status = m_DAGMod.deleteNode(o_intermediate);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		m_DAGMod.doIt();


	}


	return MS::kSuccess;
}