/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file PostProcessBase.hpp

#ifndef POSTPROCESSBASE_HPP_INCLUDED
#define POSTPROCESSBASE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

class VRendererNodeCommon;

#define VPOSTPROCESSOR_PRIORITY_DEFERRED_RESOLVE      VIS_RENDERCONTEXTPRIORITY_SCENE + 1.f
#define VPOSTPROCESSOR_PRIORITY_DEFERRED_EDGEMASK   	VIS_RENDERCONTEXTPRIORITY_SCENE + 2.f

#define VPOSTPROCESSOR_PRIORITY_DEFERRED_LIGHTS		  	VIS_RENDERCONTEXTPRIORITY_POSTPROCESSOR
#define VPOSTPROCESSOR_PRIORITY_SSSSS			          	VIS_RENDERCONTEXTPRIORITY_POSTPROCESSOR + 250.f
#define VPOSTPROCESSOR_PRIORITY_SSAO					        VIS_RENDERCONTEXTPRIORITY_POSTPROCESSOR + 500.f
#define VPOSTPROCESSOR_PRIORITY_DEFERRED_RIMLIGHT	  	VIS_RENDERCONTEXTPRIORITY_POSTPROCESSOR + 600.f
#define VPOSTPROCESSOR_PRIORITY_DEFERRED_DEPTH_FOG  	VIS_RENDERCONTEXTPRIORITY_POSTPROCESSOR + 750.f
#define VPOSTPROCESSOR_PRIORITY_GLOBAL_FOG				    VIS_RENDERCONTEXTPRIORITY_POSTPROCESSOR + 750.f

#define VPOSTPROCESSOR_PRIORITY_GLOW					        VIS_RENDERCONTEXTPRIORITY_POSTPROCESSOR_RESOLVED + 2000.f
#define VPOSTPROCESSOR_PRIORITY_DOF						        VIS_RENDERCONTEXTPRIORITY_POSTPROCESSOR_RESOLVED + 4000.f
#define VPOSTPROCESSOR_PRIORITY_FINAL_RESOLVE         VIS_RENDERCONTEXTPRIORITY_POSTPROCESSOR_RESOLVED + 5000.f

#define VPOSTPROCESSOR_PRIORITY_TONEMAPPING_ADAPTIVE  VIS_RENDERCONTEXTPRIORITY_DISPLAY - 1000.f
#define VPOSTPROCESSOR_PRIORITY_TONEMAPPING				    VIS_RENDERCONTEXTPRIORITY_DISPLAY - 500.f
#define VPOSTPROCESSOR_PRIORITY_SCREENMASKS			    	VIS_RENDERCONTEXTPRIORITY_DISPLAY - 100.f
#define VPOSTPROCESSOR_PRIORITY_SIMPLE_COPY				    VIS_RENDERCONTEXTPRIORITY_DISPLAY


/// \brief Enum defining the buffers supported by a renderer node.
///
/// These flags are used to determine compatibility of a post-processor with each renderer node type.
enum VBufferFlag
{
  VBUFFERFLAG_FINALCOLOR  = 1,   ///< Accumulation buffer is supported
  VBUFFERFLAG_DEPTH       = 2,   ///< Linear depth texture is supported
  VBUFFERFLAG_NORMAL      = 4,   ///< Normal buffer is supported
  VBUFFERFLAG_DIFFUSE     = 8,   ///< Diffuse color buffer is supported
};

/// \brief
///   Base class for post-processing effects
/// 
/// This represents the base class for post-processing effects in the Vision Engine. Post-processing effects are
/// components which can be attached to a renderer node. There is no limit regarding the number of post-processing
/// effects a single renderer node can have. Within a single renderer node, the post-processors are rendered in the
/// order of their respective render contexts' priorities. 
/// 
/// Check out the Tutorial on Setting up custom post-processing effects in the Programmer's documentation for a step-by-step
/// guide to creating a minimal self-contained post-processor.
class VPostProcessingBaseComponent : public IVObjectComponent, public IVisCallbackHandler_cl
{
  friend class VRendererNodeCommon;

public:
  /// \brief
  ///   Flags that describe how a post processor will read/write the renderer node's buffers.
  ///
  /// The renderer node mainly uses these flags to optimize preparation of a post-processor's render targets.
  enum BufferUsageFlags_e
  {
    /// Specifies that the post-processor will read from the accumulation buffer, which prevents any copy-elision prior to it
    /// by making sure that all previous post-processors will be assigned to render into the accumulation buffer.
    SAMPLES_ACCUMULATION_BUFFER   = 0x01,

    /// Specifies that the post-processor uses alpha-blending in its output pass, which makes sure the renderer node will
    /// provide valid contents inside the render target on some platforms.
    USES_BLENDING                 = 0x02,

    /// Specifies that the post-processor renders to the entire screen in its output pass, which makes it eligible for copying
    /// the accumulation buffer contents to the renderer node's final target context (unless it also uses blending).
    DRAWS_FULLSCREEN_QUAD         = 0x04,

    /// Specifies that the post-processor will set up it's own render target, so that the renderer node will not auto-assign any
    /// color or depth-stencil targets to it.
    USES_CUSTOM_RENDERTARGET      = 0x08
  };

  /// \brief
  ///   Constructor of the VPostProcessingBaseComponent class.
  ///
	EFFECTS_IMPEXP VPostProcessingBaseComponent();

  /// \brief
  ///   Destructor of the VPostProcessingBaseComponent class.
  EFFECTS_IMPEXP virtual ~VPostProcessingBaseComponent();

  /// \brief
  ///   Returns the current owner of this component, cast to a VRendererNodeCommon for convenience.
  EFFECTS_IMPEXP VRendererNodeCommon* GetOwner() const;

  /// \brief
  ///   Pure virtual main function of the post-processor. This function needs to be implemented by subclasses
  ///   and will be executed in the associated render context's post-processing render loop.
  EFFECTS_IMPEXP virtual void Execute() = 0;

  /// \brief
  ///   Returns whether the post processor is valid and may be activated.
  virtual bool IsValid() const 
  {
    return m_bValid;
  }

  /// \brief
  ///   Returns the usage flags of this post processor as a bitwise combination of BufferUsageFlags_e.
  unsigned int GetBufferUsageFlags()
  {
    return m_bufferUsageFlags;
  }

  /// \brief
  ///   Enables/disables this post-processor.
  EFFECTS_IMPEXP virtual void SetActive(bool bStatus);

  /// \brief
  ///   Returns whether this post-processor is currently enabled or disabled.
  inline bool IsActive() const { return m_bActive; }

  /// \brief
  ///   Returns which buffers are required for this post-processor. 
  /// 
  /// When checking whether a post-processor can be attached to the current renderer node, these flags will be used
  /// to check whether the renderer node can provide all the buffers required for the post-processor.
  /// Please refer to the VBufferFlags enum for a list of supported bit flags.
  inline int GetRequiredBufferFlags() const { return m_iRequiredBufferFlags; }

  /// \brief
  ///   Called internally to attach the post-processor to a renderer node. Do not call, use \c pRenderer->AttachComponent(pPostProcessor) instead.
  EFFECTS_IMPEXP virtual void SetOwner(VisTypedEngineObject_cl *pOwner) HKV_OVERRIDE;

  /// \brief
  ///   Overridden virtual function. Called internally to query whether the post-processor can be attached to a renderer node
  ///   (e.g. all buffers required by the post-processor can be generated by the renderer node).
  EFFECTS_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut) HKV_OVERRIDE;

  /// \brief
  ///   Returns the target context for the post-processor (i.e., the render context in which this post-processor is
  ///   rendered).
  ///
  /// This render context will be created by the renderer node according to the buffer usage flags specified.
  EFFECTS_IMPEXP VisRenderContext_cl* GetTargetContext();

  /// \brief
  ///  True if the post processor has been initialized.
  EFFECTS_IMPEXP inline bool IsInitialized() const
  {
    return m_bIsInitialized;
  }


  /// \brief
  ///   Overridable initialization function of the post-processor.
  /// 
  /// This function should be responsible for context creation, resource allocation and shader loading.
  /// Avoid creation of resources anywhere else.
  EFFECTS_IMPEXP virtual void InitializePostProcessor() {}

  /// \brief
  ///   Overridable de-initialization function of the post-processor.
  EFFECTS_IMPEXP virtual void DeInitializePostProcessor() {}

  /// \brief
  ///  Prevents the post processor from reinitializing after every property change.
  ///
  /// BeginPropertyUpdate/EndPropertyUpdate blocks may be nested.
  /// Reinitialization will be performed after leaving the last property update block
  /// if any property change caused a call to ScheduleReinitialization.
  ///
  /// \note
  ///   Reinitialization will be only performed if the post processor was previously initialized.
  ///
  /// \sa EndPropertyUpdate
  /// \sa ScheduleReinitialization
  inline void BeginPropertyUpdate()
  {
    m_iPropertyUpdateCounter++;
  }

  /// \brief
  ///   Overridden base function. Sets the variable and schedules reinitialization.
  EFFECTS_IMPEXP virtual bool SetVariable(const char * name, const char * value) HKV_OVERRIDE
  {
    bool bResult = IVObjectComponent::SetVariable(name, value);
    ScheduleReinitialization();
    return bResult;
  }

  /// \brief
  ///  Ends a property update block.
  /// 
  /// Performs the scheduled reinitialization if needed when leaving the last
  /// property update block.
  ///
  /// \sa BeginPropertyUpdate
  /// \sa ScheduleReinitialization
  inline void EndPropertyUpdate()
  {
    m_iPropertyUpdateCounter--;

    if(m_iPropertyUpdateCounter == 0 && m_bReinitializationRequired)
    {
      if(m_bIsInitialized)
      {
        DeInitializePostProcessor();
        InitializePostProcessor();
      }
      m_bReinitializationRequired = false;
    }
  }

  /// \brief
  ///  Reinitializes the post-processor, or defers reinitialization while inside a property update block.
  ///
  /// Only reinitializes when the post-processor was already initialized.
  ///
  /// \sa BeginPropertyUpdate
  /// \sa EndPropertyUpdate
  EFFECTS_IMPEXP void ScheduleReinitialization()
  {
    if(m_iPropertyUpdateCounter == 0)
    {
      if(m_bIsInitialized)
      {
        DeInitializePostProcessor();
        InitializePostProcessor();
      }
    }
    else
    {
      m_bReinitializationRequired = true;
    }
  }

  /// \brief
  ///   Overridable function that should return true if the post-processor returns an unmodified image.
  EFFECTS_IMPEXP virtual bool IsIdentity() const
  {
    return !m_bActive;
  }

  /// \brief
  ///   Called by the renderer node owning this post-processor when its view properties have changed.
  ///
  /// Custom implementations that depend on FOV or clip plane settings should update their internal status
  /// in this method.
  EFFECTS_IMPEXP virtual void OnViewPropertiesChanged()
  {
  }

  /// \brief
  ///   Returns the rendering priority of the post-processor.
  ///  
  /// The priority is set internally by the post-processing class and is used to determine
  /// the order in which the post-processing effects of a single renderer node will be applied.
  inline float GetPriority() const { return m_fPriority; }

  /// \brief
  ///   Implements IVisRenderLoop_cl to re-initialize shaders on Vision::Callbacks.ReassignShaders
  ///
  /// \param pData
  ///   Callback data object
  ///
  /// Override this handler if you have a more specific way of re-assigning your shaders.
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

#ifndef _VISION_DOC
  V_DECLARE_DYNAMIC_DLLEXP( VPostProcessingBaseComponent, EFFECTS_IMPEXP );
  V_DECLARE_VARTABLE(VPostProcessingBaseComponent, EFFECTS_IMPEXP);
#endif

protected:

  #ifndef _VISION_DOC

  int m_iNumSourceTextures;
  int m_iRequiredBufferFlags;
  VTextureObjectPtr m_spSourceTextures[8];
  float m_fPriority;

  // The number of currently active nested property update blocks
  int m_iPropertyUpdateCounter;

  // Flags the post processor to be reinitialized when leaving the last property update block
  bool m_bReinitializationRequired;

  bool m_bValid;
  bool m_bActive;
  bool m_bIsInitialized;

  unsigned int m_bufferUsageFlags;

  private:
  int m_iTargetIndex;

  #endif // _VISION_DOC
};


/// \brief
///   Post-processing render loop.
///
/// This render loop is used to call each post-processor's Execute method.
class PostProcessRenderLoop_cl : public IVisRenderLoop_cl
{
public:

  /// \brief
  ///   Constructor of the post-processing render loop.
  ///
  /// You should never need to construct this class, as the renderer node will already provide an instance of this class
  /// for the post-processor's target context.
  ///
  /// \param pPostProcessor
  ///   The owner post processor
  EFFECTS_IMPEXP PostProcessRenderLoop_cl(VPostProcessingBaseComponent *pPostProcessor);

  /// \brief
  ///   Overridden OnDoRenderLoop function for this render loop.
  ///
  EFFECTS_IMPEXP virtual void OnDoRenderLoop(void *pUserData) HKV_OVERRIDE;


  /// \brief Helper function to find the first post processor of the current renderer node.
  EFFECTS_IMPEXP static VPostProcessingBaseComponent *GetNextPostProcessor(IVRendererNode* pRenderer, float fMinPriority);

protected:
  VSmartPtr<VPostProcessingBaseComponent> m_spPostProcessor;

  virtual ~PostProcessRenderLoop_cl();

public:
  // VTypedObject
  V_DECLARE_DYNAMIC_DLLEXP(PostProcessRenderLoop_cl, EFFECTS_IMPEXP)
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
