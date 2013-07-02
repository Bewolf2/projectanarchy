/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


//  A basic action that just prints something to the console
//

#include "GamePluginPCH.h"
#include <Vision/Runtime/Engine/System/Vision.hpp>
#include "TemplateAction.h"


//  Register the action with a module - this always has to be in the .cpp file
//  The string "MyAction" is the name of the cammand to be used in the console
V_IMPLEMENT_ACTION( "MyAction", MyAction_cl, VAction,&g_myComponentModule, NULL )

//============================================================================================================
//============================================    A C T I O N   ==============================================
//============================================================================================================

MyAction_cl::MyAction_cl(){}
  
MyAction_cl::~MyAction_cl(){}
 
//------------------------------------------------------------------------------------------------------------
//  This function will be called when the action command is entered into the console
//------------------------------------------------------------------------------------------------------------
VBool MyAction_cl::Do( const class VArgList &argList )
{    
  //  add you code here to be called by typing 'myAction' into the console
  //  [...]

  Print ( "========================================" );
  Print ( "========== This is an action! ==========" );
  Print ( "========================================" );

  //float fCount  = 1234.223344;
  //int iCount  = 9876;

  Vision::Message.Add( "You have triggered an Action!" );
  //Vision::Message.Add("Float: %f", fCount );
  //Vision::Message.Add(0,"Int: %i", iCount );
  return TRUE;
}
//------------------------------------------------------------------------------------------------------------

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
