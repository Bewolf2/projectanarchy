/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_CONTROLLER_COMPONENT_H__
#define RPG_PLUGIN_CONTROLLER_COMPONENT_H__

#include <Vision/Samples/Engine/RPGPlugin/BaseComponent.h>

class hkaiCharacter;
class hkaiBehavior;

// Character component
class RPG_ControllerComponent : public RPG_BaseComponent
{
public:
  // Constructor needs to be public for FORCE_LINKDYNCLASS on mobile
  RPG_ControllerComponent();

  void RequestPath(hkvVec3 const& dest);
  void CancelRequestedPath();

  hkvVec3 const& GetDirection() const
  {
    return m_cachedDirection;
  }

  float GetSpeed() const
  {
    return m_cachedSpeed;
  }

  void SetSensorSize(float sensorSize);
  void SetDesiredSpeed(float desiredSpeed);

  // Update gameplay logic here. Called from entity updates, and only on the server.
  virtual void ServerTick(float deltaTime) {}

  // Called by AI world step with AI step dt
  void OnAfterCharacterStep(float deltaTime);

protected:
  // IVObjectComponent
  BOOL CanAttachToObject(VisTypedEngineObject_cl *object, VString& errorOut) HKV_OVERRIDE;

  void SetOwner(VisTypedEngineObject_cl *owner) HKV_OVERRIDE;

protected:
  // TODO: remove this
  class RPG_Character *m_characterOwner;

  hkRefPtr<hkaiCharacter> m_aiCharacter;
  hkRefPtr<hkaiBehavior> m_aiBehavior;

  float m_sensorSize;
  float m_desiredSpeed;

  hkvVec3 m_cachedDirection;
  float m_cachedSpeed;


private:
  V_DECLARE_SERIAL_DLLEXP(RPG_ControllerComponent, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_ControllerComponent, RPG_PLUGIN_IMPEXP);
};

namespace RPG_ControllerUtil
{
  // Vector slerp, with linearly independent (current, desired) check and t clamped to (0, 1)
  void CalcDirection(hkvVec3& resultDir, hkvVec3 const& currentDir, hkvVec3 const& desiredDir, float t);

  bool GetClosestPointOnNavMesh(hkvVec3 const& position, float testRadius, hkvVec3& outPosition);

  bool IsPointOnNavmesh(hkvVec3 const& position, float const testRadius = 0.1f);
}

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
