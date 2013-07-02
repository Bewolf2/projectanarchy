/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptRenderer_wrapper.hpp

#ifndef __VSCRIPTRENDERER_WRAPPER_HPP
#define __VSCRIPTRENDERER_WRAPPER_HPP


#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptDraw_wrapper.hpp>

#include <Vision/Runtime/Engine/Renderer/VisApiRendererNode.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/PostProcessBase.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/Glow.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/ToneMapping.hpp>

#if !defined(_VISION_MOBILE) && !defined(HK_ANARCHY)
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingApp.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingLights.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingRimLight.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingDepthFog.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingCopy.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingAAEdgeMask.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingAAResolve.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/SkinShading/ScreenSpaceSubsurfaceScattering.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/FXAA.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/ToneMappingAdaptive.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VGlobalFogPostprocess.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessDepthOfField.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessSSAO.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessSSAO.hpp>
#endif


/// 
/// \brief
///   Small wrapper to unify the renderer inside the scripting binding
/// 
class VScriptRenderer_wrapper
{
public:
  friend class VisTypedEngineObject_cl;

  VScriptRenderer_wrapper() {}
  ~VScriptRenderer_wrapper() {}


  //VScriptPostprocessing_wrapper Postprocessing;
  VScriptDraw_wrapper Draw;

//everything below will not appear int the interface file:

  static VPostProcessingBaseComponent* AddPostProcessor(IVRendererNode * self, const char * className, const char * szOptionalComponentName)
  {
    VPostProcessingBaseComponent* pPostProcessor = NULL;

    if(!strcmp(className, "VPostProcessGlow"))
    {
      pPostProcessor = new VPostProcessGlow(GLOW_DOWNSCALE_2X); //fwd & def
    }
    else if(!strcmp(className, "VPostProcessToneMapping"))
    {
      pPostProcessor = new VPostProcessToneMapping(TONEMAP_SCALE, 1, 1, 0); //fwd & def
    }

    #if !defined(_VISION_MOBILE) && !defined(HK_ANARCHY)
      else if(!strcmp(className, "VPostProcessSSAO"))
      {
        VPostProcessSSAO *pSSAO = new VPostProcessSSAO(24, SSAO_8_SAMPLES, 2, true, SSAO_FILTER_ADAPTIVE); //fwd & def
        pSSAO->SetOcclusionParameters(3, 0.0001f);
        pPostProcessor = pSSAO;
      }
      #if !defined(_VISION_PSP2)
        else if(!strcmp(className, "VPostProcessFXAA"))
        {
          pPostProcessor = new VPostProcessFXAA(); //fwd & def
        }
      #endif
      else if(!strcmp(className, "VPostProcessDepthOfField"))
      {
        pPostProcessor = new VPostProcessDepthOfField(true); //fwd & def
      }
      else if(!strcmp(className, "VPostProcessToneMappingAdaptive"))
      {
        pPostProcessor = new VPostProcessToneMappingAdaptive(TONEMAP_SCALE, 1, 1, 0); //fwd & def
      }
      
      else if(!strcmp(className, "VScreenSpaceSubsurfaceScatteringPostProcessor"))
      {
        pPostProcessor = new VScreenSpaceSubsurfaceScatteringPostProcessor();
      }
      else if(self->IsOfType(V_RUNTIME_CLASS(VDeferredRenderingSystem)))
      {
        if(!strcmp(className, "VDeferredShadingRimLight"))
        {
          pPostProcessor = new VDeferredShadingRimLight(V_RGBA_WHITE, 8, 1); //def
        }
        else if(!strcmp(className, "VDeferredShadingLights"))
        {
          pPostProcessor = new VDeferredShadingLights(); //def
        }
        else if(!strcmp(className, "VGlobalFogPostprocess"))
        {
          pPostProcessor = new VGlobalFogPostprocess(); //def
        }
      }
    #endif

    if(pPostProcessor!=NULL)
    {
      if(szOptionalComponentName!=NULL)
        pPostProcessor->SetComponentName(szOptionalComponentName);

      VString sError;
      bool bPossible = pPostProcessor->CanAttachToObject(self, sError) == TRUE;
      if(!bPossible)
      {
        Vision::Error.Warning("%s", sError.AsChar());
        pPostProcessor->DisposeObject();
        V_SAFE_DELETE(pPostProcessor);
      }
      else
      {
        self->AddComponent(pPostProcessor);
      }

    }

    return pPostProcessor;
  }

  static bool SetTextureForSurface(VisBaseEntity_cl *pEntity, VTextureObject *pTexture, VisSurface_cl *pSurface, const char *szSurfaceName = NULL, int iSurfaceIndex = -1, int iType = 0)
  {
    VASSERT_MSG(pEntity&&pTexture, "NULL pointer passed to SetTextureForSurface");
    VASSERT_MSG(pSurface!=NULL||szSurfaceName!=NULL||iSurfaceIndex!=-1, "Invalid surface specification for SetTextureForSurface");

    //set the texture if the surface is present
    if(pSurface!=NULL)
    {
      pSurface->SetTexture((VisSurfaceTextures_cl::VTextureType_e)iType, pTexture);
      return true;
    }

    //otherwise get the surface via the mesh
    VDynamicMesh *pMesh = pEntity->GetMesh();
    if(pMesh==NULL) return false;

    // if the entity has a custom texture set, set the texture there instead
    VisSurfaceTextureSet_cl *pSet = pEntity->GetCustomTextureSet();
    if (pSet!=NULL)
    {
      int iIndex = iSurfaceIndex;
      if (szSurfaceName!=NULL)
      {
        iIndex = pMesh->GetSurfaceIndexByName(szSurfaceName);
      }
      if (iIndex<0)
        return false;

      pSet->GetTextures(iIndex)->SetTexture((VisSurfaceTextures_cl::VTextureType_e)iType, pTexture);
      return true;
    }

    //get the surface
    if(szSurfaceName!=NULL)
      pSurface = pMesh->GetSurfaceByName(szSurfaceName);
    else
      pSurface = pMesh->GetSurface(iSurfaceIndex);

    //and set the texture if possible
    if(pSurface!=NULL)
    {
      pSurface->SetTexture((VisSurfaceTextures_cl::VTextureType_e)iType, pTexture);
      return true;
    }
    
    return false;
  } 

  static bool SetTechniqueForSurface(VisBaseEntity_cl *pEntity, VDynamicMesh *pMesh, VisSurface_cl *pSurface, const char * szShaderLib, const char * szTechnique, const char * szParamString = "", bool bReplace = true)
  { 
    VASSERT(pEntity||pMesh);

    if(!pMesh) pMesh = pEntity->GetMesh();
    if(!pMesh) return false;

    if(!pSurface || (szShaderLib && !Vision::Shaders.LoadShaderLibrary(szShaderLib))) return false;

    //create a copy of the shader set
    VCompiledTechnique *  pTechnique = Vision::Shaders.CreateTechnique(szTechnique, szParamString);
    if(!pTechnique) return false;

    VisShaderSet_cl *pShaderSet = NULL;
    if(pEntity)
    {
      pShaderSet = pEntity->GetActiveShaderSet()->Clone();
      pEntity->SetShaderSet(pShaderSet);
    }
    else 
    {
      pShaderSet = pMesh->GetShaderSet();
    }
      
    if(pShaderSet==NULL) return false;

    //remove all previous made assignments if desired
    if(bReplace) pShaderSet->RemoveSurfaceAssignments(pSurface);

    for(int i=0; i<pMesh->GetSubmeshCount();++i)
      pShaderSet->Add(pMesh->GetSubmesh(i), pSurface, pTechnique);

    return true;
  }

  static bool SetEffectForSurface(VisBaseEntity_cl *pEntity, VDynamicMesh *pMesh, VisSurface_cl *pSurface, const char * szShaderLib, const char * szEffect, const char * szParamString = "", bool bReplace = true)
  {
    VASSERT(pEntity||pMesh);

    if(!pMesh) pMesh = pEntity->GetMesh();
    if(!pMesh) return false;

    if(!pSurface || (szShaderLib && !Vision::Shaders.LoadShaderLibrary(szShaderLib))) return false;

    //create a copy of the shader set
    VCompiledEffect *pEffect = Vision::Shaders.CreateEffect(szEffect, szParamString);
    if(!pEffect) return false;
    VCompiledTechnique *  pTechnique = pEffect->GetDefaultTechnique();
    if(!pTechnique) return false;

    VisShaderSet_cl *pShaderSet = NULL;
    if(pEntity)
    {
      pShaderSet = pEntity->GetActiveShaderSet()->Clone();
      pEntity->SetShaderSet(pShaderSet);
    }
    else 
    {
      pShaderSet = pMesh->GetShaderSet();
    }
      
    if(pShaderSet==NULL) return false;

    //remove all previous made assignments if desired
    if(bReplace) pShaderSet->RemoveSurfaceAssignments(pSurface);

    for(int i=0; i<pMesh->GetSubmeshCount();++i)
      pShaderSet->Add(pMesh->GetSubmesh(i), pSurface, pTechnique);

    return true;
  }
};

#endif // __VSCRIPTRENDERER_WRAPPER_HPP

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
