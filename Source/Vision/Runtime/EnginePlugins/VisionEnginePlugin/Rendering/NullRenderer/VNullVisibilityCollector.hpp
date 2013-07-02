/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/Visibility/VisionVisibilityCollector.hpp>

#include <Vision/Runtime/Base/Container/VScopedPtr.hpp>

/// \brief
///  A visibility collector that reports all objects as invisible.
class VNullVisibilityCollector : public IVisVisibilityCollector_cl
{
public:
  VNullVisibilityCollector()
  {
    m_spStaticCollection = new VisStaticGeometryInstanceCollection_cl();
    m_spEntityCollection = new VisEntityCollection_cl(0);
    m_spVisibilityObjectCollection = new VisVisibilityObjectCollection_cl(0);
    m_spLightCollection = new VisLightSrcCollection_cl(0);
    m_spZoneCollection = new VisVisibilityZoneCollection_cl(0);
  }

  virtual void OnDoVisibilityDetermination(int iFilterBitmask) HKV_OVERRIDE {}
  virtual void PostProcessVisibilityResults() HKV_OVERRIDE {}

  virtual bool IsStaticGeometryInstanceVisible(VisStaticGeometryInstance_cl *pGeomInstance) HKV_OVERRIDE
  {
    return false;
  }

  virtual bool IsEntityVisible(const VisBaseEntity_cl *pEntity) HKV_OVERRIDE
  {
    return false;
  }

  virtual bool IsVisObjectVisible(const VisVisibilityObject_cl *pVisObject) HKV_OVERRIDE
  {
    return false;
  }

  virtual bool IsLightVisible(const VisLightSource_cl *pLight) HKV_OVERRIDE
  {
    return false;
  }

  virtual bool IsVisibilityZoneVisible(const VisVisibilityZone_cl *pZone) HKV_OVERRIDE
  {
    return false;
  }

  virtual VisStaticGeometryInstanceCollection_cl* GetVisibleStaticGeometryInstances() const HKV_OVERRIDE
  {
    return m_spStaticCollection;
  }

  virtual VisStaticGeometryInstanceCollection_cl* GetVisibleStaticGeometryInstancesForPass(VPassType_e ePassType) const HKV_OVERRIDE
  {
    return m_spStaticCollection;
  }

  virtual VisEntityCollection_cl* GetVisibleEntities() const HKV_OVERRIDE
  {
    return m_spEntityCollection;
  }

  virtual VisEntityCollection_cl* GetVisibleEntitiesForPass(VPassType_e ePassType) const HKV_OVERRIDE
  {
    return m_spEntityCollection;
  }

  virtual VisEntityCollection_cl* GetVisibleForeGroundEntities() const HKV_OVERRIDE
  {
    return m_spEntityCollection;
  }

  virtual VisVisibilityObjectCollection_cl* GetVisibleVisObjects() const HKV_OVERRIDE
  {
    return m_spVisibilityObjectCollection;
  }

  virtual VisLightSrcCollection_cl* GetVisibleLights() const HKV_OVERRIDE
  {
    return m_spLightCollection;
  }


  virtual VisVisibilityZoneCollection_cl* GetVisibleVisibilityZones() HKV_OVERRIDE
  {
    return m_spZoneCollection;
  }

  virtual void SetPropertiesFromRenderContext(VisRenderContext_cl *pContext) HKV_OVERRIDE {}

private:
  VScopedPtr<VisStaticGeometryInstanceCollection_cl> m_spStaticCollection;
  VScopedPtr<VisEntityCollection_cl> m_spEntityCollection;
  VScopedPtr<VisVisibilityObjectCollection_cl> m_spVisibilityObjectCollection;
  VScopedPtr<VisLightSrcCollection_cl> m_spLightCollection;
  VScopedPtr<VisVisibilityZoneCollection_cl> m_spZoneCollection;
};

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
