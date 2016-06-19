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
    
    for ( pIter.reset() ; !pIter.isDone() ; pIter.next() )
    {
        vertId = pIter.index();
    }
    
    MGlobal::displayInfo(MString() + "[punchHole] Vert selected - vtx[" + vertId + "]" );

  
    
    
	if (vertId < 0)
	{
		MGlobal::displayWarning("[punchHoleNode] Only vertex component selection is allowed...");
		return::MStatus::kSuccess;

	}
    
    if (component.apiType() == MFn::kMeshVertComponent)
    {
        
        
        
        
        
        
        // Create locator
        o_punchHoleNode = m_DEPNode.create("punchHole");
        
        MFnDependencyNode fnDep( o_punchHoleNode );
        MPxCommand::setResult(fnDep.name());
        
        MPlug p_vertNum = fnDep.findPlug("vertNum");
        p_vertNum.setInt( vertId );
        
        MPlug p_inMesh = fnDep.findPlug("inMesh");
        MPlug p_outMesh = fnDep.findPlug("output");
        
        
        
        
        
        
        MFnDagNode fnDepSource( m_pathBaseMeshShape.node() );
        
        
        
        
        MPlug p_source_inMesh = fnDepSource.findPlug( "inMesh" );
        
        
        if (p_source_inMesh.isConnected())
        {
            MPlug p_inMesh_source = p_source_inMesh;
            
            p_inMesh_source.selectAncestorLogicalIndex(0);
            MPlugArray inputs_plugArr;
            p_inMesh_source.connectedTo(inputs_plugArr, true, false);
            p_inMesh_source = inputs_plugArr[0];
            
            MFnDependencyNode inMesh_dn(p_inMesh_source.node());
            
            MPlug p_outMesh_source = inMesh_dn.findPlug("output");
            
            
            status = m_DGMod.connect( p_inMesh_source, p_inMesh );
            CHECK_MSTATUS_AND_RETURN_IT(status);
            
            status = m_DGMod.disconnect( p_outMesh_source, p_source_inMesh );
            CHECK_MSTATUS_AND_RETURN_IT(status);
            
            status = m_DGMod.connect( p_outMesh, p_source_inMesh );
            CHECK_MSTATUS_AND_RETURN_IT(status);
            
            m_DGMod.doIt();
            
        }
    }
    
    
	MSelectionList currSel;
	currSel.add(m_pathBaseMeshShape.partialPathName());
	MGlobal::setActiveSelectionList(currSel, MGlobal::kReplaceList);


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


	return MS::kSuccess;
}