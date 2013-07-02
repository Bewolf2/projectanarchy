/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VMobileForwardRenderer.hpp

#ifndef VMOBILE_FORWARDRENDERER_HPP_INCLUDED
#define VMOBILE_FORWARDRENDERER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Sky/Sky.hpp>

class VMobileForwardRenderingSystem;
typedef VSmartPtr<VMobileForwardRenderingSystem> VMobileForwardRenderingSystemPtr;

// Versions
#define VMOBILE_FORWARDRENDERER_VERSION_0        0                                  // Initial version
#define VMOBILE_FORWARDRENDERER_VERSION_1        1                                  // Migrated to common base class VRendererNodeCommon
#define VMOBILE_FORWARDRENDERER_VERSION_2        2                                  // Resolution Mode support
#define VMOBILE_FORWARDRENDERER_VERSION_3        3                                  // Resolution Tolerance
#define VMOBILE_FORWARDRENDERER_VERSION_CURRENT  VMOBILE_FORWARDRENDERER_VERSION_3  // Current version


/// \brief
///   Used to specify the rendering resolution mode for the VMobileForwardRenderingSystem.
///
/// For very high-resolution devices, it may be desirable to use a rendering resolution that is lower than the
/// screen resolution. Using this enumeration, you can specify how the target resolution should be chosen.
enum VRendererResolutionMode_e
{
  VRSM_FULL_RESOLUTION = 0,        ///< Always use the resolution of the final render target (typically the backbuffer). 
  VRSM_QUARTER_RESOLUTION = 1,     ///< Always render the scene at a quarter of the resolution of the final render target (e.g. 640x480 if the final render target is 1280x960).
  VRSM_USE_TARGET_DPI = 2          ///< Use a user-specified target resolution in dpi (dots per inch).
};



/// \brief
///   Forward rendering system for mobile devices (Android, iOS).
///
/// This class is a variant of the standard forward renderer optimized for mobile devices.
/// 
/// The central feature is a collapsed lighting pass. The dominant dynamic light source will be collapsed into the base pass,
/// while all remaining light sources are rendered additively.
///
/// In addition, it supports a simplified shadow mapping system and several basic post processors.
///
/// Extended features like HDR or MSAA are not supported.
class VMobileForwardRenderingSystem : public VRendererNodeCommon
{
public:

  /// \brief
  ///   Callback object for the Mobile Forward Renderer's OnSetResolution callback.
  ///
  /// This callback object can be used to override the resolution mode configured in the VMobileForwardRenderingSystem. It allows you to
  /// return an arbitrary custom resolution in the m_iTargetSize members. The VMobileForwardRenderingSystem will then use this rendering resolution
  /// rather than the one configured in the VMobileForwardRenderingSystem. Note that passing invalid/unsupported resolutions in m_iTargetSize may
  /// lead to crashes.
  class VSetResolutionCallbackObject : public IVisCallbackDataObject_cl
  {
  public:

    /// \brief
    ///   Constructor. iTargetSize receives the size as computed from the renderer node's settings; pRendererNode points to the renderer node itself.
    inline VSetResolutionCallbackObject(VCallback *pSender, const int *iTargetSize, VRendererNodeCommon *pRendererNode) : IVisCallbackDataObject_cl(pSender)
    {
      m_pRendererNode = pRendererNode;
      m_iTargetSize[0] = iTargetSize[0];
      m_iTargetSize[1] = iTargetSize[1];
    }

    VRendererNodeCommon *m_pRendererNode;
    int m_iTargetSize[2];
  };


  /// \brief
  ///   Constructor of the mobile forward rendering system.
  ///
  /// Initializes the mobile forward rendering system.
  /// 
  /// \param pTargetContext
  ///   The target context for the forward renderer node. 
  EFFECTS_IMPEXP VMobileForwardRenderingSystem(VisRenderContext_cl *pTargetContext);

  /// \brief
  ///   Virtual destructor of the mobile forward rendering system.
  EFFECTS_IMPEXP virtual ~VMobileForwardRenderingSystem();

  /// \brief
  ///   Resizes the Forward Rendering contexts.
  ///
  /// Use this function to resize the render targets for the forward renderer. Note that this function needs to
  /// reinitialize all associated render contexts and can therefore take a significant amount of time. Furthermore,
  /// calling this method frequently can lead to memory fragmentation.
  ///
  /// \param iNewWidth
  ///   New width of the render targets.
  ///
  /// \param iNewHeight
  ///   New height of the render targets.
  EFFECTS_IMPEXP void Resize(int iNewWidth, int iNewHeight);

  /// \copydoc IVRendererNode::GetReferenceContext
  EFFECTS_IMPEXP virtual VisRenderContext_cl *GetReferenceContext() HKV_OVERRIDE;

  /// \copydoc IVRendererNode::GetTranslucencyReferenceContext
  EFFECTS_IMPEXP virtual VisRenderContext_cl *GetTranslucencyReferenceContext() HKV_OVERRIDE;

  /// \copydoc IVRendererNode::GetFinalTargetContext
  EFFECTS_IMPEXP virtual VisRenderContext_cl *GetFinalTargetContext() HKV_OVERRIDE; 

  /// \copydoc IVRendererNode::SetFinalTargetContext
  EFFECTS_IMPEXP virtual void SetFinalTargetContext(VisRenderContext_cl *pNewContext) HKV_OVERRIDE;

  /// \copydoc VRendererNodeCommon::GetPostProcessColorTarget
  ///
  /// The parameter eVersion is ignored in this implementation, as MSAA is not supported.
  EFFECTS_IMPEXP virtual VTextureObject *GetPostProcessColorTarget(VRenderTargetVersion_e eVersion) HKV_OVERRIDE;

  /// \copydoc VRendererNodeCommon::GetPostProcessDepthStencilTarget
  ///
  /// The parameter eVersion is ignored in this implementation, as MSAA is not supported.
  EFFECTS_IMPEXP virtual VTextureObject *GetPostProcessDepthStencilTarget(VRenderTargetVersion_e eVersion) HKV_OVERRIDE;

  /// \copydoc IVRendererNode::GetGBuffer
  ///
  /// Currently, only VGBT_Accumulation is supported in this implementation.
  EFFECTS_IMPEXP virtual VTextureObject *GetGBuffer(VGBufferTarget_e eTarget) HKV_OVERRIDE;

  /// \copydoc VRendererNodeCommon::GetSupportedBufferFlags
  ///
  /// This implementation returns VBUFFERFLAG_FINALCOLOR.
  EFFECTS_IMPEXP virtual int GetSupportedBufferFlags() HKV_OVERRIDE;

  /// \brief Currently not supported.
  EFFECTS_IMPEXP virtual void SetGammaCorrection(VGammaCorrection_e gammaCorrection) HKV_OVERRIDE;

  /// \brief Currently not supported.
  EFFECTS_IMPEXP virtual VGammaCorrection_e GetGammaCorrection() HKV_OVERRIDE
  {
    return GammaCorrection;
  }

  /// \brief
  ///   Initializes the mobile forward rendering system.
  ///
  /// \sa
  ///   DeInitialize
  EFFECTS_IMPEXP virtual void InitializeRenderer() HKV_OVERRIDE;

  /// \brief
  ///   De-initializes the mobile forward rendering system.
  EFFECTS_IMPEXP virtual void DeInitializeRenderer() HKV_OVERRIDE;

  /// \brief
  ///   Creates the sky for the mobile forward rendering system.
  ///
  /// Creates the sky for the Mobile Forward Rendering System. In the integrated Time of Day system, the Mobile Forward
  /// Rendering System blends between three different cube map skies depending on the daytime. 
  /// 
  /// \param szPrefixNoon
  ///   Filename prefix for the set of sky textures at noon. The suffixes "_right", "_left", "_top", "_bottom",
  ///   "_front", and "_back" will be added to the prefix to construct the final file names.
  /// 
  /// \param szPrefixDawn
  ///   Filename prefix for the set of sky textures at dawn. The suffixes "_right", "_left", "_top", "_bottom",
  ///   "_front", and "_back" will be added to the prefix to construct the final file names.
  /// 
  /// \param szPrefixDusk
  ///   Filename prefix for the set of sky textures at dusk. The suffixes "_right", "_left", "_top", "_bottom",
  ///   "_front", and "_back" will be added to the prefix to construct the final file names.
  /// 
  /// \param szPrefixNight
  ///   Filename prefix for the set of sky textures at night. The suffixes "_right", "_left", "_top", "_bottom",
  ///   "_front", and "_back" will be added to the prefix to construct the final file names.
  /// 
  /// \param szExtension
  ///   Extension of the sky texture filenames (e.g. "dds" or "bmp").
  /// 
  /// \param bUseBottom
  ///   true if the bottom part of the sky texture should be loaded, otherwise false.
  EFFECTS_IMPEXP void CreateSky(const char *szPrefixNoon, const char *szPrefixDawn, const char *szPrefixDusk, const char *szPrefixNight, const char *szExtension, bool bUseBottom = true);

  /// \brief
  ///   Destroys the sky along with all associated texture resources.
  EFFECTS_IMPEXP void DestroySky();

  /// \brief
  ///   If time of day is enabled, this method will update the sun properties (and all associated settings).
  ///
  /// This method should be called periodically (i.e. once per frame) if the sun or time of day settings are
  /// changed.
  EFFECTS_IMPEXP void UpdateTimeOfDay();


  /// \brief
  ///   Sets the resolution mode, target rendering density, and filtering mode.
  ///
  /// Sometimes, it is desired to render the scene at a resolution that differs from the actual display resolution. For instance, high-resolution display
  /// with 300dpi and more on some mobile devices result in extremely high rendering resolutions, which naturally affects performance. This method allows
  /// you to specify a rendering resolution that differs from the resolution of the renderer node's target context. Note that the rendering resolution will
  /// always be clamped to the output resolution, so the rendered image will never have a higher resolution than the display.
  ///
  /// Note that 2D overlays and debug geometry will still always be rendered at full resolution.
  ///
  /// \param eResMode
  ///   Resolution mode to use. VRSM_FULL_RESOLUTION, VRSM_QUARTER_RESOLUTION, or VRSM_USE_TARGET_DPI.
  ///
  /// \param fDesiredDpi
  ///   If the resolution mode is set to VRSM_USE_TARGET_DPI, this specifies the desired rendering resolution in dpi (dots per inch).
  ///
  /// \param bUseFiltering
  ///   If set to true, bilinear filtering is used to upscale the render target to its final resolution.
  ///
  /// \param fResolutionTolerance
  ///   Defines the maximum deviation between the desired resolution and the display resolution (in percent) below which the renderer will render at the display
  ///   resolution even if the desired resolution is below the display resolution. For example, if this value is set to 10, the Display Resolution is 160dpi, and
  ///   the desired dpi is 150, rendering will still be performed at the native 160dpi. As this saves an additional upscaling operation on devices with a display
  ///   resolution very close to the desired rendering resolution, having a certain margin here is usually recommended.
  EFFECTS_IMPEXP void SetResolutionMode(VRendererResolutionMode_e eResMode, float fDesiredDpi, bool bUseFiltering, float fResolutionTolerance);

  /// \brief
  ///   Returns the current renderer resolution mode.
  inline VRendererResolutionMode_e GetResolutionMode() const { return RenderingResolutionMode; }

  /// \brief
  ///   Returns the current rendering density in dpi. Only meaningful if the resolution mode is set to VRSM_USE_TARGET_DPI.
  inline float GetRenderingDpi() const { return DesiredRenderingDpi; }

  /// \brief
  ///   Returns whether bilinear filtering is used for upscaling the render target. Only meaningful if upscaling is actually used, i.e. the rendering resolution differs from the target/display resolution.
  inline bool GetUpscaleFiltering() const { return UseUpscaleFiltering ? true : false; }

  
  static EFFECTS_IMPEXP VCallback OnSetResolution;  ///< Callback triggered by the VMobileForwardRenderer whenever it needs to determine the rendering resolution.


  /// \copydoc VRendererNodeCommon::OnPostProcessorChanged
  EFFECTS_IMPEXP virtual void OnPostProcessorChanged() HKV_OVERRIDE;


#ifndef _VISION_DOC
  // INTERNAL:
  EFFECTS_IMPEXP void GetTargetSizeFromDeviceDPI(const int *pOriginalSize, int *pTargetSize) const;
  EFFECTS_IMPEXP bool DetermineRenderResolution(int *iTargetSize);
  EFFECTS_IMPEXP void SetUpscaling(bool bStatus, int iWidth, int iHeight);
  inline bool IsUsingUpscaling() const { return m_spUpscaleTargetContext != NULL; }

  EFFECTS_IMPEXP void CreateOffscreenContext();
  EFFECTS_IMPEXP void RemoveOffscreenContext();
  EFFECTS_IMPEXP void GetRenderTargetConfig(VisRenderableTextureConfig_t &config);
  EFFECTS_IMPEXP void GetDepthStencilConfig(VisRenderableTextureConfig_t &config);

  EFFECTS_IMPEXP virtual VType *GetSupportedTimeOfDaySystem() HKV_OVERRIDE;

  void SetDebugScreenMask(bool bStatus);

  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  V_DECLARE_VARTABLE(VMobileForwardRenderingSystem, EFFECTS_IMPEXP);

  V_DECLARE_SERIAL_DLLEXP( VMobileForwardRenderingSystem, EFFECTS_IMPEXP );
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  EFFECTS_IMPEXP VMobileForwardRenderingSystem();

  EFFECTS_IMPEXP void CreateShaderProvider();

  EFFECTS_IMPEXP void ResetShaderProvider();

  // VARTABLE variables:
  VGammaCorrection_e GammaCorrection;
  VRendererResolutionMode_e RenderingResolutionMode;
  float DesiredRenderingDpi;
  float ResolutionTolerance;
  BOOL UseUpscaleFiltering;

#endif

protected:
  VisRenderableTexturePtr m_spOffscreenRenderTarget;
  VisRenderableTexturePtr m_spOffscreenDepthStencilTarget;
  VisRenderContextPtr m_spOffscreenContext;
  VisRenderContextPtr m_spUpscaleTargetContext;
  VisRenderContextPtr m_spOriginalSizeTargetContext;
  VisRenderContextPtr m_spStoreFinalTargetContext;
  VSkyPtr m_spSky;

  IVisShaderProviderPtr oldShaderProvider;
  IVisShaderProviderPtr mobileShaderProvider;

};

#endif  // VMOBILE_FORWARDRENDERER_HPP_INCLUDED

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
