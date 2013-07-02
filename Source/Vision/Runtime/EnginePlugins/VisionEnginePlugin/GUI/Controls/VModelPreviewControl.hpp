/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VModelPreviewControl.hpp

#ifndef VMODELPREVIEWCONTROL_HPP_INCLUDED
#define VMODELPREVIEWCONTROL_HPP_INCLUDED


/// \brief
///   VModelPreviewComponent renders an entity to an VisRenderableTexture_cl, used by the VModelPreviewControl.
class VModelPreviewComponent : public IVObjectComponent
{
public:
  GUI_IMPEXP VModelPreviewComponent(int iID=0, int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);
  GUI_IMPEXP VModelPreviewComponent(const char *szComponentName, int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);
  GUI_IMPEXP virtual ~VModelPreviewComponent();

  ///
  /// @name Component Setup
  /// @{
  ///
 
  /// \brief Initialize the component and it's render target with the specified dimension and FOV
  GUI_IMPEXP virtual bool InitComponent(int iResX, int iResY, float fFovH, float fFovV);

  /// \brief Initialize the component and it's render target with the specified dimension, presentation size and FOV
  GUI_IMPEXP virtual bool InitComponent(int iResX, int iResY, int iPresentationSizeX, int iPresentationSizeY, float fFovH, float fFovV);

  /// \brief create an empty light grid for the preview model
  GUI_IMPEXP virtual void CreateEmptyLightGrid();

  /// \brief sets the entity to render, if bOwnsEntity is true, the entity will be deleted when control is destroyed
  GUI_IMPEXP virtual void SetPreviewEntity(VisBaseEntity_cl *pEntity);

  /// \brief get the preview entity instance
  GUI_IMPEXP VisBaseEntity_cl* GetPreviewEntity() const { return m_spEntity; }

  /// \brief this function can be called to position the entity model automatically so that is fits nicely into the view
  GUI_IMPEXP void SetFitModel();

  /// \brief
  ///   Gets whether this component is available or not  
  GUI_IMPEXP BOOL IsAvailable() { return (GetPreviewEntity() && GetPreviewEntity()->GetMesh() && m_spContext); }

  /// \brief set rotation speed around up-axis (degrees per second)
  inline void SetYawSpeed(float fSpeed) { m_fYawSpeed=fSpeed; }

  /// \brief get rotation speed around up-axis (degrees per second)
  inline float GetYawSpeed() { return m_fYawSpeed; }

  /// @}
  /// @name Update / Access Methods
  /// @{
  // 

  /// \brief call update in your 'think'/'tick' code to see animation and yaw rotation
  GUI_IMPEXP virtual void Update(float dtime);

  /// \brief get the render target
  GUI_IMPEXP VisRenderableTexture_cl * GetRenderableTexture() { return m_spRenderTarget; }

  /// \brief get the colors of the present light grid
  inline hkvVec3*  GetLightGridColors() { return m_LightGridColors; }

  /// @}

  GUI_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut) HKV_OVERRIDE;
  V_DECLARE_SERIAL(VModelPreviewComponent, GUI_IMPEXP)
  V_DECLARE_VARTABLE(VModelPreviewComponent, GUI_IMPEXP)
  GUI_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

protected:

  GUI_IMPEXP virtual bool CreateRenderTarget(int iResX, int iResY);
  GUI_IMPEXP virtual void SetClipPlaneFromExtent();

  // render context related
  int m_iContextNum;
  VisRenderContextPtr m_spContext;
  VisContextCameraPtr m_spCamera;

  VisRenderableTexturePtr m_spRenderTarget;
  VisRenderableTexturePtr m_spDepthStencilTarget;

  VSmartPtr<VisBaseEntity_cl> m_spEntity;
  float m_fYawSpeed;
  hkvVec3 m_LightGridColors[6];
};

/// \brief
///   Represents an image control for entity model preview (render to texture). It is derived from
///   VPushButton so it supports its full functionality
class VModelPreviewControl : public VPushButton
{
public:
  GUI_IMPEXP VModelPreviewControl() : m_pComponent(NULL) { }
  GUI_IMPEXP virtual ~VModelPreviewControl() { if(m_pComponent!=NULL) RemoveComponent(m_pComponent); }

  ///
  /// @name Backward compatibility methods
  /// @{
  ///
  
  /// \brief sets the entity to render, if bOwnsEntity is true, the entity will be deleted when control is destroyed
  GUI_IMPEXP virtual void SetPreviewEntity(VisBaseEntity_cl *pEntity) { m_pComponent->SetPreviewEntity(pEntity); }

  /// \brief get the preview entity instance
  GUI_IMPEXP VisBaseEntity_cl* GetPreviewEntity() const { return m_pComponent->GetPreviewEntity(); }

  /// \brief this function can be called to position the entity model automatically so that is fits nicely into the view
  GUI_IMPEXP void SetFitModel() { m_pComponent->SetFitModel(); }

  /// \brief set rotation speed around up-axis (degrees per second)
  inline void SetYawSpeed(float fSpeed) { m_pComponent->SetYawSpeed(fSpeed); }

  ///
  /// @}
  /// @name Access the contained VModelPreviewComponent
  /// @{
  ///

  inline VModelPreviewComponent * GetPreviewComponent() { return m_pComponent; }

  ///
  /// @}
  ///

protected:
  V_DECLARE_SERIAL_DLLEXP( VModelPreviewControl, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar );

  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite);

  GUI_IMPEXP virtual void OnTick(float dtime);

  // now the component holds all render context related members
  VModelPreviewComponent *m_pComponent;
};



/// \brief
///   Render loop implementation for rendering only the preview entity
class VModelPreviewRenderLoop : public IVisRenderLoop_cl
{
public:
  VModelPreviewRenderLoop(VModelPreviewComponent *pOwner)
  {
    VASSERT(pOwner);
    m_pOwnerComp = pOwner;
  }

  virtual void OnDoRenderLoop(void *pUserData);

protected:
  VModelPreviewComponent *m_pOwnerComp;
};


/// \brief
///   Simple visibility collector which always returns only one entity
class VModelPreviewVisCollector : public IVisVisibilityCollector_cl
{
public:
  VModelPreviewVisCollector(VisBaseEntity_cl *pEntity=NULL);
  virtual ~VModelPreviewVisCollector();

  GUI_IMPEXP virtual void OnDoVisibilityDetermination(int iFilterBitmask = 0xFFFFFFFF) HKV_OVERRIDE {}
  GUI_IMPEXP virtual void PostProcessVisibilityResults() HKV_OVERRIDE {}

  GUI_IMPEXP virtual bool IsEntityVisible(const VisBaseEntity_cl *pEntity) HKV_OVERRIDE { return true; }
  GUI_IMPEXP virtual bool IsVisObjectVisible(const VisVisibilityObject_cl *pVisObject) HKV_OVERRIDE { return false; }
  GUI_IMPEXP virtual bool IsLightVisible(const VisLightSource_cl *pLight) HKV_OVERRIDE { return false; }
  GUI_IMPEXP virtual bool IsVisibilityZoneVisible(const VisVisibilityZone_cl *pZone) HKV_OVERRIDE { return false; }
  GUI_IMPEXP virtual bool IsStaticGeometryInstanceVisible(VisStaticGeometryInstance_cl *pGeomInstance) HKV_OVERRIDE { return false; }

  GUI_IMPEXP virtual const VisEntityCollection_cl* GetVisibleEntities() const HKV_OVERRIDE { return m_pEntities; }
  GUI_IMPEXP virtual const VisEntityCollection_cl* GetVisibleEntitiesForPass(VPassType_e ePassType) const HKV_OVERRIDE { return NULL; } 
  GUI_IMPEXP virtual const VisEntityCollection_cl* GetVisibleForeGroundEntities() const HKV_OVERRIDE { return NULL; }
  GUI_IMPEXP virtual const VisVisibilityObjectCollection_cl* GetVisibleVisObjects() const HKV_OVERRIDE { return NULL; }
  GUI_IMPEXP virtual const VisLightSrcCollection_cl* GetVisibleLights() const HKV_OVERRIDE { return NULL; }
  GUI_IMPEXP virtual const VisVisibilityZoneCollection_cl* GetVisibleVisibilityZones() HKV_OVERRIDE { return NULL; }
  GUI_IMPEXP virtual const VisStaticGeometryInstanceCollection_cl* GetVisibleStaticGeometryInstances() const HKV_OVERRIDE { return NULL; }
  GUI_IMPEXP virtual const VisStaticGeometryInstanceCollection_cl* GetVisibleStaticGeometryInstancesForPass(VPassType_e ePassType) const HKV_OVERRIDE { return NULL; }
  GUI_IMPEXP virtual void SetPropertiesFromRenderContext(VisRenderContext_cl *pContext) HKV_OVERRIDE {}

  GUI_IMPEXP virtual const VisFrustum_cl *GetBaseFrustum() HKV_OVERRIDE { return NULL; }

  GUI_IMPEXP void SetEntity(VisBaseEntity_cl *pEnt) {m_pEntities->Clear(); if (pEnt!=NULL) m_pEntities->AppendEntry(pEnt);}
private:

  VisEntityCollection_cl *m_pEntities;
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
