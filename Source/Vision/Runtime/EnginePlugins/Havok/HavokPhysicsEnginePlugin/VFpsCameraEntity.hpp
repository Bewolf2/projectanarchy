/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFpsCameraEntity.hpp

#ifndef FPSCAMERAENTITY_HPP_INCLUDED
#define FPSCAMERAENTITY_HPP_INCLUDED

class vHavokCharacterController;
class VInputMap;

///\brief
/// Entity class that exposes first-person shooter camera controls to a camera.
/// NOTE: For now, this only works in conjunction with a VisSampleApp!!!
///   
class VFpsCameraEntity : public VisBaseEntity_cl
{
public:
  VFpsCameraEntity();
  VHAVOK_IMPEXP virtual void InitFunction() HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual void ThinkFunction() HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual void DeInitFunction() HKV_OVERRIDE;
//  VHAVOK_IMPEXP virtual void MessageFunction( int iID, INT_PTR iParamA, INT_PTR iParamB ) HK_OVERRIDE;

  V_DECLARE_SERIAL_DLLEXP( VFpsCameraEntity,  VHAVOK_IMPEXP );
  VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );

  // entity
  IMPLEMENT_OBJ_CLASS(VFpsCameraEntity);


private:
  VisBaseEntity_cl *m_pCamera;
  VInputMap* m_pMouseCamInputMap;   // We hijack / reuse its input for now

  vHavokCharacterController *m_pPhys;

  float m_fInitialFov;              // initial field of view
  float m_fFov;                     // current field of view
  float m_fTimeDiff;               // time difference from last frame (in seconds)
  float m_fHalfScreenSizeX;			//
  float m_fHalfScreenSizeY;			//
  hkvAlignedBBox m_bbStand;     // bounding box for standing mode

  // private functions
  void SetupPhysics();            // sets up physics and collision behaviour
  void HandleInput();             // handles player input
  bool ContactsGround();          // returns true if player touches the Ground

  void CommonInit();              // non-virtual replacement for the InitFunction that gets called by InitFunction _and_ Serialization
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
