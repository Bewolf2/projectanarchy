/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleDescriptor.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>
#include <Vision/Runtime/Base/ThirdParty/tinyXML/TinyXMLHelper.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>



char VisParticleGroupDescriptor_cl::g_szCurrentRelPath[FS_MAX_PATH] = {0};


///////////////////////////////////////////////////////////////////////////////////
// Particle group descriptor
///////////////////////////////////////////////////////////////////////////////////

VisParticleGroupDescriptor_cl::VisParticleGroupDescriptor_cl(VisParticleEffectFile_cl *pOwner)
  : m_pOwner(pOwner)
  , m_bActive(true)
  , m_eTopology(PARTICLE_TOPOLOGY_BILLBOARDS)
  , m_sName()
  , m_sDescription()

  , m_sTextureFilename()
  , m_sNormalmapFilename()
  , m_sSpecularmapFilename()
  , m_sRandomColorFilename()
  , m_sMeshFilename()

  // group properties
  , m_eTransp(VIS_TRANSP_ADDITIVE)
  //, m_iAnimSubDiv
  , m_iUsedAnimFrames(0)
  , m_bUseSmoothAnimation(false)
  , m_bStartRandomAnimFrame(false)
  , m_bAlwaysInForeground(false)
  , m_bHandleWhenVisible(false)// always simulate
  , m_fSizeAspect(1.f)
  , m_iFlags(PGROUPFLAGS_NONE)
  , m_GroupLifeTime(-1.f,0.f)
  , m_TimeScaling(1.f,0.f)
  , m_iGroupSortKey(0)
  , m_InitialDelay(0.f,0.f)
  , m_vParticleCenter(0.5f,0.5f)

  // visibility 
  , m_BoundingBox(hkvInit_None()) //set invalid later on
  , m_fDynamicInflateInterval(1.f) // every 1s update the bounding box
  , m_fDepthOffset(0.f)
  , m_ModColor(255,255,255,255)
  , m_fApplySceneBrightness(0.f)
  , m_bSortParticles(false)
  , m_bRepeatLifetime(false)
  , m_bSoftParticles(false)
  , m_iVisibleBitmask(0xffffffff)

  //fade
  , m_eFadeMode(VisParticleGroup_cl::FADEMODE_NONE)
  , m_fFadeStart(1000.f)
  , m_fFadeEnd(2000.f)
  , m_iRenderOrder(VRH_PARTICLES)

  // Curves
  , m_spColorCurve()
  , m_spSizeCurve()
  , m_spAnimCurve()
  , m_spColorLookup()
  , m_spRandomColorLookup()
  , m_spProbabilityFrameWeights(16, 0.f) //this is just temp data generated in the Finish method
  , m_fProbabilityTotalWeight(0)         //same here

  // particle behavior
  , m_eParticleAnimMode(PARTICLE_ANIMATION_NONE)
  , m_ParticleLifeTime(5.f,0.3f)
  , m_ParticleAnimTime(2.f,0.3f)
  , m_ParticleSpeed(100.f,0.5f)
  , m_ParticleStartSize(5.f,0.5f)
  , m_ParticleSizeGrowth(5.f,0.5f)

  // rotation
  , m_eRotationMode(PARTICLE_ROTATION_NONE)
  //, m_fInitialAngle()
  //, m_fConstRotationSpeed()
  , m_spRotationCurve()
  , m_fRotationCurveDuration(-1.f)
  , m_bRandomRotationCurveOffset(false)
  , m_bInertiaAffectsGravity(true)
  , m_bLocalSpace(false)
  , m_bUseTransformationCurve(false)

  // physics
  , m_fFriction(0.5f)
  , m_vGravity()
  , m_ParticleInertia(0.f,0.f)
  , m_iConstraintAffectBitMask(0xffffffff)
  , m_fApplyEmitterSpeed(0)

  // destroying particles
  , m_OnDestroyCreateCount(-1.f,0.f)
  , m_sDestroyCreateGroup()
  , m_iOnDestroyCopyFlags(PARTICLE_COPYFLAG_POSITION)
  , m_spDestroyCreateDesc()

  // distortion
  , m_bDistorted(false)
  , m_bDistortionSizeMode(false)
  , m_bDistortionPlaneAligned(false)
  , m_bHasCustomCenter(false)
  , m_fTrailOverlap(1.0f)

  , m_FixDistortionLength(-1,0.f)
  , m_SpeedMultiplier(0.5f,0.1f)
  , m_vSizeMultiplier(0)

   // normal
  , m_bUseNormal(false)
  , m_vNormal(0.f, 0.f, 1.0f)
  , m_fRotationAxisRandomness(1.0f)
  , m_vRelativePosition()
  , m_vRelativeOrientation()

  // spawn
  , m_DefaultEmitter()

  // shader
  , m_CustomEffectConfig()

  // local position curve
  , m_fTransformationCurveTime(1.f)
  , m_fPositionCurveRadius(1.f)
  , m_spPositionCurve()
  , m_spOrientationCurve()

  // misc
  , m_spCustomCurve()
  , m_spCustomColorCurve()
  , m_iCustomIntValue(0)

  , m_EventList()
  , m_spGeometry()
  , m_MeshBoundingBox() //setZero

  , m_iCustomRandomSeed()
{
  //uv sub division
  SetAnimationSubdivisions(1,1,1); // no animation, will init m_iAnimSubDiv

  //bounding box
  m_BoundingBox.setInvalid();

  // rotation
  m_fInitialAngle[0] = m_fInitialAngle[1] = 0.f;
  m_fConstRotationSpeed[0] = m_fConstRotationSpeed[1] = 0.f;

  //misc
  m_MeshBoundingBox.setZero();
}


VisParticleGroupDescriptor_cl::~VisParticleGroupDescriptor_cl()
{
}

/*
  Particle structure usage
    pos : position
    velocity : particle speed
    size : size
    normal[0] : size growth
    normal[1] : inertia value [0..1]
    normal[2] : lookup table row
    distortion : distortion
*/


#ifdef SUPPORTS_SNAPSHOT_CREATION

#define SAFE_ADD_TEXTURE(_vstr) \
  if (!_vstr.IsEmpty())\
  {\
    char szTexFile[FS_MAX_PATH];\
    MakeFilenameAbsolute(szTexFile,_vstr);\
    snapshot.AddDependency(pOwner, Vision::TextureManager.Load2DTexture(szTexFile));\
  }

void VisParticleGroupDescriptor_cl::GetDependencies(VResourceSnapshot &snapshot, VManagedResource *pOwner)
{
  SAFE_ADD_TEXTURE(m_sTextureFilename);
  SAFE_ADD_TEXTURE(m_sNormalmapFilename);
  SAFE_ADD_TEXTURE(m_sSpecularmapFilename);

  if (m_spRandomColorLookup!=NULL)
    snapshot.AddDependency(pOwner, m_spRandomColorLookup);
  if (m_CustomEffectConfig.GetEffect(0)!=NULL)
    m_CustomEffectConfig.GetEffect(0)->GetDependencies(snapshot, pOwner);
}

#endif

enum VParticleGroupDescriptorVersion_e
{
  PARTICLE_GROUP_DESCRIPTOR_VERSION_01 = 1,
  PARTICLE_GROUP_DESCRIPTOR_VERSION_02 = 2,
  PARTICLE_GROUP_DESCRIPTOR_VERSION_03 = 3,
  PARTICLE_GROUP_DESCRIPTOR_VERSION_04 = 4,
  PARTICLE_GROUP_DESCRIPTOR_VERSION_05 = 5,
  PARTICLE_GROUP_DESCRIPTOR_VERSION_06 = 6,
  PARTICLE_GROUP_DESCRIPTOR_VERSION_07 = 7,
  PARTICLE_GROUP_DESCRIPTOR_VERSION_08 = 8,
  PARTICLE_GROUP_DESCRIPTOR_VERSION_09 = 9,  //binary format of render hook constants changed
  PARTICLE_GROUP_DESCRIPTOR_VERSION_10 = 10, //render hook mismatch fix
  PARTICLE_GROUP_DESCRIPTOR_VERSION_CURRENT = PARTICLE_GROUP_DESCRIPTOR_VERSION_10
};

V_IMPLEMENT_SERIALX( VisParticleGroupDescriptor_cl);
void VisParticleGroupDescriptor_cl::SerializeX( VArchive &ar )
{
  char iVersion = PARTICLE_GROUP_DESCRIPTOR_VERSION_CURRENT;
  if (ar.IsLoading())
  {
    ar >> iVersion; VASSERT_MSG(iVersion<=PARTICLE_GROUP_DESCRIPTOR_VERSION_CURRENT, "Invalid version number");
    ar >> m_bActive;
    ar >> m_sName;
    if (iVersion>=PARTICLE_GROUP_DESCRIPTOR_VERSION_06)
      ar >> m_sDescription;

    if (iVersion>=PARTICLE_GROUP_DESCRIPTOR_VERSION_02)
    {
      ar >> m_eTopology;
      ar >> m_sMeshFilename;
    }
    ar >> m_sTextureFilename;
    ar >> m_sNormalmapFilename;
    ar >> m_sSpecularmapFilename;
    ar >> m_sRandomColorFilename;
    ar >> (int &)m_eTransp;
    ar >> m_iAnimSubDiv[0] >> m_iAnimSubDiv[1];
    ar >> m_iUsedAnimFrames;
    ar >> m_bUseSmoothAnimation;
    ar >> m_bStartRandomAnimFrame;
    ar >> m_bAlwaysInForeground;
    ar >> m_bHandleWhenVisible;
    ar >> m_fSizeAspect;
    ar >> m_iFlags;
    ar >> m_GroupLifeTime;
    ar >> m_TimeScaling;
    ar >> m_iGroupSortKey;
    ar >> m_InitialDelay;
    ar >> m_vParticleCenter;
    m_BoundingBox.SerializeAs_VisBoundingBox (ar);
    ar >> m_fDynamicInflateInterval;
    ar >> m_fDepthOffset;
    ar >> m_ModColor;
    // Backwards compatibility
    if (iVersion>=PARTICLE_GROUP_DESCRIPTOR_VERSION_04)
    {
      ar >> m_fApplySceneBrightness;
    }
    else
    {
      bool bUseSceneBrightness;
      ar >> bUseSceneBrightness;
      m_fApplySceneBrightness = bUseSceneBrightness ? 1.0f : 0.0f;
    }
    bool bDummy;
    ar >> bDummy;
    ar >> m_bSortParticles;
    ar >> m_bRepeatLifetime;

    if (iVersion >= PARTICLE_GROUP_DESCRIPTOR_VERSION_05)
      ar >> m_bSoftParticles;
    ar >> m_iVisibleBitmask;

    VColorRef dummy;
    ar >> dummy;
    ar >> dummy;
    ar >> (int &)m_eFadeMode;
    ar >> m_fFadeStart;
    ar >> m_fFadeEnd;

    if(iVersion >= PARTICLE_GROUP_DESCRIPTOR_VERSION_10)
      ar >> m_iRenderOrder;
    else if(iVersion >= PARTICLE_GROUP_DESCRIPTOR_VERSION_09)
    {
      int iOrder;
      ar >> iOrder;

      // toggle VRH_ADDITIVE_PARTICLES with VRH_TRANSLUCENT_VOLUMES so that the VRenderHook_e enum order matches the execution order
      if (iOrder == VRH_ADDITIVE_PARTICLES)       m_iRenderOrder = VRH_TRANSLUCENT_VOLUMES;
      else if (iOrder == VRH_TRANSLUCENT_VOLUMES) m_iRenderOrder = VRH_ADDITIVE_PARTICLES;
      else                                        m_iRenderOrder = iOrder;
    }
    else
    {
      int iTempRenderOrder;
      ar >> iTempRenderOrder;
      m_iRenderOrder = UpdateRenderHook(iTempRenderOrder);
    }
    
    m_spColorCurve = VColorCurve::DoArchiveLookupExchange(ar,m_spColorCurve);
    m_spSizeCurve = VCurve2D::DoArchiveLookupExchange(ar,m_spSizeCurve);
    m_spAnimCurve = VCurve2D::DoArchiveLookupExchange(ar,m_spAnimCurve);

    ar >> (int &)m_eParticleAnimMode;
    ar >> m_ParticleLifeTime;
    ar >> m_ParticleAnimTime;
    ar >> m_ParticleSpeed;
    ar >> m_ParticleStartSize;
    ar >> m_ParticleSizeGrowth;

    ar >>  (int &)m_eRotationMode;
    ar >> m_fInitialAngle[0] >> m_fInitialAngle[1];
    ar >> m_fConstRotationSpeed[0] >> m_fConstRotationSpeed[1];
    m_spRotationCurve = VCurve2D::DoArchiveLookupExchange(ar,m_spRotationCurve);
    ar >> m_fRotationCurveDuration;
    ar >> m_bRandomRotationCurveOffset;
    ar >> m_bInertiaAffectsGravity;
    ar >> m_bLocalSpace;
    ar >> m_bUseTransformationCurve;

    ar >> m_fFriction;
    m_vGravity.SerializeAsVisVector (ar);
    ar >> m_ParticleInertia;
    ar >> m_iConstraintAffectBitMask;
    ar >> m_fApplyEmitterSpeed;

    ar >> m_OnDestroyCreateCount;
    ar >> m_sDestroyCreateGroup;
    ar >> m_iOnDestroyCopyFlags;

    ar >> m_bDistorted;
    ar >> m_bDistortionSizeMode;
    ar >> m_bDistortionPlaneAligned;
    ar >> m_bHasCustomCenter;

    ar >> m_FixDistortionLength;
    ar >> m_SpeedMultiplier;
    ar >> m_vSizeMultiplier;
    if (iVersion >= PARTICLE_GROUP_DESCRIPTOR_VERSION_07)
      ar >> m_fTrailOverlap; // vers.7

    ar >> m_bUseNormal;
    m_vNormal.SerializeAsVisVector (ar);
    m_vRelativePosition.SerializeAsVisVector (ar);
    m_vRelativeOrientation.SerializeAsVisVector (ar);

    if (iVersion >= PARTICLE_GROUP_DESCRIPTOR_VERSION_08)
      ar >> m_fRotationAxisRandomness; // vers.8

    ar >> m_DefaultEmitter;

    ar >> m_CustomEffectConfig;

    ar >> m_fTransformationCurveTime >> m_fPositionCurveRadius;
    bool bBrokenPosCurve = iVersion<=2;
    m_spPositionCurve = VPositionCurve::DoArchiveLookupExchange(ar, m_spPositionCurve, bBrokenPosCurve);
    m_spOrientationCurve = VPositionCurve::DoArchiveLookupExchange(ar, m_spOrientationCurve, bBrokenPosCurve);

    m_spCustomCurve = VCurve2D::DoArchiveLookupExchange(ar,m_spCustomCurve);
    m_spCustomColorCurve = VColorCurve::DoArchiveLookupExchange(ar,m_spCustomColorCurve);
    ar >> m_iCustomIntValue;

    ar >> m_EventList;
    Finish(true);
  }
  else
  {
    ar << iVersion;
    ar << m_bActive;
    ar << m_sName;
    ar << m_sDescription;  // version 6
    ar << m_eTopology;     // version 2
    ar << m_sMeshFilename; // version 2

    ar << m_sTextureFilename;
    ar << m_sNormalmapFilename;
    ar << m_sSpecularmapFilename;
    ar << m_sRandomColorFilename;

    ar << (int &)m_eTransp;
    ar << m_iAnimSubDiv[0] << m_iAnimSubDiv[1];
    ar << m_iUsedAnimFrames;
    ar << m_bUseSmoothAnimation;
    ar << m_bStartRandomAnimFrame;
    ar << m_bAlwaysInForeground;
    ar << m_bHandleWhenVisible;
    ar << m_fSizeAspect;
    ar << m_iFlags;
    ar << m_GroupLifeTime;
    ar << m_TimeScaling;
    ar << m_iGroupSortKey;
    ar << m_InitialDelay;
    ar << m_vParticleCenter;
    m_BoundingBox.SerializeAs_VisBoundingBox (ar);
    ar << m_fDynamicInflateInterval;
    ar << m_fDepthOffset;
    ar << m_ModColor;
    ar << m_fApplySceneBrightness;

    bool bDummy = false;
    ar << bDummy;
    ar << m_bSortParticles;
    ar << m_bRepeatLifetime;
    ar << m_bSoftParticles;
    ar << m_iVisibleBitmask;

    VColorRef dummy (0, 0, 0, 0);
    ar << dummy;
    ar << dummy;
    ar << (int &)m_eFadeMode;
    ar << m_fFadeStart;
    ar << m_fFadeEnd;
    ar << m_iRenderOrder;
    
    VColorCurve::DoArchiveLookupExchange(ar,m_spColorCurve);
    VCurve2D::DoArchiveLookupExchange(ar,m_spSizeCurve);
    VCurve2D::DoArchiveLookupExchange(ar,m_spAnimCurve);

    ar << (int &)m_eParticleAnimMode;
    ar << m_ParticleLifeTime;
    ar << m_ParticleAnimTime;
    ar << m_ParticleSpeed;
    ar << m_ParticleStartSize;
    ar << m_ParticleSizeGrowth;

    ar <<  (int &)m_eRotationMode;
    ar << m_fInitialAngle[0] << m_fInitialAngle[1];
    ar << m_fConstRotationSpeed[0] << m_fConstRotationSpeed[1];
    VCurve2D::DoArchiveLookupExchange(ar,m_spRotationCurve);
    ar << m_fRotationCurveDuration;
    ar << m_bRandomRotationCurveOffset;
    ar << m_bInertiaAffectsGravity;
    ar << m_bLocalSpace;
    ar << m_bUseTransformationCurve;

    ar << m_fFriction;
    m_vGravity.SerializeAsVisVector (ar);
    ar << m_ParticleInertia;
    ar << m_iConstraintAffectBitMask;
    ar << m_fApplyEmitterSpeed;

    ar << m_OnDestroyCreateCount;
    ar << m_sDestroyCreateGroup;
    ar << m_iOnDestroyCopyFlags;

    ar << m_bDistorted;
    ar << m_bDistortionSizeMode;
    ar << m_bDistortionPlaneAligned;
    ar << m_bHasCustomCenter;

    ar << m_FixDistortionLength;
    ar << m_SpeedMultiplier;
    ar << m_vSizeMultiplier;
    ar << m_fTrailOverlap; // vers.7

    ar << m_bUseNormal;
    m_vNormal.SerializeAsVisVector (ar);
    m_vRelativePosition.SerializeAsVisVector (ar);
    m_vRelativeOrientation.SerializeAsVisVector (ar);

    ar << m_fRotationAxisRandomness; // vers.8
    

    ar << m_DefaultEmitter;

    ar << m_CustomEffectConfig;

    ar << m_fTransformationCurveTime << m_fPositionCurveRadius;
    // version 3 saves out correct elements
    VPositionCurve::DoArchiveLookupExchange(ar, m_spPositionCurve);
    VPositionCurve::DoArchiveLookupExchange(ar, m_spOrientationCurve);

    VCurve2D::DoArchiveLookupExchange(ar,m_spCustomCurve);
    VColorCurve::DoArchiveLookupExchange(ar,m_spCustomColorCurve);
    ar << m_iCustomIntValue;

    ar << m_EventList;
  }
}


// static helper
bool VisParticleGroupDescriptor_cl::TechniqueUsesLightGrid(VCompiledTechnique *pTech)
{
  if (!pTech)
    return false;
  for (int i=0;i<pTech->GetShaderCount();i++)
    if (pTech->GetShader(i)->GetRenderState ()->GetTrackingMask () & (VSHADER_TRACKING_LIGHTGRID_PS | VSHADER_TRACKING_LIGHTGRID_VS | VSHADER_TRACKING_LIGHTGRID_GS))
      return true;
  return false;
}


void VisParticleGroupDescriptor_cl::Finish(bool bForceCreateLookups)
{
  if (m_spColorCurve && !m_spColorLookup)
  {
    if (m_spColorCurve->IsValid() && bForceCreateLookups)
      m_spColorCurve->CreateLookup(256);
    m_spColorLookup = m_spColorCurve->GetLookupBitmap();
  }

  if (m_spSizeCurve && m_spSizeCurve->IsValid() && bForceCreateLookups)
    m_spSizeCurve->CreateLookup(256, m_ParticleSizeGrowth.m_fAverage);

  if (m_spAnimCurve && m_spAnimCurve->IsValid() && bForceCreateLookups)
    m_spAnimCurve->CreateLookup(256, (float)m_iUsedAnimFrames-0.001f);

  // this curve is very sensitive to undersampling so use GetSuitableTableSize to determine the number of samples
  float fRotationDuration = m_fRotationCurveDuration>0.f ? m_fRotationCurveDuration : this->m_ParticleLifeTime.m_fAverage;
  if (m_spRotationCurve && m_spRotationCurve->IsValid() && bForceCreateLookups)
    m_spRotationCurve->CreateLookup(m_spRotationCurve->GetSuitableTableSize(fRotationDuration)); // already RAD

  if (m_spCustomCurve && m_spCustomCurve->IsValid() && bForceCreateLookups)
    m_spCustomCurve->CreateLookup(256);

  if (m_spCustomColorCurve && m_spCustomColorCurve->IsValid() && bForceCreateLookups)
    m_spCustomColorCurve->CreateLookup(256);

  if (m_DefaultEmitter.m_spCountCurve && m_DefaultEmitter.m_spCountCurve->IsValid() && bForceCreateLookups)
    m_DefaultEmitter.m_spCountCurve->CreateLookup(256, m_DefaultEmitter.m_ParticlesPerSec.m_fAverage);

  if (m_spPositionCurve && m_spPositionCurve->IsValid() && bForceCreateLookups)
    m_spPositionCurve->CreateLookup(256,m_fPositionCurveRadius);

  if (m_spOrientationCurve && m_spOrientationCurve->IsValid() && bForceCreateLookups)
    m_spOrientationCurve->CreateLookup(256);

  if (!m_bHasCustomCenter) // fixup non-default center for backwards compatibility
  {
    if (m_bDistorted)
    {
      if (m_bDistortionSizeMode)
        m_vParticleCenter.x = -m_vSizeMultiplier.w;
      else
        m_vParticleCenter.x = 0.f;
    }
  }

  // pre-cache texture files:
  char szTexFilename[FS_MAX_PATH];
  if (!m_sTextureFilename.IsEmpty())
  {
    MakeFilenameAbsolute(szTexFilename,m_sTextureFilename);
    Vision::TextureManager.Load2DTexture(szTexFilename);
  }
  if (!m_sNormalmapFilename.IsEmpty())
  {
    MakeFilenameAbsolute(szTexFilename,m_sNormalmapFilename);
    Vision::TextureManager.Load2DTexture(szTexFilename);
  }
  if (!m_sSpecularmapFilename.IsEmpty())
  {
    MakeFilenameAbsolute(szTexFilename,m_sSpecularmapFilename);
    Vision::TextureManager.Load2DTexture(szTexFilename);
  }
  m_spRandomColorLookup = NULL;
  if (!m_sRandomColorFilename.IsEmpty())
  {
    MakeFilenameAbsolute(szTexFilename,m_sRandomColorFilename);
    m_spRandomColorLookup = VisBitmap_cl::LoadBitmapFromFile(szTexFilename);
    if (m_spRandomColorLookup != NULL && m_spRandomColorLookup->GetWidth()==0)
      m_spRandomColorLookup = NULL;
  }

  // backwards compatibility fix-up: extract from other properties
  if (m_eTopology==PARTICLE_TOPOLOGY_UNKNOWN)
  {
    if (m_bDistortionSizeMode && hkvMath::isFloatEqual (m_vSizeMultiplier.w,-0.5f))
      m_eTopology = PARTICLE_TOPOLOGY_RINGWAVE;
    else if (m_FixDistortionLength.m_fAverage < 0.0f && m_SpeedMultiplier.m_fAverage < 0.0f)
      m_eTopology = PARTICLE_TOPOLOGY_BILLBOARDS;
    else if (m_SpeedMultiplier.m_fAverage < 0.0f)
      m_eTopology = PARTICLE_TOPOLOGY_STRETCH_FIXLENGTH;
    else if (m_FixDistortionLength.m_fAverage < 0.0f)
      m_eTopology = PARTICLE_TOPOLOGY_STRETCH_VELOCITY;
    else if (!m_sMeshFilename.IsEmpty())
      m_eTopology = PARTICLE_TOPOLOGY_MESH;
  }
  VASSERT(m_eTopology!=PARTICLE_TOPOLOGY_UNKNOWN);

  m_MeshBoundingBox.setZero();
  m_spGeometry = NULL;
  if (!m_sMeshFilename.IsEmpty() && m_eTopology==PARTICLE_TOPOLOGY_MESH)
  {
    char szMeshFile[FS_MAX_PATH];
    MakeFilenameAbsolute(szMeshFile,m_sMeshFilename);
    VBaseMesh *pMesh = NULL;
    if (VFileHelper::HasExtension(szMeshFile,"model"))
    {
      pMesh = Vision::Game.LoadDynamicMesh(szMeshFile,true,false);
    } 
    else if (VFileHelper::HasExtension(szMeshFile,"vmesh"))
    {
      pMesh = VisStaticMesh_cl::LoadStaticMesh(szMeshFile);
    }
    if (pMesh!=NULL)
    {
      m_MeshBoundingBox = pMesh->GetBoundingBox();
      m_spGeometry = pMesh->GetMeshBuffer();

#if defined(_VISION_XENON) || defined(_VISION_PS3)
      m_spGeometry = m_spGeometry->FlattenOutIndexBuffer();
#endif
    }
  }

  //probability curve (instead of an animation curve)
  if (m_bStartRandomAnimFrame && m_spAnimCurve!=NULL &&
      m_eParticleAnimMode==PARTICLE_ANIMATION_NONE && m_iUsedAnimFrames>1)
  {
    m_fProbabilityTotalWeight = 0;
    
    for(int i=0; i<m_iUsedAnimFrames; i++)
    {
      //get the accumulated probability weight for the current frame
      m_spProbabilityFrameWeights[i] = m_fProbabilityTotalWeight + m_spAnimCurve->GetValueFastInterpolated((1.f/(m_iUsedAnimFrames-1))*i);

      //update the overall weight
      m_fProbabilityTotalWeight =  m_spProbabilityFrameWeights[i];
    }
  }


  m_DefaultEmitter.OnXMLExchangeFinished(this);
}

static inline float RndBetween(float fMin, float fMax, const VRandom& randGen)
{
  return randGen.GetFloat()*(fMax-fMin)+fMin;
}

void VisParticleGroupDescriptor_cl::InitRandomParticle(ParticleExt_t *pParticle, ParticleGroupBase_cl *pGroup, const ParticleExt_t *pSrcParticle, int iCopyFlags)
{
  VASSERT(pParticle && pGroup);
  VASSERT(pSrcParticle || !iCopyFlags); // if copyflags are defined, we must pass a source particle
  VisParticleEmitter_cl *pEmitter = pGroup->GetEmitter();
  VASSERT(pEmitter);

  VASSERT((pParticle - pGroup->GetParticlesExt())<pGroup->m_iHighWaterMark);
  pParticle->valid = 1;
  const float fScale = pGroup->m_fScaling;
  const BOOL bLocalSpace = pGroup->GetUseLocalSpaceMatrix();
  const VRandom& randGen = pGroup->GetRandom();
  VColorRef iThisColor = m_ModColor;

  // put lifetime counter etc. in the "normal" member of the particle.
  // Thus "use normal" must not be set anymore
  float fLifeTime = GetRandomLifeTime(randGen);
  pParticle->m_fLifeTimeCounter = 0.f; // counts from 0.f to 1.f
  if (fLifeTime>0.f)
    pParticle->m_fLifeTimeInc = 1.f/fLifeTime; // lifetime increment
  else pParticle->m_fLifeTimeInc = 0.f;

  pParticle->m_fInertiaFactor = 1.f-m_ParticleInertia.GetRandomValue(randGen);
  if (pParticle->m_fInertiaFactor<0.f) pParticle->m_fInertiaFactor=0.f;

  if (iCopyFlags&PARTICLE_COPYFLAG_SIZE)
  {
    pParticle->size = pSrcParticle->size;
    pParticle->m_fSizeGrowth = pSrcParticle->m_fSizeGrowth;
  }
  else
  {
    if (m_spSizeCurve)
    {
      // when using size curve, the growth value specifies the lookup scaling. Get it from the descriptor's size
      // NB: m_ParticleStartSize average value is always 10.f which scales particles by factor 10 (the real maximum is encoded in the curve). 
      //     Currently the editor's GUI compensates for this factor 10.
      pParticle->size = 0.f; // get real size in the update function, but init with 0.0 so fSizeDiff is correct in DISTORTION_TYPE_SIZEMODE
      pParticle->m_fSizeGrowth = fScale * m_ParticleStartSize.GetRandomValue(randGen);
    } else
    {
      // this path is no longer supported. Match the version above
      pParticle->size = fScale * m_ParticleSizeGrowth.GetRandomValue(randGen) * m_ParticleStartSize.GetRandomValue(randGen);
      pParticle->m_fSizeGrowth = 0.f;
    }
  }

  hkvMat3 emitCS(hkvNoInitialization);
  bool bEmitCSValid = false;

  // position
  if (iCopyFlags&PARTICLE_COPYFLAG_POSITION)
  {
    pParticle->pos[0] = pSrcParticle->pos[0];
    pParticle->pos[1] = pSrcParticle->pos[1];
    pParticle->pos[2] = pSrcParticle->pos[2];
  } else
  {
    VASSERT(pEmitter);
    pEmitter->SpawnSingleParticle(pParticle, pGroup, iThisColor, emitCS, bEmitCSValid);
  }

  if (iCopyFlags&PARTICLE_COPYFLAG_VELOCITY)
  {
    pParticle->velocity[0] = pSrcParticle->velocity[0];
    pParticle->velocity[1] = pSrcParticle->velocity[1];
    pParticle->velocity[2] = pSrcParticle->velocity[2];
  } else
  {
    VASSERT(pEmitter);
    // initial velocity
    hkvVec3 vTempVec(hkvNoInitialization);
    if (pEmitter->m_fConeAngle>=0.f)
    {
      float fYaw = randGen.GetFloat()*hkvMath::pi ()*2.f;
      float fPitch = hkvMath::Deg2Rad (pEmitter->m_fMinConeAngle + randGen.GetFloat()*(pEmitter->m_fConeAngle-pEmitter->m_fMinConeAngle));
      float fSinP  = hkvMath::sinRad (fPitch);
      float fCosP  = hkvMath::cosRad (fPitch);

      const hkvMat3& finalEmitCS(bEmitCSValid ? emitCS : pGroup->GetRotationMatrix());

      vTempVec.set(fCosP,0.f,0.f);
      if (pEmitter->m_eConeMode!=CONE_ANGLE_MODE_YAW_ONLY)
        vTempVec.z += fSinP * hkvMath::sinRad (fYaw);
      if (pEmitter->m_eConeMode!=CONE_ANGLE_MODE_PITCH_ONLY)
        vTempVec.y += fSinP * hkvMath::cosRad (fYaw);

      if (!bLocalSpace || bEmitCSValid)
        vTempVec = finalEmitCS * vTempVec; // transform with emitter coordinate system
      vTempVec.setLength(GetRandomSpeed(randGen));
    } else
    {
      // all directions
      VisParticleEmitter_cl::Helper_GetRandomOnSpherePosition(vTempVec,GetRandomSpeed(randGen), randGen);
    }
    pParticle->velocity[0] = fScale * vTempVec.x;
    pParticle->velocity[1] = fScale * vTempVec.y;
    pParticle->velocity[2] = fScale * vTempVec.z;
  }

  // distortion
  if (m_bDistorted)
  {
    const char &cDistortion(pGroup->m_cUseDistortion);
    if (m_bDistortionSizeMode)
    {
      pParticle->distortion[0] = m_vSizeMultiplier.x * pParticle->size;
      pParticle->distortion[1] = m_vSizeMultiplier.y * pParticle->size;
      pParticle->distortion[2] = m_vSizeMultiplier.z * pParticle->size;
    }
    else if (cDistortion==DISTORTION_TYPE_FIXLEN)
    {
      //pParticle->distorted = DISTORTION_TYPE_FIXLEN;
      pParticle->m_fDistortionMult = - m_FixDistortionLength.GetRandomValue(randGen);
      hkvVec3 dist(pParticle->velocity[0],pParticle->velocity[1],pParticle->velocity[2]);
      if (!dist.isZero ())
      {
        dist.setLength(pParticle->m_fDistortionMult);
        pParticle->distortion[0] = fScale * dist.x;
        pParticle->distortion[1] = fScale * dist.y;
        pParticle->distortion[2] = fScale * dist.z;
      } else
      {
        pParticle->distortion[0] = 0.f;
        pParticle->distortion[1] = 0.f;
        pParticle->distortion[2] = 0.f;
      }
    } 
    else if (cDistortion==DISTORTION_TYPE_VELOCITY)
    {
      //pParticle->distorted = DISTORTION_TYPE_VELOCITY;
      pParticle->m_fDistortionMult = - m_SpeedMultiplier.GetRandomValue(randGen);
      pParticle->distortion[0] = fScale * pParticle->m_fDistortionMult * pParticle->velocity[0];
      pParticle->distortion[1] = fScale * pParticle->m_fDistortionMult * pParticle->velocity[1];
      pParticle->distortion[2] = fScale * pParticle->m_fDistortionMult * pParticle->velocity[2];
    } else
    {
      pParticle->distortion[0] = pParticle->distortion[1] = pParticle->distortion[2] = 0.f;
    }
    // this looks better when spawning distorted particles
    const float fStartMove = 0.9f; // if 1.0 then the distorted particles initially moved completely outside the emitter
    pParticle->pos[0] -= pParticle->distortion[0]*fStartMove;
    pParticle->pos[1] -= pParticle->distortion[1]*fStartMove;
    pParticle->pos[2] -= pParticle->distortion[2]*fStartMove;
  }
  else
  {
    pParticle->dummyMustBeOne = 1.0f;
    pParticle->dummySize = pParticle->size;
  }

  // normal
  if (m_bUseNormal) 
  {
    hkvVec3& nrml = (hkvVec3&) pParticle->normal;
    nrml = fScale * m_vNormal;

    if (!bLocalSpace)
      nrml = pGroup->m_cachedRotMatrix * nrml; // rotate normal
  } 
  else if (m_spGeometry!=NULL)
  {
    // for geometry shaders the normal defines the rotation axis
    hkvVec3& nrml = (hkvVec3&) pParticle->normal;
    VisParticleEmitter_cl::Helper_GetRandomSpherePosition(nrml,m_fRotationAxisRandomness, randGen); // m_vNormal.w defines the randomness for the axis
    nrml += m_vNormal; // regular direction
    nrml.normalizeIfNotZero();
  }


  float fAnimTime;

  // animation frame
  switch (m_eParticleAnimMode)
  {
    case PARTICLE_ANIMATION_REPEAT:
      fAnimTime = m_ParticleAnimTime.GetRandomValue(randGen);
      if (fAnimTime>0.f)
        pParticle->m_fAnimInc = 1.f/fAnimTime;
      else pParticle->m_fAnimInc = 0.f;
      break;
    case PARTICLE_ANIMATION_LIFETIME:
      pParticle->m_fAnimInc = pParticle->m_fLifeTimeInc;
      break;
  }

  if (m_bStartRandomAnimFrame)
  {
    pParticle->m_fAnimPos = randGen.GetFloat();
    // in this mode use the animation curve as a probability map:
    if (m_spAnimCurve!=NULL && m_eParticleAnimMode==PARTICLE_ANIMATION_NONE && m_iUsedAnimFrames>1)
    {
      pGroup->SetParticleAnimFrame(pParticle, (float)binaryWeightSearch(pParticle->m_fAnimPos*m_fProbabilityTotalWeight), false);
    }
    else
    {
      pGroup->SetParticleAnimFrame(pParticle, pParticle->m_fAnimPos*((float)m_iUsedAnimFrames-0.001f), m_eParticleAnimMode == PARTICLE_ANIMATION_REPEAT);
    }
  } else
  {
    pParticle->m_fAnimPos = 0.f;
    pGroup->SetParticleAnimFrame(pParticle,0.f);
  }

  // rotation
  switch (m_eRotationMode)
  {
    case PARTICLE_ROTATION_NONE:
      pParticle->angle = hkvMath::Deg2Rad (RndBetween(m_fInitialAngle[0],m_fInitialAngle[1],randGen));
      pParticle->m_fRotationParam0 = 0.f;
      break;
    case PARTICLE_ROTATION_CONSTSPEED:
      pParticle->angle = hkvMath::Deg2Rad (RndBetween(m_fInitialAngle[0],m_fInitialAngle[1],randGen));
      pParticle->m_fRotationParam0 = hkvMath::Deg2Rad (RndBetween(m_fConstRotationSpeed[0],m_fConstRotationSpeed[1],randGen));
      break;
    case PARTICLE_ROTATION_ANGLECURVE:
      {
        pParticle->angle = 0.f;
        pParticle->m_fRotationParam0 = m_bRandomRotationCurveOffset ? (randGen.GetFloat()*0.999f) : 0.f;
        float fDuration = m_fRotationCurveDuration>0.f ? m_fRotationCurveDuration : (fLifeTime/(1.f-pParticle->m_fRotationParam0));
        pParticle->m_fRotationParam1 = 1.f/fDuration;
      }
      break;
    default:
      VASSERT(!"Unsupported rotation mode");
  }
  
  // assign modulation color (and get a random gradient if necessary)
  if (m_spRandomColorLookup!=NULL)
  {
    pParticle->m_ModColor = iThisColor * m_spRandomColorLookup->LookupTexelColor(randGen.GetInt()%m_spRandomColorLookup->GetWidth(),randGen.GetInt()%m_spRandomColorLookup->GetHeight());
  }
  else
    pParticle->m_ModColor = iThisColor;
}


void VisParticleGroupDescriptor_cl::MakeFilenameAbsolute(char *szDest, const char *szFilename)
{
  VASSERT_MSG (szFilename != NULL, "VisParticleGroupDescriptor_cl::MakeFilenameAbsolute: The given filename must be != NULL.");
  
  if (m_pOwner)
  {
    char szPath[FS_MAX_PATH];
    VFileHelper::GetFileDir(m_pOwner->GetFilename(),szPath);
    VFileHelper::CombineDirAndFile(szDest,szPath,szFilename);
  }
  else
  {
    strcpy(szDest,szFilename);
  }
}


bool VisParticleGroupDescriptor_cl::DataExchangeXML(TiXmlElement *pRoot, bool bWrite)
{
  if (!pRoot) return false;

  // name & description
  TiXmlElement *pDescription = XMLHelper::SubNode(pRoot,"description",bWrite);
  if (pDescription)
  {
    XMLHelper::Exchange_VString(pDescription,"name",m_sName,bWrite);
    XMLHelper::Exchange_VString(pDescription,"description",m_sDescription,bWrite);
  }

  // default emitter properties
  TiXmlElement *pEmitter = XMLHelper::SubNode(pRoot,"defaultemitter",bWrite);
  if (pEmitter)
  {
    m_DefaultEmitter.DataExchangeXML(pEmitter,bWrite);
  }

  // group properties
  TiXmlElement *pGroup = XMLHelper::SubNode(pRoot,"group",bWrite);
  if (pGroup)
  {
    int eTopology = bWrite ? (int)m_eTopology : PARTICLE_TOPOLOGY_UNKNOWN;
    const int eTopologies[6] = {PARTICLE_TOPOLOGY_BILLBOARDS,PARTICLE_TOPOLOGY_STRETCH_VELOCITY,PARTICLE_TOPOLOGY_STRETCH_FIXLENGTH,PARTICLE_TOPOLOGY_RINGWAVE,PARTICLE_TOPOLOGY_MESH,PARTICLE_TOPOLOGY_TRAIL};
    const char* szTopologies[6] = {"Billboards","StretchVelocity","StretchFixLength","RingWave","Mesh","Trail"};
    XMLHelper::Exchange_Enum(pGroup,"topology",eTopology,sizeof(eTopologies)/sizeof(eTopologies[0]),szTopologies,eTopologies,bWrite);
    m_eTopology = eTopology;
    XMLHelper::Exchange_Float(pGroup,"trailoverlap",m_fTrailOverlap,bWrite);
    XMLHelper::Exchange_Bool(pGroup,"localspace",m_bLocalSpace,bWrite);
    /*iRead =*/ XMLHelper::Exchange_Floats(pGroup,"localpos",m_vRelativePosition.data,3,bWrite);
    /*iRead =*/ XMLHelper::Exchange_Floats(pGroup,"localori",m_vRelativeOrientation.data,3,bWrite);
    m_spPositionCurve = VPositionCurve::Exchange_PositionCurve(pGroup,"positioncurve",m_spPositionCurve,bWrite);
    m_spOrientationCurve = VPositionCurve::Exchange_PositionCurve(pGroup,"orientationcurve",m_spOrientationCurve,bWrite);
    XMLHelper::Exchange_Float(pGroup,"tcurvetime",m_fTransformationCurveTime,bWrite);
    XMLHelper::Exchange_Float(pGroup,"tcurveradius",m_fPositionCurveRadius,bWrite);
    XMLHelper::Exchange_Bool(pGroup,"usetcurve",m_bUseTransformationCurve,bWrite);


    // particle group sorting
    TiXmlElement *pGroupSorting = XMLHelper::SubNode(pGroup,"groupsorting",bWrite);
    if (pGroupSorting)
    {
      XMLHelper::Exchange_Int(pGroupSorting,"sortkey",m_iGroupSortKey,bWrite);
    }
    
    m_GroupLifeTime.DataExchangeXML("lifetime",pGroup,bWrite);
    m_TimeScaling.DataExchangeXML("timescale",pGroup,bWrite); 
    m_InitialDelay.DataExchangeXML("delay",pGroup,bWrite); 

    // group texture+transparency
    TiXmlElement *pTexture = XMLHelper::SubNode(pGroup,"texture",bWrite);
    if (pTexture)
    {
      if (!bWrite)
        m_iUsedAnimFrames = -1;
      const char *szFilename = XMLHelper::Exchange_String(pTexture,"filename",m_sTextureFilename,bWrite);
      if (szFilename) // NULL when writing
        SetTextureFilename(szFilename);
      szFilename = XMLHelper::Exchange_String(pTexture,"normalmap",m_sNormalmapFilename,bWrite);
      if (szFilename) // NULL when writing
        m_sNormalmapFilename = szFilename;

      VASSERT(sizeof(m_eTransp)==sizeof(int));
      XMLHelper::Exchange_Int(pTexture,"transparency",(int &)m_eTransp,bWrite);
      XMLHelper::Exchange_Float(pTexture,"sizeaspect",m_fSizeAspect,bWrite);
      XMLHelper::Exchange_Ints(pTexture,"animationsubdiv",m_iAnimSubDiv,2,bWrite);
      XMLHelper::Exchange_Int(pTexture,"usedframes",m_iUsedAnimFrames,bWrite);
      if (!bWrite && m_iUsedAnimFrames<0)
        m_iUsedAnimFrames = GetAnimationFrameCount();
      m_bHasCustomCenter = XMLHelper::Exchange_Floats(pTexture,"center",m_vParticleCenter.data,2,bWrite)>0;
    }
    

    // group lighting
    TiXmlElement *pLighting = XMLHelper::SubNode(pGroup,"lighting",bWrite);
    if (pLighting)
    {    
      XMLHelper::Exchange_Color(pLighting,"modcolor",m_ModColor,bWrite);
      // Backwards compatibility
      if (!bWrite)
      {
        bool bUseSceneBrightness;
        XMLHelper::Exchange_Bool(pLighting,"usescenebrightness",bUseSceneBrightness,bWrite);
        m_fApplySceneBrightness = bUseSceneBrightness ? 1.0f : 0.0f;
      }
      XMLHelper::Exchange_Float(pLighting,"applyscenebrightness",m_fApplySceneBrightness,bWrite);
      XMLHelper::Exchange_VString(pLighting,"randomcolor",m_sRandomColorFilename,bWrite);
    }

    TiXmlElement *pGeometry = XMLHelper::SubNode(pGroup,"geometry",bWrite);
    if (pGeometry)
    {
      XMLHelper::Exchange_VString(pGeometry,"meshfile",m_sMeshFilename,bWrite);
      
    }

    // shader node
    m_CustomEffectConfig.XMLExchange(pGroup,bWrite,NULL); // rel path?

    // visibility
    TiXmlElement *pVisibility = XMLHelper::SubNode(pGroup,"visibility",bWrite);
    if (pVisibility)
    {
      XMLHelper::Exchange_BBox(pVisibility,"boundingbox",m_BoundingBox.m_vMin.data,m_BoundingBox.m_vMax.data,bWrite);
      XMLHelper::Exchange_Float(pVisibility,"dynamicInflateInterval",m_fDynamicInflateInterval,bWrite);
      XMLHelper::Exchange_Float(pVisibility,"depthofs",m_fDepthOffset,bWrite);
      XMLHelper::Exchange_Bool(pVisibility,"softparticles",m_bSoftParticles,bWrite);
      XMLHelper::Exchange_Bool(pVisibility,"alwaysinforeground",m_bAlwaysInForeground,bWrite);
      XMLHelper::Exchange_Bool(pVisibility,"handlewhenvisible",m_bHandleWhenVisible,bWrite);
      XMLHelper::Exchange_Int(pVisibility,"filtermask",m_iVisibleBitmask,bWrite);

      // fade particles
      const char *fadeMode[] = {"none","fog","custom"};
      const int fadeModeVal[3] = {VisParticleGroup_cl::FADEMODE_NONE, VisParticleGroup_cl::FADEMODE_FOGDISTANCE, VisParticleGroup_cl::FADEMODE_CUSTOM};
      XMLHelper::Exchange_Enum(pVisibility,"fadeMode",(int &)m_eFadeMode,3,fadeMode,fadeModeVal,bWrite);
      XMLHelper::Exchange_Float(pVisibility,"fadeStart",m_fFadeStart,bWrite);
      XMLHelper::Exchange_Float(pVisibility,"fadeEnd",m_fFadeEnd,bWrite);

      // render order
      const char *renderOrderStrings[] = {
        "pre_rendering",
        "pre_primary_opaque_pass_geometry",
        "pre_primary_opaque_pass_entities",
        "pre_secondary_opaque_pass_geometry",
        "pre_secondary_opqaue_pass_entities",
        "pre_occlusion_tests",
        "post_occlusion_tests",
        "pre_transparent_pass_geometry",
        "pre_transparent_pass_entities",
        "post_transparent_pass_geometry",
        "decals",
        "particles",
        "translucent_volumes",
        "additive_particles",
        "coronas_and_flares",
        "pre_screenmasks",
        "gui",
        "after_rendering",
        "custom",
        //backwards compatibility values
        "pre_dynamic_light",
        "pre_translucent_geometry",
        "pre_translucent_lit_geometry",
        "pre_specialfx_solid_and_translucent_lit_geometry",
        "after_sky",
        "pre_particles",
        "pre_specialfx_translucent_geometry",
        "pre_foreground_objects",
        "pre_solid_geometry",
        "pre_coronas_and_flares"
      };
      const int renderOrderValues[] = {
        VRH_PRE_RENDERING,
        VRH_PRE_PRIMARY_OPAQUE_PASS_GEOMETRY,
        VRH_PRE_PRIMARY_OPAQUE_PASS_ENTITIES,
        VRH_PRE_SECONDARY_OPAQUE_PASS_GEOMETRY,
        VRH_PRE_SECONDARY_OPAQUE_PASS_ENTITIES,
        VRH_PRE_OCCLUSION_TESTS,
        VRH_POST_OCCLUSION_TESTS,
        VRH_PRE_TRANSPARENT_PASS_GEOMETRY,
        VRH_PRE_TRANSPARENT_PASS_ENTITIES,
        VRH_POST_TRANSPARENT_PASS_GEOMETRY,
        VRH_DECALS,
        VRH_PARTICLES,
        VRH_TRANSLUCENT_VOLUMES,
        VRH_ADDITIVE_PARTICLES,
        VRH_CORONAS_AND_FLARES,
        VRH_PRE_SCREENMASKS,
        VRH_GUI,
        VRH_AFTER_RENDERING,
        VRH_CUSTOM,
        // backwards compatibility values
        VRH_POST_OCCLUSION_TESTS,
        VRH_POST_TRANSPARENT_PASS_GEOMETRY,
        VRH_PRE_OCCLUSION_TESTS,
        VRH_POST_OCCLUSION_TESTS,
        VRH_PRE_OCCLUSION_TESTS,
        VRH_DECALS,
        VRH_POST_TRANSPARENT_PASS_GEOMETRY,
        VRH_ADDITIVE_PARTICLES,
        VRH_PRE_PRIMARY_OPAQUE_PASS_GEOMETRY,
        VRH_CORONAS_AND_FLARES
      };
      V_COMPILE_ASSERT(V_ARRAY_SIZE(renderOrderStrings) == V_ARRAY_SIZE(renderOrderValues));
      int iNumCompatibilityValues = 10; //number of values for backwards compatibility, to be listed at the end of the array
      if(!bWrite) //if we are writing the compatibility values are ignored
        iNumCompatibilityValues = 0;
      XMLHelper::Exchange_Bitmask(pVisibility,"renderorder",m_iRenderOrder,V_ARRAY_SIZE(renderOrderStrings) - iNumCompatibilityValues,renderOrderStrings,renderOrderValues,bWrite);
    }
  }

  // particle properties
  TiXmlElement *pParticles = XMLHelper::SubNode(pRoot,"particles",bWrite);
  if (pParticles)
  {
    // particle sorting
    TiXmlElement *pSorting = XMLHelper::SubNode(pParticles,"particlesorting",bWrite);
    if (pSorting)
    {
      XMLHelper::Exchange_Bool(pSorting,"enabled",m_bSortParticles,bWrite);
    }

    // particle lifetime
    m_ParticleLifeTime.DataExchangeXML("lifetime",pParticles,bWrite);
    // particle color
    TiXmlElement *pColor = XMLHelper::SubNode(pParticles,"color",bWrite);
    if (pColor)
    {
      // color curve
      m_spColorCurve = VColorCurve::Exchange_ColorCurve(pColor,"colorcurve",m_spColorCurve,bWrite);

      // when loading, set the lookup bitmap from the color curve
      if (!bWrite && m_spColorCurve)
        m_spColorLookup = m_spColorCurve->GetLookupBitmap();
    }

    // particle distortion
    TiXmlElement *pDistortion = XMLHelper::SubNode(pParticles,"distortion",bWrite);
    if (pDistortion)
    {
      XMLHelper::Exchange_Bool(pDistortion,"enabled",m_bDistorted,bWrite);
      XMLHelper::Exchange_Bool(pDistortion,"planealigned",m_bDistortionPlaneAligned,bWrite);
      m_FixDistortionLength.DataExchangeXML("fixlength",pDistortion,bWrite);
      m_SpeedMultiplier.DataExchangeXML("speedmultiplier",pDistortion,bWrite);


      TiXmlElement *pSizeMode = XMLHelper::SubNode(pDistortion,"sizemode",bWrite);
      if (pSizeMode)
      {
        XMLHelper::Exchange_Bool(pSizeMode,"enabled",m_bDistortionSizeMode,bWrite);
        XMLHelper::Exchange_Floats(pSizeMode,"sizemultiplier",m_vSizeMultiplier.data,4,bWrite); // 4 components (w used for position re-center)
      }
    }
    // particle normal
    TiXmlElement *pNormal = XMLHelper::SubNode(pParticles,"normal",bWrite);
    if (pNormal)
    {
      XMLHelper::Exchange_Bool(pNormal,"enabled",m_bUseNormal,bWrite);

      hkvVec4 vTemp = m_vNormal.getAsVec4 (m_fRotationAxisRandomness);
      XMLHelper::Exchange_Floats(pNormal,"dir",vTemp.data, 4, bWrite); // m_vNormal.w is used for the randomness (e.g. geometry rotation axis)
      m_vNormal = vTemp.getAsVec3 ();
      m_fRotationAxisRandomness = vTemp.w;

      if (m_vNormal.getLength() == 0.0f)
        m_vNormal.set(0,0,1); // Fix for old XML files
    }
    // particle size
    TiXmlElement *pSize = XMLHelper::SubNode(pParticles,"size",bWrite);
    if (pSize)
    {
      m_spSizeCurve = VCurve2D::Exchange_Curve(pSize,"curve",m_spSizeCurve,bWrite);
      m_ParticleStartSize.DataExchangeXML("start",pSize,bWrite);
      m_ParticleSizeGrowth.DataExchangeXML("growth",pSize,bWrite);
      if (m_spSizeCurve)
      {
        // in curve mode, the m_ParticleStartSize defines the relative maximum (+per particle variation)
        if (m_ParticleStartSize.m_fAverage!=1.f)
          m_ParticleStartSize.Set(1.f,0.f); // to migrate old groups, reset the variation to 0.f
      }
    }
    // particle animation
    TiXmlElement *pAnimation = XMLHelper::SubNode(pParticles,"animation",bWrite);
    if (pAnimation)
    {
      V_COMPILE_ASSERT(sizeof(m_eParticleAnimMode)==sizeof(int));
      XMLHelper::Exchange_Int(pAnimation,"mode",(int &)m_eParticleAnimMode,bWrite);
      XMLHelper::Exchange_Bool(pAnimation,"startrandomframe",m_bStartRandomAnimFrame,bWrite);
      m_spAnimCurve = VCurve2D::Exchange_Curve(pAnimation,"curve",m_spAnimCurve,bWrite);
      m_ParticleAnimTime.DataExchangeXML("animtime",pAnimation,bWrite);
      XMLHelper::Exchange_Bool(pAnimation,"smooth",m_bUseSmoothAnimation,bWrite);
    }

    // particle speed
    m_ParticleSpeed.DataExchangeXML("speed",pParticles,bWrite);

    // particle rotation
    TiXmlElement *pRotation = XMLHelper::SubNode(pParticles,"rotation",bWrite);
    if (pRotation)
    {
      const char *rotMode[] = {"none","constspeed","curve"};
      const int rotModeVal[3] = {PARTICLE_ROTATION_NONE, PARTICLE_ROTATION_CONSTSPEED, PARTICLE_ROTATION_ANGLECURVE};
      XMLHelper::Exchange_Enum(pRotation,"mode",(int &)m_eRotationMode,3,rotMode,rotModeVal,bWrite);
      XMLHelper::Exchange_Floats(pRotation,"initialangle",m_fInitialAngle,2,bWrite);
      XMLHelper::Exchange_Floats(pRotation,"rotationspeed",m_fConstRotationSpeed,2,bWrite);
      XMLHelper::Exchange_Float(pRotation,"curveduration",m_fRotationCurveDuration,bWrite);
      XMLHelper::Exchange_Bool(pRotation,"randomcurveofs",m_bRandomRotationCurveOffset,bWrite);
      m_spRotationCurve = VCurve2D::Exchange_Curve(pRotation,"curve",m_spRotationCurve,bWrite);
    }

    // particle physics
    TiXmlElement *pPhysics = XMLHelper::SubNode(pParticles,"physics",bWrite);
    if (pPhysics)
    {
      if (XMLHelper::Exchange_Floats(pPhysics,"gravity",m_vGravity.data,3,bWrite)==1)
        m_vGravity.set(0.f,0.f,m_vGravity.x); // backwards compatibility for 1 component
      XMLHelper::Exchange_Float(pPhysics,"friction",m_fFriction,bWrite);
      XMLHelper::Exchange_Float(pPhysics,"emitterspeed",m_fApplyEmitterSpeed,bWrite);
      m_ParticleInertia.DataExchangeXML("inertia",pPhysics,bWrite);
      XMLHelper::Exchange_Int(pPhysics,"constraintbitmask",(int&)m_iConstraintAffectBitMask,bWrite);
      XMLHelper::Exchange_Bool(pPhysics,"gravityinertia",m_bInertiaAffectsGravity,bWrite);
    }
    // on destroy
    TiXmlElement *pOnDestroy = XMLHelper::SubNode(pParticles,"ondestroy",bWrite);
    if (pOnDestroy)
    {
      XMLHelper::Exchange_Bool(pOnDestroy,"repeat",m_bRepeatLifetime,bWrite);
      m_OnDestroyCreateCount.DataExchangeXML("newparticlecount",pOnDestroy,bWrite);
      TiXmlElement *pNewType = XMLHelper::SubNode(pOnDestroy,"newparticletype",bWrite);
      if (pNewType)
      {
        XMLHelper::Exchange_VString(pNewType,"groupname",m_sDestroyCreateGroup,bWrite);
        XMLHelper::Exchange_Int(pNewType,"copyflags",m_iOnDestroyCopyFlags,bWrite);
      }
    }
  }

  // events
  if (bWrite)
  {
    const int iEventCount = m_EventList.GetEventCount();
    for (int i=0;i<iEventCount;i++)
    {
      int iID = m_EventList.GetEventValueByIndex(i);
      if (!Vision::Animations.IsStringEvent(iID))
        continue;
      TiXmlElement *pEventNode = XMLHelper::SubNode(pRoot,"event",bWrite);
      float fTime = m_EventList.GetEventTimeByIndex(i);
      XMLHelper::Exchange_String(pEventNode,"id",Vision::Animations.GetEventString(iID),bWrite);
      XMLHelper::Exchange_Float(pEventNode,"time",fTime,bWrite);
    }
  }
  else
  {
    for (TiXmlElement *pEventNode=pRoot->FirstChildElement("event"); pEventNode!=NULL; pEventNode=pEventNode->NextSiblingElement("event"))
    {
      float fTime = -1.f;
      XMLHelper::Exchange_Float(pEventNode,"time",fTime,bWrite);
      if (fTime<0.f) continue;
      const char *szID = XMLHelper::Exchange_String(pEventNode,"id",NULL,bWrite);
      m_EventList.AddEvent(fTime, szID);
    }
    m_EventList.SetSequenceLength(1.f); // always relative
  }

  // optional custom data goes here
  if (!bWrite || m_spCustomCurve!=NULL || m_spCustomColorCurve!=NULL || m_iCustomIntValue!=0)
  {
    TiXmlElement *pOptional = XMLHelper::SubNode(pRoot,"optional",bWrite);
    if (pOptional)
    {
      m_spCustomCurve = VCurve2D::Exchange_Curve(pOptional,"customcurve",m_spCustomCurve,bWrite);
      m_spCustomColorCurve = VColorCurve::Exchange_ColorCurve(pOptional,"customcolorcurve",m_spCustomColorCurve,bWrite);
      XMLHelper::Exchange_Int(pOptional,"customInt",m_iCustomIntValue,bWrite);
    }
  }

  if (!bWrite)
    Finish();

  return true;
}


void VisParticleGroupDescriptor_cl::SetCurrentRelPath(const char *szFilename)
{
  VFileHelper::GetFileDir(szFilename,g_szCurrentRelPath);
}

const char* VisParticleGroupDescriptor_cl::GetCurrentRelPath()
{
  return g_szCurrentRelPath;
}

bool VisParticleGroupDescriptor_cl::CombineWithRelPath(char *szDest, const char *szFilename)
{
  VFileHelper::CombineDirAndFile(szDest,g_szCurrentRelPath,szFilename);
  return true;
}


bool VisParticleGroupDescriptor_cl::LoadFromXML(const char *szFilename)
{
  SetCurrentRelPath(szFilename);
  TiXmlDocument doc;
  if (!doc.LoadFile(szFilename,Vision::File.GetManager()))
    return false;

  // find the first descriptor node in this file
  VASSERT(doc.RootElement());
  TiXmlNode *pDescNode = doc.RootElement()->FirstChild(XML_GROUPDESCRIPTOR_NAME);
  if (!pDescNode) // try with old one
  {
    pDescNode = doc.RootElement()->FirstChild(XML_OLD_GROUPDESCRIPTOR_NAME);
    if (!pDescNode) 
      return false;
  }
  bool bResult = DataExchangeXML(pDescNode->ToElement(),false);
  return bResult;
}



bool VisParticleGroupDescriptor_cl::SaveToXML(const char *szFilename)
{
  SetCurrentRelPath(szFilename);

  TiXmlDocument doc(szFilename);
  TiXmlElement root("root");
  TiXmlNode *pRoot = doc.InsertEndChild(root);
  TiXmlElement descNode(XML_GROUPDESCRIPTOR_NAME);
  pRoot = pRoot->InsertEndChild(descNode);

  if (!DataExchangeXML(pRoot->ToElement(),true))
    return false;
  if (!doc.SaveFile(Vision::File.GetManager()))
    return false;

  return true;
}


///////////////////////////////////////////////////////////////////////////////

VisParticleGroupDescriptor_cl *VisParticleDescriptorList_cl::FindDescriptor(const char *szName) const
{
  const int iCount = Count();
  for (int i=0;i<iCount;i++)
  {
    const char *szDescName = GetAt(i)->GetName();
    if (!szName || (szDescName != NULL && !_stricmp(szDescName,szName)))
      return GetAt(i);
  }

  return NULL;
}

V_IMPLEMENT_SERIALX( VisParticleDescriptorList_cl);
void VisParticleDescriptorList_cl::SerializeX( VArchive &ar, VisParticleEffectFile_cl *pOwner )
{
  if (ar.IsLoading())
  {
    int iCount;
    ar >> iCount;
    EnsureCapacity(iCount);
    for (int i=0;i<iCount;i++)
    {
      VisParticleGroupDescriptor_cl *pDesc = new VisParticleGroupDescriptor_cl(pOwner);
      ar >> pDesc;
      Add(pDesc);
    }
  }
  else
  {
    int iCount = Count();
    ar << iCount;
    for (int i=0;i<iCount;i++)
      ar << GetAt(i);
      
  }
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
