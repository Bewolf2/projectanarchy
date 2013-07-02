/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// NodeManaged.cpp : Defines the entry point for the DLL application.
//

#include <Vision/Samples/vForge/CustomShape/CustomShapeManaged/CustomShapeManagedPCH.h>

#if (_MSC_VER < 1400)  //MSVC 8.0
  // Microsoft Knowledge Base 814472
  #include "_vcclrit.h"
#endif

/*
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

*/

VIMPORT IVisPlugin_cl* GetEnginePlugin_NodeEnginePlugin();


namespace NodeManaged
{
  public ref class ManagedModule
  {
  public:
  
    static void InitManagedModule()
    {
#if (_MSC_VER < 1400)  //pre MSVC 8.0
      // Microsoft Knowledge Base 814472
      __crt_dll_initialize();
#endif
      //Init the entity plugin directly since we link statically to it
      GetEnginePlugin_NodeEnginePlugin()->InitEnginePlugin();
    }

    static void DeInitManagedModule()
    {
      //Deinit the entity plugin directly since we link statically to it
      GetEnginePlugin_NodeEnginePlugin()->DeInitEnginePlugin();

#if (_MSC_VER < 1400)  //pre MSVC 8.0
      // Microsoft Knowledge Base 814472
      __crt_dll_terminate();
#endif
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
