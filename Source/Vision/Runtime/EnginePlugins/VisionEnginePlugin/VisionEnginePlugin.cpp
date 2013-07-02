/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

////////////////////////////////////////////////////////////////////////////
//
// This plugin compiles some of the shared Vision source into
// a standard plugin. The sub-components that this plugin uses
// (particles, gui, cloth) could also be compiled into their
// own separate plugins or directly into your application.
//
// Most samples just use this DLL as a dynamic plugin.
// Some samples also link statically against the DLL to have
// direct access to its classes and functions.
//
////////////////////////////////////////////////////////////////////////////

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Mirror.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VWallmarkManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Projector.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VStringInputMap.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/_AnimEntity.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/_DanglingEntity.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/PathCameraEntity.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/TriggerBoxEntity.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/TriggerDoorEntity.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/VCustomVolumeManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Profiling/VGraphManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Cloth/_ClothEntity.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VPlayableCharacterComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VTimeOfDayComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VAnimationEventEffectTrigger.hpp>

#if defined (SUPPORTS_SHADOW_MAPS) && !defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapComponentSpotDirectional.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapComponentSunlight.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapComponentPoint.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VFogObject.hpp>
#else
  // No shadow mapping support on this platform
#endif

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VPlayerCamera.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/BillboardGroup.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/BlobShadowManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/CubeMapHandle.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VCoronaManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VLensFlareManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/Glow.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Sky/Sky.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VPrefab.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VSkeletalBoneProxy.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VFollowPathComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VEntityLODComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VLightClippingVolumeComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/ScratchTexturePool.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/TimeOfDay.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VSceneExporter.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationGroup.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/SectorVisibilityZone.hpp>

#if !defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ForwardRenderer/ForwardRenderer.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingApp.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/AdditionalForwardPass.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VCloudLayer.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VolumetricCone.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VolumetricShadowCone.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VSunGlare.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Water.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/SkinShading/SkinTransmittance.hpp>

  #if !defined(_VISION_MOBILE)
    #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VProjectedDecal.hpp>
    #if !defined(_VISION_PSP2)
      #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/FXAA.hpp>
      #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/ToneMappingAdaptive.hpp>
    #endif
    #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VGlobalFogPostprocess.hpp>
    #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Sky/AtmosphericScattering.hpp>
  #endif

#endif

#if defined(WIN32) || defined(_VISION_MOBILE) || defined(_VISION_PSP2)
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/MobileForwardRenderer/VMobileForwardRenderer.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VMobileShadowMapComponentSpotDirectional.hpp>
#endif

class VisionEnginePlugin_cl : public IVisPlugin_cl
{
public:

  void OnInitEnginePlugin();
  void OnDeInitEnginePlugin();
  
  const char *GetPluginName()
  {
    return "VisionEnginePlugin";  //must match DLL name
  }
};

VisionEnginePlugin_cl g_VisionEnginePlugin;
extern VModule g_VisionEngineModule;


DECLARE_THIS_MODULE( g_VisionEngineModule, MAKE_VERSION(1, 0),
                     "VisionEngineModule", "Havok", "Engine Effect additions", &g_VisionEnginePlugin);



////////////////////////////////////////////////////////////////////////////
//  -Use this to get and initialize the plugin when you link statically
//   e.g. GetEnginePlugin_VisionEnginePlugin()->InitEnginePlugin();
//  -See VISION_PLUGIN_ENSURE_LOADED for a macro that does 
//   dynamic and static initialization.
////////////////////////////////////////////////////////////////////////////

VEXPORT IVisPlugin_cl* GetEnginePlugin_VisionEnginePlugin()
{
  return &g_VisionEnginePlugin;
}


#if ((defined _DLL) || (defined _WINDLL)) && !defined(VISIONDLL_LIB)

/////////////////////////////////////////////////////////////////////////////
//  The engine uses this to get and initialize the plugin dynamically
/////////////////////////////////////////////////////////////////////////////

VEXPORT IVisPlugin_cl* GetEnginePlugin()
{
  return GetEnginePlugin_VisionEnginePlugin();
}

#endif // _DLL or _WINDLL


/////////////////////////////////////////////////////////////////////////////

#if !defined(_VISION_MOBILE) && !defined( HK_ANARCHY )
// A serialization binder to convert old VDeferredShadingDepthFog instances to new VGlobalFogPostprocess instances
class DepthFogBinder : public IVSerializationBinder
{
  IVSerializationBinder* m_pOldBinder;

  virtual VType *BindToType(const char *szClassName) HKV_OVERRIDE
  {
    if (!strcmp(szClassName,"VDeferredShadingDepthFog"))
    {
      return V_RUNTIME_CLASS(VGlobalFogPostprocess);
    }
    if (m_pOldBinder != NULL)
    {
      return m_pOldBinder->BindToType(szClassName);
    }
    return NULL;
  }

public:
  inline void SetOldBinder(IVSerializationBinder* pOldBinder)
  {
    m_pOldBinder = pOldBinder;
  }
};

DepthFogBinder g_DepthFogSerializationBinder;
#endif


//Initialize our plugin
void VisionEnginePlugin_cl::OnInitEnginePlugin()
{
  #if !defined(_VISION_MOBILE) && !defined( HK_ANARCHY )
  // Register the serialization binder
  g_DepthFogSerializationBinder.SetOldBinder(Vision::GetTypeManager()->GetSerializationBinder());
  Vision::GetTypeManager()->SetSerializationBinder(&g_DepthFogSerializationBinder);
  #endif

  // register our classes

  // Particles / wallmarks
  VisParticleGroupManager_cl::GlobalManager().OneTimeInit();
  VWallmarkManager::GlobalManager().OneTimeInit();

  // Register the module - there is only one module for all effects
  Vision::RegisterModule(&g_VisionEngineModule);

  // Disable deprecation warning
#pragma warning(push)
#pragma warning(disable: 4996)
  FORCE_LINKDYNCLASS(AnimEntity_cl);
#pragma warning(pop)

  FORCE_LINKDYNCLASS(ClothEntity_cl);
  FORCE_LINKDYNCLASS(DanglingEntity_cl);
  FORCE_LINKDYNCLASS(PathCameraEntity);
  FORCE_LINKDYNCLASS(StaticCollisionEntity_cl);
  FORCE_LINKDYNCLASS(TriggerBoxEntity_cl);
  FORCE_LINKDYNCLASS(TriggerDoorEntity_cl);
  FORCE_LINKDYNCLASS(VBillboardGroupInstance);
  FORCE_LINKDYNCLASS(VBlobShadowShader);
  FORCE_LINKDYNCLASS(VProjectedWallmark);
  FORCE_LINKDYNCLASS(VPlayerCamera);
  FORCE_LINKDYNCLASS(VPlayableCharacterComponent);
  FORCE_LINKDYNCLASS(VTransitionStateMachine);
  FORCE_LINKDYNCLASS(VTimeOfDayComponent);
  FORCE_LINKDYNCLASS(VLensFlareComponent);
  FORCE_LINKDYNCLASS(VCoronaComponent);
  FORCE_LINKDYNCLASS(VAnimationEventEffectTrigger);
  FORCE_LINKDYNCLASS(CubeMapHandle_cl);
#if !defined( HK_ANARCHY )
  FORCE_LINKDYNCLASS(VForwardRenderingSystem);
#endif
  FORCE_LINKDYNCLASS(VPostProcessGlow);

#if (defined (WIN32) && !defined(_VISION_MOBILE) ) || defined (_VISION_XENON) || defined (_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WIIU)

#if !defined( HK_ANARCHY )
  // VolumetricShadowCone
  FORCE_LINKDYNCLASS_ARG(VolumetricShadowCone_cl,0);

  // Sun Glare
  FORCE_LINKDYNCLASS(VSunGlare);

  FORCE_LINKDYNCLASS(VDeferredRenderingSystem);
  FORCE_LINKDYNCLASS(VShadowMapComponentSpotDirectional);
  FORCE_LINKDYNCLASS(VShadowMapComponentSunlight);
  FORCE_LINKDYNCLASS(VShadowMapComponentPoint);
#if !defined( _VISION_PSP2 )
  FORCE_LINKDYNCLASS(VPostProcessToneMappingAdaptive);
#endif

  FORCE_LINKDYNCLASS(VAtmosphere);

  //Skin Shading
  FORCE_LINKDYNCLASS(VAdditionalForwardPassShader);
  FORCE_LINKDYNCLASS(VScreenSpaceSubsurfaceScatteringForwardShader);
  FORCE_LINKDYNCLASS(VScreenSpaceSubsurfaceScatteringDeferredShader);
  FORCE_LINKDYNCLASS(VSkinTransmittanceShader);

  // Volumetric cone
  VolumetricConeManager_cl::GlobalManager().OneTimeInit();

  // Glares
  VSunGlareManager::GlobalManager().OneTimeInit();

  // Fog
  VFogObjectManager::GlobalManager().OneTimeInit();

  // Planar water
  FORCE_LINKDYNCLASS(PlanarWater_cl);
#endif

  FORCE_LINKDYNCLASS(VTimeOfDay);
  FORCE_LINKDYNCLASS(VLightClippingVolumeComponent);
  FORCE_LINKDYNCLASS(VTerrainDecorationGroup);

  //init named trigger system
  VStringInputMap::OneTimeInit();

  // Scratch texture pool
  ScratchTexturePool_cl::GlobalManager().OneTimeInit();

  // Mirror
  VisMirrorManager_cl::GlobalManager().OneTimeInit();

  //Postprocessing
  //VPostProcessingManager::GlobalManager().OneTimeInit();

  // Terrain
  VTerrainManager::GlobalManager().InitOneTime();

#if !defined( HK_ANARCHY )
  // Cloud Layers
  VCloudLayerManager::GlobalManager().OneTimeInit();
#endif

  // vForge scene exporter
#ifdef WIN32
  if (Vision::Editor.IsInEditor())
    Vision::Editor.SetExporterFactory(VSceneExporterFactory::GetFactory());
#endif
  if (Vision::Renderer.GetTimeOfDayFactory()==NULL)
    Vision::Renderer.SetTimeOfDayFactory(&VTimeOfDayFactory::g_Instance);

#elif defined(_VR_GLES2) || defined(_VISION_WINRT)

#if !defined( _VR_GLES2 )
  FORCE_LINKDYNCLASS(PlanarWater_cl);
#endif

  // Mirror
  VisMirrorManager_cl::GlobalManager().OneTimeInit();

#else
#error Undefined platform!
#endif

#if !defined(_VISION_MOBILE) && !defined(_VISION_PSP2) && !defined( HK_ANARCHY )
  FORCE_LINKDYNCLASS(VPostProcessFXAA);
#endif

#if defined(WIN32) || defined(_VISION_MOBILE) || defined(_VISION_PSP2)
  FORCE_LINKDYNCLASS(VMobileForwardRenderingSystem);
  FORCE_LINKDYNCLASS(VMobileShadowMapComponentSpotDirectional);
#endif

#if defined(_VISION_MOBILE)
  FORCE_LINKDYNCLASS(VSectorVisibilityZone);
#endif

  // GUI related
  VGUIManager::GlobalManager().OneTimeInit();

  // Scripting
  VScriptResourceManager::GlobalManager().OneTimeInit();

  // blob shadows
  VBlobShadowManager::GlobalManager().OneTimeInit();
  // transition
  VTransitionManager::GlobalManager().OneTimeInit();
  VPlayableCharacterComponentManager::GlobalManager().OneTimeInit();
  //bone proxy:
  VSkeletalBoneProxyManager::GlobalManager().OneTimeInit();
  // prefabs
  VPrefabManager::GlobalManager().OneTimeInit();
  // Follow Path component
  VFollowPathComponentManager::GlobalManager().OneTimeInit();
  // LOD component
  VEntityLODComponentManager::GlobalManager().OneTimeInit();
  // Coronas
  VCoronaManager::GlobalManager().OneTimeInit();
  // Lens Flares
  VLensFlareManager::GlobalManager().OneTimeInit();

  //Animation Components
  VSimpleAnimationComponentManager::GlobalManager().OneTimeInit();

  // Sky
  VSkyManager::GlobalManager().OneTimeInit();

  // Custom Volume Objects
  VCustomVolumeManager::GlobalManager().OneTimeInit();

  // Graph Objects
  VGraphManager::GlobalManager().OneTimeInit();

#if ( defined(WIN32) && !defined( HK_ANARCHY ) ) || defined(_VISION_XENON) || defined (_VISION_PS3)
  // Projected decals
  VProjectedDecalManager::GlobalManager().OneTimeInit();
#endif
}

//Deinitialize our plugin
void VisionEnginePlugin_cl::OnDeInitEnginePlugin()
{
  // Unregister the serialization binder
  Vision::GetTypeManager()->SetSerializationBinder(NULL);

  //Particles
  VisParticleGroupManager_cl::GlobalManager().OneTimeDeInit();
  VWallmarkManager::GlobalManager().OneTimeDeInit();

#if ( defined (WIN32) && !defined(_VISION_MOBILE)) || defined (_VISION_XENON) || defined (_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WIIU)

#if !defined( HK_ANARCHY )
  //Volumetric cone
  VolumetricConeManager_cl::GlobalManager().OneTimeDeInit();

  // Glares
  VSunGlareManager::GlobalManager().OneTimeDeInit();

  // Fog objects
  VFogObjectManager::GlobalManager().OneTimeDeInit();
#endif

  //Mirror
  VisMirrorManager_cl::GlobalManager().OneTimeDeInit();

  //Postprocessing
  //VPostProcessingManager::GlobalManager().OneTimeDeInit();

  // Terrain
  VTerrainManager::GlobalManager().DeInitOneTime();

#if !defined( HK_ANARCHY )
  // Cloud Layer
  VCloudLayerManager::GlobalManager().OneTimeDeInit();
#endif

  // vForge scene exporter
#ifdef WIN32
  if (Vision::Editor.IsInEditor() && Vision::Editor.GetExporterFactory()==VSceneExporterFactory::GetFactory())
    Vision::Editor.SetExporterFactory(NULL);
#endif
  if (Vision::Renderer.GetTimeOfDayFactory()==&VTimeOfDayFactory::g_Instance)
    Vision::Renderer.SetTimeOfDayFactory(NULL);

#elif defined(_VR_GLES2) || defined(_VISION_WINRT)

  //Mirror
  VisMirrorManager_cl::GlobalManager().OneTimeDeInit();

#else
#error Undefined platform!
#endif

  //unregister the modules
  Vision::UnregisterModule(&g_VisionEngineModule);

  //GUI
  VGUIManager::GlobalManager().OneTimeDeInit();

  // Scripting
  VScriptResourceManager::GlobalManager().OneTimeDeInit();

  // blob shadows
  VBlobShadowManager::GlobalManager().OneTimeDeInit();

  // transition
  VTransitionManager::GlobalManager().OneTimeDeInit();
  VPlayableCharacterComponentManager::GlobalManager().OneTimeDeInit();

  //bone proxy:
  VSkeletalBoneProxyManager::GlobalManager().OneTimeDeInit();

  // prefabs
  VPrefabManager::GlobalManager().OneTimeDeInit();

  // Follow Path component
  VFollowPathComponentManager::GlobalManager().OneTimeDeInit();

  // LOD component
  VEntityLODComponentManager::GlobalManager().OneTimeDeInit();

  // Coronas
  VCoronaManager::GlobalManager().OneTimeDeInit();

  // Lens Flares
  VLensFlareManager::GlobalManager().OneTimeDeInit();

  //Animation Components
  VSimpleAnimationComponentManager::GlobalManager().OneTimeDeInit();

#if !defined(_VR_GLES2)

  // Scratch texture pool
  ScratchTexturePool_cl::GlobalManager().OneTimeDeInit();

#endif

  // Sky
  VSkyManager::GlobalManager().OneTimeDeinit();

  // Custom Volume Objects
  VCustomVolumeManager::GlobalManager().OneTimeDeInit();

  //deregister named input maps
  VStringInputMap::OneTimeDeInit();

  // Graph Objects
  VGraphManager::GlobalManager().OneTimeDeInit();

#if ( defined(WIN32) && !defined( HK_ANARCHY ) ) || defined(_VISION_XENON) || defined (_VISION_PS3)
  // Projected decals
  VProjectedDecalManager::GlobalManager().OneTimeDeInit();
#endif
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
