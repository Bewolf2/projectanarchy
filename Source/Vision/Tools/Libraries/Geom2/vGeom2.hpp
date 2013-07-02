/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


#if defined(WIN32)

  // Define VGEOM2_LIB to compile vGeom2 as a static lib
  // or if you want to use vGeom2 as a static lib in your project
  #if defined(VGEOM2_LIB)
    #define VGEOM2_IMPEXP
    #define VGEOM2_IMPEXP_CLASS

  #elif defined(VGEOM2_EXPORTS)
  // Only defined in the vGeom2 project to export functions
    #define VGEOM2_IMPEXP       __declspec(dllexport)
    #define VGEOM2_IMPEXP_CLASS __declspec(dllexport)
  #endif

  // if not defined (or in a LIB, not exported from a dll)
  // define it as import from a DLL (this is the case of client project which uses the dll)
  #if !defined(VGEOM2_IMPEXP_CLASS)
    #define VGEOM2_IMPEXP       __declspec(dllimport)
    #define VGEOM2_IMPEXP_CLASS __declspec(dllimport)
  #endif

#else // WIN32

  #define VGEOM2_IMPEXP
  #define VGEOM2_IMPEXP_CLASS

#endif // WIN32



// vBase headers.
#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Runtime/Base/Container/VDictionary.hpp>

#include <Vision/Runtime/Base/ThirdParty/tinyXML/TinyXMLHelper.hpp>


// We need D3DX9 for some functions.
#if ((_MSC_VER < 1700) || defined(_VISION_DIRECTX_2010) )
#include <d3d9.h>
#include <d3d9types.h>
#include <d3dx9.h>
#endif

#if defined(WIN32)
#pragma warning(push)
#pragma warning(disable:4251) // ... needs to have dll-interface to be used by clients of class ...
#endif

// vGeom2 headers.
#include <Vision/Tools/Libraries/Geom2/Primitives/VGNameable.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGTransformable.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGNode.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGInstantiable.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGInstance.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGVertex.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGAnimation.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGAnimatable.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGVertexList.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGShaderAssignment.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGMaterial.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGGeometryInfo.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGPhysicsInfo.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGTriangleList.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGBone.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGSkeleton.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGMesh.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGMeshProxy.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGLight.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGPath.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGDummy.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGPortal.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGLightGrid.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGScene.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor_UVGenerator.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor_TangentGenerator.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor_CollisionMeshGenerator.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor_Merger.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor_Baker.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor_BoneFilter.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor_PathRewriter.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor_TextureCopier.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor_Transformator.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor_Recenterer.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor_GroupIndexGenerator.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor_VisibilityInfoGenerator.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor_VisionImporterExporter.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor_VisionSceneExportPreprocessor.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGProcessor_MaterialMerger.hpp>
#include <Vision/Tools/Libraries/Geom2/Backend/VGBackend.hpp>

#if defined(WIN32)
#pragma warning(pop)
#endif

void hkvMat3_invertPrecise (hkvMat3& m);
void hkvMat4_invertPrecise (hkvMat4& m);

/*
 * Havok SDK - Base file, BUILD(#20130624)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
