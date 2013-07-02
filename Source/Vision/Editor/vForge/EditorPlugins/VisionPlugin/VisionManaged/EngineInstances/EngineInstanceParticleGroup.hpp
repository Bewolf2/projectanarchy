/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace ManagedFramework;
using namespace CSharpFramework::Scene;

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.hpp>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceEntity.hpp>



namespace VisionManaged
{
  /*public */ref class EngineInstanceConstraint; // forward decl

  // this enum matches the edit tabs in the particle editor
  public enum class ParticlePreviewType_e
  {
    None = -1,
    LayerProperties,
    ParticleProperties,
    EmitterProperties,
    VisibilityProperties,
    PhysicsProperties
  };

  public ref class EngineInstanceParticleGroup : public IEngineInstanceObject3D
  {
  public:
    EngineInstanceParticleGroup();
    VOVERRIDE void DisposeObject() override;

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetVisible(bool bStatus) override {m_bVisible=bStatus;UpdateVisibleStatus();}
    VOVERRIDE void SetScaling(float x,float y, float z) override;

    // Overriden IEngineInstanceObject3D functions
    VOVERRIDE void *GetObject3D() override;
    VOVERRIDE void SetPosition(float x,float y,float z) override;
    VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override;

    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;

    // misc:
    bool GetLocalBoundingBox(hkvAlignedBBox &bbox);
    void SetVisibleBitmask(unsigned int iMask) {m_iVisibleBitmask=iMask;UpdateVisibleStatus();}
    void UpdateVisibleStatus()
    {
      if (!m_pGroup) return;
      m_pGroup->SetVisibleBitmask(m_bVisible ? m_iVisibleBitmask:0);
    }

    // special functions
    void RenderShape(VisionViewBase ^view, ShapeRenderMode mode);
    void RespawnAllParticles();
    void BeginUpdateParams() {m_iUpdateLockCtr++;}
    void EndUpdateParams()
    {
      m_iUpdateLockCtr--;
      if (m_iUpdateLockCtr==0)
        RespawnAllParticles();
    }

    void UpdateParticleColors();


    bool SetEffectFile(String ^effect); // loads the xml file
    bool IsHalted() {return !Vision::Editor.IsAnimatingOrPlaying();}
    //void SetEffectKey(String ^fxkey);
    void SetAmbientColor(unsigned int iColor);
    void SetPaused(bool bStatus) {if (m_pGroup) m_pGroup->SetPause(bStatus);}
    void SetHalted(bool bStatus) {if (m_pGroup) m_pGroup->SetHalted(bStatus);}
    void SetIntensity(float fIntensity) {if (m_pGroup) m_pGroup->SetIntensity(fIntensity);}
    void SetWindSpeed(Vector3F wind, bool bLocalSpace);
    void SetRemoveWhenFinished(bool value);
    void Restart();
    void SetApplyTimeOfDayLight(bool bApply) {if (m_pGroup) m_pGroup->SetApplyTimeOfDayLight (bApply);}
    bool GetApplyTimeOfDayLight(void) {if (m_pGroup) return (m_pGroup->GetApplyTimeOfDayLight ()); return (false);}
    void SetRandomBaseSeed(unsigned int uiBaseSeed) { m_uiRandomBaseSeed = uiBaseSeed; }
    void SetMeshEmitterEntity(EngineInstanceEntity ^entityInst);

    // constraint functions:
    void AddConstraint(EngineInstanceConstraint ^pConstraint);
    void RemoveConstraint(EngineInstanceConstraint ^pConstraint);
    void RemoveAllConstraints();

    static void SetPreviewType(ParticlePreviewType_e mode) {g_CurrentPreviewType = mode;}

  protected:
    void RenderPreview_LayerProperties(VisionViewBase ^view, IVRenderInterface *pRI, ParticleGroupBase_cl *pGroup);
    void RenderPreview_ParticleProperties(VisionViewBase ^view, IVRenderInterface *pRI, ParticleGroupBase_cl *pGroup);
    void RenderPreview_EmitterProperties(VisionViewBase ^view, IVRenderInterface *pRI, ParticleGroupBase_cl *pGroup);
    void RenderPreview_VisibilityProperties(VisionViewBase ^view, IVRenderInterface *pRI, ParticleGroupBase_cl *pGroup);
    void RenderPreview_PhysicsProperties(VisionViewBase ^view, IVRenderInterface *pRI, ParticleGroupBase_cl *pGroup);

    // Allows a custom implementation to supply it's own particle instance objects for SetEffectFile()
    // (void* and managed types seem to be required for matching signatures in other libraries.)
    virtual void* CreateParticleEffectInstance(void* pFXFile, Vector3F pos, Vector3F ori, unsigned int uiRandomSeed);

  private:
    VisParticleEffectFile_cl *m_pFXFile;
    VisParticleEffect_cl *m_pGroup;
    VisBaseEntity_cl *m_pMeshEmitterEntity;
    int m_iUpdateLockCtr;
    float m_fYaw,m_fPitch,m_fRoll;
    float m_fPosX,m_fPosY,m_fPosZ;
    unsigned int m_iVisibleBitmask;
    bool m_bVisible;
    unsigned int m_uiRandomBaseSeed;

    static ParticlePreviewType_e g_CurrentPreviewType = ParticlePreviewType_e::None;
  };



  public ref class EngineInstanceConstraint : public IEngineInstanceObject3D
  {
  public:
    enum class ConstraintType_e
    {
      GroundPlane,
      Plane,
      Sphere,
      XAxis, // inf cylinder x
      YAxis, // inf cylinder y
      ZAxis, // inf cylinder z
      AABox,
      Fan,
      Cyclone,
      Point,
      GravityPoint,
      Terrain,
      CameraBox,
    };

    enum class ConstraintReflectionType_e // make sure managed enum uses same values for directly casting
    {
      Nothing = CONSTRAINT_REFLECT_NOTHING,
      Bounce  = CONSTRAINT_REFLECT_BOUNCE,
      Glide   = CONSTRAINT_REFLECT_GLIDE,
      Destroy  = CONSTRAINT_REFLECT_REMOVE,
    };

    EngineInstanceConstraint();
    VOVERRIDE void DisposeObject() override;

    // overridden IEngineShapeInstance functions
    VOVERRIDE void SetVisible(bool bStatus) override  {}
    VOVERRIDE void SetPosition(float x,float y,float z) override;
    VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override;
    VOVERRIDE void SetScaling(float x,float y, float z) override  {}

    // Overriden IEngineInstanceObject3D functions
    VOVERRIDE void *GetObject3D() override;

    VOVERRIDE void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    VOVERRIDE bool OnExport(SceneExportInfo ^info) override;

    // special functions:
    void HighlightShape(VisionViewBase ^view);

    void SetType(ConstraintType_e eType);
    void SetReflection(ConstraintReflectionType_e eMode, float fPersistance);
    VisParticleConstraint_cl *GetConstraint() {return m_pConstraint;}

    // set properties (not valid for all kinds, m_eType is checked)
    void SetRadius(float fRadius);
    void SetInside(bool bStatus);
    void SetInfiniteExtent(bool bStatus);
    void SetBoxSize(float x,float y,float z);
    void SetIntensity(float fIntensity);
    void SetConeAngle(float fAngle);
    void SetLength(float fLength);
    void SetRandomness(float fVariation);
    void SetAffectBitmask(unsigned int iMask);
    void SetReflectionNoise(float fNoise);

    inline ConstraintType_e GetConstraintType() {return m_eType;}
  private:
    VisParticleConstraint_cl *m_pConstraint;
    ConstraintType_e m_eType;
    float m_fPosX,m_fPosY,m_fPosZ;
    float m_fYaw,m_fPitch,m_fRoll;
    float m_fBoxX,m_fBoxY,m_fBoxZ;
    bool m_bInfinite;
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
