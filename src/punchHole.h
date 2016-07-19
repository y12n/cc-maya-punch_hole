#include <maya/MIOStream.h>
//#include <maya/MSimple.h>

#include <maya/MPxNode.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MGlobal.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MIntArray.h>
#include <maya/MFloatVector.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MPointArray.h>
#include <maya/MDGModifier.h>
#include <maya/MQuaternion.h>
#include <maya/MFloatPoint.h>
#include <maya/MMatrix.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MEulerRotation.h>

#include <maya/MPlugArray.h>

#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MNodeMessage.h>
#include <maya/MCallbackIdArray.h>

#include <vector>

//nclude <iostream>

#include <fstream>

using namespace std;

class punchHole : public MPxNode
{
public:
    
	punchHole();
	virtual                     ~punchHole();
	static  void*               creator();

	virtual MStatus             compute(const MPlug& plug, MDataBlock& data);
	static  MStatus             initialize();

    
    virtual void                postConstructor();
    
//    
//    static void					aboutToDeleteCB( MObject& node, MDGModifier& modifier, void* clientData );
//    
    
	static MTypeId id;

	static MObject              aInMesh;
	static MObject              aOutMesh;
	static MObject              aOffset;
	static MObject              aVertNum;
	static MObject              aExtrusionCount;
	static MObject              aExtrusionOffset;
	static MObject              aRotate;
	static MObject              aTaper;
	static MObject              aNormalOffset;
	static MObject              aProfilePresets;
	static MObject              aReverseOrder;
//
//private:
//    MCallbackIdArray			m_callbackIDs;


};
