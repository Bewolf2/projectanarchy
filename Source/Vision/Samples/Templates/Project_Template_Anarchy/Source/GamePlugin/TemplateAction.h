/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#ifndef H_MYACTION
#define H_MYACTION

#include <Vision/Runtime/Base/Action/VAction.hpp>


//  if one needs access to the module somewhere else, just include this line but never declare multiple times: 
//  only one declare in one .cpp file!
extern VModule g_sampleModule; 

//============================================================================================================
//============================================    A C T I O N   ==============================================
//============================================================================================================
//  all actions are dereived from VAction

class MyAction_cl : public VAction
{
  // required first line 
  V_DECLARE_ACTION(MyAction_cl)
public:
  // construct
  MyAction_cl();

  // destruct
  ~MyAction_cl();

  // this function is the only one you actually have use
  VOVERRIDE VBool Do(const class VArgList &argList);
    
private:    
};

#endif

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
