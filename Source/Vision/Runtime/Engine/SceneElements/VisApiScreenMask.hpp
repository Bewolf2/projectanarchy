/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiScreenMask.hpp

#ifndef DEFINE_VISAPISCREENMASK
#define DEFINE_VISAPISCREENMASK

#include <Vision/Runtime/Engine/Renderer/Texture/VisApiTextureAnim.hpp>


class VisScreenMask_cl;
typedef VSmartPtr<VisScreenMask_cl> VisScreenMaskPtr;
class VisScreenMaskCollection_cl;

/// \brief
///   Screen mask class for 2-dimensional screen overlays
/// 
/// Each instance of the VisScreenMask class represents one screen overlay with bitmap data and
/// rendering properties.
/// 
/// The bitmap data of screen masks can both be loaded from a texture file or from the memory.
/// Additionally you can set the transparency, position, z-depth and other settings of the screen
/// mask class for each instance.
/// 
/// Rendering overlays with screen masks requires an object per overlay. The engine also provides a
/// straightforward way to render 2D overlays in immediate mode. See
/// VisRenderLoopHelper_cl::BeginOverlayRendering/EndOverlayRendering functions or refer to the GUI
/// samples.
/// 
/// \note
///   All unreferenced screenmask objects are purged when creating a new world. Thus screenmask
///   objects with no external (smart pointer) reference are not valid after LoadWorld anymore.
/// 
/// \note
///   However, screenmasks can be rendered inside VisionApp_cl::OnLoadSceneStatus().
/// 
/// \example
///   \code
///   // m_pScreenMask is a class variable which is deleted in DeInitFunction
///   m_pScreenMask = new VisScreenMask_cl(); // create screen mask object
///   m_pScreenMask->SetTransparency(VIS_TRANSP_ALPHA); // set alpha transparency
///   m_pScreenMask->LoadFromFile("console3-color"); // load texture
///   m_pScreenMask->SetPos(0,0); // upper left corner
///   m_pScreenMask->SetTargetSize(Vision::Video.GetXRes(),Vision::Video.GetYRes()); // size of the screen
///   m_pScreenMask->SetZVal(1000.0f); // not in the front - nice effect because the screen mask is clipped 
///   \endcode
class VisScreenMask_cl : public VisElementManager_cl<class VisScreenMask_cl *>, public VRefCounter, public VUserDataObj
{
public:

  ///
  /// @name Constructors / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor of the screen mask class. Loads the mask texture from file.
  /// 
  /// \param pszFileName
  ///   file name of the texture file (with extension)
  VISION_APIFUNC VisScreenMask_cl(const char *pszFileName);

  /// \brief
  ///   Constructor of the screen mask class. Loads the mask texture from file.
  /// 
  /// \param pszFileName
  ///   file name of the texture file (with extension)
  /// 
  /// \param iFlags
  ///   Texture loading flags. Same bit constants as in VisTextureManager_cl::Load2DTexture
  VISION_APIFUNC VisScreenMask_cl(const char *pszFileName, int iFlags);

  /// \brief
  ///   Constructor of the screen mask class
  VISION_APIFUNC VisScreenMask_cl();
  

  /// \brief
  ///   Constructor of the screen mask class. Creates a texture from system memory raw data.
  /// 
  /// The data will be uploaded to the graphics card. The user is responible for free'ing the
  /// memory again.
  /// 
  /// \param maskPtr
  ///   pointer to system memory that stores the bitmap data
  /// 
  /// \param xSize
  ///   Size of the texture in x direction. Must be a power of two.
  /// 
  /// \param ySize
  ///   Size of the texture in y direction. Must be a power of two.
  /// 
  /// \param bpp
  ///   bits per pixel of the mask data in memory. Must be 24 or 32 (for RGBA).
  VISION_APIFUNC VisScreenMask_cl(char *maskPtr, int xSize, int ySize, int bpp);

  /// \brief
  ///   Constructor of the screen mask class that copies the current frame buffer into the mask
  ///   texture.
  /// 
  /// \param xpos
  ///   horizontal position in the frame buffer to start copying
  /// 
  /// \param ypos
  ///   vertical position in the frame buffer to start copying
  /// 
  /// \param xsize
  ///   size of the mask in x direction. Must be a power of two.
  /// 
  /// \param ysize
  ///   size of the mask in y direction. Must be a power of two.
  /// 
  /// \param bpp
  ///   bits per pixel of the mask data in memory. Must be 24 or 32 (for RGBA).
  VISION_APIFUNC VisScreenMask_cl(int xpos, int ypos, int xsize, int ysize, int bpp);

  /// \brief
  ///   Destructor. Releases the texture reference resp. deletes the custom texture.
  VISION_APIFUNC virtual ~VisScreenMask_cl();
  
  /// \brief
  ///   Dispose the object and remove it from scene
  VISION_APIFUNC virtual void DisposeObject();

  /// \brief
  ///   Indicates whether this instance has been disposed already. Not that this class does not inherit from VTypedObject
  inline bool IsDisposed() const
  {
    return m_iListIndex==VIS_INVALID;
  }

  /// \brief
  ///   Non-virtual override of Release function
  inline void Release()
  {
    // NB: VisScreenMask_cl does not inherit from VisTypedEngineObject_cl and thus does not know the VObjectFlag_AutoDispose flag
    //     But since it is added to an element manager, we have to take care that a refcount of 1 deletes the object.
    //     In fact this is the same code as in VisObject3D_cl::Release(), except the VObjectFlag_AutoDispose flag.
    int iOldRef = m_iRefCount;
    VRefCounter::Release();
    if (iOldRef==2/* && IsObjectFlagSet(VObjectFlag_AutoDispose)*/)
      DisposeObject();
  }
  

  /// \brief
  ///   This function unloads the mask (i.e. releases the texture reference) and resets the screen
  ///   mask settings.
  VISION_APIFUNC void Unload();
  
 


  ///
  /// @}
  ///

  ///
  /// @name Setup Functions
  /// @{
  ///

  /// \brief
  ///   Copies the current frame buffer to the texture object of this screen mask.
  /// 
  /// \param xpos
  ///   horizontal position in the frame buffer to start copying
  /// 
  /// \param ypos
  ///   vertical position in the frame buffer to start copying
  /// 
  /// \param xsize
  ///   size of the mask in x direction. If not a power of two, a subrectangle is updated in the
  ///   texture.
  /// 
  /// \param ysize
  ///   size of the mask in y direction. If not a power of two, a subrectangle is updated in the
  ///   texture.
  /// 
  /// \param bpp
  ///   bits per pixel of the mask data in memory. Must be 24 or 32 (for RGBA).
  /// 
  /// \param flags
  ///   ignored, should be 0
  /// 
  /// \return
  ///   BOOL result: TRUE if successful
  /// 
  /// \note
  ///   This function should only be used when the mask has been created with the respective front
  ///   buffer constructor.
  /// 
  /// \note
  ///   For updating the frame buffer in the mask, call UpdateMask rather than LoadFromFrontBuffer
  /// 
  /// \sa VisScreenMask_cl::UpdateMask
  /// \sa VisScreenMask_cl::VisScreenMask_cl(int xpos, int ypos, int xsize, int ysize, int bpp)
  VISION_APIFUNC BOOL LoadFromFrontBuffer(int xpos, int ypos, int xsize, int ysize, int bpp, int flags=0);


  /// \brief
  ///   Uploads system memory raw data to the current texture object.
  /// 
  /// If this mask does not yet have a texture object, a new texture is created. The sizes must be
  /// a power of two in that case.
  /// 
  /// For existing textures, xSize and ySize might also define a sub-rectangle.
  /// 
  /// \param maskPtr
  ///   pointer to a system memory raw buffer
  /// 
  /// \param xSize
  ///   size of the update region in x direction
  /// 
  /// \param ySize
  ///   size of the update region in y direction
  /// 
  /// \param bpp
  ///   bits per pixel. Must be 24 or 32.
  /// 
  /// \return
  ///   BOOL result: TRUE if successful.
  VISION_APIFUNC BOOL LoadFromMemory(char *maskPtr, int xSize, int ySize, int bpp);

  
  /// \brief
  ///   Loads a texture from the file
  /// 
  /// The texture reference to the old texture object is releases.
  /// 
  /// Like all other texture loading functions, this function tries to find a texture with matching
  /// filename in the texture resource manager.
  /// 
  /// \param pszFileName
  ///   name of the texture file with file extension
  /// 
  /// \return
  ///   BOOL result: TRUE if successful.
  VISION_APIFUNC BOOL LoadFromFile(const char *pszFileName);

  /// \brief
  ///   Loads a texture from the file
  /// 
  /// This version additionally supports loading flags. See VisTextureManager_cl::Load2DTexture
  /// for supported flag constants.
  /// 
  /// The texture reference to the old texture object is releases.
  /// 
  /// Like all other texture loading functions, this function tries to find a texture with matching
  /// filename in the texture resource manager.
  /// 
  /// \param pszFileName
  ///   Name of the texture file with file extension.
  /// 
  /// \param iFlags
  ///   Loading flags for the texture.
  /// 
  /// \return
  ///   BOOL result: TRUE if successful.
  VISION_APIFUNC BOOL LoadFromFile(const char *pszFileName, int iFlags);
  

  /// \brief
  ///   Sets the transparency mode of this screen mask.
  /// 
  /// The transparency mode is a constant that maps to pre-defined blending operations such as
  /// additive or alpha blending.
  /// 
  /// By default new screen masks have a transparency type of VIS_TRANSP_NONE which renders the
  /// screenmask opaque.
  /// 
  /// \param transp
  ///   The new transparency type.
  inline void SetTransparency(VIS_TransparencyType transp)
  {
    m_iRenderState.SetTransparency(transp);
    if (transp==VIS_TRANSP_COLORKEY)
      m_iRenderState.SetFlag(RENDERSTATEFLAG_ALPHATEST);
    else
      m_iRenderState.RemoveFlag(RENDERSTATEFLAG_ALPHATEST);
  }

  /// \brief
  ///   Sets the full render state that supersedes depth write and transparency mode.
  inline void SetRenderState(const VSimpleRenderState_t &iState)
  {
    m_iRenderState = iState;
  }

  /// \brief
  ///   Return the render state flags.
  inline const VSimpleRenderState_t& GetRenderState() const
  {
    return m_iRenderState;
  }

  /// \brief
  ///   Sets the modulation color for the screen mask
  /// 
  /// This color tints the base texture. The default is V_RGBA_WHITE.
  /// 
  /// The alpha portion might influence the blending.
  /// 
  /// \param iColor
  ///   New color value (RGBA)
  /// 
  /// \sa VisScreenMask_cl::GetColor
  /// \sa VisScreenMask_cl::SetTransparency
  inline void SetColor(VColorRef iColor)
  {
    m_iColor=iColor;
  }

  /// \brief
  ///   Gets the current modulation color of the screen mask
  /// 
  /// GetColor returns the current modulation color of the screen mask.
  /// 
  /// The alpha portion of the color value specifies the current transparency value.
  /// 
  /// \return
  ///   VColorRef iColor : modulation color (RGBA) of the mask
  /// 
  /// \sa VisScreenMask_cl::SetColor
  inline VColorRef GetColor() const
  {
    return m_iColor;
  }

  /// \brief
  ///   Sets the wrapping mode of the screen mask in u- and v-direction
  /// 
  /// The default is TRUE for both directions. If set to FALSE, the texture is clamped at its
  /// border.
  /// 
  /// \param wrapU
  ///   new wrapping status for the texture's u-direction
  /// 
  /// \param wrapV
  ///   new wrapping status for the texture's v-direction
  VISION_APIFUNC void SetWrapping(BOOL wrapU, BOOL wrapV);

  /// \brief
  ///   Gets the current wrapping status of the screen mask in u- and v-direction
  /// 
  /// and writes the result to the wrapU and wrapV parameter.
  /// 
  /// \param wrapU
  ///   BOOL that receives the current wrapping status of the masks's u-direction
  /// 
  /// \param wrapV
  ///   BOOL that receives the current wrapping status of the masks's v-direction
  /// 
  /// \sa VisScreenMask_cl::SetWrapping
  /// \sa VisScreenMask_cl::SetTextureRange
  /// \sa VisScreenMask_cl::SetFrames
  /// \sa VisScreenMask_cl::SetFiltering
  inline void GetWrapping(BOOL &wrapU, BOOL &wrapV) const
  {
    wrapU = this->wrapU;
    wrapV = this->wrapV;
  }

  /// \brief
  ///   Sets the size of the screen mask on the screen in pixels
  /// 
  /// By default the target size is set to the texture's size.
  /// 
  /// \param xSize
  ///   x-size of screen mask in pixels 
  /// 
  /// \param ySize
  ///   y-size of screen mask in pixels
  inline void SetTargetSize(float xSize, float ySize)
  {  
    targetsize[0] = xSize;
    targetsize[1] = ySize;
  }
  
  /// \brief
  ///   Sets the texture coordinate range (source UV rectangle) of the screen mask
  /// 
  /// Texture coordinates are specified in pixels.
  /// 
  /// By default, the range is set to the full texture size.
  /// 
  /// \param uLower
  ///   u-position of the upper left corner on the texture (in pixels)
  /// 
  /// \param vLower
  ///   v-position of the upper left corner on the texture (in pixels)
  /// 
  /// \param uUpper
  ///   u-position of the lower right corner on the texture (in pixels)
  /// 
  /// \param vUpper
  ///   v-position of the lower right corner on the texture (in pixels)
  inline void SetTextureRange(float uLower, float vLower, float uUpper, float vUpper)
  {
    texlower[0] = uLower;
    texlower[1] = vLower;
    texupper[0] = uUpper;
    texupper[1] = vUpper;
  }
  
  /// \brief
  ///   Sets the 2D position and the z-depth of the screen mask on the screen
  /// 
  /// The zVal value can be used to define the z value relative to the camera, this allows you to
  /// "clip" masks into the world. 
  /// 
  /// \param xPos
  ///   x-position of the screen mask in pixels
  /// 
  /// \param yPos
  ///   x-position of the screen mask in pixels
  /// 
  /// \param zVal
  ///   z-depth of the screen mask relative to the camera. A higher value means that the screen
  ///   mask is placed more in the depth, a value of 0 means that the screen mask is always on top.
  ///   (limit is: renderdepth)
  /// 
  /// \note
  ///   By default new screen masks have a xPos, yPos and zVal of 0
  inline void SetPos(float xPos, float yPos, float zVal)
  {
    position[0] = xPos;
    position[1] = yPos;
    SetZVal(zVal);
  }
  
  
  /// \brief
  ///   Sets the 2D position of the screen mask on the screen
  /// 
  /// The default position is (0,0).
  /// 
  /// \param xPos
  ///   x-position of the screen mask in pixels
  /// 
  /// \param yPos
  ///   y-position of the screen mask in pixels
  inline void SetPos(float xPos, float yPos)
  {
    position[0] = xPos;
    position[1] = yPos;
  }
  

  /// \brief
  ///   Sets the visible status of this screen mask.
  /// 
  /// To have more control over the mask's visible status in different contexts, use the
  /// VisScreenMask_cl::SetVisibleBitmask function.
  /// 
  /// \param visible
  ///   if TRUE, the mask is visible in all rendering contexts that render screen masks
  inline void SetVisible(BOOL visible)
  {
    SetVisibleBitmask(visible ? 0xffffffff:0);
  }
    
  /// \brief
  ///   Sets the filter bitmask for rendering only in some contexts.
  /// 
  /// This screenmask will only be rendered in a render context, if the logical AND result of this
  /// bitmask and the context's bitmask (see VisRenderContext_cl::SetRenderFilterMask) is not zero.
  /// 
  /// By default, all bits are set, so it will be rendered in all contexts with a non-zero bitmask.
  /// 
  /// The VisMaskFrame_t structure has its own bitmask member that is additionally tested.
  /// 
  /// \param iMask
  ///   The new filter bitmask
  /// 
  /// \sa VisRenderContext_cl::SetRenderFilterMask
  inline void SetVisibleBitmask(unsigned int iMask)
  {
    m_iVisibleBitmask=iMask;
  }

  /// \brief
  ///   Returns the filter bitmask that has been set via SetVisibleBitmask.
  /// 
  /// \return
  ///   int iBitmask : The current bitmask
  /// 
  /// \sa VisRenderContext_cl::SetRenderFilterMask
  inline unsigned int GetVisibleBitmask() const 
  {
    return m_iVisibleBitmask;
  }

  /// \brief
  ///   Sets the z-depth of the screen mask.
  /// 
  /// The zVal value can be used to define the z value relative to the camera, this allows you to
  /// "clip" masks into the world.
  /// 
  /// The z value doesn't influence the order the screen masks are rendered, but rather the z
  /// position of the screen mask in the world. To change the order of the screen mask rendering
  /// the function VisScreenMask_cl::SetOrder can be used instead.
  /// 
  /// \param zVal
  ///   z-depth of the screen mask relative to the camera. The depth is specified in world units.
  ///   The valid range is between the near and the far clipping plane of the render context. A
  ///   value of 0 renders the mask always on top.
  VISION_APIFUNC void SetZVal(float zVal);
   

  /// \brief
  ///   Sets the screen mask key.
  /// 
  /// SetKey sets the key string of this screen mask. You can use this key to identify the
  /// screenmask (e.g. for selection).
  /// 
  /// \param pszNewKey
  ///   new screen mask key. Can be NULL to remove the key.
  inline void SetKey(const char *pszNewKey)
  {
    m_szKey = pszNewKey;
  }

  /// \brief
  ///   Enables or disables bi-linear texture filtering when rendering the mask.
  /// 
  /// By default, filtering is enabled.
  /// 
  /// Screen masks that are displayed in actual pixel size usually look better when rendered
  /// without filtering.
  /// 
  /// \param status
  ///   TRUE to enable bilinear texture filtering, FALSE to use closest filtering.
  /// 
  /// \sa VisScreenMask_cl::GetFiltering
  /// \sa VisScreenMask_cl::SetWrapping
  VISION_APIFUNC void SetFiltering(BOOL status);

  /// \brief
  ///   Get the current bi-linear filtering status of the mask
  /// 
  /// \return
  ///   BOOL status : The current filtering status of the mask. 
  /// 
  /// \sa VisScreenMask_cl::SetFiltering
  /// \sa VisScreenMask_cl::SetWrapping
  inline BOOL GetFiltering() const
  {
    return filtering;
  }

  /// \brief
  ///   Sets the drawing order of the screen mask.
  /// 
  /// All visible screen masks are sorted by this sorting key before rendering.
  /// 
  /// Masks with a large order value are rendered first, so the mask with the lowest order will be
  /// on top.
  /// 
  /// By default the order of a mask is 0.
  /// 
  /// \param newOrder
  ///   The new order sorting key value for this mask
  inline void SetOrder(int newOrder)
  {
    order = newOrder;
  }



  /// \brief
  ///   Updates the mask texture. Only required for memory masks or frame buffer masks.
  /// 
  /// For memory masks this function re-uploads the system memory to the texture object using the
  /// memory pointer passed in the constructor resp. in LoadFromMemory.
  /// 
  /// For frame buffer masks it copies the current frame buffer content into the texture.
  /// 
  /// \sa VisScreenMask_cl::LoadFromMemory
  /// \sa VisScreenMask_cl::LoadFromFrontBuffer

  VISION_APIFUNC void UpdateMask();




  ///
  /// @}
  ///

  ///
  /// @name Information Functions
  /// @{
  ///

  /// \brief
  ///   Returns the screen mask's key string (see SetKey)
  /// 
  /// \return
  ///   const char *szKey: The mask's key string, or NULL
  inline const char *GetKey() const
  {
    return m_szKey;
  }

  /// \brief
  ///   Test if the screenmask has the specified key (required for the element manager).
  inline BOOL HasObjectKey(const char *szKey, BOOL bIgnoreCase=TRUE) const
  {
    return VStringHelper::SafeCompare(m_szKey.AsChar(), szKey, bIgnoreCase==TRUE)==0;
  }

  /// \brief
  ///   Returns the current z-depth of the screen mask (see SetZVal)
  /// 
  /// \return
  ///   float zDepth: z-depth of screenmask
  /// 
  /// \sa VisScreenMask_cl::SetZVal
  /// \sa VisScreenMask_cl::SetPos
  inline float GetZVal() const
  {
    return zval;
  }

  /// \brief
  ///   Gets the current 2D screen position of the screen mask.
  /// 
  /// \param xPos
  ///   float reference which receives the x position
  /// 
  /// \param yPos
  ///   float reference which receives the y position
  /// 
  /// \param zVal
  ///   If specified the float receives the z-depth
  /// 
  /// \sa VisScreenMask_cl::SetPos
  inline void GetPos(float &xPos, float &yPos, float *zVal = NULL) const
  {
    xPos = position[0];
    yPos = position[1];

    if(zVal)
      *zVal = zval;
  }


  /// \brief
  ///   Returns the visibility status of this screen mask.
  /// 
  /// \param iMask
  ///   A visible mask (e.g. context specific) to test against
  /// 
  /// \return
  ///   BOOL: Returns TRUE if the screen mask is visible in respect to the passed mask
  /// 
  /// \sa VisScreenMask_cl::SetVisibleBitmask
  inline BOOL IsVisible(unsigned int iMask=0xffffffff) const 
  {
    return (m_iVisibleBitmask&iMask)>0;
  }


  /// \brief
  ///   Returns the size of the screen mask in pixels. (See VisScreenMask_cl::SetTargetSize)
  /// 
  /// \param xSize
  ///   Reference that receives the width in pixels.
  /// 
  /// \param ySize
  ///   Reference that receives the height in pixels.
  /// 
  /// \sa VisScreenMask_cl::SetTargetSize
  /// 
  /// \example
  ///   \code
  ///   float x,y;
  ///   pScreenMask->GetTargetSize(x,y);
  ///   \endcode
  inline void GetTargetSize(float &xSize, float &ySize) const
  {
    xSize = targetsize[0];
    ySize = targetsize[1];
  }

  /// \brief
  ///   Returns the texture range (source UV rectangle) of the screen mask
  /// 
  /// Texture coordinates are specified in pixels, see SetTextureRange.
  /// 
  /// \param uLower
  ///   reference which receives the u-position of the upper left corner on the texture
  /// 
  /// \param vLower
  ///   reference which receives the v-position of the upper left corner on the texture
  /// 
  /// \param uUpper
  ///   reference which receives the u-position of the lower right corner on the texture
  /// 
  /// \param vUpper
  ///   reference which receives the v-position of the lower right corner on the texture
  /// 
  /// \sa VisScreenMask_cl::SetTextureRange
  /// 
  /// \example
  ///   \code
  ///   float ul,vl,uu,vu;
  ///   pScreenMask->GetTextureRange(ul,vl,uu,vu);
  ///   \endcode
  inline void GetTextureRange(float &uLower, float &vLower, float &uUpper, float &vUpper) const
  {
    uLower = texlower[0];
    vLower = texlower[1];
    uUpper = texupper[0];
    vUpper = texupper[1];
  }

  /// \brief
  ///   Returns the drawing order key of the screen mask
  /// 
  /// \return
  ///   int iOrder: The current order value
  /// 
  /// \sa VisScreenMask_cl::SetOrder
  inline int GetOrder() const
  {
    return order;
  }



  /// \brief
  ///   Gets the type of transparency used for rendering this mask.
  /// 
  /// \return
  ///   VIS_TransparencyType eType: Current transparency type.
  /// 
  /// \sa VisScreenMask_cl::SetTransparency
  inline VIS_TransparencyType GetTransparencyType() const
  {
    return (VIS_TransparencyType) m_iRenderState.GetTransparency();
  }

  /// \brief
  ///   Gets the size of the texture object associated with this screen mask.
  /// 
  /// This can be different from the actual on-screen size of the mask.
  /// 
  /// \param iXSize
  ///   Out: Width of the screen mask's texture.
  /// 
  /// \param iYSize
  ///   Out: Height of the screen mask's texture.
  /// 
  /// \sa VisScreenMask_cl::GetTargetSize
  /// \sa VisScreenMask_cl::GetTextureObject
  inline void GetTextureSize (int &iXSize, int &iYSize ) const
  {
    iXSize = size[0];
    iYSize = size[1];
  }

  /// \brief
  ///   Switches this mask into "Frame Mode" and defines the frames. 
  /// 
  /// In Frame Mode the screen mask doesn't get rendered in the usual linear way (i.e. one input
  /// texture, one rendered quad on screen). Instead multiple quads can be rendered from a single
  /// input texture, each using arbitrary texture coordinates.
  /// 
  /// A screen mask remains in Frame Mode until DeleteFrames() is called, or SetFrames is called
  /// again with a list count of 0.
  /// 
  /// The frame data passed to this function is not deep copied by this function, instead the
  /// pointer has to remain valid until rendering the frames.
  /// 
  /// However, the VisRenderLoopHelper_cl::BeginOverlayRendering/EndOverlayRendering mechanism is
  /// more flexible to use. See GUI classes.
  /// 
  /// \param framelist
  ///   Pointer to a list of frame lists. The user is responsible for allocating and freeing the
  ///   data; the engine will just read from it.
  /// 
  /// \param numframelists
  ///   Number of frame lists
  /// 
  /// \sa VisScreenMask_cl::DeleteFrames
  inline void SetFrames (VisMaskFrameList_t *framelist, int numframelists)
  {
    maskframes = framelist;
    nummaskframes = numframelists;
  }

  /// \brief
  ///   Disables the Frame Mode for this screen mask. (See SetFrames)
  /// 
  /// \sa VisScreenMask_cl::SetFrames
  inline void DeleteFrames()
  {
    maskframes = NULL;
    nummaskframes = 0;
  }
  
  
  ///
  /// @}
  ///

  ///
  /// @name Texture Animations
  /// @{
  ///



  /// \brief
  ///   Returns the texture animation instance of the animated screen mask texture
  /// 
  /// If the texture of this screen mask is not animated, this function returns NULL.
  /// 
  /// The returned object can be used to control the texture animation (e.g. Pause, ...)
  /// 
  /// \return
  ///   VisTextureAnimInstance_cl* pAnim : Pointer to texture animation instance of the animated
  ///   texture, or NULL if texture is not animated
  /// 
  /// \sa VisTextureAnimInstance_cl
  VisTextureAnimInstance_cl* GetTextureAnimationInstance() const
  {
    return texanim_id;
  }


  ///
  /// @}
  ///

  ///
  /// @name Mask Rotation
  /// @{
  ///

  /// \brief
  ///   Sets the rotation angle of this mask around the rotation center
  /// 
  /// To set the rotation center, use the VisScreenMask_cl::SetRotationCenter function.
  /// 
  /// The texture coordinates are not affected by this rotation.
  /// 
  /// The rotation does not affect the mask when it is in frame mode. In frame mode, each frame has
  /// its own rotation angle (see VisMaskFrame_t::fRotation).
  /// 
  /// \param fAngle
  ///   The rotation angle in degrees of the mask. 0.f (default) means no rotation, any other angle
  ///   rotates the mask counter clock wise.
  /// 
  /// \sa VisScreenMask_cl::SetRotationCenter
  /// \sa VisScreenMask_cl::GetRotationAngle
  /// \sa VisMaskFrame_t
  /// \sa VisMeshBuffer_cl
  /// \sa VisMeshBuffer_cl::SetUseProjectionMatrix
  inline void SetRotationAngle(float fAngle)
  {
    m_bUseRotation = fAngle!=0.f;
    fRotation = fAngle;
  }


  /// \brief
  ///   Returns the current rotation angle of the mask (in degree)
  /// 
  /// \return
  ///   float fAngle : The current rotation angle in degrees
  /// 
  /// \sa VisScreenMask_cl::SetRotationAngle
  inline float GetRotationAngle() const
  {
    return fRotation;
  }


  /// \brief
  ///   Sets the relative rotation center of the screenmask when rotating it
  /// 
  /// For instance, to rotate the mask around its center use sizeX/2,sizeY/2 as a center.
  /// 
  /// The default rotation center of a mask is (0,0).
  /// 
  /// \param fCenterX
  ///   The rotation center x-coordinate, relative to the mask's upper left corner
  /// 
  /// \param fCenterY
  ///   The rotation center y-coordinate, relative to the mask's upper left corner
  /// 
  /// \sa VisScreenMask_cl::SetRotationAngle
  /// \sa VisMaskFrame_t
  /// \sa VisMeshBuffer_cl
  /// \sa VisMeshBuffer_cl::SetUseProjectionMatrix
  inline void SetRotationCenter(float fCenterX, float fCenterY)
  {
    m_vRotOfs.set(fCenterX,fCenterY);
  }

  /// \brief
  ///   Gets the relative rotation center of the screenmask
  /// 
  /// The values are set via SetRotationCenter.
  /// 
  /// \param fCenterX
  ///   Reference that receives the relative x-coordinate of the center
  /// 
  /// \param fCenterY
  ///   Reference that receives the relative y-coordinate of the center
  inline void GetRotationCenter(float &fCenterX, float &fCenterY) const
  {
    fCenterX = m_vRotOfs.u;
    fCenterY = m_vRotOfs.v;
  }



  ///
  /// @}
  ///

  ///
  /// @name Texture Object Functions
  /// @{
  ///

  /// \brief
  ///   Returns the texture object associated with this mask
  /// 
  /// \return
  ///   VTextureObject* pTexture : Texture object of the screen mask
  /// 
  /// \sa VTextureObject
  /// \sa VisScreenMask_cl::SetTextureObject
  inline VTextureObject* GetTextureObject()
  {
    return spTexture;
  }
  
  /// \brief
  ///   Sets the texture object for this screen mask
  /// 
  /// When settings a new texture, the screenmask first calls the Unload function to free memory
  /// used by memory masks etc.
  /// 
  /// \param pTexture
  ///   The new texture object. Must be !=NULL
  /// 
  /// \sa VisScreenMask_cl::GetTextureObject
  /// \sa VisScreenMask_cl::LoadFromFile
  VISION_APIFUNC void SetTextureObject(VTextureObject* pTexture);
  
  ///
  /// @}
  ///

  ///
  /// @name Render State Functions
  /// @{
  ///


  /// \brief
  ///   Defines whether the screen mask rendering writes to the depth buffer
  /// 
  /// \param bStatus
  ///   If set to TRUE, the screen masks writes to the depth buffer
  /// 
  /// \sa VisScreenMask_cl::GetDepthWrite
  inline void SetDepthWrite(BOOL bStatus)
  {
    if (bStatus)
      m_iRenderState.SetFlag(RENDERSTATEFLAG_WRITETOZBUFFER);
    else
      m_iRenderState.RemoveFlag(RENDERSTATEFLAG_WRITETOZBUFFER);
  }

  /// \brief
  ///   Returns whether the screen masks writes to the depth buffer
  /// 
  /// \return
  ///   BOOL: If TRUE, the screen masks writes to the depth buffer
  /// 
  /// \sa VisScreenMask_cl::SetDepthWrite
  inline BOOL GetDepthWrite() const
  {
    return m_iRenderState.IsFlagSet(RENDERSTATEFLAG_WRITETOZBUFFER);
  }


  /// \brief
  ///   Sets the shader technique for this screen mask
  /// 
  /// NULL can be passed to this method in order to remove the technique from the screen mask again
  /// The render state of the given technique overrides the render state set in the screen mask.
  /// 
  /// \param pTechnique
  ///   Shader technique to be applied to this screen mask
  /// 
  /// \sa VisScreenMask_cl::GetTechnique
  inline void SetTechnique(VCompiledTechnique *pTechnique)
  {
    m_spTechnique = pTechnique;
  }

  /// \brief
  ///   Returns the current shader technique assigned to this screen mask
  /// 
  /// \return
  ///   VCompiledTechnique *pTechnique: Shader technique assigned to this screen mask, or NULL if
  ///   no shader technique is assigned
  /// 
  /// \sa VisScreenMask_cl::SetTechnique
  inline VCompiledTechnique *GetTechnique() const
  {
    return m_spTechnique;
  }


  ///
  /// @}
  ///

  ///
  /// @name Unsupported Functions
  /// @{
  ///

  inline void SetUseOpenGLTexelShift(BOOL bStatus)
  {
    bUseTexelShift = bStatus?1:0;
  }

  ///
  /// @}
  ///

private:
  friend class VisShaders_cl;
  
  /// \brief
  ///   Initialises the empty mask
  BOOL Init();
  
  friend void DrawMasks(unsigned int iVisibleMask);
  friend void DrawMasks(const VisScreenMaskCollection_cl &masks, VCompiledShaderPass *pShader);
  friend void DrawSingleMask_Hardware(VisScreenMask_cl *mask, unsigned int iVisibleMask, VCompiledShaderPass *pShader);
  friend void DrawSingleMaskQuad(VisScreenMask_cl *mask, VColorRef iColor, char flipped, unsigned int iVisibleMask, int iStreamMask);
  friend void DrawSingleShaderOnScreenMask(VCompiledShaderPass *shader, VisScreenMask_cl *mask, char flipped, unsigned int iVisibleMask);
  friend class VisStateHandler_cl;
  friend class VisRenderStates_cl;
  friend void HandleLostDevice();

//  int internalFormat;             // internal format (for framebuffer mask)
  VString m_szKey;                ///< screen mask key

  char  *ptr;                   ///< pointer to image data
  int   size[2];                ///< image width/height
  VColorRef m_iColor;           ///< modulation color (usually 255,255,255 = white)
  char  colordepth;             ///< color depth (8 or 24)
  char  wrapU,wrapV;            ///< wrapping for texture
  char  useS3TC;                ///< use texture compression (if available!)
  char  filtering;              ///< flag for bil.filtering
  char  masktype;               ///< constant: file/memory/frontbuffer frontbuffer mask
  char  bUseTexelShift;         ///< at initialisation, copy from global settings
  bool  m_bUseRotation;
  float targetsize[2];          ///< target size of image (scaled if needed)
  float   texupper[2];
  float   texlower[2];
  float zval;                   ///< z-Position of the image in frame buffer
  int   order;
  int  m_iVisibleBitmask;       ///< and operation with context
  float position[2];            ///< position on screen
  VisMaskFrameList_t *maskframes; ///< frame mode
  int   nummaskframes;          ///< frame mode
  int   srcpos[2];
  VSimpleRenderState_t m_iRenderState;

  VTextureObjectPtr spTexture;
  VisTextureAnimInstancePtr texanim_id;
  VCompiledTechniquePtr m_spTechnique;
  
  float fRotation;            ///< rotation angle of screenmask in degrees
  hkvVec2 m_vRotOfs;   ///< rotation offset
};


VISION_ELEMENTMANAGER_TEMPLATE_DECL(VisScreenMask_cl)

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
