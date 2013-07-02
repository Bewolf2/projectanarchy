/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VParamComposite.hpp

#ifndef _VISION_VCOMPOSITEPARAM_HPP
#define _VISION_VCOMPOSITEPARAM_HPP

#include <Vision/Runtime/Base/System/Param/VParamContainerBase.hpp>

// these classes should not be used by the engine
#ifndef _VISION_DOC

class VParamBlock;
class VParamDesc;

/// \brief Internal: Used by some of Vision's Tools. Might get deprecated at some point. Do not use.
class VCompositeParam : public VParamContainerBase
{
  V_DECLARE_SERIAL_DLLEXP(VCompositeParam, VBASE_IMPEXP)

public:
  VBASE_IMPEXP VCompositeParam();
  VBASE_IMPEXP ~VCompositeParam();

  VParamBlock *m_pBlock;
  VParamDesc  *m_pDesc;

  VBASE_IMPEXP void Reset();

  VBASE_IMPEXP VCompositeParam *Clone();
  VBASE_IMPEXP void FillIn(VCompositeParam &src);

protected:
  VBASE_IMPEXP virtual void Serialize(VArchive &ar);

};

#endif // _VISION_DOC


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
