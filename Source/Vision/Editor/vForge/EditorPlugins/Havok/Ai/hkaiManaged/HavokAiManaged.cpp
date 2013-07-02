/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ManagedPlugin.cpp : Defines the entry point for the DLL application.

// Note: the dependency on _vcclrit.h has been removed (during the migration to VS2005).
// See the following page for details: http://msdn2.microsoft.com/en-us/library/ms173267(VS.80).aspx
// The _vcclrit.h header file was used in VS2003 for fixing static member initialisation/deinitialisation (MS-KB #814472).

#include <Vision/Editor/vForge/EditorPlugins/Havok/Ai/hkaiManaged/HavokAiManagedPCH.h>

#using <mscorlib.dll>
using namespace System;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace ManagedFramework;

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiModule.hpp>
#include <Ai/Pathfinding/World/hkaiWorld.h>

VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavokAi();


namespace HavokAiManaged
{

  public ref class ManagedModule
  {
  public:
  
    static void InitManagedModule()
    {
      //Init the entity plugin directly since we link statically to it
      GetEnginePlugin_vHavokAi()->InitEnginePlugin();
    }

    static void DeInitManagedModule()
    {
      //Deinit the entity plugin directly since we link statically to it
      GetEnginePlugin_vHavokAi()->DeInitEnginePlugin();
    }

	static void SetConnectToPhysicsWorld(bool connect)	
	{ 
		vHavokAiModule::GetInstance()->SetConnectToPhysicsWorld(connect); 
	}

	static bool ComputeAndDrawPath(VisionViewBase ^view, Vector3F startPoint, Vector3F endPoint, float radius, float height, float displayOffset, UINT color)
	{
		unsigned int b = (color>> 16) & 0xff;
		unsigned int g = (color>> 8) & 0xff;
		unsigned int r = color & 0xff;
		unsigned int havokColor =  (255 << 24) + (r << 16) + (g << 8) + b;

		IVRenderInterface *ri = (static_cast<VisionView ^>(view))->GetRenderInterface();
		hkvVec3 sp (startPoint.X, startPoint.Y, startPoint.Z);
		hkvVec3 ep (endPoint.X, endPoint.Y, endPoint.Z);
		return vHavokAiModule::GetInstance()->ComputeAndDrawPath(ri, &sp, &ep, radius, height, displayOffset, havokColor);
	}
  }; 
}

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
