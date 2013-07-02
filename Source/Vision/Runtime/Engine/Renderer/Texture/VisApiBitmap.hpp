/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiBitmap.hpp

#ifndef DEFINE_VISAPIBITMAP
#define DEFINE_VISAPIBITMAP

#include <Vision/Runtime/Engine/System/Resource/VisApiResource.hpp>


// bitmap flags for loading bitmaps (VisBitmap_cl class)
#define BITMAPFLAG_NONE               0x00000000
#define BITMAPFLAG_TEXTURE1D          0x00000001
#define BITMAPFLAG_SCALEPOWEROF2      0x00000002
#define BITMAPFLAG_ONLOADFAIL_CREATEBLACK 0x00010000
#define BITMAPFLAG_RELEVANT_FINDFLAGS     0x0000ffff

// flags for bitmap lookup
#define BITMAPLOOKUPFLAG_NONE         0x00000000
#define BITMAPLOOKUPFLAG_REPEAT       0x00000001
#define BITMAPLOOKUPFLAG_REPEAT_X     0x00000002
#define BITMAPLOOKUPFLAG_REPEAT_Y     0x00000004

class VisBitmap_cl;
typedef VSmartPtr<VisBitmap_cl>       VisBitmapPtr;
class VisBitmapManager_cl;


/// \brief
///   Derived proxy class for loading a VisBitmap_cl resource. Used internally
class VBitmapSerializationProxy : public IVSerializationProxy
{
public:
  inline VBitmapSerializationProxy(VisBitmap_cl *pBitmap) {VASSERT(pBitmap);m_pBitmap=pBitmap;}
  VOVERRIDE IVSerializationProxyCreator *GetInstance();

protected:
  // serialization
  inline VBitmapSerializationProxy() {m_pBitmap=NULL;}
  V_DECLARE_SERIAL_DLLEXP( VBitmapSerializationProxy, VISION_APIDATA )
    VOVERRIDE void Serialize( VArchive &ar );
private:
  VisBitmap_cl *m_pBitmap;
};


/// \brief
///   Resource class for loading a bitmap into system memory without uploading to the GPU.
/// 
/// This class provides functionality to load textures and access its color buffer.
/// 
/// The texel colors are always unpacked to 32bit color values (VColorRef class), mipmaps and
/// animated textures are ignored.
/// 
/// Bitmaps are useful for doing color lookups in software. This is used for instance for animated
/// light curves.
/// 
/// Loaded bitmaps are handled by a dedicated resource manager. To iterate through all loaded
/// bitmap resources, the static  VisElementManager_cl implementation can be used. Alternatively
/// the bitmap's resource manager can be accessed via
/// Vision::ResourceSystem.GetResourceManagerByName(VIS_RESOURCEMANAGER_BITMAPS);
class VisBitmap_cl : public VManagedResource, public VisRMElementManager_cl<VisBitmap_cl *>
{
public:

  ///
  /// @name Constructor/destructor
  /// @{
  ///

  /// \brief
  ///   Constructor of a bitmap that loads a bitmap from file
  /// 
  /// The constructor will add the bitmap to the global bitmap resource manager.
  /// 
  /// To access a bitmap that is already loaded by the bitmap manager, use the static
  /// VisBitmap_cl::LoadBitmapFromFile function.
  /// 
  /// \param szFilename
  ///   Filename of the bitmap. If no file extension is added, the .tex extension will be appended
  /// 
  /// \param iFlags
  ///   flags for loading the bitmap. The following flags are currently supported:
  ///   \li BITMAPFLAG_NONE : No flags (default)
  /// 
  ///   \li BITMAPFLAG_TEXTURE1D : Load the bitmap as a 1D texture which means only the first texel
  ///     row is loaded. Useful  when the texture is used as a 1D color lookup.
  /// 
  ///   \li BITMAPFLAG_SCALEPOWEROF2 : If specified, the texture is scaled to the nearest power of
  ///     two size. Usually this is not necessary, since bitmaps are not used by the hardware and
  ///     there is no power-of-2 restriction for software lookups.
  /// 
  ///   \li BITMAPFLAG_ONLOADFAIL_CREATEBLACK : If specified, a black dummy texture will be created
  ///     if loading fails.
  VISION_APIFUNC VisBitmap_cl(const char *szFilename, int iFlags=BITMAPFLAG_NONE);

  /// \brief
  ///   Constructor of a bitmap that creates a memory bitmap
  /// 
  /// of specified dimension.
  /// 
  /// This constructor will reserve an array of iSizeX*iSizeY elements of type VColorRef.
  /// 
  /// This array can be accessed via GetDataPtr() and modified.
  /// 
  /// \param szInternalName
  ///   Internal name of the bitmap in the resource manager.  Can be used for finding a bitmap in
  ///   the bitmap manager.
  /// 
  /// \param iSizeX
  ///   Width of the bitmap in pixels
  /// 
  /// \param iSizeY
  ///   Height of the bitmap in pixels
  VISION_APIFUNC VisBitmap_cl(const char *szInternalName, int iSizeX, int iSizeY);

  /// \brief
  ///   Destructor of a bitmap. Frees all the allocated memory.
  VISION_APIFUNC virtual ~VisBitmap_cl();



  ///
  /// @}
  ///

  ///
  /// @name Load, Save and Free Functions
  /// @{
  ///


  /// \brief
  ///   Free the bitmap data
  VISION_APIFUNC void FreeBitmap();

  /// \brief
  ///   Load the bitmap from file.
  /// 
  /// To re-use bitmap resources that are already loaded, use the static LoadBitmapFromFile/FindBitmap
  /// function.
  /// 
  /// \param szFilename
  ///   filename of the bitmap file (with file extension)
  /// 
  /// \param iFlags
  ///   Additional flags for loading the file. See constructor for supported flags.
  /// 
  /// \return
  ///   BOOL bResult: TRUE, if the bitmap has been successfully loaded
  VISION_APIFUNC BOOL LoadFromFile(const char *szFilename, int iFlags=BITMAPFLAG_NONE);

  /// \brief
  ///   Saves the bitmap to file
  /// 
  /// Saves the bitmap as 32 bit texture to file (RGB + alpha channel).
  /// 
  /// If a filename is passed the resource receives the new filename.
  /// 
  /// The file type (bmp/tga/..) is determined by the file extension.
  /// 
  /// \param szFilename
  ///   filename of the destination file with extension. If NULL, the resource's own filename is
  ///   used.
  /// 
  /// \return
  ///   BOOL bResult: Indicates whether the bitmap has been saved successfully
  /// 
  /// \example
  ///   \code
  ///   BOOL bResult = pBitmap->SaveToFile();
  ///   \endcode
  VISION_APIFUNC BOOL SaveToFile(const char *szFilename = NULL);

  /// \brief
  ///   Creates a 1x1 dummy tetxure with specified color
  /// 
  /// \param iColor
  ///   color of the texture
  VISION_APIFUNC void CreateDummyTexture(VColorRef iColor);


  ///
  /// @}
  ///

  ///
  /// @name Access Bitpmap Properties Functions
  /// @{
  ///

  /// \brief
  ///   Clears the bitmap using the passed color value
  VISION_APIFUNC void Clear(VColorRef iClearColor=V_RGBA_BLACK);


  /// \brief
  ///   Get the width of the bitmap texture
  /// 
  /// If the BITMAPFLAG_SCALEPOWEROF2 flag has been specified for loading, the returned value is a
  /// power-of-two.
  /// 
  /// \return
  ///   int iWidth: The width of the texture in pixels
  inline int GetWidth() const {return m_iSizeX;}

  /// \brief
  ///   Get the height of the bitmap texture
  /// 
  /// If the BITMAPFLAG_SCALEPOWEROF2 flag has been specified for loading, the returned value is a
  /// power-of-two.
  /// 
  /// If the BITMAPFLAG_TEXTURE1D flag has been specified for loading, the returned value is 1.
  /// 
  /// \return
  ///   int iHeight: The height of the texture in pixels
  inline int GetHeight() const {return m_iSizeY;}

  /// \brief
  ///   Get the pitch in bytes of the bitmap data
  /// 
  /// The pitch value should be used to access single rows in the texture raw data.
  /// 
  /// Currently, this is always GetWidth()*sizeof(VColorRef)
  /// 
  /// \return
  ///   int iBytes: The pitch of the texture data
  inline int GetPitch() const {return m_iSizeX*4;}

  /// \brief
  ///   Get the loading flags of the bitmap
  /// 
  /// \return
  ///   int iFlags: The flags used for loading the bitmap (see LoadBitmapFromFile)
  inline int GetFlags() const {return m_iFlags;}

  /// \brief
  ///   Returns the swizzle mask of the original texture resource. E.g. 'RGBA', 'BGR1' etc.
  ///   Use this to determine which channels are contained in the bitmap.
  ///
  /// If you want to know if the bitmap contains valid data in the red channel, search
  /// for an 'R' in the swizzle mask. The same applies to the channels 'G', 'B' and 'A'.
  ///
  /// \return
  ///   Pointer to the swizzle mask string.
  inline const char* GetSwizzleMask() const {return m_szSwizzle;}

  /// \brief
  ///   Returns a pointer to the bitmap raw data
  /// 
  /// This system memory pointer can be used to modify the bitmap data.
  /// 
  /// The pointer remains valid until the bitmap is unloaded by the resource manager.
  /// 
  /// \return
  ///   VColorRef *pData: A pointer to the bitmap data 
  /// 
  /// \note
  ///   Each texel is represented by a 32 bit VColorRef value, even when the texture was only RGB
  ///   or compressed DXT.
  /// 
  /// \example
  ///   \code
  ///   VColorRef *pData = pMyBitmap->GetDataPtr();
  ///   VASSERT(pData);
  ///   \endcode
  inline VColorRef *GetDataPtr() {EnsureLoaded();return m_pData;}

  /// \brief
  ///   Returns a pointer to the bitmap raw data at specified texel position
  /// 
  /// \param iPosX
  ///   horizontal position in pixels
  /// 
  /// \param iPosY
  ///   vertical position in pixels
  /// 
  /// \param iLookupFlags
  ///   Specifies the lookup flags. The lookup flag constants can be logically combined.  The
  ///   following flags are supported: BITMAPLOOKUPFLAG_REPEAT_X : if specified, the source
  ///   position will wrap in x-direction,  otherwise the source position will be clamped to the
  ///   valid range [0..width-1]. BITMAPLOOKUPFLAG_REPEAT_Y : if specified, the source position
  ///   will wrap in y-direction,  otherwise the source position will be clamped to the valid range
  ///   [0..height-1].
  /// 
  /// \return
  ///   VColorRef *pData: A pointer to the bitmap data starting at specified position
  VISION_APIFUNC VColorRef *GetDataPtr(int iPosX, int iPosY, int iLookupFlags=BITMAPLOOKUPFLAG_NONE);

  /// \brief
  ///   Returns a pointer to the bitmap raw data at as const data. In contrast to GetDataPtr, this
  ///   function does not call EnsureLoaded() 
  inline VColorRef *GetConstDataPtr() const {return m_pData;}


  ///
  /// @}
  ///

  ///
  /// @name Texel Lookup Functions
  /// @{
  ///

  /// \brief
  ///   Performs a (non-interpolated) texel color lookup at specified source coordinates
  /// 
  /// \param iPosX
  ///   source u texel position (e.g. [0..width-1])
  /// 
  /// \param iPosY
  ///   source v texel position (e.g. [0..height-1])
  /// 
  /// \param iLookupFlags
  ///   Specify the lookup flags. The lookup flag constants can be logically combined.  The
  ///   following flags are supported: BITMAPLOOKUPFLAG_REPEAT_X : if specified, the source
  ///   position will wrap in x-direction,  otherwise the source position will be clamped to the
  ///   valid range [0..width-1]. BITMAPLOOKUPFLAG_REPEAT_Y : if specified, the source position
  ///   will wrap in y-direction,  otherwise the source position will be clamped to the valid range
  ///   [0..height-1].
  /// 
  /// \return
  ///   VColorRef: The RGBA color value at specified position (non-interpolated).
  VISION_APIFUNC VColorRef LookupTexelColor(int iPosX, int iPosY, int iLookupFlags=BITMAPLOOKUPFLAG_NONE);


  /// \brief
  ///   Performs a bilinearly interpolated texel color lookup at specified source coordinates
  /// 
  /// \param destCol
  ///   reference to destination color. This vector will receive the interpolated color at the
  ///   texel position. The range for each portion r,g,b and a is still [0..255.f]
  /// 
  /// \param u
  ///   The source u coordinate. In this version the u value ranges from 0.0f to 1.0f
  /// 
  /// \param v
  ///   The source v coordinate. In this version the v value ranges from 0.0f to 1.0f
  /// 
  /// \param iLookupFlags
  ///   Specify the lookup flags. The lookup flag constants can be logically combined.  The
  ///   following flags are supported: BITMAPLOOKUPFLAG_REPEAT_X : if specified, the source
  ///   position will wrap in x-direction,  otherwise the source position will be clamped to the
  ///   valid range [0..width-1]. BITMAPLOOKUPFLAG_REPEAT_Y : if specified, the source position
  ///   will wrap in y-direction,  otherwise the source position will be clamped to the valid range
  ///   [0..height-1].
  VISION_APIFUNC void LookupTexelColor(hkvVec4 &destCol, float u, float v, int iLookupFlags=BITMAPLOOKUPFLAG_NONE);

  /// \brief
  ///   Maps incoming float-based uv-coordinates from [0..1] range to integer-based bitmap coordinates. No clamping takes place.
  inline void MapUVToTexel(float u, float v, int &iX, int &iY) const
  {
    iX = (int)(u*this->m_vUVToSize.x);
    iY = (int)(v*this->m_vUVToSize.y);
  }

  ///
  /// @}
  ///

  ///
  /// @name Static Bitmap Manager Functions
  /// @{
  ///

  /// \brief
  ///   Static function to Find a bitmap with specified filename and flags in the global resource
  ///   manager
  /// 
  /// If the bitmap with specified properties cannot be found in the resource manager, this
  /// function will return NULL.
  /// 
  /// Both filename and flags have to match to find a bitmap in the resource manager.
  /// 
  /// \param szFilename
  ///   filename of the texture, If no file extension is specified, a .tex  extension is appended.
  /// 
  /// \param iFlags
  ///   flags for loading the bitmap. See constructor for valid flag constants.
  /// 
  /// \return
  ///   VisBitmap_cl *pBitmap: A pointer to the bitmap instance with specified properties, or NULL,
  ///   if a bitmap cannot be found
  VISION_APIFUNC static VisBitmap_cl* FindBitmap(const char *szFilename, int iFlags=BITMAPFLAG_NONE);


  /// \brief
  ///   Static function to load a bitmap from file (if not already in resource manager).
  /// 
  /// For re-using loaded bitmaps, this function calls FindBitmap first.
  /// 
  /// If a new bitmap is loaded, it will be added to the resource manager as well.
  /// 
  /// \param szFilename
  ///   filename of the texture, If no file extension is specified, a .tex  extension is appended.
  /// 
  /// \param iFlags
  ///   flags for loading the bitmap. See constructor for valid flag constants.
  /// 
  /// \return
  ///   VisBitmap_cl *pBitmap: A pointer to the bitmap instance with specified properties. If the
  ///   texture couldn't be loaded (e.g. file is missing) the IsLoaded member function of the
  ///   returned instance will return FALSE.
  /// 
  /// \note
  ///   The resource manager for bitmaps will be purged when the map is unloaded.
  /// 
  /// \example
  ///   \code
  ///   VisBitmap_cl* pMyBitmap = VisBitmap_cl::LoadBitmapFromFile("Textures\\lookup.bmp");
  ///   \endcode
  VISION_APIFUNC static VisBitmap_cl* LoadBitmapFromFile(const char *szFilename, int iFlags=BITMAPFLAG_NONE);


  /// \brief
  ///   Access the global manager that holds all bitmap instances
  VISION_APIFUNC static VisBitmapManager_cl& GetGlobalManager();


  ///
  /// @}
  ///

  ///
  /// @name Serialization Functions
  /// @{
  ///


  /// \brief
  ///   Static helper function to serialize a bitmap reference from archive or to archive
  /// 
  /// This function saves the filename reference rather than saving the bitmap raw data itself.
  /// 
  /// If the archive is in saving mode, it uses the pSource bitmap pointer to serialize to the
  /// archive.
  /// 
  /// In loading mode, it returns an instance to the de-serialized bitmap. The passed pSource
  /// pointer is ignored.
  /// 
  /// This function is useful to de-serialize and check if a bitmap with same properties is already
  /// loaded.
  /// 
  /// An alternative way to serialize a bitmap file reference is using the proxy mechanism. See
  /// IVSerializationProxy class.
  /// 
  /// \param ar
  ///   The archive to read from or write to.
  /// 
  /// \param pSource
  ///   Bitmap to serialize (writing mode only)
  /// 
  /// \return
  ///   VisBitmap_cl *pBitmap: Deserialized instance of a bitmap
  VISION_APIFUNC static VisBitmap_cl* DoArchiveExchange(VArchive &ar, VisBitmap_cl* pSource=NULL);

  V_DECLARE_SERIALX( VisBitmap_cl,  VISION_APIFUNC )

    /// \brief
    ///   Serializes the bitmap to/from the specified archive
    void SerializeX( VArchive &ar );

protected:
  VisBitmap_cl(); ///< serialisation

  // overridden resource functions
  VISION_APIFUNC VOVERRIDE BOOL Reload();
  VISION_APIFUNC VOVERRIDE BOOL Unload();
  VISION_APIFUNC void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU);

  /// \brief
  ///   Implements the IVSerializationProxyCreator interface.
  VISION_APIFUNC VOVERRIDE IVSerializationProxy *CreateProxy() {return new VBitmapSerializationProxy(this);}

  /// \brief
  ///   Calculates the required memory size of the bitmap's raw data
  inline int CalculateInternalSize() {return m_iSizeX*m_iSizeY*sizeof(VColorRef);}

  inline void OnSizeChanged()
  {
    m_vUVToSize.set((float)m_iSizeX,(float)m_iSizeY);
  }

  /// \brief
  ///   Initializes an empty bitmap
  void Init();

  /// \brief
  ///   Allocates the raw data memory, based on the width/height of the bitmap
  void AllocateData();

  short m_iSizeX;           ///< Width of the bitmap
  short m_iSizeY;           ///< Height of the bitmap
  int m_iFlags;             ///< Flags set for this bitmap. See constructor for list of fags
  VColorRef *m_pData;       ///< Pointer to the bitmap raw data
  hkvVec2 m_vUVToSize;      ///< maps the float [0..1] range to absolute index (int-based)
  char m_szSwizzle[5];      ///< The 4 component swizzle mask of the texture, e.g. RGBA, BGR1 etc.

  ///
  /// @}
  ///
};


/// \brief
///   Resource manager class for instances of VisBitmap_cl resources.
/// 
/// The engine has one global instance of a bitmap manager that can be accessed via
/// Vision::ResourceSystem.GetResourceManagerByName(VIS_RESOURCEMANAGER_BITMAPS).
/// 
/// The memory footprint of this manager can be observed in the resource viewer tool (Bitmap tab).
class VisBitmapManager_cl : public VisResourceManager_cl
{
public:

  /// \brief
  ///   Constructor, for internal use
  VisBitmapManager_cl(const char *szManagerName,int iFlags, int iMemLimit) : VisResourceManager_cl(szManagerName,iFlags,iMemLimit) {}

  /// \brief
  ///   Loads a bitmap file or returns an existing one.
  /// 
  /// This is a convenience function that casts the result of LoadResource to VisBitmap_cl*.
  /// 
  /// See also static VisBitmap_cl::LoadBitmapFromFile function.
  /// 
  /// \param szFilename
  ///   Bitmap file to load (with file extension)
  /// 
  /// \return
  ///   VisBitmap_cl *pBitmap: Loaded bitmap instance (or NULL, when failed)
  inline VisBitmap_cl *LoadBitmapFile(const char *szFilename) 
  {
    return (VisBitmap_cl*)LoadResource(szFilename);
  }

protected:
  // overridden resource manager function
  VOVERRIDE VManagedResource *LoadResource(const char *szFilename) {return VisBitmap_cl::LoadBitmapFromFile(szFilename);}

  /// \brief
  ///   Overridden resource manager function
  VISION_APIFUNC VOVERRIDE VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo);
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
