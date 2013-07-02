/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vShaderPassResource.hpp

#ifndef VSHADERPROGRAMRESOURCE_HPP_INCLUDED
#define VSHADERPROGRAMRESOURCE_HPP_INCLUDED

#include <Vision/Runtime/Base/System/vLog.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderParam.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vCompiledShaderPass.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludes.hpp>

class VShaderPassResource;
class VShaderEffectLib;

#if defined (_VISION_PS3)
  #include <Vision/Runtime/Base/Graphics/Shader/vShaderProgramResourcePS3.inl>
#elif defined (_VISION_PSP2)
  #include <Vision/Runtime/Base/Graphics/Shader/vShaderProgramResourcePSP2.inl>
#elif defined (_VR_GLES2)
  #include <Vision/Runtime/Base/Graphics/Shader/vShaderProgramResourceGLES2.inl>
#elif defined (_VR_GX2)
  #include <Vision/Runtime/Base/Graphics/Shader/vShaderProgramResourceWiiU.inl>
#endif

/// \brief
///   Represents the source code state of a shader program for a specific shader stage (e.g. Pixel Shader, Vertex Shader, etc.)
/// 
/// This class holds the source code in text format and optionally has a pre-compiled bytecode version of the shader.
/// Furthermore, this class provides helper functions to compile the source code version to the bytecode representation
///
/// Note that actual shader compilation only occurs inside vForge. Shaders are never compiled at runtime.
class VShaderProgramResource : public VBaseObject
{
public:

  ///
  /// @name Constructor/Destructor and general Initialization
  /// @{
  ///

public:

  /// \brief
  ///   Constructor
  VBASE_IMPEXP VShaderProgramResource();

  /// \brief
  ///   Copy Constructor
  VBASE_IMPEXP VShaderProgramResource(const VShaderProgramResource& cc);

  /// \brief
  ///   Destructor
  VBASE_IMPEXP ~VShaderProgramResource();

  /// \brief
  ///   Assignment operator
  VBASE_IMPEXP const VShaderProgramResource& operator= (const VShaderProgramResource& rhs);

  /// \brief
  ///   Sets the shader stage this program is used in.
  VBASE_IMPEXP void SetProgramUsage (VShaderPassResource* pOwnerShader, VShaderStage_e Shader);

  ///
  /// @}
  ///

  ///
  /// @name Constant Table
  /// @{
  ///

public:

  /// \brief
  ///   Returns or creates a constant table object for the specified platform.
  VBASE_IMPEXP VShaderConstantTable* GetConstantTable (VTargetPlatform_e platform, bool bCreateIfNotExisting) const;

  /// \brief
  ///   Sets the constant table object for the specified platform.
  VBASE_IMPEXP void SetConstantTable (VTargetPlatform_e platform, VShaderConstantTable* pTable);

  ///
  /// @}
  ///

  ///
  /// @name Byte Code
  /// @{
  ///

public:

  /// \brief
  ///   Frees all the associated pre-compiled Bytecode.
  VBASE_IMPEXP void FreeByteCode (void);

  /// \brief
  ///   Frees all the associated pre-compiled Bytecode.
  VBASE_IMPEXP void FreeByteCode (VTargetPlatform_e platform);

  /// \brief Static helper
  VBASE_IMPEXP static void FreeByteCode(int* pDest);

  /// \brief
  ///   Indicates whether this source code has pre-compiled bytecode
  VBASE_IMPEXP bool HasByteCode(VTargetPlatform_e platform = TARGETPLATFORM_THIS) const {return m_pByteCode[platform]!=NULL;}

  /// \brief
  ///   Indicates whether the bytecode is compressed
  VBASE_IMPEXP bool HasCompressedByteCode(VTargetPlatform_e platform = TARGETPLATFORM_THIS) const;

  /// \brief
  ///   Returns the bytecode buffer
  VBASE_IMPEXP const int* GetByteCodeBuffer (VTargetPlatform_e platform = TARGETPLATFORM_THIS) const { VASSERT (m_pByteCode[platform] != NULL); return (m_pByteCode[platform]); }

  /// \brief
  ///   Returns the length of the bytecode.
  VBASE_IMPEXP int GetByteCodeLength (VTargetPlatform_e Platform = TARGETPLATFORM_THIS) const { if (IsShaderUsingSharedSource (Platform)) return (0); return (m_iByteCodeLen[Platform]); }

  /// \brief
  ///   Helper function to copy the byte code buffer from a source
  VBASE_IMPEXP void CopyByteCode(VTargetPlatform_e platform, VShaderModel_e target, const int *pSrc, int iSourceLen);

  /// \brief
  ///   Internal callback that is called after parsing bytecode from file
  VBASE_IMPEXP void OnByteCodeLoaded(VTargetPlatform_e platform);

  /// \brief
  ///   Internal copy helper function
  VBASE_IMPEXP static void CopyByteCode(const int *pSrc, int iSourceLen, int **pDest);

  ///
  /// @}
  ///

  ///
  /// @name Samplers
  /// @{
  ///

public:

  /// \brief
  ///   Allocates iCount texture samplers for this shader. Internally used while loading.
  VBASE_IMPEXP void SetActiveSamplerCount(int iCount);

  /// \brief
  ///   Allocates iCount texture samplers for this shader. Internally used while loading.
  VBASE_IMPEXP int MapSamplers(const VShaderConstantTable &table, int iTargetCount, VStateGroupSampler *pTargetSamplers, VStateGroupTexture *pTargetTextures, short *pMapping) const;

  /// \brief
  ///   Returns the number of active samplers.
  VBASE_IMPEXP unsigned int GetNumActiveSamplers (void) const { return (m_uiNumActiveSamplers); }

  /// \brief
  ///   Returns the name of the sampler at the given index.
  VBASE_IMPEXP const VString& GetSamplerName (unsigned int uiSamplerIndex) const { VASSERT (uiSamplerIndex < m_uiNumActiveSamplers); return (m_pSamplerName[uiSamplerIndex]); }

  /// \brief
  ///   Sets the name of the sampler at the given index.
  VBASE_IMPEXP void SetSamplerName (unsigned int uiSamplerIndex, const char* szName) { VASSERT (uiSamplerIndex < m_uiNumActiveSamplers); m_pSamplerName[uiSamplerIndex] = szName; }

  /// \brief
  ///   Returns the Sampler State for the sampler at the given index.
  VBASE_IMPEXP const VStateGroupSampler& GetSampler (unsigned int uiSamplerIndex) const { VASSERT (uiSamplerIndex < m_uiNumActiveSamplers); return (m_pSamplers[uiSamplerIndex]); }
  /// \brief
  ///   Returns the Sampler State for the sampler at the given index.
  VBASE_IMPEXP VStateGroupSampler& GetSampler_WriteAccess (unsigned int uiSamplerIndex) { VASSERT (uiSamplerIndex < m_uiNumActiveSamplers); return (m_pSamplers[uiSamplerIndex]); }

  /// \brief
  ///   Returns the Texture-State for the sampler at the given index.
  VBASE_IMPEXP const VStateGroupTexture& GetTexture (unsigned int uiSamplerIndex) const { VASSERT (uiSamplerIndex < m_uiNumActiveSamplers); return (m_pTextures[uiSamplerIndex]); }
  /// \brief
  ///   Returns the Texture-State for the sampler at the given index.
  VBASE_IMPEXP VStateGroupTexture& GetTexture_WriteAccess (unsigned int uiSamplerIndex) { VASSERT (uiSamplerIndex < m_uiNumActiveSamplers); return (m_pTextures[uiSamplerIndex]); }

  /// \brief
  ///   Returns the custom texture filename for the sampler at the given index.
  VBASE_IMPEXP const VString& GetCustomTextureFilename (unsigned int uiSamplerIndex) const { VASSERT (uiSamplerIndex < m_uiNumActiveSamplers); return (m_pCustomTextureFilename[uiSamplerIndex]); }
  /// \brief
  ///   Returns the custom texture filename for the sampler at the given index.
  VBASE_IMPEXP void SetCustomTextureFilename (unsigned int uiSamplerIndex, const char* szName) { VASSERT (uiSamplerIndex < m_uiNumActiveSamplers); m_pCustomTextureFilename[uiSamplerIndex] = szName; }

  ///
  /// @}
  ///

  ///
  /// @name Compile Shaders
  /// @{
  ///

public:

  /// \brief
  ///   Creates a shader program of the given type and stores it as a void pointer in out_ResultPtr which needs to be casted to VVertexShaderObject*, VPixelShaderObject*, etc.
  VBASE_IMPEXP void CreateShader (VShaderStage_e Shader, void** out_ResultPtr, IVLog* pLog);

  /// \brief
  ///   Returns the Minimum Shader Model required by this shader.
  VBASE_IMPEXP VShaderModel_e GetMinShaderModel (void) const { return (m_eMinModel); }

  /// \brief
  ///   Returns the Minimum Shader Model required by this shader in DX11.
  VBASE_IMPEXP VShaderModel_e GetMinShaderModelDX11 (void) const { return (m_eMinModelDX11); }

  /// \brief
  ///   Sets the Minimum Shader Model required by this shader. If ModelDX11 is set to VSM_INVALID it will use the previously set (default) value.
  VBASE_IMPEXP void SetMinShaderModel (VShaderModel_e Model, VShaderModel_e ModelDX11 = VSM_INVALID);

  /// \brief
  ///   Return the custom shader source code.
  VBASE_IMPEXP const VString& GetCustomSource (void) const { return (m_sCustomSource); }

  /// \brief
  ///   Return the custom shader source code.
  VBASE_IMPEXP void SetCustomSource (const VString& sSource) { m_sCustomSource = sSource; }

  /// \brief
  ///   Return the function name of the shader entry point.
  VBASE_IMPEXP const VString& GetEntryFunctionName (void) const { return (m_sFunctionName); }

  /// \brief
  ///   Sets the function name of the shader entry point.
  VBASE_IMPEXP void SetEntryFunctionName (const char* sz) { m_sFunctionName = sz; }

  /// \brief
  ///   Returns whether this shader actually uses another shader (shared).
  VBASE_IMPEXP bool IsShaderUsingSharedSource (VTargetPlatform_e Platform) const { return ((m_iByteCodeRefBitmask & (1 << Platform)) != 0); }

  /// \brief
  ///   Sets the shared shader index that this shader should use.
  VBASE_IMPEXP void SetShaderUsesSharedSource (VTargetPlatform_e Platform, int iSharedShaderIndex);

  /// \brief
  ///   Removes all shared shader references in this shader.
  VBASE_IMPEXP void ResetAllSharedShaderReferences (void);

  /// \brief
  ///   Returns the index of the shared shader.
  VBASE_IMPEXP int GetSharedShaderIndex (VTargetPlatform_e Platform) const { return (m_iByteCodeLen[Platform]); }

  /// \brief
  ///   Returns the input stream mask that defines which input streams are used.
  VBASE_IMPEXP int GetInputStreamMask (VTargetPlatform_e Platform) const { return (m_iInputStreamMask[Platform]); }

  /// \brief
  ///   Returns the output stream mask that defines which output streams are used.
  VBASE_IMPEXP int GetOutputStreamMask (VTargetPlatform_e Platform) const { return (m_iOutputStreamMask[Platform]); }

  /// \brief
  ///   Returns the target shader model for this shader on the given platform.
  VBASE_IMPEXP VShaderModel_e GetTargetModel (VTargetPlatform_e Platform) const { return (m_eTargetModel[Platform]); }

  /// \brief
  ///   Updates the target-model for some platform and shader type. 
  VBASE_IMPEXP void UpdateTargetModelForShader (VTargetPlatform_e Platform, VShaderStage_e Shader);

  /// \brief
  ///   Sets the input stream mask that defines which input streams are used.
  VBASE_IMPEXP void SetInputStreamMask  (VTargetPlatform_e Platform, int iMask) { m_iInputStreamMask[Platform]  = iMask; }

  /// \brief
  ///   Sets the output stream mask that defines which output streams are used.
  VBASE_IMPEXP void SetOutputStreamMask (VTargetPlatform_e Platform, int iMask) { m_iOutputStreamMask[Platform] = iMask; }

  #ifdef _VR_DX11
    /// \brief
    ///   Returns whether a certain shader model is supported by the installed hardware
    static VBASE_IMPEXP bool IsShaderModelSupported (VShaderModel_e shaderModel);
  #endif

  #ifdef _VR_GCM

    /// \brief
    ///   Returns the number of bytes a cg type represents
    /// 
    /// \param CGType
    ///   The cg type the byte value is searched for
    /// 
    /// \return
    ///   uint32_t NumBytes : The number of bytes the cgtype has
    VBASE_IMPEXP uint32_t NumBytesFromCGType(CGtype CGType);
  #endif

  #ifdef _VR_GXM
    VBASE_IMPEXP void ExtractMappingTableAndShader (void* pByteCode, int iByteCodeSize, SceGxmProgram** ppShader, VGXMMappingTableEntry** ppMappingTable, int* piMappingTableEntries);
  #endif

  ///
  /// @}
  ///

  ///
  /// @name Shader Profile Helpers
  /// @{
  ///

public:

  /// \brief
  ///   Static helper function to convert a shader profile into a profile string used by DirectX.
  ///   E.g. "vs_3_0" for ShaderStage == VSS_VertexShader and Model == VSM_SM30.
  VBASE_IMPEXP static const char* GetShaderProfileName (VShaderStage_e ShaderStage, VShaderModel_e Model, VTargetPlatform_e Platform = TARGETPLATFORM_DX11);

  /// \brief
  ///   Static helper function to convert a DX profile string into an enum value describing the Shader Model.
  ///   E.g. VSM_SM30 for "vs_3_0".
  VBASE_IMPEXP static VShaderModel_e GetShaderModelFromProfileName (const char* szProfile);

  /// \brief
  ///   Returns the user-accessible range of the vertex shader constant table. Constants outside of this range are reserved by Vision.
  VBASE_IMPEXP static void GetVSUserConstantRange(VShaderModel_e model, int &iMin, int &iMax, bool bAllowOverwriteGlobalConst=false);

  /// \brief
  ///   Returns the user-accessible range of the pixel shader constant table. Constants outside of this range are reserved by Vision.
  VBASE_IMPEXP static void GetPSUserConstantRange(VShaderModel_e model, int &iMin, int &iMax);
  

  ///
  /// @}
  ///

  ///
  /// @name Owner
  /// @{
  ///

public:

  /// \brief
  ///   Accesses the shader pass resource that owns this source instance (e.g. vertex or pixel shader).
  VBASE_IMPEXP VShaderPassResource* GetOwnerShader() const {return m_pOwnerShader;}

  /// \brief
  ///   Accesses the shader library that owns this source instance (i.e. the parent library of GetOwnerShader).
  VBASE_IMPEXP VShaderEffectLib* GetOwnerEffectLib() const;


  ///
  /// @}
  ///


  ///
  /// @name Saving & Loading
  /// @{
  ///

public:

  /// \brief
  ///   Internal function for binary file data exchange (load and save). Used for loading and saving .vShaderBin files.
  VBASE_IMPEXP void ChunkFileExchange(VChunkFile &file, VShaderPassResource *pOwner, VTargetPlatform_e targetPlatform, bool bByteCodeOnly = false);

  ///
  /// @}
  ///


public:

#if !defined(_VISION_DOC)

  /// \brief
  ///   Internal class used to represent convenient access to the (potentially existing) bytecode header for compressed shaders
  class VCompressedShaderByteCodeHeader
  {
  public:

    explicit VCompressedShaderByteCodeHeader(unsigned int uiUncompressedSize, unsigned int uiCompressedSize)
    {
      m_cHeader[0] = 'V'; m_cHeader[1] = 'C'; m_cHeader[2] = 'B'; m_cHeader[3] = 'C';
      m_uiVersion = 1;
      m_uiUncompressedSize = uiUncompressedSize;
      m_uiCompressedSize = uiCompressedSize;
    }

    explicit VCompressedShaderByteCodeHeader(const void* pDataBlock)
    {
      VASSERT(pDataBlock != NULL);
      memcpy(this, pDataBlock, sizeof(*this));
      LittleEndianToNativeDWord(&m_uiVersion);
      LittleEndianToNativeDWord(&m_uiUncompressedSize);
      LittleEndianToNativeDWord(&m_uiCompressedSize);
    }

    inline bool IsValidHeader() const
    {
      return m_cHeader[0] == 'V' && m_cHeader[1] == 'C' && m_cHeader[2] == 'B' && m_cHeader[3] == 'C' && m_uiVersion <= 1;
    }

    inline void CopyTo(void* pDataBlock)
    {
      VASSERT(pDataBlock != NULL);
      memcpy(pDataBlock, this, sizeof(*this));
    }

    inline unsigned int GetUncompressedSize() const
    {
      return m_uiUncompressedSize;
    }

    inline unsigned int GetCompressedSize() const
    {
      return m_uiCompressedSize;
    }

  private:

    char m_cHeader[4];
    unsigned int m_uiVersion;
    unsigned int m_uiUncompressedSize;
    unsigned int m_uiCompressedSize;
  };

  V_COMPILE_ASSERT(sizeof(VCompressedShaderByteCodeHeader) == 16);

#endif

private:
  #ifdef WIN32
  friend bool ParseFXLib (char *text, int numChars, VShaderCompileFlags_e flags, VShaderEffectLib &fxlib);
  #endif

  VShaderPassResource *m_pOwnerShader;

  VString m_sFunctionName;    ///< Name of the function entry point in the global source code block
  VString m_sCustomSource;    ///< Can be empty; in that case the global source from the library is used

  VShaderModel_e m_eMinModel;    ///< Shader model to start compiling with
  VShaderModel_e m_eMinModelDX11; ///< Shader model to start compiling with (for DX11 targets)

  unsigned int m_uiNumActiveSamplers;                  ///< Used for sampler and texture states

  int m_iByteCodeRefBitmask;  ///< For every platform there is one bit (1<<platform) that specifies whether m_iByteCodeLen[platform] is interpreted as a shader index in the same lib of the shader that has the same bytecode to share.
  int m_iInputStreamMask[TARGETPLATFORM_COUNT];     ///< Bitflags of the used input streams
  int m_iOutputStreamMask[TARGETPLATFORM_COUNT];    ///< Bitflags of the used output streams

  int m_iByteCodeLen[TARGETPLATFORM_COUNT];      ///< Length of the bytecode
  mutable VShaderConstantTablePtr m_spConstantTable[TARGETPLATFORM_COUNT];     ///< This table gets filled out by the shader compiling.
  VShaderModel_e m_eTargetModel[TARGETPLATFORM_COUNT]; ///< Shader model actually used in the engine

  // arrays of data
  VString* m_pCustomTextureFilename;                  ///< The resource also keeps the filename list
  VString* m_pSamplerName;                            ///< The resource also keeps the sampler name list

  VStateGroupSampler* m_pSamplers;
  VStateGroupTexture* m_pTextures;

  int* m_pByteCode[TARGETPLATFORM_COUNT];        ///< Compiled byte code (optional) for the shader model specified by m_eTargetModel. Supported for more than one target in parallel.

  friend class VShaderPassResource;

private:
  void ConstructorInit (void);
  void DestructorClear (void);

  void CreateVertexShader_Internal (void** out_ResultPtr, IVLog *pLog);
  void CreatePixelShader_Internal (void** out_ResultPtr, IVLog *pLog);
  void CreateGeometryShader_Internal (void** out_ResultPtr, IVLog *pLog);
  void CreateHullShader_Internal (void** out_ResultPtr, IVLog *pLog);
  void CreateDomainShader_Internal (void** out_ResultPtr, IVLog *pLog);
  void CreateComputeShader_Internal (void** out_ResultPtr, IVLog *pLog);

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
