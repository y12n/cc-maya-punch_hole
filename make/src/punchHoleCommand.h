#ifndef punchHoleCommand_H
#define punchHoleCommand_H

#ifdef __linux__
	#include <maya/MArgDatabase.h>
#else
	#include <maya/MArgDataBase.h>
#endif

#include <maya/MDagPath.h>
#include <maya/MDGModifier.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MIntArray.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MMeshIntersector.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MPointArray.h>
#include <maya/MPxCommand.h>
#include <maya/MSelectionList.h>
#include <maya/MSyntax.h>
#include <maya/MDagModifier.h>
#include <maya/MFnSet.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnTransform.h>
#include <maya/MDagPathArray.h>
#include <maya/MItSelectionList.h>
#include <maya/MItMeshVertex.h>



class punchHoleCommand : public MPxCommand
{
public:
    punchHoleCommand();
    virtual MStatus			doIt( const MArgList& argList );
    virtual MStatus			redoIt();
    virtual MStatus			undoIt();
    virtual bool			isUndoable() const;
    static void*			creator();
    static MSyntax			newSyntax();

	void setResult();

private:

	MStatus					assignSameMaterial(MDagPath& inputShapeDagPath, MObject& outputShapeDagPath);

	MDagModifier			m_DAGMod;
	MDGModifier				m_DGMod;
	MFnDependencyNode		m_DEPNode;

	MObject					o_punchHoleNode;
	MObject					o_outMesh;
	MObject					o_intermediate;

	MDagPath				m_pathBaseMeshShape;

	MPlug					p_visibility;
	MPlug					p_intermediateObject;

};


#endif