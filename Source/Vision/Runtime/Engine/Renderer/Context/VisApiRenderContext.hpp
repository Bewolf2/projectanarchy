/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file

#ifndef DEFINE_VISAPIRENDERCONTEXT
#define DEFINE_VISAPIRENDERCONTEXT

#include <Vision/Runtime/Base/System/Memory/VBaseMem.hpp>
#include <Vision/Runtime/Engine/Renderer/OcclusionQuery/VisApiOcclusionQueryBuffer.hpp>

#include <Vision/Runtime/Engine/Renderer/RenderLoop/VisionRenderLoop.hpp>
#include <Vision/Runtime/Engine/Visibility/VisionVisibilityCollector.hpp>

#include <Vision/Runtime/Engine/SceneManagement/VisionScene.hpp>
#include <Vision/Runtime/Engine/Renderer/Texture/VisApiRenderableTexture.hpp>
#include <Vision/Runtime/Engine/Renderer/Texture/VisApiRenderableCubeMap.hpp>

#if defined(_VR_DX11)
  #include <Vision/Runtime/Engine/Renderer/Texture/VisApiRenderableTexture3D.hpp>
  #include <Vision/Runtime/Engine/Renderer/Texture/VisApiRenderableTexture2DArray.hpp>
#endif

#include <Vision/Runtime/Engine/Renderer/Context/VisApiContextViewProperties.hpp>

class VisContextCamera_cl;
typedef VSmartPtr<VisContextCamera_cl> VisContextCameraPtr;

class VisContextViewProperties;
typedef VSmartPtr<VisContextViewProperties> VisContextViewPropertiesPtr;

/// \brief
///   Simple render interface to provide functionality for line and text rendering.
/// 
/// Since line/triangle rendering is performed by single calls, this way of rendering is very slow
/// and should only be used for debug or editor rendering.
/// 
/// Furthermore, an object of type IVRenderInterface is useful to pass it to class instances that
/// should be able to render themselves (e.g. a particle constraint).
///
/// This class is the engine-side implementation of the IVRenderInterface interface, using the engine's
/// rendering functionality to implement the pure virtual functions of the base class.
class VisionRenderInterface_cl : public IVRenderInterface
{

public:

  inline VisionRenderInterface_cl()
  {
    m_pCurrentQueue = NULL;
  }

  /// \brief
  ///   Renders a single line in 3D or optionally in 2D.
  ///
  /// 2D lines can be rendered by passing a VSimpleRenderState_t parameter to this function that uses the RENDERSTATEFLAG_LINE2D flag. In 2D line rendering mode
  /// the xy positions are measured in screen pixels and the z-coordinate is ignored.
  /// 
  /// \param vPos1
  ///   First vertex of the line.
  ///
  /// \param vPos2
  ///   Final vertex of the line.
  /// 
  /// \param iColor
  ///   Color of the line.
  ///
  /// \param fWidth
  ///   Width of the line (in pixels). Ignored on most platforms.
  ///
  /// \param iProperties
  ///   Render state/properties to render this line with. Define the transparency mode and some more flags. 
  ///   The pre-defined RENDERSTATEFLAG_LINE2D flags can be used to render 2D lines.
  ///
  virtual void DrawLine(const hkvVec3& vPos1, const hkvVec3& vPos2, VColorRef iColor, float fWidth, const VSimpleRenderState_t& iProperties);


  /// \brief
  ///   Renders a single triangle.
  /// 
  /// \param vPos1
  ///   First vertex of the triangle.
  ///
  /// \param vPos2
  ///   Second vertex of the triangle.
  /// 
  /// \param vPos3
  ///   Third vertex of the triangle.
  /// 
  /// \param iColor
  ///   Color of the triangle.
  ///
  /// \param iProperties
  ///   Render state/properties to render this triangle with.
  ///
  virtual void DrawTriangle(const hkvVec3& vPos1, const hkvVec3& vPos2, const hkvVec3& vPos3, VColorRef iColor, const VSimpleRenderState_t &iProperties);


  /// \brief
  ///   Renders debug text at a 2D (screen-space) location.
  /// 
  /// \param x
  ///   Screen x position at which to render the text.
  ///
  /// \param y
  ///   Screen y position at which to render the text.
  /// 
  /// \param szMsg
  ///   String to render.
  /// 
  /// \param iColor
  ///   Color of the text.
  ///
  /// \param iProperties
  ///   Render state/properties to render this text with.
  ///
  virtual void DrawText2D(float x, float y, const char *szMsg, VColorRef iColor, const VSimpleRenderState_t &iProperties);

  /// \brief
  ///   Renders debug text at a 2D (screen-space) location.
  /// 
  /// \param x
  ///   Screen x position at which to render the text.
  ///
  /// \param y
  ///   Screen y position at which to render the text.
  /// 
  /// \param szMsg
  ///   String to render.
  /// 
  /// \param iColor
  ///   Color of the text.
  ///
  ///
  virtual void DrawText2D(float x, float y, const char *szMsg, VColorRef iColor);


  /// \brief
  ///   Renders debug text at a 3D (world-space) location.
  /// 
  /// \param vPos
  ///   World-space position at which to render the text.
  ///
  /// \param szMsg
  ///   String to render.
  /// 
  /// \param iColor
  ///   Color of the text.
  ///
  /// \param iProperties
  ///   Render state/properties to render this text with.
  ///
  virtual void DrawText3D(const hkvVec3& vPos, const char *szMsg, VColorRef iColor, const VSimpleRenderState_t &iProperties);


  /// \brief
  ///   Renders debug text at a 3D (world-space) location.
  /// 
  /// \param vPos
  ///   World-space position at which to render the text.
  ///
  /// \param szMsg
  ///   String to render.
  /// 
  /// \param iColor
  ///   Color of the text.
  ///
  virtual void DrawText3D(const hkvVec3& vPos, const char *szMsg, VColorRef iColor);


  /// \brief
  ///   Renders a sprite (billboard) at a given world-space location.
  /// 
  /// \param vCenter
  ///   World-space position at which to render the sprite's center.
  ///
  /// \param pTexture
  ///   Texture to render.
  /// 
  /// \param iColor
  ///   Color of the text.
  ///
  /// \param iProperties
  ///   Render state/properties to render this triangle with.
  ///
  /// \param fSizeX
  ///   Size of the sprite in horizontal direction.
  ///
  /// \param fSizeY
  ///   Size of the sprite in vertical direction.
  ///
  /// \param uv0
  ///   Left-top coordinates in the texture object where to retrieve the sprite. [0..1]
  ///
  /// \param uv1
  ///   Right-bottom coordinates in the texture object where to retrieve the sprite. [0..1]
  virtual void DrawSprite(const hkvVec3 &vCenter, VTextureObject *pTexture, VColorRef iColor, const VSimpleRenderState_t &iProperties, float fSizeX, float fSizeY, const hkvVec2& uv0=hkvVec2(0,0), const hkvVec2& uv1=hkvVec2(1,1));


  /// \brief
  ///   Renders a sprite (billboard) at a given world-space location.
  /// 
  /// \param vCenter
  ///   World-space position at which to render the sprite's center.
  ///
  /// \param szTextureFilename
  ///   Texture to render (filename).
  /// 
  /// \param iColor
  ///   Color of the text.
  ///
  /// \param iProperties
  ///   Render state/properties to render this triangle with.
  ///
  /// \param fSizeX
  ///   Size of the sprite in horizontal direction.
  ///
  /// \param fSizeY
  ///   Size of the sprite in vertical direction.
  ///
  /// \param uv0
  ///   Left-top coordinates in the texture object where to retrieve the sprite. [0..1]
  ///
  /// \param uv1
  ///   Right-bottom coordinates in the texture object where to retrieve the sprite. [0..1]
  virtual void DrawSprite(const hkvVec3 &vCenter, const char *szTextureFilename, VColorRef iColor, const VSimpleRenderState_t &iProperties, float fSizeX, float fSizeY, const hkvVec2& uv0=hkvVec2(0,0), const hkvVec2& uv1=hkvVec2(1,1));

  VisDebugRenderQueue_cl *m_pCurrentQueue;
};


class VisRenderContext_cl;
typedef VSmartPtr<VisRenderContext_cl> VisRenderContextPtr;

/// \brief
///   Collection class (reference counted) for instances of VisRenderContext_cl
typedef VRefCountedCollection<VisRenderContext_cl> VisRenderContextCollection_cl;


/// \brief
///   Render Context class. 
/// 
/// This class allows you to set up multiple simultaneous render contexts with individual settings
/// (e.g., field of view, view port, camera position, render target) and render to these render
/// contexts independently of each other.
/// 
/// A render context has to be bound to a VisContextCamera_cl object. Render contexts may share
/// VisContextCamera_cl objects. An VisContextViewProperties to hold the projection properties
/// will be created, but may be shared between contexts as well.
/// 
/// Render contexts also allow rendering to renderable textures as render targets. Typically, this
/// is used for rendering shadow buffers, mirror textures, post-processing effects, and the like.
/// Specifying the render target(s) for a render context can be done with the SetRenderTarget()
/// method and its separate variants for cube maps and 2D texture arrays. Render targets can be set
/// to NULL, though you should take into account that setting render target 0 to NULL is illegal in
/// DirectX9. Depth Stencil targets can be set separately from color render targets.
/// 
/// Render contexts may have individual render loop and visibility collector implementations.
/// 
/// There is a permanent main context which gets created when the Vision engine is initialized and
/// destroyed when the engine is deinitialized. This main context has its render target set to the
/// back buffer and its depth stencil target set to the depth stencil buffer created along with the
/// device (through VisVideo_cl::InitializeScreen).
/// 
/// By default, the VisionApp_cl::Run method takes care of executing the rendering for the
/// individual contexts. If you do not use the VisionApp_cl::Run method, you will have to take care
/// of rendering to the individual contexts yourself.
class VisRenderContext_cl : public VRefCounter, public VUserDataObj, public VisElementManager_cl<VisRenderContext_cl*>, public IVisCallbackHandler_cl
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Default constructor of the VisRenderContext_cl class.
  /// 
  /// A render context initialized with these parameters can't be used for rendering right away. It
  /// additionally needs at least a context camera and render target.
  VISION_APIFUNC VisRenderContext_cl();

  /// \brief
  ///   Constructor of the VisRenderContext_cl class.
  /// 
  /// This constructor takes all required render context parameters as input. A render context
  /// created with this constructor may be used for rendering immediately.
  /// 
  /// \param pCamera
  ///   Pointer to the camera to be used for this render context.
  /// 
  /// \param fFovX
  ///   Field of view in horizontal direction in degrees.
  /// 
  /// \param fFovY
  ///   Field of view in vertical direction in degrees.
  /// 
  /// \param iWidth
  ///   Width of the render context.
  /// 
  /// \param iHeight
  ///   Height of the render context.
  /// 
  /// \param fNearClip
  ///   Near clip plane distance in units.
  /// 
  /// \param fFarClip
  ///   Far clip plane distance in units.
  /// 
  /// \param iRenderFlags
  ///   Set of render flags that may be provided for render contexts.
  ///   See enum VRenderContextFlags for more details.
  VISION_APIFUNC VisRenderContext_cl(const VisContextCamera_cl *pCamera, float fFovX, float fFovY, int iWidth, int iHeight, float fNearClip, float fFarClip, int iRenderFlags = VIS_RENDERCONTEXT_FLAGS_RENDERTOTEXTURE);

  /// \brief
  ///   Destructor of the VisRenderContext_cl class
  VISION_APIFUNC virtual ~VisRenderContext_cl();

  /// \brief
  ///  Sets itself as the current context, triggers context switching callbacks and kicks off rendering the scene.
  VISION_APIFUNC virtual void Execute();

  /// \brief
  ///   Dispose the object and removes it
  VISION_APIFUNC virtual void DisposeObject();

  /// \brief
  ///   Non-virtual override of Release function
  inline void Release()
  {
    // NB: VisRenderContext_cl does not inherit from VisTypedEngineObject_cl and thus does not know the VObjectFlag_AutoDispose flag
    //     But since it is added to an element manager, we have to take care that a refcount of 1 deletes the object.
    //     In fact this is the same code as in VisObject3D_cl::Release(), except the VObjectFlag_AutoDispose flag.
    int iOldRef = m_iRefCount;
    VRefCounter::Release();
    if (iOldRef==2/* && IsObjectFlagSet(VObjectFlag_AutoDispose)*/)
      DisposeObject();
  }

  ///
  /// @}
  ///

  ///
  /// @name Render Context Name
  /// @{
  ///

  /// \brief
  ///   Returns the render context name.
  /// 
  /// \return
  ///   const char *: render context name 
  inline const char* GetName() const {return m_szName;}

  /// \brief
  ///   Sets the name of this render context
  /// \param szNewString
  inline void SetName(const char *szNewString)
  {
    m_szName = szNewString;
  }

  ///
  /// @}
  ///

  ///
  /// @name Activate / Deactivate
  /// @{
  ///

  /// \brief
  ///   Activates a render context
  /// 
  /// Once a context is activated, all operations performed on the context may immediately affect
  /// the rendering output. On Win32 (DX9+10) and Playstation 3, activating a context also results
  /// in its render target being set.
  /// 
  /// On Xbox360, setting the render targets is deferred until rendering to the context actually
  /// starts.
  VISION_APIFUNC void Activate();

  /// \brief
  ///   Deactivates a render context and removes the global references to it
  /// 
  /// This function should only be called when freeing a render context is intended.
  VISION_APIFUNC void RemoveGlobalReferences() {ReleaseContext();}

  ///
  /// @}
  ///

  ///
  /// @name Camera
  /// @{
  ///

  /// \brief
  ///   Sets the camera for this render context.
  /// 
  /// Position and orientation information for this context is taken from the respective context
  /// camera.
  /// 
  /// A context may not be used for rendering unless it has a valid camera object assigned.
  /// 
  /// Multiple render contexts may share the same camera object if desired.
  /// 
  /// \param pCamera
  ///   Context Camera object for this render context.
  VISION_APIFUNC void SetCamera(const VisContextCamera_cl *pCamera);

  /// \brief
  ///   Returns this context's current camera
  /// 
  /// \return
  ///   VisContextCamera_cl *: The camera used for this render context.
  VISION_APIFUNC VisContextCamera_cl *GetCamera() const;

  /// \brief
  ///   A LOD reference context can be associated with this context.
  ///
  /// All render/visibility operations that calculate level-of-detail should use this context to determine distance and
  /// settings. As an example, a shadow context should calculate the same LODs as the view context that it is associated with.
  ///
  /// \param pContext
  ///   Reference context to set. Use NULL if no reference context should be used.
  inline void SetLODReferenceContext(VisRenderContext_cl *pContext)
  {
    VASSERT(pContext!=this);
    m_spLODReferenceContext = pContext;
  }

  /// \brief
  ///   Returns the context that has been set via SetLODReferenceContext. By default this is NULL
  inline VisRenderContext_cl *GetLODReferenceContext() const
  {
    return m_spLODReferenceContext;
  }

  ///
  /// @}
  ///

  ///
  /// @name Viewport and View Properties
  /// @{
  ///

  /// \brief
  ///   Returns this context's target rendering size
  /// 
  /// \param iWidth
  ///   Out: Width of the render context.
  /// 
  /// \param iHeight
  ///   Out: Height of the render context.
  VISION_APIFUNC void GetSize(int &iWidth, int &iHeight) const;

  /// \brief
  ///   Specifies the viewport to be used for this render context
  /// 
  /// By default, the viewport will be set to use the whole render target size specified with
  /// SetSize (when rendering to the back buffer), or to the size of the renderable texture which
  /// is set as render target (when rendering to a texture). This function can be used if you only
  /// want to render to a part of the render target.
  /// 
  /// \param iPosX
  ///   X coordinate of upper left corner of the viewport, in pixels.
  /// 
  /// \param iPosY
  ///   Y coordinate of upper left corner of the viewport, in pixels.
  /// 
  /// \param iWidth
  ///   Width of the viewport, in pixels.
  /// 
  /// \param iHeight
  ///   Height of the viewport, in pixels.
  VISION_APIFUNC void SetViewport(int iPosX, int iPosY, int iWidth, int iHeight);

  /// \brief
  ///   Specifies the viewport to be used for this render context
  /// 
  /// By default, the viewport will be set to use the whole render target size specified with
  /// SetSize (when rendering to the back buffer), or to the size of the renderable texture which
  /// is set as render target (when rendering to a texture). This function can be used if you only
  /// want to render to a part of the render target.
  /// 
  /// \param iPosX
  ///   X coordinate of upper left corner of the viewport, in pixels.
  /// 
  /// \param iPosY
  ///   Y coordinate of upper left corner of the viewport, in pixels.
  /// 
  /// \param iWidth
  ///   Width of the viewport, in pixels.
  /// 
  /// \param iHeight
  ///   Height of the viewport, in pixels.
  ///
  /// \param zMin
  ///   Near z value of the viewport (in the 0..1 range)
  ///
  /// \param zMax
  ///   Far z value of the viewport (in the 0..1 range)
  VISION_APIFUNC void SetViewport(int iPosX, int iPosY, int iWidth, int iHeight, float zMin, float zMax);

  /// \brief
  ///   Returns the viewport to be used for this render context
  /// 
  /// \param iPosX
  ///   Out: X coordinate of upper left corner of the viewport, in pixels.
  /// 
  /// \param iPosY
  ///   Out: Y coordinate of upper left corner of the viewport, in pixels.
  /// 
  /// \param iWidth
  ///   Out: Width of the viewport, in pixels.
  /// 
  /// \param iHeight
  ///   Out: Height of the viewport, in pixels.
  VISION_APIFUNC void GetViewport(int &iPosX, int &iPosY, int &iWidth, int &iHeight) const;

  /// \brief
  ///   Returns the viewport to be used for this render context
  /// 
  /// \param iPosX
  ///   Out: X coordinate of upper left corner of the viewport, in pixels.
  /// 
  /// \param iPosY
  ///   Out: Y coordinate of upper left corner of the viewport, in pixels.
  /// 
  /// \param iWidth
  ///   Out: Width of the viewport, in pixels.
  /// 
  /// \param iHeight
  ///   Out: Height of the viewport, in pixels.
  ///
  /// \param zNear
  ///   Out: Near z value of the viewport (in the 0..1 range)
  ///
  /// \param zFar
  ///   Out: Far z value of the viewport (in the 0..1 range)
  VISION_APIFUNC void GetViewport(int &iPosX, int &iPosY, int &iWidth, int &iHeight, float &zNear, float &zFar) const;

  /// \brief
  ///  Returns the VisContextViewProperties instance that holds the projection properties for this context.
  VISION_APIFUNC VisContextViewProperties* GetViewProperties() const
  {
    return m_spViewProperties;
  }

  /// \brief
  ///  Sets the VisContextViewProperties instance that holds the projection properties for this context.
  VISION_APIFUNC void SetViewProperties(VisContextViewProperties* pViewProperties)
  {
    m_spViewProperties = pViewProperties;
  }

  /// \brief
  ///   Sets the desired horizontal or vertical field of view of this render context in degrees.
  /// 
  /// If either FOV value is 0.0f, it will be computed automatically using the other value and the current
  /// display aspect ratio.
  /// 
  /// \param fFovX
  ///   Horizontal field of view in degrees.
  /// 
  /// \param fFovY
  ///   Vertical field of view in degrees.
  VISION_APIFUNC void SetFOV(float fFovX, float fFovY = 0.0f);

  /// \brief
  ///   Returns the field of view of this render context
  /// 
  /// Values are specified in degrees.
  /// 
  /// \param fFovX
  ///   Out: Field of View in horizontal direction.
  /// 
  /// \param fFovY
  ///   Out: Field of View in vertical direction.
  VISION_APIFUNC void GetFOV(float &fFovX, float &fFovY) const;

    /// \brief
  ///   Returns the field of view of this render context.
  ///
  /// \param fFovX
  ///   Out: Field of View in horizontal direction.
  /// 
  /// \param fFovY
  ///   Out: Field of View in vertical direction.
  /// 
  /// Unlike GetFOV, this method returns the values automatically filled-in for parameters that
  /// were passed to SetFOV as 0.0f.
  VISION_APIFUNC void GetFinalFOV(float &fFovX, float &fFovY) const;

  /// \brief
  ///   LOD implementations should use this value to scale the distances before a LOD is retrieved
  ///
  /// This value incorporates the FOV, so distances become larger with a low camera FOV. This value is 1.0 for a FOV of 90deg and smaller than 1 for smaller angles.
  /// Furthermore, this value is internally multiplied with a custom value set by the SetCustomLODScaling function (which again is 1.0 by default).
  /// If the VIS_RENDERCONTEXT_FLAG_NO_LOD_SCALING flag is set on the context, the returned value always matches the custom value (SetCustomLODScaling), 
  /// with no FOV dependency.
  /// This value can also be used inside shaders to scale fade distances. This scalar is automatically tracked for the active context. In DX9-style register layout
  /// it is tracked into register c[24].w (Context clip plane register) and in DX11 it is tracked into c[12].w of b0. See constant table in shader editor.
  ///
  /// \returns
  ///  The scalar factor that should be used to scale the camera distance that is used for LOD calculations.
  inline float GetLODDistanceScaling() const
  {
    return m_fLODCorrection;
  }

  /// \brief
  ///   A relative multiplier to apply to the LOD distance scaling value which is returned by function GetLODDistanceScaling and used by LOD calculations
  ///
  /// This value is 1.0 by default and allows for manually controlling the distance scaling factor, in addtion
  /// to scaling with FOV, which is already done internally. See function GetLODDistanceScaling.
  ///
  /// \param fScale
  ///   The new relative scaling factor (1.0 for default)
  VISION_APIFUNC void SetCustomLODScaling(float fScale);

  /// \brief
  ///   Sets the near and far clip distances of the render context
  /// 
  /// \param fNearClipDist
  ///   Near clip distance for this render context.
  ///
  /// \param fFarClipDist
  ///   Far clip distance for this render context.
  VISION_APIFUNC void SetClipPlanes(float fNearClipDist, float fFarClipDist);

  /// \brief
  ///   Returns the near and far clip distances of the render context
  /// 
  /// \param fNearClipDist
  ///   Reference that receives the current near clip distance
  ///
  /// \param fFarClipDist
  ///   Reference that receives the current far clip distance
  VISION_APIFUNC void GetClipPlanes(float &fNearClipDist, float &fFarClipDist) const;

  /// \brief
  ///   Fills in 3 planes to be used for projections
  /// 
  /// All planes pass through the context camera's origin and point into the respective projection
  /// direction.
  /// 
  /// The planeRight and planeUp take the FOV into account, Their plane normal is not normalized,
  /// but rather has the length 1.f/hkvMath::tanRad (fov).
  /// 
  /// Thus the planes are set up in a way that the texture coordinate projection can be performed
  /// in a straightforward right/dir, up/dir way.
  /// 
  /// \param planeRight
  ///   Plane that will receive the plane with a normal that points in the camera's right direction
  /// 
  /// \param planeUp
  ///   Plane that will receive the plane with a normal that points in the camera's up direction
  /// 
  /// \param planeDir
  ///   Plane that will receive the plane with a normal that points in the camera's view direction
  VISION_APIFUNC void GetProjectionPlanes(hkvPlane& planeRight,hkvPlane& planeUp,hkvPlane& planeDir) const;

  /// \brief
  ///   Projects a 3D world position to 2D screen coordinates using the FOV and the camera position
  ///   of this context
  /// 
  /// This function can for instance be used to display 2D text next to 3D positions.
  /// 
  /// \param pos
  ///   Absolute 3D position to be transformed
  /// 
  /// \param x2d
  ///   Reference that receives the projected on-screen x-position. Only valid if function returns
  ///   TRUE.
  /// 
  /// \param y2d
  ///   Reference that receives the projected on-screen y-position. Only valid if function returns
  ///   TRUE.
  /// 
  /// \return
  ///   BOOL bResult: FALSE if the 3D position is behind the camera and thus cannot be projected.
  ///   The function succeeds even if the projected 2D position is outside the viewport, e.g. x2d<0
  VISION_APIFUNC BOOL Project2D(const hkvVec3& pos, float &x2d, float &y2d) const;


  /// \brief
  ///   Projects a 3D world position to 2D screen coordinates using the FOV and the camera position
  ///   of this context
  /// 
  /// This function can for instance be used to display 2D text next to 3D positions.
  /// 
  /// This overload in addition computes the z-coordinate in world units. This value can be passed
  /// to the IVRender2DInterface::SetDepth function
  /// 
  /// \param pos
  ///   Absolute 3D position to be transformed
  /// 
  /// \param x2d
  ///   Reference that receives the projected on-screen x-position. Only valid if function returns
  ///   TRUE.
  /// 
  /// \param y2d
  ///   Reference that receives the projected on-screen y-position. Only valid if function returns
  ///   TRUE.
  /// 
  /// \param zcoord
  ///   Reference that receives the view-space depth. Only valid if function returns
  ///   TRUE.
  /// 
  /// \return
  ///   BOOL bResult: FALSE if the 3D position is behind the camera and thus cannot be projected.
  ///   The function succeeds even if the projected 2D position is outside the viewport, e.g. x2d<0
  VISION_APIFUNC BOOL Project2D(const hkvVec3& pos, float &x2d, float &y2d, float &zcoord) const;

  /// \brief
  ///   This version of Project2D takes a camera rotation matrix and -position that might be
  ///   different from the actual context camera's transformation
  VISION_APIFUNC BOOL Project2D(const hkvVec3& pos, float &x2d, float &y2d, float &zcoord, const hkvVec3& cameraPos, const hkvMat3& cameraRotation) const;

  /// \brief
  ///   Creates a 3D direction vector in world space from 2D screen position
  ///
  /// considering the camera orientation of this context.
  /// 
  /// This helper function is useful if you want to perform tracelines in the direction of the 2D
  /// mouse cursor.
  /// 
  /// All positions generated from the camera position and this direction are projected to the same
  /// 2D position again using VisGame_cl::Project2D.
  /// 
  /// Note that any line rendered with VisGame_cl::DrawSingleLine and that starts at the camera
  /// position will not be visible, since it's collapsed to a single point on screen.
  /// 
  /// Also note that in orthographic projection modes this direction is always just the camera
  /// direction
  /// 
  /// \param fScreenX
  ///   screen x position, e.g. [0..1024]
  /// 
  /// \param fScreenY
  ///   screen y position, e.g. [0..768]
  /// 
  /// \param destDir
  ///   vector reference that receives the direction
  /// 
  /// \param fLen
  ///   target length of the direction vector
  /// 
  /// \param pStartPos
  ///   Optional position that receives the position on the near clip plane (important for
  ///   orthographic views)
  /// 
  /// \sa VisRenderContext_cl::Project2D
  /// 
  /// \example
  ///   \code
  ///   hkvVec3 traceStart = pCamera->GetPosition();
  ///   hkvVec3 traceDir;
  ///   Vision::Contexts.GetMainRenderContext()->GetTraceDirFromScreenPos(fMouseX,fMouseX,traceDir,1000.f);
  ///   hkvVec3 traceEnd = traceStart + traceDir;
  ///   \endcode
  VISION_APIFUNC void GetTraceDirFromScreenPos(float fScreenX, float fScreenY, hkvVec3& destDir, float fLen=1.f, hkvVec3* pStartPos=NULL);

  /// \brief
  ///  Returns a view-space depth which is close to the far plane but guaranteed not to be far clipped after projection.
  ///
  /// This helper function assumes a depth buffer precision of 24b to guarantee that the distance of the returned
  /// depth from the far plane is greater than the depth buffer resolution at that point.
  ///
  /// \return
  ///  A depth value close to the far plane.
  VISION_APIFUNC float GetSafeFarPlaneDepth() const;

  ///
  /// @}
  ///


  ///
  /// @name Access Global Instances
  /// @{
  ///

  /// \brief
  ///   Returns the currently active render context
  /// 
  /// \return
  ///   VisRenderContext_cl *pContext: Pointer to the current render context
  VISION_APIFUNC static VisRenderContext_cl *GetCurrentContext(); 

  /// \brief
  ///   Returns the engine's main render context
  /// 
  /// The main render context is created when a display mode/window is initialized by the Vision
  /// engine. Its default render target is the back-buffer of the screen, its size and viewport
  /// correspond to the size of the display mode/window.
  /// 
  /// The main render context should not be deleted manually; it will automatically be
  /// deinitialized by the engine.
  /// 
  /// \return
  ///   VisRenderContext_cl *pContext: Pointer to the main render context
  VISION_APIFUNC static VisRenderContext_cl *GetMainRenderContext(); 


  ///
  /// @}
  ///

  ///
  /// @name Render Target / Render Properties
  /// @{
  ///
  
  /// \brief
  ///  True if this render context renders into the device back buffer.
  VISION_APIFUNC bool RendersIntoBackBuffer() const
  {
    return m_bRendersIntoBackBuffer;
  }
  
  /// \brief
  ///   Function that returns the render target texture of this context
  /// 
  /// The render target can be set using the SetRenderTarget method.
  /// 
  /// Note that if this function returns NULL, this only means that there is no renderable texture
  /// set as a render target for this context. If e.g. VisRenderContext_cl::SetRenderSurface has
  /// been used to directly set a DirectX surface as render target, this method will return NULL.
  /// Similarly, calling this method on the main render context will also return NULL.
  /// 
  /// In order to determine whether the returned object is a renderable cube map, a renderable 2D
  /// texture, or a renderable 2D texture array, simply check the type of the returned
  /// VTextureObject (GetTextureType), or call IsCubemap() and IsTexture2DArray() or it. You may
  /// then cast the object to the respective target type (e.g. VisRenderableCubemap_cl,
  /// VisRenderableTexture_cl).
  /// 
  /// \param iRenderTargetIndex
  ///   Render Target index for which to return the render target texture (default is 0).
  /// 
  /// \param pFaceOrIndex
  ///   If not NULL and the render target texture is a cube map, this will contain the face index
  ///   of the cube map which is set as the render target for this context. If not NULL and the
  ///   render target is a 2D texture array, this will contain the array index of the texture array
  ///   which is set as the render target for this context. In DirectX10/11, if the render target is a
  ///   cube map or array texture and this value is -1, this means that the render target is not a
  ///   specific face or slice of the cubemap/array texture, but it is the complete texture object.
  ///   In this case, the face or array index to render to can be chosen in the geometry shader.
  /// 
  /// \return
  ///   VTextureObject *: NULL or the offscreen render target (renderable texture)
  VISION_APIFUNC VTextureObject *GetRenderTarget(int iRenderTargetIndex = 0, int *pFaceOrIndex=NULL) const;

  /// \brief
  ///   Function that returns the depth stencil target of this context
  /// 
  /// The depth stencil target can be set using the SetDepthStencilTarget method.
  /// 
  /// Note that if this function returns NULL, this only means that there is no renderable texture
  /// set as a depth stencil target for this context. If e.g.
  /// VisRenderContext_cl::SetDepthStencilSurface has been used to directly set a DirectX surface
  /// as depth stencil target, this method will return NULL. Similarly, calling this method on the
  /// main render context will also return NULL.
  /// 
  /// In order to determine whether the returned object is a renderable cube map, a renderable 2D
  /// texture, or a renderable 2D texture array, simply check the type of the returned
  /// VTextureObject (GetTextureType), or call IsCubemap() and IsTexture2DArray() or it. You may
  /// then cast the object to the respective target type (e.g. VisRenderableCubemap_cl,
  /// VisRenderableTexture_cl).
  /// 
  /// \param pFaceOrIndex
  ///   If not NULL and the depth stencil texture is a cube map, this will contain the face index
  ///   of the cube map which is set as the depth stencil target for this context. If not NULL and
  ///   the depth stencil target is a 2D texture array, this will contain the array index of the
  ///   texture array which is set as the depth stencil target for this context. In DirectX10/11, if
  ///   the depth stencil target is a cube map or array texture and this value is -1, this means
  ///   that the depth stencil target is not a specific face or slice of the cubemap/array texture,
  ///   but it is the complete texture object. In this case, the face or array index to render to
  ///   can be chosen in the geometry shader.
  /// 
  /// \return
  ///   VTextureObject *: NULL or the offscreen render target (renderable texture)
  VISION_APIFUNC VTextureObject *GetDepthStencilTarget(int *pFaceOrIndex=NULL) const;

  /// \brief
  ///   Method to set the render target(s) for this context
  /// 
  /// This function only sets the color buffers of the render target; the depth/stencil target may
  /// be set with SetDepthStencilTarget.
  /// 
  /// This function will also adjust the viewport to the render target's size.
  /// 
  /// Note that when using multiple render targets, all render targets have to have the same size.
  /// 
  /// Note that the passed renderable texture has to be in a valid target format for color buffer
  /// rendering (the exact requirements depend on the target platform, hardware, and drivers). 
  /// 
  /// Some platforms/hardware configurations may add additional requirements, such as all
  /// simultaneous render targets having to be in the same format.
  /// 
  /// The number of simultaneous render targets supported depends on the platform/hardware
  /// configuration.
  /// 
  /// Is is valid to specify NULL as pTexObject to disable a specific render target; however, note
  /// that the DirectX9 renderer does not allow setting the render target for index 0 to NULL.
  /// 
  /// Note that cube map render targets have to be set using the SetRenderTargetCubeMap method
  /// instead. 2D Array Texture render targets have to be set using the
  /// SetRenderTarget2DArrayTexture method (DirectX10/11 only).
  /// 
  /// \param iRenderTargetIndex
  ///   Index of the render target to set
  /// 
  /// \param pTexObject
  ///   Has to be either NULL or a renderable texture object.
  /// 
  /// \return
  ///   bool: true if setting the render target was successful
  VISION_APIFUNC bool SetRenderTarget(int iRenderTargetIndex, const VisRenderableTexture_cl *pTexObject);

  /// \brief
  ///   Method to set a renderable cube map as a render target for this context.
  /// 
  /// This function will also adjust the viewport to the render target's size.
  /// 
  /// Note that when using multiple render targets, all render targets have to have the same size.
  /// 
  /// Note that the passed renderable cubemap has to be in a valid target format for color buffer
  /// rendering (the exact requirements depend on the target platform, hardware, and drivers). 
  /// 
  /// Some platforms/hardware configurations may add additional requirements, such as all
  /// simultaneous render targets having to be in the same format.
  /// 
  /// The number of simultaneous render targets supported depends on the platform/hardware
  /// configuration.
  /// 
  /// Is is valid to specify NULL as pTexObject to disable a specific render target; however, note
  /// that the DirectX9 renderer does not allow setting the render target for index 0 to NULL.
  /// 
  /// Note that 2D render targets have to be set using the SetRenderTarget method instead. 2D Array
  /// Texture render targets have to be set using the SetRenderTarget2DArrayTexture method
  /// (DirectX10/11 only).
  /// 
  /// \param iRenderTargetIndex
  ///   Index of the render target to set
  /// 
  /// \param pTexObject
  ///   Has to be either NULL or a renderable cube map object.
  /// 
  /// \param iFace
  ///   The face index of the cube map to render to (e.g. VTM_CUBEMAPFACE_POS_X). In DirectX10/11,
  ///   this can be -1 to indicate that the whole cube map should be used as render target. In this
  ///   case, the geometry shader can be used to select the face to render to.
  /// 
  /// \return
  ///   bool: true if setting the render target was successful
  VISION_APIFUNC bool SetRenderTargetCubeMap(int iRenderTargetIndex, const VisRenderableCubeMap_cl *pTexObject, int iFace);

#if defined(_VR_DX11)

  /// \brief
  ///   Method to set a renderable 2d array texture as a render target for this context.
  /// 
  /// This method is only available in DirectX10/DirectX11.
  /// 
  /// This function will also adjust the viewport to the render target's size.
  /// 
  /// Note that when using multiple render targets, all render targets have to have the same size.
  /// 
  /// Note that the passed renderable 2D array texture has to be in a valid target format for color
  /// buffer rendering (the exact requirements depend on the target platform, hardware, and
  /// drivers). 
  /// 
  /// Some platforms/hardware configurations may add additional requirements, such as all
  /// simultaneous render targets having to be in the same format.
  /// 
  /// The number of simultaneous render targets supported depends on the platform/hardware
  /// configuration.
  /// 
  /// Is is valid to specify NULL as pTexObject to disable a specific render target.
  /// 
  /// Note that 2D render targets have to be set using the SetRenderTarget method instead. Cube map
  /// render targets have to be set using the SetRenderTargetCubeMap method.
  /// 
  /// \param iRenderTargetIndex
  ///   Index of the render target to set
  /// 
  /// \param pTexObject
  ///   Has to be either NULL or a renderable cube map object.
  /// 
  /// \param iArrayIndex
  ///   The array index of the texture array to render to. This can be -1 to indicate that the
  ///   whole texture array should be used as render target. In this case, the geometry shader can
  ///   be used to select the index to render to.
  /// 
  /// \return
  ///   bool: true if setting the render target was successful
  VISION_APIFUNC bool SetRenderTarget2DArrayTexture(int iRenderTargetIndex, const VisRenderableTexture2DArray_cl *pTexObject, int iArrayIndex);

#endif

  /// \brief
  ///   Function to set the depth-stencil target for this context
  /// 
  /// Note that the depths stencil target usually has to have the same size as the color buffer
  /// that is being rendered to.
  /// 
  /// Note that the passed VisRenderableTexture_cl has to be in a valid depth-stencil target format
  /// (the exact requirements depend on the target platform, hardware, and drivers). 
  /// 
  /// It is valid to specify NULL for pTexObject.
  /// 
  /// \param pTexObject
  ///   Has to be either NULL or a renderable texture object.
  /// 
  /// \return
  ///   bool: true if setting the render target was successful
  VISION_APIFUNC bool SetDepthStencilTarget(const VisRenderableTexture_cl *pTexObject);

  /// \brief
  ///   Function to set the depth-stencil target for this context
  /// 
  /// Note that the depths stencil target usually has to have the same size as the color buffer
  /// that is being rendered to.
  /// 
  /// Note that the passed VisRenderableCubeMap_cl has to be in a valid depth-stencil target format
  /// (the exact requirements depend on the target platform, hardware, and drivers). 
  /// 
  /// It is valid to specify NULL for pTexObject.
  /// 
  /// \param pTexObject
  ///   Has to be either NULL or a renderable cube map object
  /// 
  /// \param iFace
  ///   The face index of the cube map to render to (e.g. VTM_CUBEMAPFACE_POS_X). In DirectX10/11,
  ///   this can be -1 to indicate that the whole cube map should be used as render target. In this
  ///   case, the geometry shader can be used to select the face to render to.
  /// 
  /// \return
  ///   bool: true if setting the render target was successful
  VISION_APIFUNC bool SetDepthStencilCubeMap(const VisRenderableCubeMap_cl *pTexObject, int iFace);

#if defined(_VR_DX11)

  /// \brief
  ///   Function to set the depth-stencil target for this context
  /// 
  /// This method is available in DirectX10/11 only.
  /// 
  /// Note that the depth-stencil target usually has to have the same size as the color buffer that
  /// is being rendered to.
  /// 
  /// Note that the passed VisRenderableTexture2DArray_cl has to be in a valid depth-stencil target
  /// format (the exact requirements depend on the target platform, hardware, and drivers). 
  /// 
  /// It is valid to specify NULL for pTexObject.
  /// 
  /// \param pTexObject
  ///   Has to be either NULL or a renderable 2D texture array object.
  /// 
  /// \param iArrayIndex
  ///   The array index of the cube map to render to (e.g. VTM_CUBEMAPFACE_POS_X). In DirectX10/11,
  ///   this can be -1 to indicate that the whole cube map should be used as render target. In this
  ///   case, the geometry shader can be used to select the face to render to.
  /// 
  /// \return
  ///   bool: true if setting the render target was successful
  VISION_APIFUNC bool SetDepthStencil2DArrayTexture(const VisRenderableTexture2DArray_cl *pTexObject, int iArrayIndex);

#endif

  /// \brief
  ///   Function to get all color and depth stencil targets from a different render context.
  /// 
  /// This will duplicate the render target settings from another render context. Useful to quickly
  /// set up different render contexts with the same render targets, or to duplicate the render
  /// target settings of the main render context.
  /// 
  /// \param pRefContext
  ///   Reference context to copy the settings from.
  VISION_APIFUNC void SetRenderAndDepthStencilTargets(const VisRenderContext_cl *pRefContext);

  /// \brief
  ///   Returns the number of active render targets in this render context.
  VISION_APIFUNC unsigned int GetNumRenderTargets() const;


  ///
  /// @}
  ///

  ///
  /// @name Render Flags
  /// @{
  ///

  /// \brief
  ///   Function to set the render flags for this context
  /// 
  /// Do not call this function inside a render loop.
  /// 
  /// \param iFlags
  ///   Set of render flags that may be provided for render contexts.
  ///   See enum VRenderContextFlags.
  VISION_APIFUNC void SetRenderFlags(int iFlags);

  /// \brief
  ///   Returns the context's render flags
  /// 
  /// \return
  ///   int: Render flags.
  /// 
  /// \sa SetRenderFlags
  inline int GetRenderFlags() const {return m_iRenderFlags;}


  ///
  /// @}
  ///

  ///
  /// @name Usage Hints
  /// @{
  ///

  /// \brief
  ///   Indicates whether the VIS_RENDERCONTEXT_FLAG_VIEWCONTEXT bitflag is set for this context
  inline BOOL IsViewContext() const {return (m_iRenderFlags&VIS_RENDERCONTEXT_FLAG_VIEWCONTEXT)>0;}


  /// \brief
  ///   Sets a usage hint for this context.
  /// 
  /// This value is ignored by the core engine. It helps plugin code to identify the usage type of
  /// a context.
  /// 
  /// For instance, if custom rendering code renders into a context, it can respond to the usage
  /// type and for instance render with a special depth shader when the type of the context is set
  /// to VIS_CONTEXTUSAGE_DEPTHSHADOW.
  /// 
  /// The default value for new contexts is VIS_CONTEXTUSAGE_VIEWCONTEXT, i.e. the context is
  /// supposed to be a a context that renders the scene elements in a standard way.
  /// 
  /// Supplier contexts that are for instance used to scale down the glow result should be set to
  /// VIS_CONTEXTUSAGE_NONE.
  /// 
  /// \param value
  ///   The new usage type. See VisContextUsageHint_e
  inline void SetUsageHint(VisContextUsageHint_e value) {m_eUsageHint=value;}

  /// \brief
  ///   Returns the value set via SetUsageHint
  inline VisContextUsageHint_e GetUsageHint() const {return m_eUsageHint;}


  ///
  /// @}
  ///

  ///
  /// @name Render Loop
  /// @{
  ///

  /// \brief
  ///   Sets the render loop of this render context
  /// 
  /// Render contexts may have their own render loop implementation. The render loop can be set
  /// using this method.
  /// 
  /// By default, render contexts use the Vision engine's default render loop (VisionRenderLoop).
  /// 
  /// \param pRenderLoop
  ///   Render loop for this render context
  VISION_APIFUNC void SetRenderLoop(IVisRenderLoop_cl *pRenderLoop);

  /// \brief
  ///   Gets the render loop for this render context
  /// 
  /// \return
  ///   IVisRenderLoop_cl *: Render loop for this render context
  VISION_APIFUNC IVisRenderLoop_cl *GetRenderLoop() const;

  ///
  /// @}
  ///

  ///
  /// @name Filter Bitmasks
  /// @{
  ///

  /// \brief
  ///   Sets the shader render flags for this render context
  /// 
  /// A shader will only be rendered if an and-operation of the shader's render flags and the
  /// render context's shader flags is not zero.
  /// 
  /// \param iFlags
  ///   Shader render flags
  /// 
  /// \sa VisRenderContext_cl::SetRenderFilterMask
  inline void SetShaderFlags(int iFlags) {m_iShaderFlags = iFlags;}

  /// \brief
  ///   Returns the shader render flags for this render context
  /// 
  /// \return
  ///   int: Shader render flags
  inline int GetShaderFlags() const {return m_iShaderFlags;}

  /// \brief
  ///   Sets a render filter bitmask for various geometry types
  /// 
  /// Affected scene elements are only rendered in this context if the result of an AND operation
  /// of this bitmask and the object's bitmask is not zero.
  /// 
  /// Affected geometry types are:
  /// \li static geometry instances (use VisStaticGeometryInstance_cl::SetVisibleBitmask)
  /// 
  /// \li entities (use VisBaseEntity_cl::SetVisibleBitmask),
  /// 
  /// \li screenmasks (use VisScreenMask_cl::SetVisibleBitmask),
  /// 
  /// \li screenmask single mask frames (VisMaskFrame_t::iVisibleBitmask member),
  /// 
  /// \li particle groups (use VisParticleGroup_cl::SetVisibleBitmask)
  /// 
  /// \li mesh buffer objects (use VisMeshBufferObject_cl::SetVisibleBitmask)
  /// 
  /// \param iBitMask
  ///   The bitmask against which to test the scene elements' mask.
  /// 
  /// \sa VisStaticGeometryInstance_cl::SetVisibleBitmask
  /// \sa VisBaseEntity_cl::SetVisibleBitmask
  /// \sa VisScreenMask_cl::SetVisibleBitmask
  /// \sa VisParticleGroup_cl::SetVisibleBitmask
  /// \sa VisMeshBufferObject_cl::SetVisibleBitmask
  /// \sa VisRenderContext_cl::GetRenderFilterMask
  /// \sa VisRenderContext_cl::SetShaderFlags
  inline void SetRenderFilterMask(unsigned int iBitMask) {m_iRenderFilter=iBitMask;}

  /// \brief
  ///   Returns the render filter bitmask of this context
  /// 
  /// \return
  ///   unsigned int iBitMask : The bitmask against which to test scene elements while rendering
  /// 
  /// \sa VisRenderContext_cl::SetRenderFilterMask
  inline unsigned int GetRenderFilterMask() const {return m_iRenderFilter;}


  ///
  /// @}
  ///

  ///
  /// @name Matrices
  /// @{
  ///

  /// \brief
  ///   Returns a reference to the current projection matrix
  /// 
  /// The projection matrix is generated from the current view frustum and thus affected by clip
  /// planes and FOV, and includes flipping of the y-coordinate on OpenGL platforms when rendering to a texture.
  ///
  /// To retrieve the raw (non y-corrected) projection matrix, use GetViewProperties()->getProjectionMatrix(hkvClipSpaceYRange::MinusOneToOne).
  /// 
  /// \return
  ///   hkvMat4& matrix : The context specific 4x4 projection matrix
  /// 
  /// \sa VisRenderContext_cl::SetFOV
  /// \sa VisRenderContext_cl::SetClipPlanes
  /// \sa VisContextCamera_cl::GetInverseEyeMatrix
  VISION_APIFUNC const hkvMat4& getProjectionMatrix() const;

  /// \brief DEPRECATED: Use 'getProjectionMatrix' instead, but don't forget to transpose the used matrix (or rather, remove the transpose).
  HKV_DEPRECATED_2012_3 VISION_APIFUNC const hkvMat4 GetProjectionMatrix() { return getProjectionMatrix().getTransposed(); }

  /// \brief
  ///   Allows setting a custom projection matrix for rendering the scene in this context.
  /// 
  /// This can be very useful for creating a custom clip plane, e.g. for water rendering.
  /// 
  /// The custom projection matrix will be used for rendering, but not for visibility
  /// determination.
  /// 
  /// Instead, visibility will always be determined by the FOV and clip planes.
  /// 
  /// To reset the context to the default projection matrix (which is computed from the FOV and the
  /// clip planes), a NULL pointer may be passed to this function.
  /// 
  /// If this function is called with a matrix!=NULL, then the projection type is set to
  /// VIS_PROJECTIONTYPE_CUSTOM.
  /// 
  /// \param pMatrix
  ///   Pointer to the new projection matrix. The matrix will be copied rather than referenced. A
  ///   NULL pointer can be used to reset to the default projection matrix.
  /// 
  /// \param bAutoFrustum
  //    If true, the frustum will be automatically extracted from the projection matrix.
  //
  /// \sa VisRenderContext_cl::SetFOV
  /// \sa VisRenderContext_cl::SetClipPlanes
  /// \sa VisContextCamera_cl::GetInverseEyeMatrix
  VISION_APIFUNC void SetCustomProjectionMatrix(const hkvMat4* pMatrix, bool bAutoFrustum = true);

  /// \brief
  ///   Sets a new pre-defined projection matrix type
  /// 
  /// If the type is VIS_PROJECTIONTYPE_PERSPECTIVE or VIS_PROJECTIONTYPE_ORTHOGRAPHIC then the
  /// context takes care of creating the appropriate projection matrix. These two modes are also
  /// gracefully handled by the standard visibility implementations.
  /// 
  /// GetProjectionMatrix returns the projection matrix for the respective mode.
  /// 
  /// The default is mode for every new context is VIS_PROJECTIONTYPE_PERSPECTIVE.
  /// 
  /// The VIS_PROJECTIONTYPE_ORTHOGRAPHIC mode can be used for top views (used in vForge for
  /// instance)
  /// 
  /// \param eType
  ///   The new mode constant. The following modes are supported:
  ///   \li VIS_PROJECTIONTYPE_PERSPECTIVE: A perspective projection matrix is used defined by clip
  ///     planes and FOV of this context.
  /// 
  ///   \li VIS_PROJECTIONTYPE_ORTHOGRAPHIC: An orthographic projection matrix is used defined by
  ///     the orthographic size of this context.
  /// 
  ///   \li VIS_PROJECTIONTYPE_CUSTOM: The projection matrix is a custom matrix that needs to be
  ///     set via SetCustomProjectionMatrix.
  /// 
  /// \sa VisRenderContext_cl::SetFOV
  /// \sa VisRenderContext_cl::SetClipPlanes
  /// \sa VisContextCamera_cl::GetInverseEyeMatrix
  inline void SetProjectionType(VisProjectionType_e eType)
  {
    m_spViewProperties->setProjectionType(eType);
  }

  /// \brief
  ///   Returns the projection type (VisProjectionType_e) of this context. See SetProjectionType
  inline VisProjectionType_e GetProjectionType() const 
  {
    return m_spViewProperties->getProjectionType();
  }

  /// \brief
  ///   This function must be called when the projection matrix has changed within the render loop and the changes should take place immediately.
  /// 
  /// It re-builds the projection matrix and re-uploads the shader constants (projection and modelview projection). This is necessary when the projection matrix
  /// changes inside the execution of a renderloop through calls of SetFOV or SetClipPlanes. It is not necessary to call this function
  /// in normal cases where changing projection settings should be applied in the next frame. Accordingly this function asserts when called
  /// from outside a render loop.
  VISION_APIFUNC void ApplyProjectionMatrixChanges();

  /// \brief
  ///   Sets the orthographic size for this context. can be set to screen's size to get a 1:1
  ///   mapping. Only affects the VIS_PROJECTIONTYPE_ORTHOGRAPHIC mode.
  VISION_APIFUNC void SetOrthographicSize(float fWidth, float fHeight);

  /// \brief
  ///   Get the current orthographic sizes
  inline void GetOrthographicSize(float &fWidth, float &fHeight) const
  {
    fWidth = m_spViewProperties->getOrthographicWidth();
    fHeight = m_spViewProperties->getOrthographicHeight();
  }

  /// \brief
  ///   Helper function to build a 4x4 projection matrix for the passed projection type.
  VISION_APIFUNC void ComputeProjectionMatrix(hkvMat4& matrix, VisProjectionType_e projectionType) const;
 
  
 
  ///
  /// @}
  ///

  ///
  /// @name Debug Rendering
  /// @{
  ///

  /// \brief
  ///   Returns a render interface for debug rendering for the current context.
  /// 
  /// The render interface provides useful functionality to render debug geometry such as lines or
  /// single triangles.
  /// 
  /// The render interface can be passed to objects that want to render themselves e.g. as an
  /// editor preview. This rendering is only performed into this context. To render lines into all contexts at the same time,
  /// a global interface can be retrieved via Vision::Game.GetDebugRenderInterface().
  /// 
  /// \return
  ///   IVRenderInterface* pRenderer : pointer to render interface.
  /// 
  /// \sa IVRenderInterface
  /// 
  /// \example
  ///   \code
  ///   pBox->RenderSelection(Vision::Contexts.GetCurrentContext()->GetRenderInterface());
  ///   \endcode
  VISION_APIFUNC IVRenderInterface* GetRenderInterface();


  ///
  /// @}
  ///

  ///
  /// @name Resolving
  /// @{
  ///

  /// \brief
  ///   Resolves the current render target contents to the passed texture object, using the data
  ///   of this render context.
  /// 
  /// Resolving requires that the relevant properties of the target texture (e.g. format and
  /// dimensions) match those of the currently set render target.
  /// 
  /// Note that for renderable textures which support "internal" resolving
  /// (VisRenderableTextureConfig_t::m_bResolve set to true) and therefore have an internal resolve
  /// target, you have to use the Resolve method of the respective renderable texture / cube map /
  /// texture array instead.
  /// 
  /// This method is intended for manual resolve operations.
  /// 
  /// \param pTexObj
  ///   Target texture object. This texture object will contain a copy of the render target
  ///   contents after a successful resolve operation. If the render target is a  multi-sampled
  ///   render target, resolving will automatically transform the data into a non-multi-sampled
  ///   target, which is required for sampling the target texture. The texture object may be one of
  ///   the derived types (VTextureCubeObject, VTexture2DArrayObject, but NOT VTexture3DObject)!
  /// 
  /// \param bDepthTexture
  ///   true if the depth stencil buffer should be resolved, false if the color buffer should be
  ///   resolved.
  /// 
  /// \param iRenderTargetIndex
  ///   Index of the render target to be resolved
  /// 
  /// \param iXOfs
  ///   horizontal offset where resolving should start. Notice that there are limits regarding the
  ///   legal offset values. For Xbox360, consult the description of the Resolve operation in the
  ///   Xbox360 manual for details. 
  /// 
  /// \param iYOfs
  ///   vertical offset where resolving should start. Notice that there are limits regarding the
  ///   legal offset values. For Xbox360, consult the description of the Resolve operation in the
  ///   Xbox360 manual for details. 
  /// 
  /// \param iWidth
  ///   width of the area to resolve. If 0 is specified, the context's width is used.
  /// 
  /// \param iHeight
  ///   height of the area to resolve. If 0 is specified, the context's height is used.
  /// 
  /// \param iLevel
  ///   Mip Level of the texture to resolve to.
  /// 
  /// \param iFaceOrSlice
  ///   If the resolve target is a cubemap, this specifies the face of the cube map to resolve to.
  ///   If the resolve target is a 2D texture array, this specifies the slice (array element) of
  ///   the texture array to resolve.
  // TODO: DOKU
  VISION_APIFUNC void ResolveToTexture(VTextureObject *pTexObj, bool bDepthTexture=false, unsigned int iRenderTargetIndex=0, unsigned int iXOfs=0, unsigned int iYOfs=0, unsigned int iWidth=0, unsigned int iHeight=0, unsigned int iLevel=0, unsigned int iFaceOrSlice=0);



#ifdef _VISION_XENON  

  /// \brief
  ///   Enables/disables automatic setting of tiling brackets for this context
  /// 
  /// By default, Vision automatically sets a tiling bracket around each call to OnDoRenderLoop. In
  /// some cases, this may not be desired, e.g. for deferred rendering scenarios.
  /// 
  /// This method can be used to switch off automatic insertion of tiling brackets for individual
  /// render contexts.
  /// 
  /// This method is only available on Xbox360.
  /// 
  /// \param bAutoTiling
  ///   true to enable (default), false to disable automatic tiling.
  inline void SetAutoTiling(bool bAutoTiling) { m_bAutoTiling = bAutoTiling; }

  /// \brief
  ///   Returns whether automatic tiling is currently enabled (default) or disabled.
  inline bool GetAutoTiling() const { return m_bAutoTiling; }

  // Currently unsupported; usage is discouraged
  VISION_APIFUNC void SetDepthFillPass(bool bStatus);
  VISION_APIFUNC bool GetDepthFillPass();
#endif



  ///
  /// @}
  ///

  ///
  /// @name Updating / Rendering Count and Priority
  /// @{
  ///

  /// \brief
  ///   Specifies whether this context has "recently" been rendered
  /// 
  /// This method is important for determining which contexts are currently relevant. When using
  /// the VisionApp::Run method, SetRecentlyRendered will automatically be called on all registered
  /// context. This way, the engine can determine whether a context has been rendered since the
  /// last tick, and only update the information required for the relevant contexts.
  /// 
  /// If you are not calling VisionApp::Run, it is up to you to call this method manually.
  /// 
  /// \param bStatus
  ///   true if the context is currently relevant, otherwise false.
  /// 
  /// \sa WasRecentlyRendered
  VISION_APIFUNC void SetRecentlyRendered(bool bStatus);

  /// \brief
  ///   Returns the value previously set with SetRecentlyRendered
  /// 
  /// \return
  ///   bool: true if the context is currently relevant, otherwise false.
  /// 
  /// \sa SetRecentlyRendered
  VISION_APIFUNC bool WasRecentlyRendered() const;

  /// \brief
  ///   Sets the render context's render priority
  /// 
  /// The priority can be used to ensure a specific render order of render contexts which depend on
  /// each other. For instance, a shadow context will typically have a lower priority than the main
  /// scene context, since the shadow results are needed for rendering the scene and lower
  /// priorities are rendered first.
  /// 
  /// The priorities are taken into account by the VisRenderContextManager_cl class, which
  /// automatically resorts contexts based on their priority. The resulting list is taken as input
  /// by the VisionApp_cl::Run method, which iterates over the render contexts and renders them.
  /// Since your application does not have to use the VisionApp_cl class, you can naturally
  /// implement similar functionality yourself.
  /// 
  /// The default priority is 0.0f
  /// 
  /// \param fPriority
  ///   Render priority of the render context
  VISION_APIFUNC void SetPriority(float fPriority);

  /// \brief
  ///   Returns the render context's render priority
  /// 
  /// The priority has been previously set with SetPriority.
  /// 
  /// \return
  ///   float fPriority: Render priority of the render context
  inline float GetPriority() const { return m_fPriority; }

  /// \brief
  ///   Enables/disables rendering for this render context
  /// 
  /// This state is only relevant when using the VisionApp_cl::Run method. This method checks
  /// whether rendering is enabled for each render context, and only activates and renders it if
  /// this is the case. Otherwise, the render context is simply ignored.
  /// 
  /// In case you are not using the VisionApp_cl class and still want to disable/enable rendering
  /// to specific render contexts using this method, you have to query the state manually using the
  /// GetRenderingEnabled function.
  /// 
  /// \param bStatus
  ///   true to enable rendering, false to disable it
  VISION_APIFUNC void SetRenderingEnabled(bool bStatus) { m_bRenderingEnabled = bStatus; }

  /// \brief
  ///   Returns whether rendering is enabled for this render context
  /// 
  /// \return
  ///   bool bStatus: true to enable rendering, false to disable it
  VISION_APIFUNC bool GetRenderingEnabled() const { return m_bRenderingEnabled; }

  /// \brief
  ///   Sets, whether wireframe-mode should be ignored for rendering in this render context
  /// 
  /// \param bIgnoreWireframe
  ///    true, if the wireframe-mode should be ignored, otherwise false
  VISION_APIFUNC void SetIgnoreWireframe(bool bIgnoreWireframe) {m_bIgnoreWireframe = bIgnoreWireframe; }

  /// \brief
  ///   Returns whether wireframe-mode is ignored for rendering in this render context
  VISION_APIFUNC bool IsWireframeIgnored() const { return m_bIgnoreWireframe; }

  ///
  /// @}
  ///

  ///
  /// @name Low-level Platform-specific Render Target Handling
  /// @{
  ///

#ifdef _VR_DX9

  /// \brief
  ///   Directly sets the render target surface to a specified Direct3D surface
  /// 
  /// For reasons of portability, it is generally preferred to use the SetRenderTarget method.
  /// 
  /// You will have to make sure yourself that the surface actually supports being rendered to.
  /// 
  /// \param iRenderTargetIndex
  ///   Index of the render target to set
  /// 
  /// \param pColor
  ///   color render target surface this context should render to
  /// 
  /// \platforms
  ///   DX9, Xbox360
  ///
  /// \sa VisRenderContext_cl::SetRenderTarget
  VISION_APIFUNC void SetRenderSurface(int iRenderTargetIndex, D3DSurface *pColor);

  /// \brief
  ///   Returns the render surface previously set with SetRenderSurface
  /// 
  /// \param iRenderTargetIndex
  ///   Index of the render target
  /// 
  /// \param pColor
  ///   reference to a pointer to the color render target surface this context renders to
  /// 
  /// \platforms
  ///   DX9, Xbox360
  ///
  /// \sa VisRenderContext_cl::GetRenderTarget
  VISION_APIFUNC void GetRenderSurface(int iRenderTargetIndex, D3DSurface *&pColor);

  /// \brief
  ///   Directly sets the depth stencil surface to a specified Direct3D surface
  /// 
  /// For reasons of portability, it is generally preferred to use the SetDepthStencilTarget
  /// method.
  /// 
  /// You will have to make sure yourself that the surface actually supports being rendered to.
  /// 
  /// \param pDepthStencil
  ///   depth stencil render target surface this context should render to
  /// 
  /// \platforms
  ///   DX9, Xbox360
  ///
  /// \sa VisRenderContext_cl::SetDepthStencilTarget
  VISION_APIFUNC void SetDepthStencilSurface(D3DSurface *pDepthStencil);

  /// \brief
  ///   Returns the depth stencil surface previously set with SetDepthStencilSurface
  /// 
  /// \param pDepthStencil
  ///   reference to a pointer to the depth stencil render  target surface this context renders to.
  /// 
  /// \platforms
  ///   DX9, Xbox360
  ///
  /// \sa VisRenderContext_cl::GetDepthStencilTarget
  VISION_APIFUNC void GetDepthStencilSurface(D3DSurface *&pDepthStencil);


#elif defined(_VR_DX11)

  /// \brief
  ///   Directly sets the render target to a specified render target view
  /// 
  /// For reasons of portability, it is generally preferred to use the SetRenderTarget method
  /// instead.
  /// 
  /// \param iRenderTargetIndex
  ///   Index of the render target to set
  /// 
  /// \param pColor
  ///   render target view this context should render to
  /// 
  /// \sa VisRenderContext_cl::SetRenderTarget
  VISION_APIFUNC void SetRenderTargetView(int iTargetIndex, D3DRenderTargetView *pColor);

  /// \brief
  ///   Returns the render target view previously set with SetRenderTargetView.
  VISION_APIFUNC void GetRenderTargetView(int iTargetIndex, D3DRenderTargetView *&pColor);

  /// \brief
  ///   Directly sets the depth stencil target to a specified depth stencil view
  /// 
  /// For reasons of portability, it is generally preferred to use the SetDepthStencilTarget method
  /// instead.
  /// 
  /// \param pColor
  ///   depth stencil view this context should render to
  /// 
  /// \sa VisRenderContext_cl::SetDepthStencilTarget
  VISION_APIFUNC void SetDepthStencilView(D3DDepthStencilView *pDepthStencil);

  /// \brief
  ///   Returns the depth stencil view previously set with SetDepthStencilView.
  VISION_APIFUNC void GetDepthStencilView(D3DDepthStencilView *&pDepthStencil);

#elif defined(_VR_OPENGL) || defined(_VR_GLES2)

  VISION_APIFUNC void SetRenderSurface(int iTargetIndex, unsigned int iColorTargetHandle, unsigned int iColorTextureHandle = 0);
  VISION_APIFUNC void GetRenderSurface(int iTargetIndex, unsigned int &iColorHandle);
  VISION_APIFUNC void SetDepthStencilSurface(unsigned int iDepthStencilHandle, bool bUseStencil=false, unsigned int iDepthStencilTextureHandle = 0);
  VISION_APIFUNC void GetDepthStencilSurface(unsigned int &iDepthStencilHandle);

  void CreateFrameBufferObject();
  void DeleteFrameBufferObject();
  static inline bool IsMultisamplingSupported() { return g_bMultisamplingSupported; }


#elif defined(_VR_GCM)
  VISION_APIFUNC void SetDepthStencilSurface(CellGcmTexture *pSurface);
  VISION_APIFUNC void SetRenderSurface(int iRenderTargetIndex, CellGcmTexture *pColorSurface, VVIDEO_Multisample eMultisample = VVIDEO_MULTISAMPLE_OFF);
  VISION_APIFUNC void GetDepthStencilSurface(CellGcmTexture *&pDepthStencilSurface);
  VISION_APIFUNC void GetRenderSurface(int iRenderTargetIndex, CellGcmTexture *&pColorSurface);
  VISION_APIFUNC bool UpdateAndValidateSurfaceConfiguration();
  VISION_APIFUNC void CopyRenderTargetConfig(CellGcmSurface& xOut) const { memcpy(&xOut,&m_RenderTargetConfiguration,sizeof(CellGcmSurface)); }

  static inline bool IsMultisamplingSupported() { return g_bMultisamplingSupported; }


#elif defined(_VR_GXM)

  VISION_APIFUNC void SetDepthStencilSurface(SceGxmDepthStencilSurface* pSurface);
  VISION_APIFUNC void SetRenderSurface(int iRenderTargetIndex, SceGxmColorSurface *pColorSurface, VVIDEO_Multisample eMultisample = VVIDEO_MULTISAMPLE_OFF);
  VISION_APIFUNC SceGxmRenderTarget* GetGXMRenderTarget() { return m_pRenderTarget; }
  VISION_APIFUNC SceGxmDepthStencilSurface* GetDepthStencilSurface() { return m_pDepthStencilSurface; }
  VISION_APIFUNC SceGxmColorSurface* GetRenderSurface(int iRenderTargetIndex) { return m_pColorSurface[iRenderTargetIndex]; }
  VISION_APIFUNC void ForceDepthStencilSurfaceLoad(bool bEnable);

  static inline bool IsMultisamplingSupported() { return g_bMultisamplingSupported; }

#elif defined(_VR_GX2)

  VISION_APIFUNC void SetDepthStencilSurface(GX2DepthBuffer* pSurface);
  VISION_APIFUNC void SetRenderSurface(int iRenderTargetIndex, GX2ColorBuffer* pColorSurface, VVIDEO_Multisample eMultisample);
  VISION_APIFUNC void GetDepthStencilSurface(GX2DepthBuffer*& pDepthStencilSurface);
  VISION_APIFUNC void GetRenderSurface(int iRenderTargetIndex, GX2ColorBuffer*& pColorSurface);

#endif


  ///
  /// @}
  ///

  ///
  /// @name Occlusion Query Management
  /// @{
  ///

  /// \brief
  ///   Sets the max percent of hardware occlusion queries that should be used for visibility tests by all
  ///   render contexts.
  /// 
  /// To prevent the render contexts from using all hardware occlusion queries, this function
  /// allows to set the maximal percent of queries that should be used for visibility tests.
  /// The default value is 90%.
  ///
  /// \param iPercent
  ///   Maximal percent of hardware occlusion queries used for visibility tests. Must be between 5 and 95.
  ///  
  /// \sa VisRenderContext_cl::GetMaxOcclusionTestQueryUsage
  VISION_APIFUNC static void SetMaxOcclusionTestQueryUsage(unsigned char iPercent);

  /// \brief
  ///   Returns the max percent of hardware occlusion queries that are used for visibility tests
  ///   by all render contexts.
  ///
  /// \sa VisRenderContext_cl::SetMaxOcclusionTestQueryUsage
  VISION_APIFUNC static unsigned char GetMaxOcclusionTestQueryUsage();

  /// \brief
  ///   Renders all scheduled occlusion tests.
  /// 
  /// Occlusion tests can be scheduled for rendering using the ScheduleOcclusionTest method.
  /// 
  /// \sa VisRenderContext_cl::FetchOcclusionTestResults
  /// \sa VisRenderContext_cl::ScheduleOcclusionTest
  VISION_APIFUNC void RenderScheduledOcclusionTests();

  /// \brief
  ///   Fetches the results of all pending occlusion tests.
  /// 
  /// Occlusion tests can be scheduled for rendering using the ScheduleOcclusionTest method. They
  /// will then be rendered in a call to RenderScheduledOcclusionTests.
  ///
  /// \param bForceWait
  ///   If true, forces to wait for all pending occlusion queries.
  ///
  /// \sa VisRenderContext_cl::RenderScheduledOcclusionTests
  /// \sa VisRenderContext_cl::ScheduleOcclusionTest
  VISION_APIFUNC void FetchOcclusionTestResults(bool bForceWait = false);

  /// \brief
  ///   Schedules an occlusion test for rendering.
  /// 
  /// Scheduled occlusion tests will be rendered in the next call to the
  /// RenderScheduledOcclusionTests method.
  /// 
  /// \param iOccTestIndex
  ///   The index of the respective VisOcclusionQueryObject_cl instance in its object element
  ///   manager.
  /// 
  /// \sa VisRenderContext_cl::RenderScheduledOcclusionTests
  /// \sa VisRenderContext_cl::FetchOcclusionTestResults
  /// \sa VisRenderContext_cl::ShouldScheduleOcclusionTest
  /// \sa VisRenderContext_cl::HasOcclusionTestPassed
  /// 
  /// \example
  ///   \code
  ///   int iOccQueryIndex = pEntity->GetOccQueryObject().GetNumber(); 
  ///   if (pRenderContext->ShouldScheduleOcclusionTest(iOccQueryIndex))
  ///     pRenderContext->ScheduleOcclusionTest(iOccQueryIndex);
  ///   \endcode
  inline void ScheduleOcclusionTest(int iOccTestIndex) { m_ScheduledOcclusionTests[m_iNumScheduledOcclusionTests++]=iOccTestIndex; }
  
  /// \brief
  ///   Returns whether an object has successfully passed occlusion query.
  /// 
  /// This is the result of the last visibility check done for this object.
  /// False is only returned in the case of queries classified as invisible.
  /// 
  /// \param iOccTestIndex
  ///   The index of the respective occlusion query object in the occlusion query object element
  ///   manager.
  /// 
  /// \return
  ///   true if the object has been classified as visible (or if it has not been tested),
  ///   otherwise false.
  inline bool HasOcclusionTestPassed(int iOccTestIndex) const
  {
    unsigned char *pFlags = m_PassedOcclusionTest.GetDataPtr();
    return pFlags[iOccTestIndex>>3] & (char)(1<<(iOccTestIndex&7)) ? true : false;
  }

  /// \brief
  ///   Returns whether an occlusion test is ready for scheduling.
  /// 
  /// If an object has been classified as visible during the last occlusion test, it is likely that
  /// it will remain visible for some more frames. Since occlusion query operations are not free,
  /// it is therefore common practice to render occlusion queries only every n frames as long as
  /// they are classified as visible.
  /// 
  /// In order to ensure a roughly even distribution of occlusion query rendering operations across
  /// frames, this function uses the occlusion query object's element manager index and the current
  /// render count of the render context. 
  /// 
  /// The default mask value of 3 means that the object will be rescheduled every fourth frame. A
  /// mask of 7, for instance, would mean rescheduling every eighth, a value of 15 every 16th
  /// frame.
  /// 
  /// Legal values for iMask are 1, 3, 7, 15, 31, 63, 127, and 255.
  /// 
  /// \param iOccTestIndex
  ///   The index of the respective occlusion query object in the occlusion query object element
  ///   manager.
  /// 
  /// \param iMask
  ///   Reschedule mask, see function description.
  /// 
  /// \return
  ///   bool: true if the object should be rescheduled, otherwise false.
  /// 
  /// \example
  ///   \code
  ///   int iOccQueryIndex = pEntity->GetOccQueryObject().GetNumber(); 
  ///   if (pRenderContext->ShouldScheduleOcclusionTest(iOccQueryIndex))
  ///     pRenderContext->ScheduleOcclusionTest(iOccQueryIndex);
  ///   \endcode
  inline bool ShouldScheduleOcclusionTest(int iOccTestIndex, unsigned char iMask=3) const
  {
    if ( (GetRenderCount()&(unsigned int)iMask) == (iOccTestIndex&(unsigned int)iMask) )
      return true;
    return false;
  }

  void RemoveOcclusionQuery(int iIndex);
  static void RemoveOcclusionQueryFromAllContexts(int iIndex);

  ///
  /// @}
  ///


  ///
  /// @name Pixel Counter Query Management
  /// @{
  ///

  /// \brief
  ///   Renders all scheduled pixel counter tests.
  /// 
  /// Pixel counter tests can be scheduled for rendering using the SchedulePixelCounterTest method.
  /// 
  /// \sa VisRenderContext_cl::FetchPixelCounterTestResults
  /// \sa VisRenderContext_cl::SchedulePixelCounterTest
  VISION_APIFUNC void RenderScheduledPixelCounterTests();

  /// \brief
  ///   Fetches the results of all pending occlusion tests.
  /// 
  /// Occlusion tests can be scheduled for rendering using the ScheduleOcclusionTest method. They
  /// will then be rendered in a call to RenderScheduledOcclusionTests.
  ///
  /// \param bForceWait
  ///   If true, forces to wait for all pending occlusion queries.
  ///
  /// \sa VisRenderContext_cl::RenderScheduledPixelCounterTests
  /// \sa VisRenderContext_cl::SchedulePixelCounterTest
  VISION_APIFUNC void FetchPixelCounterTestResults(bool bForceWait = false);

  /// \brief
  ///   Schedules a pixel counter occlusion test for rendering.
  /// 
  /// Scheduled pixel counter occlusion tests will be rendered in the next call to the
  /// RenderScheduledOcclusionTests method.
  /// 
  /// \param iPixelCounterIndex
  ///   The index of the respective VOcclusionQueryObjectPixelCounter instance in its object element
  ///   manager.
  /// 
  /// \sa VisRenderContext_cl::GetPixelCounterResult
  /// \sa VisRenderContext_cl::SetPixelCounterResult
  /// \sa VisRenderContext_cl::IsPixelCounterQueryInProgress
  inline void SchedulePixelCounterTest(int iPixelCounterIndex) { m_ScheduledPixelCounters[m_iNumScheduledPixelCounters++]=iPixelCounterIndex; }
  
  /// \brief
  ///   Returns whether an pixel counter query is currently in progress.
  /// 
  /// The returned flag is set to true on each query render call and to false on each receive
  /// of the query result. If no query was started, the return value is false. If multiple
  /// queries are started for the same object, the flag will be reset to false by the first arriving
  /// query result.
  /// 
  /// \param iPixelCounterIndex
  ///   The index of the respective VOcclusionQueryObjectPixelCounter in its element manager.
  /// 
  /// \return
  ///   false if the started query's result is ready or no query was started.
  inline bool IsPixelCounterQueryInProgress(int iPixelCounterIndex) const
  {
    unsigned char *pFlags = m_PixelCounterInProgress.GetDataPtr();
    return pFlags[iPixelCounterIndex>>3]&(char)(1<<(iPixelCounterIndex&7))?true:false;
  }

  /// \brief
  ///   Returns the cached number of counted pixels for the given object.
  /// 
  /// \param iPixelCounterIndex
  ///   The index of the respective VOcclusionQueryObjectPixelCounter in its element manager.
  /// 
  /// \return
  ///   Number of pixels cached for this object in the render context.
  ///
  /// \sa VisRenderContext_cl::SetPixelCounterResult
  /// \sa VisRenderContext_cl::IsPixelCounterQueryInProgress
  /// \sa VisRenderContext_cl::SchedulePixelCounterTest
  inline unsigned int GetPixelCounterResult(int iPixelCounterIndex) const
  {
    return m_PixelCounterResults[iPixelCounterIndex];
  }

  /// \brief
  ///   Overrides the cached number of counted pixels for the given object.
  ///
  /// Sometimes it can be useful to override the cached number of pixels, for example to set a default
  /// value for a newly added object.
  /// 
  /// \param iPixelCounterIndex
  ///   The index of the respective VOcclusionQueryObjectPixelCounter in its element manager.
  /// 
  /// \param iPixels
  ///   New pixel count value to be saved in the current cache.
  ///
  /// \sa VisRenderContext_cl::GetPixelCounterResult
  /// \sa VisRenderContext_cl::IsPixelCounterQueryInProgress
  /// \sa VisRenderContext_cl::SchedulePixelCounterTest
  inline void SetPixelCounterResult(int iPixelCounterIndex, unsigned int iPixels)
  {
    if (m_iRenderFlags & VIS_RENDERCONTEXT_FLAG_USE_PIXELCOUNTER)
    {
      m_PixelCounterResults.EnsureSize(iPixelCounterIndex + 1);
      m_PixelCounterResults[iPixelCounterIndex] = iPixels;
    }
  }

  /// \brief
  ///   Overrides the cached number of counted pixels for the given object in all render contexts.
  ///
  /// Sometimes it can be useful to override the cached number of pixels, for example to set a default
  /// value for a newly added object.
  /// 
  /// \param iPixelCounterIndex
  ///   The index of the respective VOcclusionQueryObjectPixelCounter in its element manager.
  /// 
  /// \param iPixels
  ///   New pixel count value to be saved in the current cache.
  ///
  /// \sa VisRenderContext_cl::SetPixelCounterResult
  VISION_APIFUNC static void SetPixelCounterResultInAllContexts(int iPixelCounterIndex, unsigned int iPixels);

  /// \brief
  ///   Searches for all pixel counter queries that have an element manager index of \c iElementManagerIndex and removes those queries from the list of queries that are currently in progress.
  ///
  /// \param iElementManagerIndex
  ///   The index of the element manager whose pixel counter queries shall be removed
  ///
  /// \sa RemovePixelCounterQueryFromAllContexts
  void RemovePixelCounterQuery(int iElementManagerIndex);

  /// \brief
  ///   Calls RemovePixelCounterQuery() on all VisRenderContext_cl instances.
  static void RemovePixelCounterQueryFromAllContexts(int iIndex);

  ///
  /// @}
  ///



  ///
  /// @name Miscellaneous Visibility-related Functions
  /// @{
  ///

  /// \brief
  ///   Sets the visibility collector of this render context
  /// 
  /// Render contexts may have their own visibility collector implementation. Visibility collectors
  /// can be shared across render contexts. Setting the visibility collector to NULL is allowed;
  /// however, this will result in all scene elements being classified as invisible. This is only
  /// recommended for e.g. Post-processing render contexts.
  /// 
  /// By default, the visibility collector is set to NULL for all render contexts except for the
  /// main render context, which uses a VisionVisibilityCollector_cl instance.
  /// 
  /// If bIsReferenceContext is set to true, the visibility collector will inherit the properties
  /// of this render context (field of view, clip planes, relevant scene elements). Note that  it
  /// is not recommended to set this value to true for multiple render contexts referencing the
  /// same visibility collector.
  /// 
  /// \param pVisCollector
  ///   Visibility collector for this render context.
  /// 
  /// \param bIsReferenceContext
  ///   Whether to use this context as reference context for the visibility collector (true by
  ///   default).
  VISION_APIFUNC void SetVisibilityCollector(IVisVisibilityCollector_cl *pVisCollector, bool bIsReferenceContext = true);

  /// \brief
  ///   Returns the visibility collector of this render context (as previously set with
  ///   SetVisibilityCollector).
  VISION_APIFUNC IVisVisibilityCollector_cl *GetVisibilityCollector() const;

  /// \brief
  ///   Computes and returns the view frustum generated using the camera position and orientation
  ///   and the render context's field of view/clip plane settings.
  /// 
  /// Every call to this function recomputes the frustum. Check whether using
  /// VisionVisibilityCollector_cl::GetBaseFrustum may be an alternative for you.
  /// 
  /// \param viewFrustum
  ///   Empty frustum object to be filled with data in this function.
  /// 
  /// \return
  ///   Nothing.
  /// 
  /// \sa IVisVisibilityCollector_cl::GetBaseFrustum
  VISION_APIFUNC void GetViewFrustum(VisFrustum_cl &viewFrustum) const;


  ///
  /// @}
  ///

  ///
  /// @name Other Miscellaneous Functions
  /// @{
  ///

  /// \brief
  ///   Static function to increase global counters. Should be called as shown in the
  ///   VisionApp_cl::Run() function.
  static VISION_APIFUNC void GlobalTick();

  /// \brief
  ///   Returns the current number of global ticks, i.e. the number of frames globally rendered
  static VISION_APIFUNC unsigned int GetGlobalTickCount();

  /// \brief
  ///   Returns the number of frames that this context has been rendered so far. Used for some
  ///   visibility functions.
  inline unsigned int GetRenderCount() const
  {
    return m_iRenderCount;
  }

  /// \brief
  ///   Returns whether the render context is the reference context of its visibility collector as
  ///   previously set with SetVisibilityCollector().
  inline bool IsReferenceContextForVisibilityCollector() const { return m_bIsReferenceContext; }

  /// \brief
  ///   Returns the index of the passed render target (-1 if the passed texture object is not a
  ///   render target of this render context)
  VISION_APIFUNC int GetRenderTargetIndex(VTextureObject *pTexObj);

  /// \brief
  ///   When the render targets of the main render context have been changed, call this method to ensure that the render
  ///   targets of the main render context are set back to their original state.
  VISION_APIFUNC static void ResetMainRenderContext();

  /// \brief Internal Function.
  void ComputeMaxOcclusionQueryCount();

  /// \brief Internal Function.
  void ClearOcclusionTestResults();

  /// \brief Internal Function.
  void ClearPixelCounterTestResults();

  #if defined(_VR_DX9)
    static IDirect3DSurface9 *GetRenderTargetSurfaceForTexture(VTextureObject *pTexObj, int iFaceOrArrayIndex = -1);
  #elif defined (_VR_OPENGL) || defined(_VR_GLES2)
    static unsigned int GetRenderTargetSurfaceHandleForTexture(VTextureObject *pTexObj);
  
  #elif defined(_VR_GCM)
    static CellGcmTexture *GetRenderTargetSurfaceForTexture(VTextureObject *pTexObj, int iFaceOrArrayIndex = -1);
    static unsigned int ClearTexFlags(unsigned int iFormat);
  
  #elif defined(_VR_DX11)
    static D3DView *GetRenderTargetViewForTexture(VTextureObject *pTexObj, int iFaceOrArrayIndex = -1);
  
  #elif defined(_VR_GXM)
    static SceGxmColorSurface *GetRenderTargetSurfaceForTexture(VTextureObject *pTexObj, int iFaceOrArrayIndex = -1);
  #endif
  

  /// \brief Returns the VisRenderableTextureConfig_t for the render target with index \c iRenderTarget or NULL if it does not exist.
  VISION_APIFUNC const VisRenderableTextureConfig_t *GetTargetConfig(int iRenderTarget = 0) const;

  /// \brief Internal Function.
  inline void SetInverseViewportZ(bool bStatus) { m_bInverseZViewport = bStatus; }

  /// \brief Internal Function.
  inline bool GetInverseViewportZ() const { return m_bInverseZViewport; }

  /// \brief Internal Function.
  void ForceMakeCurrent();

  /// \brief Internal Function.
  void SortScheduledOcclusionTests(int *pEntityList, unsigned int iNumEntities, unsigned int iMaxEntities);

  /// \brief
  ///   Implements IVisCallbackHandler_cl
  VISION_APIFUNC virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  ///
  /// @}
  ///

public:

  /// \brief Inserts a sync point, which can be waited upon using WaitForSyncFence(). Only implemented on XBox 360.
  /// \sa WaitForSyncFence
  void InsertSyncFence();

  /// \brief Waits for a previously added sync point. Only implemented on XBox 360.
  /// \sa InsertSyncFence()
  void WaitForSyncFence();

#ifdef _VISION_PSP2
  void DestroyGxmTarget();
#endif

  /// \brief Calls UpdateRenderTargets() on all VisRenderContext_cl instances.
  static void UpdateAllRenderTargets(VTextureObject *pTex = NULL);

  /// \brief Sets all render targets. Needs to be done after lost device events.
  void UpdateRenderTargets();

  /// \brief
  ///   Static function that calls the Tick function on all registered contexts. Called by the default application class in VisionApp_cl::OnUpdateScene()
  VISION_APIFUNC static void HandleAllRenderContexts(float dtime);

  /// \brief Calls Reset() on all VisRenderContext_cl instances.
  static void ResetAllRenderContexts();

  /// \brief Returns the index of the last frame.
  VISION_APIFUNC unsigned int GetLastRenderedFrame();

  VISION_APIFUNC static void PerformPendingContextSwitch();
  VISION_APIFUNC void ReleaseContext();
  VISION_APIFUNC static void UnbindRenderTarget(const VTextureObject *pTarget);

  void TrackContextPlanes();

protected:

  virtual void TickFunction(float dtime);

private:

  void UpdatePerspectiveScale();

  friend class VisGame_cl;
  friend class VisionRenderInterface_cl;
  friend class IVRenderContextContainer;
  friend void SetProjectionMatrix();
  friend void vrSetScissorRect(const VRectanglef *pScreenRect);
  friend void vrIdentityMatrix();

  
  friend void vrSetProjectionMatrix (const hkvMat4* pMatrix);



  bool IsValid() const;
  void Reset();

  void AssignCoreVariables(VisRenderContext_cl *pOldContext = NULL);
  static void SetMainRenderContext(VisRenderContext_cl *pContext = NULL);
  static void UnsetMainRenderContext();

  void MakeCurrent();
  void Init(const VisContextCamera_cl *pCamera, float fFovX, float fFovY, int iWidth, int iHeight, float fNearClip, float fFarClip, int iRenderFlags);
  void CommonInit(float fFovX, float fFovY, int iRenderFlags);

  static VisRenderContextPtr g_spCurrentContext;
  static VisRenderContextPtr g_spMainRenderContext;
  static unsigned char s_iMaxPercentVisibilityTests;
  static bool m_bContextSwitched;

  VStaticString<48> m_szName;

  VisContextCameraPtr m_spCamera;
  VisContextViewPropertiesPtr m_spViewProperties;

  VTextureObjectPtr m_spTexObject[MAX_NUM_RENDERTARGETS];
  VTextureObjectPtr m_spDepthStencilObject;

  bool m_bRendersIntoBackBuffer;

  int m_iCubeMapFaceOrArrayIndex[MAX_NUM_RENDERTARGETS];
  int m_iCubeMapFaceOrArrayIndexDepthStencil;



#ifdef _VISION_XENON
  bool m_bDepthFillPass;
  DWORD m_iSyncFence;
  bool m_bAutoTiling;
#endif


  // Occlusion query members
  unsigned int m_iNumScheduledOcclusionTests;
  unsigned int m_iNumScheduledUnforcedOcclusionTests;
  unsigned int m_iNumExecutedOcclusionTests;
  unsigned int m_iMaxNumOcclusionTests;
  DynArray_cl<int> m_ScheduledOcclusionTests;
  DynArray_cl<unsigned char> m_PassedOcclusionTest;
  VOcclusionQueryBuffer<128, 512> m_QueryBuffer;

  unsigned int m_iNumScheduledPixelCounters;
  unsigned int m_iNumScheduledUnforcedPixelCounters;
  unsigned int m_iNumExecutedPixelCounters;
  DynArray_cl<int> m_ScheduledPixelCounters;
  DynArray_cl<unsigned char> m_PixelCounterInProgress;
  DynArray_cl<unsigned int> m_PixelCounterResults;
  VOcclusionQueryBuffer<64, 64> m_PixelCounterQueryBuffer;

  VisDebugRenderQueue_cl m_DebugQueue; // queued line rendering

  float m_fPriority;
  bool m_bInverseZViewport;
  bool m_bRenderingEnabled;
  bool m_bRecentlyRendered;
  bool m_bIgnoreWireframe;
  
  int m_iSize[2];
  int m_iViewportPos[2];
  int m_iViewportSize[2];
  float m_fPerspScale[2];
  float m_fViewportZRange[2];
  float m_fLODCorrection, m_fCustomLODScaling;

  unsigned int m_iLastRenderedFrame;
  unsigned int m_iRenderCount;
  int m_iRenderFlags;
  VisContextUsageHint_e m_eUsageHint;
  VisRenderContextPtr m_spLODReferenceContext;
  int m_iShaderFlags;
  unsigned int m_iRenderFilter;
  bool m_bUseClearScreen;
  bool m_bRenderTargetChanged;
  IVisRenderLoopPtr m_spRenderLoop;
  IVisVisibilityCollectorPtr m_spVisibilityCollector;
  bool m_bIsReferenceContext;

#if defined(_VR_OPENGL) || defined(_VR_GLES2)

  unsigned int m_iColorSurface[MAX_NUM_RENDERTARGETS];
  unsigned int m_iDepthStencilSurface;
  bool m_bUseStencil;
  static bool g_bMultisamplingSupported;
  unsigned int m_iFrameBufferObject;

#elif defined(_VR_GCM)

  CellGcmTexture *m_pDepthStencilSurface;
  CellGcmTexture *m_pColorSurface[MAX_NUM_RENDERTARGETS];
  CellGcmSurface m_RenderTargetConfiguration;

  static bool g_bMultisamplingSupported;
  static unsigned int g_iLastDepthBufferOffset;
  static unsigned int g_iLastZCullStart;
  static unsigned int g_iLastZCullSize;

  bool m_bSurfaceConfigurationInvalidated;
  VVIDEO_Multisample m_eMultisample;

#elif defined(_VR_DX9)

  D3DSurface *m_pColorSurface[MAX_NUM_RENDERTARGETS];
  D3DSurface *m_pDepthStencilSurface;
  static D3DSurface *m_pPendingColorSurface[MAX_NUM_RENDERTARGETS];
  static D3DSurface *m_pPendingDepthStencilSurface;

#elif defined(_VR_DX11)
  D3DRenderTargetView *m_pRenderTargetView[MAX_NUM_RENDERTARGETS];
  D3DDepthStencilView *m_pDepthStencilView;

#elif defined(_VR_GXM)
  static bool g_bMultisamplingSupported;
  bool m_bSurfaceConfigurationInvalidated;
  VVIDEO_Multisample m_eMultisample;

  SceGxmDepthStencilSurface* m_pDepthStencilSurface;
  SceGxmColorSurface* m_pColorSurface[MAX_NUM_RENDERTARGETS];

  SceGxmRenderTarget* m_pRenderTarget;
  SceGxmRenderTarget* m_pOffscreenRenderTarget;

#elif defined(_VR_GX2)

  GX2ColorBuffer* m_pColorBuffers[MAX_NUM_RENDERTARGETS];
  GX2DepthBuffer* m_pDepthStencilBuffer;

  VVIDEO_Multisample m_eMultisample;

#endif

  friend class VResourceSystem_cl;
  friend class VisResourceSystemBackgroundRestorer_cl;

  friend class VisRenderableTexture_cl;
  friend class VisContextCamera_cl;
  friend class Vision;
  friend class VisVideo_cl;
  friend void Vision_GL_Init();
  friend void StartNewFrame();
  friend void EndFrame();

  friend class VTextureManager;
};

VISION_ELEMENTMANAGER_TEMPLATE_DECL(VisRenderContext_cl)


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
