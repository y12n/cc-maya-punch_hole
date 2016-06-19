#include "punchHole.h"

MTypeId     punchHole::id(0x00123945);

MObject     punchHole::aInMesh;
MObject     punchHole::aOutMesh;
MObject     punchHole::aVertNum;
MObject     punchHole::aOffset;
MObject     punchHole::aExtrusionCount;
MObject     punchHole::aExtrusionOffset;
MObject		punchHole::aRotate;
MObject		punchHole::aTaper;
MObject		punchHole::aNormalOffset;
MObject		punchHole::aProfilePresets;
MObject		punchHole::aReverseOrder;


punchHole::punchHole()
{
}


punchHole::~punchHole()
{
//    MMessage::removeCallbacks(m_callbackIDs);
//    m_callbackIDs.clear();
}


void* punchHole::creator()
{
	return new punchHole();
}

void punchHole::postConstructor()
{
    MStatus status;
    
    
}

MStatus punchHole::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus status;
    

    if ( plug != aOutMesh )
    {
        return MS::kUnknownParameter;
    }

	// Colled input data
	MObject inMeshData = data.inputValue(aInMesh, &status).asMesh();
	CHECK_MSTATUS_AND_RETURN_IT(status);
	MDataHandle hOutput = data.outputValue(aOutMesh, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	int selVertID = data.inputValue(aVertNum, &status).asInt();
	CHECK_MSTATUS_AND_RETURN_IT(status);
	int extrusionCount = data.inputValue(aExtrusionCount, &status).asInt();
	CHECK_MSTATUS_AND_RETURN_IT(status);
	double extrusionOffset = data.inputValue(aExtrusionOffset, &status).asDouble();
	CHECK_MSTATUS_AND_RETURN_IT(status);
	double normalOffset = data.inputValue(aNormalOffset, &status).asDouble();
	CHECK_MSTATUS_AND_RETURN_IT(status);
	double holeOffset = data.inputValue(aOffset, &status).asDouble();
	CHECK_MSTATUS_AND_RETURN_IT(status);
	double m_rotate = data.inputValue( aRotate, &status ).asDouble();
	CHECK_MSTATUS_AND_RETURN_IT(status);
//	float taperVal = data.inputValue( aTaper, &status ).asFloat();
//	CHECK_MSTATUS_AND_RETURN_IT(status);
	short m_profilePreset = data.inputValue(aProfilePresets, &status).asShort();
	CHECK_MSTATUS_AND_RETURN_IT(status);
	bool reverseOrder = data.inputValue(aReverseOrder, &status).asBool();
	CHECK_MSTATUS_AND_RETURN_IT(status);

    

    
    
	// Convert rotateion to euler
	m_rotate *= M_PI/180.0;


	// Profile presets
	MFloatArray					m_curve_positions;
	MFloatArray					m_curve_values;

	if (m_profilePreset == 1)
	{
		float pos [3] = { 0.1f,0.9f,1.0f };
		float val [3] = { 0.1f,0.9f,0.0f };
		m_curve_positions = MFloatArray( pos, (sizeof(pos)/sizeof(*pos)) );
		m_curve_values = MFloatArray( val, (sizeof(val)/sizeof(*val)));
	}



	// Base Arrays
	MPointArray allPA;
	MIntArray connFaces;
	MIntArray connVerts;
	MPoint centerVertP;


	// Compute if outmesh is plugged in
	if (plug == aOutMesh) {

		MFnMeshData meshDataFn;
		MObject newMeshData = meshDataFn.create();
		MFnMesh inMeshFn(inMeshData);
		MItMeshVertex inMeshVtxFn(inMeshData);











		inMeshFn.copy(inMeshData, newMeshData);
		MFnMesh outMeshFn(newMeshData);

		inMeshFn.getPoints(allPA);

		if (selVertID>allPA.length())
		{
			selVertID = allPA.length();
		}


		MItMeshVertex mItVert(newMeshData);




		// Check if there are 4 connected verts
		MIntArray connVertsA_first;
		int prevID = selVertID;
		inMeshVtxFn.setIndex(selVertID, prevID);
		inMeshVtxFn.getConnectedVertices(connVertsA_first);
		MPoint firstConnVertP = allPA[connVertsA_first[0]];

		if (connVertsA_first.length() < 4)
		{
			hOutput.set(newMeshData);
			return::MStatus::kSuccess;
		}



		// Store average center normal before extrude
		MVector averageV;
		outMeshFn.getVertexNormal(selVertID,true,averageV);
		averageV.normalize();


		// Calculate center vert point
		centerVertP = allPA[selVertID];

		inMeshVtxFn.reset();

		while (!inMeshVtxFn.isDone())
		{
			if (inMeshVtxFn.index() == selVertID)
			{
				inMeshVtxFn.getConnectedFaces(connFaces);
				inMeshVtxFn.getConnectedVertices(connVerts);
			}
			inMeshVtxFn.next();
		}


		if (connVerts.length() == 0)
		{
			return MStatus::kSuccess;
		}


		// Calculate average length from center point
		double averagePLen = 0.0;

		for (unsigned int i = 0; i < connVerts.length(); i++)
		{
			averagePLen += centerVertP.distanceTo(allPA[i]);
		}

		if (averagePLen == 0.0)
		{
			return MStatus::kSuccess;
		}

		averagePLen = averagePLen / double(connVerts.length());
		averagePLen *= 0.5;





		// Do extrude
		status = outMeshFn.extrudeFaces(connFaces, 1, NULL, true, 0.0f, 0.0f);
		//status = outMeshFn.extrudeFaces(connFaces, extrusionCount, NULL, true, taperVal, extrusionOffset);
		CHECK_MSTATUS_AND_RETURN_IT(status);


		// Sort arrays
		vector <MIntArray> v_polygonVertsA;
		MIntArray polygonVertsA;

		for (unsigned int i = 0; i < connFaces.length(); i++)
		{

			MIntArray polygonVerts;

			status = outMeshFn.getPolygonVertices(connFaces[i], polygonVerts);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			v_polygonVertsA.push_back(polygonVerts);

		}

		MIntArray countA;

		for (int i = 0; i < v_polygonVertsA.size(); i++)
		{

			for (int p = 0; p < v_polygonVertsA[i].length(); p++)
			{
				for (int u = 0; u < v_polygonVertsA.size(); u++)
				{

					for (int v = 0; v < v_polygonVertsA[u].length(); v++)
					{

						if (v_polygonVertsA[i][p] == v_polygonVertsA[u][v])
						{

							countA.append(v_polygonVertsA[i][p]);

						}

					}


				}
			}


		}

		// Bubble Sort array
		int swapHolder_pos = -1;
		int aend = countA.length()-1;
		int alength = aend;

		for (int c = alength-1; c > 0; c--)
		{
			for (int i = 0; i < aend; i++)
			{
				if (countA[i] > countA[ i + 1 ])
				{
					swapHolder_pos = countA[i + 1];
					countA[i + 1] = countA[i];
					countA[i] = swapHolder_pos;

				}
			}

			aend --;
		}

		// Find center vert id / by searching for the most connected vert
		int maxId = countA[0];
		int lastId = countA[0];
		int count = 0;
		int maxcount = 0;
		int maxcount_total = 0;

		int centerVertID = -1;

		for (int i = 0; i < countA.length(); i++)
		{

			if (countA[i] == lastId)
			{


				maxId = countA[i];
				count += 1;

			}

			else
			{
				maxcount = count;

				if (maxcount_total < maxcount )
				{
					maxcount_total = maxcount;
					centerVertID = maxId;
				}

				count = 0;
			}

			lastId = countA[i];

		}



		// Get polygon vertecies after extrude
		polygonVertsA.clear();

		for (unsigned int i = 0; i < connFaces.length(); i++)
		{

			MIntArray polygonVerts;

			status = outMeshFn.getPolygonVertices(connFaces[i], polygonVerts);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			for (int u = 0; u < polygonVerts.length(); u++)
			{

				bool isFound = false;

				for (int p = 0; p < polygonVertsA.length(); p++)
				{

					if (polygonVertsA[p] == polygonVerts[u])
					{
						isFound = true;
					}
				}


				if (!isFound)
				{
					polygonVertsA.append(polygonVerts[u]);
				}


			}

		}

		if (polygonVertsA.length() < 1)
		{
			return MStatus::kSuccess;
		}


		// remove center vert ID
		for (int i = 0; i < polygonVertsA.length(); i++)
		{
			if (polygonVertsA[i] == centerVertID)
			{
				polygonVertsA.remove(i);
			}
		}



		MIntArray connpA; // This will hold the sorted border verts
		MIntArray polygonVertsA_delete = polygonVertsA;

		int currIndex = polygonVertsA[0];
		int previndex;

		for (unsigned int z = 0; z < polygonVertsA.length(); z++)
		{

			MIntArray connVertsA;
			mItVert.setIndex(currIndex,previndex);
			mItVert.getConnectedVertices(connVertsA);

			bool ff = false;

			for (unsigned int i = 0; i < connVertsA.length(); i++)
			{

				for (unsigned int p = 0; p < polygonVertsA_delete.length(); p++)
				{
					if (polygonVertsA_delete[p] == connVertsA[i])
					{
						currIndex = polygonVertsA_delete[p];
						connpA.append(currIndex);

						polygonVertsA_delete.remove(p);

						ff=true;
						break;
					}

					if (ff)
					{
						break;
					}


				}

				if (ff)
				{
					break;
				}

			}

		}




		// get All points again after extrude
		outMeshFn.getPoints(allPA);



		// Get averaged distance from the center
		double average_radius = 0.0;

		for (int i = 0 ; i < connpA.length(); i++)
		{
			average_radius += allPA[connpA[i]].distanceTo(centerVertP);
		}

		average_radius = average_radius / connpA.length();
		average_radius *= 0.5;
		average_radius += holeOffset;


		// -------------------------------------------
		// calculate rotation matrix


		MVector v2 = centerVertP - firstConnVertP;
		v2.normalize() ;


		if (v2 == MVector(0.0,0.0,0.0))
		{
			v2 = MVector::zAxis;
		}


		MVector cross1 = averageV^v2;
		cross1.normalize() ;
		MVector cross2 =  cross1^averageV;
		cross2.normalize();


		double m[4][4] = {{cross1.x, cross1.y , cross1.z, 0},
		{ averageV.x, averageV.y , averageV.z, 0},
		{ cross2.x, cross2.y , cross2.z, 0},
		{ 0.0, 0.0, 0.0, 1.0}};
		MMatrix rotMatrix = m;


		// -----------------------------------------------------




		// Rearrange points in a circle
		MPointArray circlePA;

		double slice = 2.0 * M_PI /  connpA.length();

		int x = 0;
		for (unsigned int i = 0; i < connpA.length(); i++)
		{
			double angle = slice * double(x);


			angle += m_rotate;

			double newX = average_radius * cos(angle);
			double newZ = average_radius * sin(angle);

			MPoint circleP( newX, 0.0, newZ );

			circleP *= rotMatrix;

			circleP += centerVertP;
			circlePA.append( circleP );


			x+=1;
		}


		// Reverse points if necessary
		if (reverseOrder)
		{
			MIntArray revPA;


			for (int i = connpA.length()-1; i >= 0; i--)
			{
				revPA.append(connpA[i]);
			}

			connpA = revPA;
		}


		// Set all points
		for (int i = 0; i < connpA.length(); i++)
		{
			// Add extra offset based on average normal to border points
			MPoint currP = circlePA[i];
			currP += averageV * normalOffset;

			outMeshFn.setPoint(connpA[i], currP);

		}





		// Add extra offset based on average normal to center point
		MPoint currP = centerVertP;
		currP += averageV * normalOffset;
		outMeshFn.setPoint(centerVertID, currP);




		// No profiles only extrude
		if (m_profilePreset == 0)
		{

			float extrusionVal = float(extrusionOffset) / float(extrusionCount);

			// Do extrude
			for (int i = 0; i < extrusionCount; i++)
			{
				status = outMeshFn.extrudeFaces(connFaces, 1, NULL, true, extrusionVal, 0.0f);
				CHECK_MSTATUS_AND_RETURN_IT(status);
			}

		}









		// Profiles
		if (m_profilePreset > 0)
		{

			// Store number of points before extrude
			int numVerts_before = outMeshFn.numVertices();

			// Do extrude
			for (int i = 0; i < m_curve_positions.length(); i++)
			{
				status = outMeshFn.extrudeFaces(connFaces, 1, NULL, true, 0.0f, 0.0f);
				CHECK_MSTATUS_AND_RETURN_IT(status);
			}




			// Store number of points after extrude
			//int numVerts_after = outMeshFn.numVertices();


			// Get all points after extrude
			MPointArray allPA_after;
			outMeshFn.getPoints(allPA_after);





			// Move points along the normal based on the profile presets
			int counter = 0;
			int loop_counter = 0;
			int loop_counter_limit = connpA.length();

			for (int i = numVerts_before-1; i < numVerts_before + (connpA.length()* m_curve_positions.length()) + 1; i++)
			{

				MPoint currP = allPA_after[i];
				currP += averageV * (m_curve_positions[loop_counter] * float(extrusionOffset)  );

				if (i <= outMeshFn.numVertices()-1)
				{
					status = outMeshFn.setPoint(i, currP);
					CHECK_MSTATUS_AND_RETURN_IT(status)
				}

				counter += 1;

				if (loop_counter == m_curve_positions.length()-1)
				{
					loop_counter_limit += 1;
				}


				if (counter == loop_counter_limit)
				{
					counter = 0;
					loop_counter += 1;
				}



			}

		}



		//	// -------------------------------------------

		//	//
		//	//
		//	//			// Store number of points after extrude
		//	//			numVerts_after = outMeshFn.numVertices();
		//	//
		//	//
		//	//			// Get all points after extrude
		//	////			allPA_after;
		//	////			outMeshFn.getPoints(allPA_after);
		//	//
		//	//
		//	//			// Move points along the normal based on the profile presets
		//	//			counter = 0;
		//	//			loop_counter = 0;
		//	//			loop_counter_limit = connpA.length();
		//	//
		//	//			MPointArray offsetPA;
		//	//			MIntArray offsetPA_id;
		//	//
		//	//			for (int i = numVerts_before-1; i < numVerts_before + (connpA.length()* m_curve_values.length()) + 1; i++)
		//	//			{
		//	//
		//	//				MPoint currP = allPA_after[i];
		//	//
		//	//				MVector sideNormal;
		//	//				outMeshFn.getVertexNormal(i,true,sideNormal);
		//	//				sideNormal.normalize();
		//	//
		//	//				currP += sideNormal * (m_curve_values[loop_counter] * taperVal / m_curve_values.length() );
		//	//
		//	//				offsetPA.append(currP);
		//	//				offsetPA_id.append(i);
		//	//
		//	//				// status = outMeshFn.setPoint(i, currP);
		//	//				//CHECK_MSTATUS_AND_RETURN_IT(status);
		//	//
		//	//				counter += 1;
		//	//
		//	//				if (loop_counter == m_curve_values.length()-1)
		//	//				{
		//	//					loop_counter_limit += 1;
		//	//				}
		//	//
		//	//
		//	//				if (counter == loop_counter_limit)
		//	//				{
		//	//					counter = 0;
		//	//					loop_counter += 1;
		//	//				}
		//	//
		//	//
		//	//
		//	//			}
		//	//
		//	//			// Move points along the normal based on the profile presets
		//	//
		//	//
		//	//			for (int i = 0; i < offsetPA.length(); i++)
		//	//			{
		//	//
		//	//
		//	//				MPoint currP = offsetPA[i];
		//	//
		//	//				if (offsetPA_id[i] <= outMeshFn.numVertices()-1)
		//	//				{
		//	//					status = outMeshFn.setPoint(offsetPA_id[i], currP);
		//	//					CHECK_MSTATUS_AND_RETURN_IT(status);
		//	//				}
		//	//
		//	//
		//	//
		//	//			}
		//	//



		//}





		hOutput.set(newMeshData);
		data.setClean(plug);
	}



	return MS::kSuccess;
}


MStatus punchHole::initialize()
{
	MStatus status;

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnEnumAttribute eAttr;


	aProfilePresets = eAttr.create( "profilePresets", "profilePresets", 0);
	eAttr.setStorable(true);
	eAttr.addField("Custom", 0);
	eAttr.addField("ChaserEdge 1%", 1);
	eAttr.setDefault(0);


	// Vertex number to operate on

	aVertNum = nAttr.create("vertNum", "vertNum", MFnNumericData::kInt);
	nAttr.setDefault(0);
	nAttr.setMin(0);
	nAttr.setHidden(true);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);



	aOffset = nAttr.create("offset", "offset", MFnNumericData::kDouble);
	nAttr.setDefault(0.0);
	nAttr.setSoftMin(-0.5);
	nAttr.setSoftMax(0.5);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);

	aTaper = nAttr.create("taper", "taper", MFnNumericData::kFloat);
	nAttr.setDefault(0.0);
	nAttr.setSoftMin(0.0);
	nAttr.setSoftMax(1.0);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);


	aExtrusionCount = nAttr.create("extrusionCount", "extrusionCount", MFnNumericData::kInt);
	nAttr.setDefault(1);
	nAttr.setMin(0);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);

	aExtrusionOffset = nAttr.create("extrusionOffset", "extrusionOffset", MFnNumericData::kDouble);
	nAttr.setDefault(0.0);
	nAttr.setSoftMin(-0.5);
	nAttr.setSoftMax(0.5);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);

	aNormalOffset = nAttr.create("normalOffset", "normalOffset", MFnNumericData::kDouble);
	nAttr.setDefault(0.0);
	nAttr.setSoftMin(-0.5);
	nAttr.setSoftMax(0.5);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);

	aRotate = nAttr.create( "rotate", "rotate", MFnNumericData::kDouble );
	nAttr.setDefault(0.0);
	nAttr.setSoftMin(-180.0);
	nAttr.setSoftMax(180.0);
	nAttr.setKeyable( true );

	aReverseOrder = nAttr.create("reverseOrder", "reverseOrder", MFnNumericData::kBoolean);
	nAttr.setDefault(false);


	// Input mesh
	aInMesh = tAttr.create("inMesh", "inMesh", MFnData::kMesh);
	tAttr.setWritable(true);
	tAttr.setReadable(false);
	tAttr.setStorable(false);
	tAttr.setKeyable(false);
	tAttr.setChannelBox(false);

	aOutMesh = tAttr.create("output", "output", MFnData::kMesh);
	tAttr.setWritable(false);
	tAttr.setReadable(true);
	tAttr.setStorable(false);
	tAttr.setKeyable(false);
	tAttr.setChannelBox(false);;

	addAttribute(aInMesh);
	addAttribute(aOutMesh);
	addAttribute(aVertNum);
	addAttribute(aOffset);
	addAttribute(aExtrusionCount);
	addAttribute(aExtrusionOffset);
	addAttribute(aRotate);
	addAttribute(aTaper);
	addAttribute(aNormalOffset);
	addAttribute(aProfilePresets);
	addAttribute(aReverseOrder);

	attributeAffects(aInMesh, aOutMesh);
	attributeAffects(aVertNum, aOutMesh);
	attributeAffects(aOffset, aOutMesh);
	attributeAffects(aExtrusionCount, aOutMesh);
	attributeAffects(aExtrusionOffset, aOutMesh);
	attributeAffects(aRotate, aOutMesh);
	attributeAffects(aTaper, aOutMesh);
	attributeAffects(aNormalOffset, aOutMesh);
	attributeAffects(aProfilePresets, aOutMesh);
	attributeAffects(aReverseOrder, aOutMesh);


	return MS::kSuccess;
}
