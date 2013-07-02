/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file _DanglingEntity.hpp
/// Generic entity class for object dangling on a hook or something similar.
///

#ifndef VISION__DANGLINGENTITY
#define VISION__DANGLINGENTITY

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>


// class DanglingEntity_cl is deprecated - should not be used anymore
#ifndef _VISION_DOC

class DanglingEntity_cl : public VisBaseEntity_cl, public IVisCallbackHandler_cl
{
public:

  EFFECTS_IMPEXP DanglingEntity_cl();
  EFFECTS_IMPEXP ~DanglingEntity_cl();
  EFFECTS_IMPEXP virtual void InitFunction() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void ThinkFunction() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void MessageFunction( int iID, INT_PTR iParamA, INT_PTR iParamB ) HKV_OVERRIDE;

  EFFECTS_IMPEXP void CommonInit();

  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE {}

  // entity parameter
  float Speed;
  float Deflection;
  float Angle;
  float Random;
  float Rotation;

  hkvVec3 m_vInitialOrientation;
  bool m_bForward;
  float m_fTime;
  float m_fTimeOffset;
  float m_fInfluenceY;
  float m_fInfluenceX;
  hkvVec3 m_vHookPosition;
  hkvVec3 m_vInitialPosition;

  //serialization
  V_DECLARE_SERIAL_DLLEXP( DanglingEntity_cl,  EFFECTS_IMPEXP );
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  IMPLEMENT_OBJ_CLASS(DanglingEntity_cl)
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
