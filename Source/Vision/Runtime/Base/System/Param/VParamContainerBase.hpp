/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VParamContainerBase.hpp

#ifndef _VISION_VPARAMCONTAINERBASE_HPP
#define _VISION_VPARAMCONTAINERBASE_HPP

#include <Vision/Runtime/Base/System/VRefTarget.hpp>

// these classes should not be used in the engine
#ifndef _VISION_DOC


class VParam;

class VParamContainerBase : public VRefTarget
{
protected:
  VBASE_IMPEXP VParamContainerBase() {}
public:
  VBASE_IMPEXP virtual int GetParamUpdateFlags() {return 0;}
  VBASE_IMPEXP virtual int SetParamUpdateFlags( int iFlags) {return 0;}
  VBASE_IMPEXP virtual void OnParamChange(VParam *pParam = 0) {}
  VBASE_IMPEXP virtual void OnPreParamChange( VParam* pParam, void* pNewVal ) {}
};

#endif // _VISION_DOC

#endif//_VISION_VPARAMCONTAINERBASE_HPP

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
