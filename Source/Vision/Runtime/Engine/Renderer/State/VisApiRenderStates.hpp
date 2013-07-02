/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiRenderStates.hpp

#ifndef DEFINE_VISAPIRENDERSTATES
#define DEFINE_VISAPIRENDERSTATES

/// \brief
///   Class with static helper functions to access and modify Vision's internal shadowed render
///   state of the graphics API.
class VisRenderStates_cl
{
public:

  ///
  /// @name Set Group States
  /// @{
  ///

  /// \brief
  ///   Sets a new rasterizer group state. Early outs when the hash key is identical to the current state's hash key.
  VISION_APIFUNC static void SetRasterizerState(const VStateGroupRasterizer &pState);

  /// \brief
  ///   Sets a new depth/stencil group state. Early outs when the hash key is identical to the current state's hash key.
  VISION_APIFUNC static void SetDepthStencilState(const VStateGroupDepthStencil &pState);

  /// \brief
  ///   Sets a new tessellation group state. Early outs when the hash key is identical to the current state's hash key.
  VISION_APIFUNC static void SetTesselationState(const VStateGroupTesselation &pState);

  /// \brief
  ///   Sets a new blending group state. Early outs when the hash key is identical to the current state's hash key.
  VISION_APIFUNC static void SetBlendState(const VStateGroupBlend &pState);

  /// \brief
  ///   Sets a new sampler group state for a specific sampler index. Early outs when the hash key is identical to the current state's hash key.
  VISION_APIFUNC static void SetSamplerState(int iSampler, const VStateGroupSampler &pState);

  /// \brief
  ///   Sets a new texture group state for a specific sampler index. Early outs when the hash key is identical to the current state's hash key.
  VISION_APIFUNC static void SetTextureState(int iSampler, const VStateGroupTexture &pState);

  ///
  /// @}
  ///

  ///
  /// @name Access Current States
  /// @{
  ///

  /// \brief
  ///   Returns the pointer to the engine's current rasterizer state. The return value points to a copy of the state set by SetRasterizerState, so their pointer values will not be identical.
  VISION_APIFUNC static const VStateGroupRasterizer *GetRasterizerState();

  /// \brief
  ///   Returns the pointer to the engine's current depth/stencil state. The return value points to a copy of the state set by SetDepthStencilState, so their pointer values will not be identical.
  VISION_APIFUNC static const VStateGroupDepthStencil *GetDepthStencilState();

  /// \brief
  ///   Returns the pointer to the engine's current tessellation state. The return value points to a copy of the state set by SetTesselationState, so their pointer values will not be identical.
  VISION_APIFUNC static const VStateGroupTesselation *GetTesselationState();

  /// \brief
  ///   Returns the pointer to the engine's current blending state. The return value points to a copy of the state set by SetBlendState, so their pointer values will not be identical.
  VISION_APIFUNC static const VStateGroupBlend *GetBlendState();

  /// \brief
  ///   Returns the pointer to the engine's current sampler state. The return value points to a copy of the state set by SetSamplerState, so their pointer values will not be identical.
  VISION_APIFUNC static const VStateGroupSampler *GetSamplerState(int iSampler);

  /// \brief
  ///   Returns the pointer to the engine's current texture state. The return value points to a copy of the state set by SetTextureState, so their pointer values will not be identical.
  VISION_APIFUNC static const VStateGroupTexture *GetTextureState(int iSampler);

  ///
  /// @}
  ///

  ///
  /// @name Default States
  /// @{
  ///


  /// \brief
  ///   Returns the pointer to an instance of a rasterizer group that represents the default state.
  VISION_APIFUNC static const VStateGroupRasterizer *GetRasterizerDefaultState();

  /// \brief
  ///   Returns the pointer to an instance of a depth/stencil group that represents the default state.
  VISION_APIFUNC static const VStateGroupDepthStencil *GetDepthStencilDefaultState();

  /// \brief
  ///   Returns the pointer to an instance of a tessellation group that represents the default state.
  VISION_APIFUNC static const VStateGroupTesselation *GetTesselationDefaultState();

  /// \brief
  ///   Returns the pointer to an instance of a blend group that represents the default state.
  VISION_APIFUNC static const VStateGroupBlend *GetBlendDefaultState();

  /// \brief
  ///   Returns the pointer to an instance of a sampler group that represents the default state.
  VISION_APIFUNC static const VStateGroupSampler *GetSamplerDefaultState();

  /// \brief
  ///   Returns the pointer to an instance of a texture group that represents the default state.
  VISION_APIFUNC static const VStateGroupTexture *GetTextureDefaultState();


  ///
  /// @}
  ///

  ///
  /// @name Predefined State Groups
  /// @{
  ///

  /// \brief
  ///   Returns an instance of a depth/stencil group state determined by the enum VisStencilState_e.
  VISION_APIFUNC static const VStateGroupDepthStencil *GetDepthStencilStateGroup(VisStencilState_e state);

  /// \brief
  ///   Returns an instance of a rasterizer group state determined by the enum VIS_RasterizerStyle.
  VISION_APIFUNC static const VStateGroupRasterizer *GetRasterizerStateGroup(VIS_RasterizerStyle state);

  /// \brief
  ///   Returns an instance of a blend group state determined by the enum VIS_TransparencyType.
  VISION_APIFUNC static const VStateGroupBlend *GetBlendStateGroup(VIS_TransparencyType state);

  /// \brief
  ///   Returns an instance of a blend group state (used for 2D overlay rendering) determined by the enum
  ///   VIS_TransparencyType.
  VISION_APIFUNC static const VStateGroupBlend *GetOverlayBlendStateGroup(VIS_TransparencyType state);


  /// \brief
  ///   Returns an instance of a sampler group state determined by the enum VisSamplerState_e
  VISION_APIFUNC static const VStateGroupSampler *GetSamplerStateGroup(VisSamplerState_e state);


  ///
  /// @}
  ///

  ///
  /// @name Track Shader Constants
  /// @{
  ///

#if !defined (_VR_DX11) || defined (_VISION_DOC)

  /// \brief
  ///   Sets one or more vertex shader constant registers.
  /// 
  /// This function directly wraps around the graphics API function. Preferably, you should modify shader constants
  /// using the VShaderConstantBuffer and VConstantBufferRegister, which combine constant buffer updates. It can be
  /// useful however to change constants between rendering passes without rebinding the shader pass object.
  /// 
  /// Every register consists of 4 floats, so the passed array is expected to contain iNumConstants * 4 float values.
  /// 
  /// The actual valid register range is determined by the platform resp. by the shader model of the system.
  ///
  /// Use this function carefully because it might overwrite the register ranges predefined by the engine.
  /// 
  /// Note that this function is not supported on DX10/DX11.
  ///
  /// \param iFirstIndex
  ///   Index of the first register to set.
  /// 
  /// \param pConstants
  ///   Pointer to an array of iNumConstants * 4 floating-point values to set.
  ///
  /// \param iNumConstants
  ///   The number of constants to set.
  VISION_APIFUNC static void SetVertexShaderConstant(int iFirstIndex, const float *pConstants, int iNumConstants);

  /// \brief
  ///   Sets one or more vertex shader constant registers.
  /// 
  /// This function directly wraps around the graphics API function. Preferably, you should modify shader constants
  /// using the VShaderConstantBuffer and VConstantBufferRegister, which combine constant buffer updates. It can be
  /// useful however to change constants between rendering passes without rebinding the shader pass object.
  /// 
  /// Every register consists of 4 BOOLs, so the passed array is expected to contain iNumConstants * 4 BOOL values.
  /// 
  /// The actual valid register range is determined by the platform resp. by the shader model of the system.
  ///
  /// Use this function carefully because it might overwrite the register ranges predefined by the engine.
  /// 
  /// Note that this function is not supported on DX10/DX11.
  ///
  /// \param iFirstIndex
  ///   Index of the first register to set.
  /// 
  /// \param pConstants
  ///   Pointer to an array of iNumConstants * 4 boolean values to set.
  ///
  /// \param iNumConstants
  ///   The number of constants to set.
  VISION_APIFUNC static void SetVertexShaderConstantB(int iFirstIndex, const BOOL *pConstants, int iNumConstants);

  /// \brief
  ///   Sets one or more pixel shader constant registers.
  /// 
  /// This function directly wraps around the graphics API function. Preferably, you should modify shader constants
  /// using the VShaderConstantBuffer and VConstantBufferRegister, which combine constant buffer updates. It can be
  /// useful however to change constants between rendering passes without rebinding the shader pass object.
  /// 
  /// Every register consists of 4 floats, so the passed array is expected to contain iNumConstants * 4 float values.
  /// 
  /// The actual valid register range is determined by the platform resp. by the shader model of the system.
  ///
  /// Use this function carefully because it might overwrite the register ranges predefined by the engine.
  /// 
  /// Note that this function is not supported on DX10/DX11.
  ///
  /// \param iFirstIndex
  ///   Index of the first register to set.
  /// 
  /// \param pConstants
  ///   Pointer to an array of iNumConstants * 4 floating-point values to set.
  ///
  /// \param iNumConstants
  ///   The number of constants to set.
  VISION_APIFUNC static void SetPixelShaderConstant(int iFirstIndex, const float *pConstants, int iNumConstants);


  /// \brief
  ///   Sets one or more pixel shader constant registers.
  /// 
  /// This function directly wraps around the graphics API function. Preferably, you should modify shader constants
  /// using the VShaderConstantBuffer and VConstantBufferRegister, which combine constant buffer updates. It can be
  /// useful however to change constants between rendering passes without rebinding the shader pass object.
  /// 
  /// Every register consists of 4 BOOLs, so the passed array is expected to contain iNumConstants * 4 BOOL values.
  /// 
  /// The actual valid register range is determined by the platform resp. by the shader model of the system.
  ///
  /// Use this function carefully because it might overwrite the register ranges predefined by the engine.
  /// 
  /// Note that this function is not supported on DX10/DX11.
  ///
  /// \param iFirstIndex
  ///   Index of the first register to set.
  /// 
  /// \param pConstants
  ///   Pointer to an array of iNumConstants * 4 boolean values to set.
  ///
  /// \param iNumConstants
  ///   The number of constants to set.
  VISION_APIFUNC static void SetPixelShaderConstantB(int iFirstIndex, const BOOL *pConstants, int iNumConstants);


  /// \brief
  ///   Helper function to upload a 4x4 matrix to the vertex shader using 4 registers.
  ///
  /// Platform specific row/column conversion is performed internally.
  ///
  /// This function directly wraps around the graphics API function. Preferably, you should modify shader constants
  /// using the VShaderConstantBuffer and VConstantBufferRegister, which combine constant buffer updates. It can be
  /// useful however to change constants between rendering passes without rebinding the shader pass object.
  /// 
  /// Note that this function is not supported on DX10/DX11.
  ///
  /// \param iFirstIndex
  ///   Index of the first register to set.
  /// 
  /// \param mConstants
  ///   The floating point matrix to set.
  VISION_APIFUNC static void SetVertexShaderMatrixConstant(int iFirstIndex, const hkvMat4& mConstants);

  /// \brief
  ///   Helper function to upload a 4x4 matrix to the pixel shader using 4 registers.
  ///
  /// Platform specific row/column conversion is performed internally.
  ///
  /// This function directly wraps around the graphics API function. Preferably, you should modify shader constants
  /// using the VShaderConstantBuffer and VConstantBufferRegister, which combine constant buffer updates. It can be
  /// useful however to change constants between rendering passes without rebinding the shader pass object.
  /// 
  /// Note that this function is not supported on DX10/DX11.
  ///
  /// \param iFirstIndex
  ///   Index of the first register to set.
  /// 
  /// \param mConstants
  ///   The floating point matrix to set.
  VISION_APIFUNC static void SetPixelShaderMatrixConstant(int iFirstIndex, const hkvMat4& mConstants);

  /// \brief
  ///   Uploads an arbitrary number of matrices. See SetVertexShaderMatrixConstant.
  ///
  /// This function directly wraps around the graphics API function. Preferably, you should modify shader constants
  /// using the VShaderConstantBuffer and VConstantBufferRegister, which combine constant buffer updates. It can be
  /// useful however to change constants between rendering passes without rebinding the shader pass object.
  ///
  /// \param iFirstIndex
  ///   Index of the first register to set.
  /// 
  /// \param pConstants
  ///   Pointer to a set of matrices to set.
  ///
  /// \param iNumMatrices
  ///   Number of matrix constants to set.
  VISION_APIFUNC static void SetVertexShaderMatrixConstants(int iFirstIndex, const hkvMat4* pConstants, int iNumMatrices);

  /// \brief
  ///   Uploads an arbitrary number of matrices. See SetPixelShaderMatrixConstant.
  ///
  /// This function directly wraps around the graphics API function. Preferably, you should modify shader constants
  /// using the VShaderConstantBuffer and VConstantBufferRegister, which combine constant buffer updates. It can be
  /// useful however to change constants between rendering passes without rebinding the shader pass object.
  ///
  /// \param iFirstIndex
  ///   Index of the first register to set.
  /// 
  /// \param pConstants
  ///   Pointer to a set of matrices to set.
  ///
  /// \param iNumMatrices
  ///   Number of matrix constants to set.
  VISION_APIFUNC static void SetPixelShaderMatrixConstants(int iFirstIndex, const hkvMat4* pConstants, int iNumMatrices);

#endif

  /// \brief
  ///   Sets a simple vertex and pixel shaders optimized for depth-only output. It is undefined
  ///   whether color output is generated or not.
  VISION_APIFUNC static void SetDepthFillShader();

  /// \brief
  ///   Sets the 2x2 lightmap to lightmask coordinate transformation matrix. The according global
  ///   shader constant value will be updated.
  VISION_APIFUNC static void VSSetLightmapToLightMaskTransform(const float *pMatrix2x2);

  /// \brief
  ///   Sets the static shadow light color. The according global shader constant value will be
  ///   updated.
  VISION_APIFUNC static void VSSetStaticShadowLightColor(const hkvVec4& vColor);

  /// \brief
  ///   Sets the 2x2 model UV to lightmap transformation matrix. The according global shader
  ///   constant value will be updated.
  VISION_APIFUNC static void VSSetModelUVToLightmap(const float *pMatrix2x2);


  /// \brief
  ///   Binds a shader constant buffer to the specified VS slot.
  /// 
  /// This function is natively supported in DX10/DX11 and emulated on other platforms that support shader constants (e.g. DX9).
  /// Emulation means that the iSlot index is ignored and the constant range defined in pBuffer is directly uploaded.
  ///
  /// \param iSlot
  ///   VS Constant Buffer slot to bind this constant buffer to.
  ///
  /// \param pBuffer
  ///   The Shader Constant Buffer to use.
  VISION_APIFUNC static void SetVSConstantBuffer(int iSlot, VShaderConstantBuffer *pBuffer);

#if defined(_VR_DX11)

  /// \brief
  ///   Binds a shader constant buffer to the specified GS slot. Only supported in DX10/DX11.
  ///
  /// \param iSlot
  ///   GS Constant Buffer slot to bind this constant buffer to.
  ///
  /// \param pBuffer
  ///   The Shader Constant Buffer to use.
  VISION_APIFUNC static void SetGSConstantBuffer(int iSlot, VShaderConstantBuffer *pBuffer);

  /// \brief
  ///   Binds a shader constant buffer to the specified HS slot. Only supported in DX11.
  ///
  /// \param iSlot
  ///   HS Constant Buffer slot to bind this constant buffer to.
  ///
  /// \param pBuffer
  ///   The Shader Constant Buffer to use.
  VISION_APIFUNC static void SetHSConstantBuffer(int iSlot, VShaderConstantBuffer *pBuffer);

  /// \brief
  ///   Binds a shader constant buffer to the specified DS slot. Only supported in DX11.
  ///
  /// \param iSlot
  ///   DS Constant Buffer slot to bind this constant buffer to.
  ///
  /// \param pBuffer
  ///   The Shader Constant Buffer to use.
  VISION_APIFUNC static void SetDSConstantBuffer(int iSlot, VShaderConstantBuffer *pBuffer);

  /// \brief
  ///   Binds a shader constant buffer to the specified CS slot. Only supported in DX11.
  ///
  /// \param iSlot
  ///   CS Constant Buffer slot to bind this constant buffer to.
  ///
  /// \param pBuffer
  ///   The Shader Constant Buffer to use.
  VISION_APIFUNC static void SetCSConstantBuffer(int iSlot, VShaderConstantBuffer *pBuffer);

#endif

  /// \brief
  ///   Binds a shader constant buffer to the specified PS slot.
  /// 
  /// This function is natively supported in DX10/DX11 and emulated on other platforms that support shader constants (e.g. DX9).
  /// Emulation means that the iSlot index is ignored and the constant range defined in pBuffer is directly uploaded.
  ///
  /// \param iSlot
  ///   PS Constant Buffer slot to bind this constant buffer to.
  ///
  /// \param pBuffer
  ///   The Shader Constant Buffer to use.
  VISION_APIFUNC static void SetPSConstantBuffer(int iSlot, VShaderConstantBuffer *pBuffer);


  /// \brief
  ///   Returns the texture object for a given combination of texture state group and surface
  ///   texture set.
  VISION_APIFUNC static VTextureObject *GetSpecificTexture(VStateGroupTexture *pSGTexture, VisSurfaceTextures_cl *pSurface, VisTextureAnimInstance_cl *pAnimInstance);

  /// \brief
  ///   Returns the texture object for a given combination of texture state group and static
  ///   geometry instance.
  VISION_APIFUNC static VTextureObject *GetSpecificTexture(VStateGroupTexture *pSGTexture, VisStaticGeometryInstance_cl *pMeshInstance);

  /// \brief
  ///   Returns the texture object for a given combination of texture state group and particle
  ///   group.
  VISION_APIFUNC static VTextureObject *GetSpecificTexture(VStateGroupTexture *pSGTexture, VisParticleGroup_cl *pGroup);

  /// \brief
  ///   Returns the texture object for a given combination of texture state group and entity
  ///   surface.
  VISION_APIFUNC static VTextureObject *GetSpecificTexture(VStateGroupTexture *pSGTexture, VisBaseEntity_cl *pEntity, VBaseSubmesh *pSubmesh, VisSurfaceTextures_cl *surface);

  /// \brief
  ///   Returns the texture object for a given combination of texture state group and screen mask.
  VISION_APIFUNC static VTextureObject *GetSpecificTexture(VStateGroupTexture *pSGTexture, VisScreenMask_cl *pMask);

  /// \brief
  ///   Returns the texture object for a given combination of texture state group and mesh buffer.
  VISION_APIFUNC static VTextureObject *GetSpecificTexture(VStateGroupTexture *pSGTexture, class VisMeshBuffer_cl *pBuffer, int iStage);

  ///
  /// @}
  ///

  ///
  /// @name Matrices and Clip Plane
  /// @{
  ///

  /// \brief
  ///   This function bypasses the automatic tracking of projection matrix from the context and
  ///   allows for changing the projection matrix inside the renderloop.
  ///   It should not be necessary to use this function. Current internal use case: foreground
  ///   entities with custom projection matrix.
  VISION_APIFUNC static void SetCurrentProjectionMatrix(const hkvMat4* pMatrix);

  /// \brief
  ///   This function sets the camera related shader constants like the automatically tracked view matrix
  ///   and the eye position.
  ///   This function should be used when manually rendering a context inside another one, and the shaders
  ///   used by the manually rendered context are using the camera parameters mentioned above.
  ///   Current internal use case: When using interleaved shadow map rendering, the shadow map context
  ///   is executed inside the deferred shading lights context. In case instancing is used, the camera
  ///   parameters need to be updated to correctly compute the shadow maps.
  VISION_APIFUNC static void UpdateCurrentCameraShaderConstants();

  /// \brief
  ///   Sets the shader constants reserved for storing time of day information (sun direction and color). 
  ///   The Vision Engine will not touch this data internally; it is fully up to the shader and custom code
  ///   to update and use these values.
  VISION_APIFUNC static void SetTimeOfDayShaderConstants(const hkvVec3& sunDirection, const hkvVec4& sunColor);


  ///
  /// @}
  ///
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
