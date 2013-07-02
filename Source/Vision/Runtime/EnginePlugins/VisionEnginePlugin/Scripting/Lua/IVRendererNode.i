/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor IVRendererNode;
%nodefaultdtor IVRendererNode;

class IVRendererNode : public VisTypedEngineObject_cl
{
public:
  
  %extend{
    bool RemovePostProcessor(VPostProcessingBaseComponent *pPostProcessor)
    {
	    if(pPostProcessor==0)
		    return false;
			
      return self->RemoveComponent(pPostProcessor)==TRUE;
    }
        
    VPostProcessingBaseComponent* AddPostProcessor(const char * className, const char * szOptionalComponentName=NULL)
    {
      return VScriptRenderer_wrapper::AddPostProcessor(self, className, szOptionalComponentName);
    }
	
    int GetMultisampleMode()
    {
      if( self->IsOfType(V_RUNTIME_CLASS(VRendererNodeCommon)) )
        return ((VRendererNodeCommon *)self)->GetMultisampleMode();
      
      return -1;
    }
    
    bool SetGammaCorrection(int enable)
    {
      if( self->IsOfType(V_RUNTIME_CLASS(VRendererNodeCommon)) )
      {
        ((VRendererNodeCommon *)self)->SetGammaCorrection(enable==0?VGC_None:(enable>0?VGC_PostTransform:VGC_PreTransform));
        return true;
      }
      
      return false;
    }
    
    int GetGammaCorrection()
    {
      if( self->IsOfType(V_RUNTIME_CLASS(VRendererNodeCommon)) )
      {
        VGammaCorrection_e correction = ((VRendererNodeCommon *)self)->GetGammaCorrection();
        return correction==VGC_None?0:(correction==VGC_PostTransform?1:-1);
      }

      return -1;
    }

    bool SetUseHDR(bool bHDR)
    {
      if( self->IsOfType(V_RUNTIME_CLASS(VRendererNodeCommon)) )
      {
        ((VRendererNodeCommon *)self)->SetUseHDR(bHDR);
        return true;
      }
      
      return false;
    }

    bool GetUseHDR()
    {
      if( self->IsOfType(V_RUNTIME_CLASS(VRendererNodeCommon)) )
        return ((VRendererNodeCommon *)self)->GetUseHDR();
      
      return false;
    }

    bool IsInternalPostProcessor(VPostProcessingBaseComponent *pPostprocessor)
    {
      if( self->IsOfType(V_RUNTIME_CLASS(VRendererNodeCommon)) )
        return ((VRendererNodeCommon *)self)->IsInternalPostProcessor(pPostprocessor);
      
      return false;
    }

    bool RemovePostProcessors()
    {
      if( self->IsOfType(V_RUNTIME_CLASS(VRendererNodeCommon)) )
      {
        ((VRendererNodeCommon *)self)->RemovePostprocessors();
        return true;
      }
      
      return false;
    }
  }
  
  void SetRenderingEnabled(bool bStatus);
  inline bool GetRenderingEnabled() const;

};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(IVRendererNode, 128, "%s [%p]", self->GetTypeId()->m_lpszClassName, self)
VSWIG_CREATE_TOSTRING(IVRendererNode, "%s [%p, PostProcessors: %d]", self->GetTypeId()->m_lpszClassName, self, self->Components().Count())


//Implement this method native in order to return the concrete type instance
//instead of the interface IVObjectComponent
%native(IVRendererNode_GetPostProcessor) int IVRendererNode_GetPostProcessor(lua_State *L);
%{
  SWIGINTERN int IVRendererNode_GetPostProcessor(lua_State *L)
  {
    IS_MEMBER_OF(IVRendererNode) //this will move this function to the method table of the specified class

    SWIG_CONVERT_POINTER(L, 1, IVRendererNode, pSelf)

    //param #2: type name of the component
    if(!SWIG_lua_isnilstring(L,2)) luaL_error(L, "Expected a string value as parameter 2 for VisTypedEngineObject_AddComponentOfType");
    const char * szComponentType = lua_tostring(L, 2);       
    
    //param #3: optional name of the component
    const char * szComponentName = NULL;
    if(lua_isstring(L, 3))  szComponentName = lua_tostring(L, 3);
    
    IVObjectComponent *pComponent = NULL;
    
    if(szComponentName)
      pComponent = pSelf->Components().GetComponentOfTypeAndName(szComponentType, szComponentName);
    else
      pComponent = pSelf->Components().GetComponentOfType(szComponentType);
        
    //clean stack (remove all call params including self)
    lua_settop(L, 0);                                //stack: TOP

    if(pComponent!=NULL && pComponent->IsOfType(V_RUNTIME_CLASS(VPostProcessingBaseComponent)))
      LUA_PushObjectProxy(L, pComponent);            //stack: result, TOP
    else
      lua_pushnil(L);                                //stack: nil, TOP

    return 1; //one element on the stack
  }
%} 

#else

/// \brief Represents the rendering pipeline used for rendering the scene.
class IVRendererNode : public VisTypedEngineObject_cl
{
public:

  /// @name Post Processor Handling
  /// @{

  /// \brief Add a post processor of the specified class name.
  /// \param className Name of the post processors class. Possible values are:
  ///         * "VPostProcessSSAO"
  ///         * "VPostProcessFXAA"
  ///         * "VPostProcessGlow"
  ///         * "VPostProcessDepthOfField"
  ///         * "VPostProcessToneMapping"
  ///         * "VPostProcessToneMappingAdaptive"
  ///         * "VScreenSpaceSubsurfaceScatteringPostProcessor"
  ///         * "VDeferredShadingRimLight" (Deferred Renderer Only)
  ///         * "VGlobalFogPostprocess" (Deferred Renderer Only)
  ///         * "VDeferredShadingLights" (Deferred Renderer Only)
  /// \param optionalComponentName [\b Optional] The component name of the post processor.
  /// \return An instance of the post processor as VPostProcessingBaseComponent on success or nil on failure.
  VPostProcessingBaseComponent AddPostProcessor(string className, string optionalComponentName = nil);
  
  /// \brief Get installed post processor of the specified class name.
  /// \param className The class name of the post processor to search for. Look at AddPostProcessor for possible values.
  /// \param optionalComponentName [\b Optional] The component name of the post processor.
  /// \return An instance of the post processor as VPostProcessingBaseComponent or nil if not present.
  /// \see AddPostProcessor
  VPostProcessingBaseComponent GetPostProcessor(string className, string optionalComponentName = nil);
   
  /// \brief Remove athe specified post processor.
  /// \param postProcessor The post processor to remove.
  /// \return Returns true if post processor has been successfully removed, otherwise false.
  /// \note Do not remove post processors managed by vForge, this will lead to unpredicted behavior.
  boolean RemovePostProcessor(VPostProcessingBaseComponent postProcessor);

  /// \brief Remove all post procesoors
  /// \note Be careful with this, especially inside vForge, you will also remove post processors created by vForge
  ///  which will lead to unpredicted behavior.
  void RemovePostProcessors();

  /// \brief Function that returns whether this post processor was created internally by this renderer node.
  ///        This can be useful when looping through renderer's components e.g. to detect duplication.
  /// \param postProcessor The post processor to test.
  /// \return true if it is an internal post processor, false otherwise.
  boolean IsInternalPostProcessor(VPostProcessingBaseComponent postProcessor);

  /// @}
  /// @name Render Node Settings
  /// @{
  
  /// \brief Returns the multisampling mode used by the renderer node.
  /// \returns 0: disabled 1: multisample 1x
  number GetMultisampleMode();

  /// \brief Requested gamma correction mode (available if implemented in this node).
  /// \param gammaCorrection 0: disable +1: use gamma correct after tranformation -1: use gamma correction before transformation
  void SetGammaCorrection(number gammaCorrection);

  /// \brief Returns the gamma correction mode.
  /// \retrn 0 if the gamma correction is disabled, -1 if it is applied before the transformation, +1 if it is applied after the transformation.
  number GetGammaCorrection();
 
  /// \brief Use this method to set whether the renderer node should use HDR rendering. Implementations may choose to ignore this.
  /// \param enable Set to true to enable, false to disable.
  void SetUseHDR(boolean enable);

  /// \brief Returns whether the renderer node uses HDR rendering. Implementations should return the correct value here.
  /// \return true if the render node uses HDR, false otherwise.
  boolean GetUseHDR();

  /// \brief Enabled/disables rendering of this renderer node. Lightweight function, use this if you frequently switch between
  ///        renderer nodes without intending to de-initialize them.
  /// \param status Set to true to enable the render node, false to disable.
  void SetRenderingEnabled(boolean status);

  /// \brief Returns whether rendering of this renderer node is currently enabled.
  /// \return Returns true if render node is enabled, otherwise false.
  boolean GetRenderingEnabled();

  /// @}
  
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
