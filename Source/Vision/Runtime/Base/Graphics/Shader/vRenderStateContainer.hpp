/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vRenderStateContainer.hpp

#ifndef VRENDERSTATECONTAINER_HPP_INCLUDED
#define VRENDERSTATECONTAINER_HPP_INCLUDED

#include <Vision/Runtime/Base/Graphics/Textures/VTextureObject.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludes.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vConstantBuffer.hpp>
#include <Vision/Runtime/Base/Graphics/Video/VVideo.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vStateGroupSampler.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vStateGroups.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderDeclarations.hpp>


#ifdef WIN32
bool ParseFXLib (char *text, int numChars, VShaderCompileFlags_e flags, VShaderEffectLib &fxlib);
#endif


/// \brief Holds all shareable render-state. Shaders reference instances of this class to not store state redundantly.
class VRenderStateContainer : public VRefCounter
{
public:
  /// \brief Constructor.
  VRenderStateContainer (bool bIsShaderUnique);

  /// \brief Reads/Writes data from/to the binary file.
  VBASE_IMPEXP void ChunkFileExchange (VChunkFile &file, VTargetPlatform_e targetPlatform, char iLocalVersion);
  /// \brief Writes the render-state data to a text-file.
  VBASE_IMPEXP void WriteToConfigFile (IVFileOutStream *pOut, int iNumRSCinFile) const;

  /// \brief Compares two render-states for equality.
  bool operator== (const VRenderStateContainer& rhs) const;
  /// \brief Compares two render-states for inequality.
  bool operator!= (const VRenderStateContainer& rhs) const;
  /// \brief assignment operator
  VRenderStateContainer& operator= (const VRenderStateContainer& rhs);

public:
  /// \brief Returns whether blending from surface is enabled.
  bool IsBlendFromSurfaceEnabled (void) const { return (m_Data.m_bBlendingFromSurface); }
  /// \brief Sets the blending-from-surface state.
  void SetBlendFromSurface (bool bEnabled) { m_Data.m_bBlendingFromSurface = bEnabled; }

  /// \brief Returns whether the rasterizer style should be taken from the surface.
  bool IsRasterizerStyleFromSurfaceEnabled (void) const { return (m_Data.m_bRasterizerStyleFromSurface); }
  /// \brief Sets whether to take the rasterizer style from the surface.
  void SetRasterizerStyleFromSurface (bool bEnabled) { m_Data.m_bRasterizerStyleFromSurface = bEnabled; }

  /// \brief Returns whether this is a detail-pass.
  bool IsDetailPass (void) const { return (m_Data.m_bIsDetailPass); }
  /// \brief Sets whether this is a detail-pass.
  void SetIsDetailPass (bool b) { m_Data.m_bIsDetailPass = b; }

  /// \brief Returns whether geometry should be far-clipped.
  bool IsFarClipped (void) const { return (m_Data.m_bIsFarClipped); }
  /// \brief Sets whether geometry should be far-clipped.
  void SetIsFarClipped (bool b) { m_Data.m_bIsFarClipped = b; }

  /// \brief Returns whether the geometry-shader (if any) uses stream-out.
  bool GetGeometryShaderUsesStreamOut (void) const { return (m_Data.m_bGeometryShaderUsesStreamOut); }
  /// \brief Sets whether the geometry-shader uses stream-out.
  void SetGeometryShaderUsesStreamOut (bool b) { m_Data.m_bGeometryShaderUsesStreamOut = b; }

  /// \brief Returns whether this state overwrites global constants.
  bool GetOverwritesGlobalConstants (void) const { return (m_Data.m_bOverwritesGlobalConstants); }
  /// \brief Sets whether this state overwrites global constants.
  void SetOverwritesGlobalConstants (bool b) { m_Data.m_bOverwritesGlobalConstants = b; }

  /// \brief [internal] DO NOT USE!
  unsigned int GetStreamMask_BackwardsCompatibility (void) const { return (m_Data.m_uiStreamMask); }
  /// \brief Sets the vertex-stream mask.
  //void SetStreamMask (unsigned int ui) { m_Data.m_uiStreamMask = ui; }

  /// \brief Returns the tracking-mask.
  unsigned int GetTrackingMask (void) const { return (m_Data.m_uiTrackingMask); }
  /// \brief Sets the tracking-mask.
  void SetTrackingMask (unsigned int ui) { m_Data.m_uiTrackingMask = ui; }

  /// \brief Returns additional render-flags.
  unsigned int GetRenderFlags (void) const { return (m_Data.m_uiRenderFlags); }
  /// \brief Sets the render-flags.
  void SetRenderFlags (unsigned int ui) { m_Data.m_uiRenderFlags = ui; }

  /// \brief Returns the distance at which an object switches its detail
  float GetDetailDistance (void) const { return (m_Data.m_fDetailDistance); }
  /// \brief Sets the distance at which an object switches its detail
  void  SetDetailDistance (float f) { m_Data.m_fDetailDistance = f; }

  /// \brief Returns the name of the compiled class. If this returns NULL, the shader system will assume the default shader class
  const char* GetCompiledClassName (void) const { return m_sCompiledClassName.IsEmpty() ? NULL : m_sCompiledClassName.AsChar(); }

  /// \brief Sets the name of the compiled class. If default class name "VCompiledShaderPass" is passed, it will fall back to empty string, according to specification.
  void SetCompiledClassName (const char* sz) 
  { 
    if (sz!=NULL && strcmp(sz,"VCompiledShaderPass")==0)
      sz = NULL;
    m_sCompiledClassName = sz; 
  }

  /// \brief Indicates whether this is a container that has been created uniquely for a shader or whether it is shared in the library
  bool IsShaderUnique() const {return m_bIsShaderUnique;}

  /// \brief Allows read/write access to the blend state object.
  VStateGroupBlend& GetBlendState (void) { return (m_Data.m_sgBlend); }

  /// \brief Allows read/write access to the depth-stencil state object.
  VStateGroupDepthStencil& GetDepthStencilState (void) { return (m_Data.m_sgDepthStencil); }

  /// \brief Allos read/write access to the rasterizer state object.
  VStateGroupRasterizer& GetRasterizerState (void) { return (m_Data.m_sgRasterizer); }

  /// \brief Allows read/write access to the tessellation state object.
  VStateGroupTesselation& GetTesselationState (void) { return (m_Data.m_sgTesselation); }

public:
  /// \brief Recomputes all hashes for the internal state objects.
  void ComputeStateHashes (void);

private:
  #ifdef WIN32
  friend bool ParseFXLib (char *text, int numChars, VShaderCompileFlags_e flags, VShaderEffectLib &fxlib);
  #endif
  friend class VShaderPassResource;

  // all the data is in its own struct, such that comparisons can be done with a memcmp
  // and need not be done member-wise
  struct DataBlock
  {
    bool m_bBlendingFromSurface;
    bool m_bRasterizerStyleFromSurface;
    bool m_bIsDetailPass;
    bool m_bIsFarClipped;
    bool m_bGeometryShaderUsesStreamOut;
    bool m_bOverwritesGlobalConstants;

    unsigned int m_uiStreamMask;
    unsigned int m_uiTrackingMask;
    unsigned int m_uiRenderFlags;

    float m_fDetailDistance;

    VStateGroupBlend m_sgBlend;
    VStateGroupDepthStencil m_sgDepthStencil;
    VStateGroupRasterizer   m_sgRasterizer;
    VStateGroupTesselation  m_sgTesselation;    ///< Xbox360 only

    bool operator ==(const DataBlock &other) const
    {
      if ((m_bBlendingFromSurface          != other.m_bBlendingFromSurface)
       || (m_bRasterizerStyleFromSurface   != other.m_bRasterizerStyleFromSurface)
       || (m_bIsDetailPass                 != other.m_bIsDetailPass)
       || (m_bIsFarClipped                 != other.m_bIsFarClipped)
       || (m_bGeometryShaderUsesStreamOut  != other.m_bGeometryShaderUsesStreamOut)
       || (m_bOverwritesGlobalConstants    != other.m_bOverwritesGlobalConstants)
       || (m_uiStreamMask                  != other.m_uiStreamMask)
       || (m_uiTrackingMask                != other.m_uiTrackingMask)
       || (m_uiRenderFlags                 != other.m_uiRenderFlags)
       || (m_fDetailDistance               != other.m_fDetailDistance))
      {
        return false;
      }

      if ((m_sgBlend != other.m_sgBlend)
       || (m_sgDepthStencil != other.m_sgDepthStencil)
       || (m_sgRasterizer != other.m_sgRasterizer)
       || (m_sgTesselation != other.m_sgTesselation))
      {
        return false;
      }
      
      return true;
    }
  };

  bool m_bIsShaderUnique; ///< indicates whether this instance is unique for a shader
  VString m_sCompiledClassName;

  DataBlock m_Data;
};

typedef VSmartPtr<VRenderStateContainer> VRenderStateContainerPtr;



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
