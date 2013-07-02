/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTextureManager.hpp

#ifndef VTEXTUREMANAGER_HPP_INCLUDED
#define VTEXTUREMANAGER_HPP_INCLUDED

#include <Vision/Runtime/Base/System/VCallbacks.hpp>
#include <Vision/Runtime/Base/Graphics/Textures/VTextureObject.hpp>


/// \brief
///   Modes for globally handling SRGB textures in Vision.
enum VSRGBMode
{
  V_SRGB_DISABLE = 0,        ///< Disable SRGB in the Engine; never load textures as SRGB
  V_SRGB_ASSUME_FOR_DIFFUSE, ///< Enable SRGB in the Engine; assume all diffuse textures are SRGB
  V_SRGB_FROM_METADATA       ///< Enable SRGB in the Engine; load textures as SRGB or linear depending on their asset metadata
};


class VTextureManager;

/// \brief
///   Derived callback data object class that is used for the OnNewTextureLoading callback function
/// 
/// The callback gets triggered right before the texture is loaded, so it can e.g. be used to 
/// modify the m_iLoadingFlags member. 
/// For instance, the VTM_FLAG_NO_DOWNSCALE bit flag can be added or removed.
/// 
/// If you want to replace the texture data, there is a dedicated interface that should used instead (IVTextureFormatProvider).
/// The IVTextureFormatProvider also allows support for custom texture formats to be added in vForge.
/// 
/// \sa class IVisCallbackHandler_cl
/// \sa VisCallbackManager_cl::OnNewTextureLoading
class VisTextureLoadingDataObject_cl : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor
  VBASE_IMPEXP VisTextureLoadingDataObject_cl(VTextureManager *pManager, VCallback *pSender, const char *szTextureFilename, int iFlags);

  const char *m_szTextureFilename;      ///< Filename of the texture as passed to the file manager
  int m_iLoadingFlags;                  ///< Current texture loading flags (combination of constants of type VTM_FLAG_XYZ)
  VTextureObject *m_pNewTexture;        ///< Can be filled by the listener in custom texture provider implementations. If left to NULL, the engine loads the texture
  VTextureManager *m_pTextureManager;   ///< Pointer to the texture manager that loads this texture (read only)
  bool m_bProcessed;                    ///< Set this value to true if your callback changed the m_pNewTexture member.
};


#define MAX_TEXTURE_PROVIDER 4

/// \brief
///   Interface to add support for custom texture formats to the engine and to vForge.
///
/// By installing a custom format provider, plugins can hook into the texture loading pipeline and load custom texture file formats.
/// The engine tries to load textures via the installed providers before it loads textures through the default path.
/// Installed providers not only load custom textures inside the runtime, they also show up in the file browser dialog inside vForge.
/// Providers can be installed via VTextureManager::RegisterFormatProvider (accessible through Vision::TextureManager.GetManager().RegisterFormatProvider(...)).
/// A good place to do that is at plugin initialization time (function OnInitEnginePlugin).
class IVTextureFormatProvider
{
public:

  virtual ~IVTextureFormatProvider()
  {
  }

  /// \brief
  ///   Returns a list of supported file extensions for this provider. 
  ///
  /// \param iListCount
  ///   This reference must receive the number of list entries in the returned array (typically this is 1)
  ///
  /// \return 
  ///   List of strings with iListCount entries. Each string represents a file extension (without '.' and case-insensitive) that can be loaded by this provider. 
  virtual const char** GetSupportedFileExtensions(int &iListCount) = 0;

  /// \brief
  ///   Creates a texture instance for the passed filename
  ///
  /// \param szFilename
  ///   Filename of the texture to load. The filename has an extension that is returned by GetSupportedFileExtensions.
  ///
  /// \param iFlags
  ///   Loading flags. Passed through from VTextureManager::Load2DTextureFromFile. It is a reference so it can be changed inside the function
  ///
  /// \return 
  ///   A new instance of VTextureObject. This can be of a class derived from it for custom VTextureObject::Reload/VTextureObject::Unload implementations. 
  ///
  virtual VTextureObject* CreateTexture(const char *szFilename, int &iFlags) = 0;

  /// \brief
  ///   Optional: Create a system memory preview of the passed texture filename. This function is only used inside the editor, so the relevant code can be ifdef'd with WIN32.
  ///
  /// \param szFilename
  ///   Filename to load
  ///
  /// \param destImg
  ///   Reference that should receive the image at full size.
  ///
  /// \param bAlphaMap
  ///   if true, the alpha channel of this texture is to be loaded
  ///
  /// \return 
  ///   bool bResult: true if the image has been generated successfully
  ///
  virtual bool CreatePreview(const char *szFilename, Image_cl &destImg, bool bAlphaMap)
  {
    return false;
  }

  /// \brief
  ///   Returns a list of subtexture names for texture files which consist of multiple separate textures
  ///
  /// If the texture format provider supports container formats where a single texture file contains multiple texture images,
  /// this method should be implemented to allow these subtextures to have separate names.
  ///
  /// \param szFilename
  ///   Filename to load
  ///
  /// \param destNames
  ///   List of names for the subtextures
  ///
  /// \param destNiceNames
  ///   List of nice names for the subtextures (e.g. as more descriptive nice names in UIs)
  ///
  /// \return 
  ///   bool bResult: true if the texture file contains subtextures, otherwise false.
  ///
  virtual bool GetSubTextureNames(const char *szFilename, VStrList &destNames, VStrList &destNiceNames)
  {
    return false;
  }
};


/// \brief
///   Texture resource manager base class that implements the IVTextureLoader interface
class VTextureManager : public VResourceManager, public IVTextureLoader
{
public:

  /// \brief
  ///   Constructor
  VBASE_IMPEXP VTextureManager(const char *szName="Textures");

  ///
  /// @name Resource manager overrides
  /// @{
  ///

  /// \brief
  ///   Overridden function to create a texture resource from the specified file
  VBASE_IMPEXP virtual VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo);

  ///
  /// @}
  ///

  ///
  /// @name IVTextureLoader overrides
  /// @{
  ///

  /// \brief
  ///   Overridden function to load a 2D texture from file
  VBASE_IMPEXP virtual VTextureObject* Load2DTextureFromFile(const char *szFilename, int iFlags=VTM_DEFAULT_FLAGS);

  /// \brief
  ///   Overridden function to load a 3D volume texture from file
  VBASE_IMPEXP virtual VTextureObject* Load3DTextureFromFile(const char *szFilename, int iFlags=VTM_DEFAULT_FLAGS);
  
  #if defined(SUPPORTS_2D_TEXTURE_ARRAYS)
    /// \brief
    ///   Overridden function to load a 2D texture array from file
    VBASE_IMPEXP virtual VTextureObject* Load2DArrayTextureFromFile(const char *szFilename, int iFlags=VTM_DEFAULT_FLAGS);
  #endif

  /// \brief
  ///   Overridden function to load a cubemap texture from file
  VBASE_IMPEXP virtual VTextureObject* LoadCubemapTextureFromFile(const char *szFilename, int iFlags=VTM_DEFAULT_FLAGS);  

  /// \brief
  ///   Overridden function to create a cubemap render target proxy with the specified key
  VBASE_IMPEXP virtual VTextureObject* GetRenderableCubemap(const char *szKey, bool bForceCreate=true)
  {
    return NULL;
  }

  ///
  /// @}
  ///

  ///
  /// @name Custom Texture Format Providers
  /// @{
  ///

  /// \brief
  ///   Registers a custom texture format provider. See IVTextureFormatProvider. IVisPlugin_cl::OnInitEnginePlugin is a good place to call this function.
  inline void RegisterFormatProvider(IVTextureFormatProvider *pProvider);

  /// \brief
  ///   De-registers a custom texture format provider that has been registered with RegisterFormatProvider. See IVTextureFormatProvider. IVisPlugin_cl::OnDeInitEnginePlugin is a good place to call this function.
  inline void DeRegisterFormatProvider(IVTextureFormatProvider *pProvider);

  /// \brief
  ///   Returns the number of installed custom texture format providers (0 by default)
  inline int GetFormatProviderCount() const;

  /// \brief
  ///   Returns the provider instance with specified index. iIndex must be in valid range [0..GetFormatProviderCount()-1]
  inline IVTextureFormatProvider& GetFormatProvider(int iIndex) const;

  /// \brief
  ///   Helper function to return the first matching installed provider (or NULL) for the passed file extension (extension string without '.'). 
  VBASE_IMPEXP IVTextureFormatProvider *GetProviderForExtension(const char *szExt);
  
  ///
  /// @}
  ///

  ///
  /// @name Callbacks and Logging
  /// @{
  ///

  /// \brief
  ///   Implementations can respond to texture not found warnings
  VBASE_IMPEXP virtual void TriggerFileNotFound(const char *szFilename) {}

  /// \brief
  ///   Implementations can respond to texture loading errors
  VBASE_IMPEXP virtual void TriggerLoadingError(const char *szFilename, const char *szErrorMsg) {}

  /// \brief
  ///   Implementations can provide a log for error/warning/info output
  VBASE_IMPEXP virtual IVLog* GetLoadingLog(VTextureObject *pLoadingRes) {return NULL;}

  /// \brief
  ///   Implementations must trigger the texture loading callback after loading a texture
  VBASE_IMPEXP virtual void TriggerLoadingCallback(VisTextureLoadingDataObject_cl *pData) {}

  ///
  /// @}
  ///

  ///
  /// @name SRGB Handling
  /// @{
  ///

  /// \brief
  ///   Sets the mode for handling textures with regard to sRGB
  ///
  /// \param eMode
  ///   the new handling mode
  VBASE_IMPEXP void SetSRGBMode(VSRGBMode eMode);

  /// \brief
  ///   Returns the mode in which textures are handled with regard to sRGB
  /// \return
  ///   the current handling mode
  VBASE_IMPEXP VSRGBMode GetSRGBMode() const { return m_eSRGBMode; }
  
  ///
  /// @}
  ///

  ///
  /// @name Cleanup
  /// @{
  ///

  /// \brief
  ///   Implementations must unbind the passed texture so the texture can be destroyed
  VBASE_IMPEXP virtual void UnbindTexture(VTextureObject *pTexture) {}

  ///
  /// @}
  ///

#ifdef _VR_GLES2

  /// @name Resource access
  /// @{

  /// \brief
  ///   Gets a replacement filename (e.g. for compressed textures)
  VBASE_IMPEXP virtual const char* GetStreamingReplacementFilename(VResourceSnapshotEntry &resourceDesc, const char* szResolvedFilename, char* szBuffer);

  /// @}

#endif

  /// @name Device information
  /// @{

  /// \brief
  ///   Returns the maximum texture size available on the current device / platform
  VBASE_IMPEXP int GetMaxTextureSize() const;

  /// \brief
  ///   Sets the maximum texture size available on the current device / platform (used internally during initialization for platforms
  ///   where this value can vary)
  VBASE_IMPEXP void SetMaxTextureSize(int iMaxTextureSize);

  /// @}


  ///
  /// @name Global access
  /// @{
  ///

  /// \brief
  ///   Sets a global manager so the base library can access it
  VBASE_IMPEXP static void SetGlobalManager(VTextureManager *pManager);

  /// \brief
  ///   Returns a reference to the global instance
  VBASE_IMPEXP static VTextureManager &GlobalManager();

  static VTextureManager* g_pGlobalManager; ///< set by the engine

  ///
  /// @}
  ///

#ifndef _VISION_DOC
  // DEPRECATED functionality:
  static int GetTexture2DFlags(char a, char b, char c, char d, char e, char f) {return 0;}
  static int GetCubemapFlags(char a, char b) {return 0;}
#endif

  ///
  /// @name Internal methods
  /// @{
  ///

  /// \brief
  ///   Internal
  VBASE_IMPEXP void *GetGlobalScratchBuffer(int iRequestedSize);

  /// \brief
  ///   Internal
  VBASE_IMPEXP void ResetGlobalScratchBuffer();

  ///
  /// @}
  ///

  void *m_pTextureScratchBuffer;
  int m_iTextureScratchBufferSize;

  const char *m_pszPathPrefix;

  int m_iDownScale;
  bool m_bForceUseRenderableCubemaps;
  bool m_bDX9UseDefaultPool;
  VSRGBMode m_eSRGBMode;

  int m_iProviderCount;
  IVTextureFormatProvider* m_pProvider[MAX_TEXTURE_PROVIDER];

protected:

  int m_iMaxTextureSize;
};


inline void VTextureManager::RegisterFormatProvider(IVTextureFormatProvider *pProvider)
{
  VASSERT(pProvider);
  DeRegisterFormatProvider(pProvider);
  VASSERT_MSG(m_iProviderCount<MAX_TEXTURE_PROVIDER,"Maximum number of possible format providers exceeded");
  m_pProvider[m_iProviderCount++] = pProvider;
}

inline void VTextureManager::DeRegisterFormatProvider(IVTextureFormatProvider *pProvider)
{
  VASSERT(pProvider);
  int iCount = m_iProviderCount;
  m_iProviderCount = 0;
  for (int i=0;i<iCount;i++)
    if (m_pProvider[i]!=pProvider)
      m_pProvider[m_iProviderCount++] = m_pProvider[i];
}

inline int VTextureManager::GetFormatProviderCount() const
{
  return m_iProviderCount;
}

inline IVTextureFormatProvider& VTextureManager::GetFormatProvider(int iIndex) const
{
  VASSERT(iIndex>=0 && iIndex<m_iProviderCount);
  return *m_pProvider[iIndex];
}


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
