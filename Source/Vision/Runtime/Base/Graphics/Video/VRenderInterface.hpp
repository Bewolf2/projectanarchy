/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VRenderInterface.hpp

#ifndef VRENDERERINTERFACE_HPP_INCLUDED
#define VRENDERERINTERFACE_HPP_INCLUDED

struct VSimpleRenderState_t;
class hkvVec3;
class hkvAlignedBBox;
class VTextureObject;
class VCompiledShaderPass;
class VRectanglef;

#define RENDERSHAPEFLAGS_LINES   0x00000001
#define RENDERSHAPEFLAGS_SOLID   0x00000002
#define RENDERSHAPEFLAGS_CAP0    0x00000100
#define RENDERSHAPEFLAGS_CAP1    0x00000200


/// \brief
///   Simple render interface to provide functionality for line and text rendering.
/// 
/// As rendering geometry through this interface is relatively slow, it should only be used for very simple rendering operations, or
/// for debug/in-editor visualization. vForge uses this interface for rendering shape previews.
class IVRenderInterface
{
public:
  virtual ~IVRenderInterface() {}

  /// \brief
  ///   Pure virtual function used to render a single line
  virtual void DrawLine(const hkvVec3& vPos1, const hkvVec3& vPos2, VColorRef iColor, float fWidth, const VSimpleRenderState_t &iProperties) = 0;

  /// \brief
  ///   Convenience function to render a single line with a default render state
  inline void DrawLine(const hkvVec3& vPos1, const hkvVec3& vPos2, VColorRef iColor, float fWidth)
  {
    VSimpleRenderState_t iProperties(VIS_TRANSP_NONE);
    DrawLine(vPos1, vPos2, iColor, fWidth, iProperties);
  }

  /// \brief
  ///   Pure virtual function used to render a single triangle (solid color)
  virtual void DrawTriangle(const hkvVec3& vPos1, const hkvVec3& vPos2, const hkvVec3& vPos3, VColorRef iColor, const VSimpleRenderState_t &iProperties) = 0;

  /// \brief
  ///   Render a text message on the screen, using a small debug font
  virtual void DrawText2D(float x, float y, const char *szMsg, VColorRef iColor, const VSimpleRenderState_t &iProperties) {}

  /// \brief
  ///   Render a text message on the screen, using a small debug font and default VSimpleRenderState_t for 2D text
  virtual void DrawText2D(float x, float y, const char *szMsg, VColorRef iColor) {}

  /// \brief
  ///   Render a text message at 3D position, using a small debug font
  virtual void DrawText3D(const hkvVec3& vPos, const char *szMsg, VColorRef iColor, const VSimpleRenderState_t &iProperties) {}

  /// \brief
  ///   Render a text message at 3D position, using a small debug font and default VSimpleRenderState_t for 3D text
  virtual void DrawText3D(const hkvVec3& vPos, const char *szMsg, VColorRef iColor) {}

  /// \brief
  ///   Render a 2D overlay (sprite) at a given 3D position
  virtual void DrawSprite(const hkvVec3 &vCenter, VTextureObject *pTexture, VColorRef iColor, const VSimpleRenderState_t &iProperties, float fSizeX, float fSizeY, const hkvVec2& uv0 = hkvVec2 (0,0), const hkvVec2& uv1 = hkvVec2(1,1)){};

  /// \brief
  ///   Render a 2D overlay (sprite) at a given 3D position
  virtual void DrawSprite(const hkvVec3 &vCenter, const char *szTextureFilename, VColorRef iColor, const VSimpleRenderState_t &iProperties, float fSizeX, float fSizeY, const hkvVec2& uv0 = hkvVec2 (0,0), const hkvVec2& uv1 = hkvVec2 (1,1)) {};

  /// \brief
  ///   Helper function to render an axis aligned bounding box with lines
  VBASE_IMPEXP void DrawLineBox(const hkvAlignedBBox &bbox, VColorRef iColor, float fWidth);

  /// \brief
  ///   Helper function to render a plane
  VBASE_IMPEXP void RenderPlane(const hkvPlane& plane, const hkvVec3& vCenterPos, float fSize, VColorRef iColor, const VSimpleRenderState_t &state, int iFlags=RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID, int iSubDiv=5);

  /// \brief
  ///   Helper function to render a plane with X and Y extents
  VBASE_IMPEXP void RenderPlane(const hkvPlane& plane, const hkvVec3& vCenterPos, float fSizeX, float fSizeY, VColorRef iColor, const VSimpleRenderState_t &state, int iFlags=RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID, int iSubDiv=5);


  /// \brief
  ///   Helper function to render a plane
  VBASE_IMPEXP void RenderPlane(const hkvVec3& vDirX, const hkvVec3& vDirY, const hkvVec3& vCenterPos, VColorRef iColor, const VSimpleRenderState_t &state, int iFlags=RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID, int iSubDiv=5);

  /// \brief
  ///   Helper function to render a solid axis aligned bounding box
  VBASE_IMPEXP void RenderAABox(const hkvAlignedBBox &box,VColorRef iColor, const VSimpleRenderState_t &state, int iFlags=RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID);


  /// \brief
  ///   Generic box rendering function. Can be used to render oriented boxes. To render a simple
  ///   axis aligned box, use RenderAABox
  /// 
  /// This function takes 8 vertices that define the corners of the box. The order of these corners
  /// must be compatible with the result of hkvAlignedBBox::getCorners
  /// 
  /// To render an oriented box, get the corners of the local box and transform each vertex with
  /// the desired transformation matrix.
  /// 
  /// \param pFirstCorner
  ///   Pointer to an array of 8 corner vertices (e.g. result of hkvAlignedBBox::getCorners)
  /// 
  /// \param iVertexStride
  ///   The stride of the vertices in bytes. E.g. sizeof(hkvVec3) or sizeof(hkvVec4)
  /// 
  /// \param iColor
  ///   Color of the box (alpha channel for solid boxes)
  /// 
  /// \param state
  ///   Render state flags, e.g. transparency mode
  /// 
  /// \param iFlags
  ///   rendering bitflags; supported here: RENDERSHAPEFLAGS_LINES and RENDERSHAPEFLAGS_SOLID
  /// 
  /// \example
  ///   \code
  ///   // Example1:
  ///   hkvVec3 corners[8];
  ///   box.GetCorners(corners);
  ///   pRI->RenderBox(&corners[0],sizeof(hkvVec3),iColor,state);
  ///   // Example2 (oriented box):
  ///   hkvVec3 corners[8];
  ///   hkvAlignedBBox bbox(...);
  ///   box.GetCorners(corners, pObj->GetRotationMatrix(), pObj->GetPosition());
  ///   pRI->RenderBox(&corners[0],sizeof(hkvVec3),iColor,state);
  ///   \endcode
  VBASE_IMPEXP void RenderBox(const hkvVec3* pFirstCorner, int iVertexStride,VColorRef iColor, const VSimpleRenderState_t &state, int iFlags=RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID);

  /// \brief
  ///   Helper function to render a cone
  VBASE_IMPEXP void RenderCone(const hkvVec3& vPos, const hkvVec3& vDir, float fAngle, float fLength, VColorRef iColor, const VSimpleRenderState_t &state, int iFlags=RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID, int iSubDiv=14);

  /// \brief
  ///   Helper function to render a frustum
  VBASE_IMPEXP void RenderFrustum(const hkvVec3& vPos, const hkvVec3& vDir, const hkvVec3& vUp, float fAngleX, float fAngleY, float fNearClip, float fFarClip, VColorRef iColor, const VSimpleRenderState_t &state, int iFlags=RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID);

  /// \brief
  ///   Helper function to render a sphere
  VBASE_IMPEXP void RenderSphere(const hkvVec3& vCenter, float fRadius, VColorRef iColor, const VSimpleRenderState_t &state, int iFlags=RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID,int iSubDivX=14, int iSubDivY=14);

  /// \brief
  ///   Helper function to render a cylinder
  VBASE_IMPEXP void RenderCylinder(const hkvVec3& vStart, const hkvVec3& vDirection, float fRadius, VColorRef iColor, const VSimpleRenderState_t &state, int iFlags=RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID|RENDERSHAPEFLAGS_CAP0|RENDERSHAPEFLAGS_CAP1, int iSubDiv=14, int iSegments=5);

  /// \brief
  ///   Helper function to render a capsule
  VBASE_IMPEXP void RenderCapsule(const hkvVec3& vStart, const hkvVec3& vDirection, float fRadius, VColorRef iColor, const VSimpleRenderState_t &state, int iFlags=RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID, int iSubDivH = 2, int iSubDivA = 8);

};


struct Shader_t;


/// \brief
///   Simple vertex structure that is used for 2D overlay rendering.
/// 
/// \sa IVRender2DInterface
struct Overlay2DVertex_t
{
  inline Overlay2DVertex_t() : screenPos(hkvNoInitialization), texCoord(hkvNoInitialization), color(false) ///< the members should all be uninitialized
  {
  }
  inline void Set(float screenX, float screenY, float u, float v, VColorRef iColor = V_RGBA_WHITE) {screenPos.set(screenX,screenY);texCoord.set(u,v);color=iColor;}
  hkvVec2 screenPos;
  hkvVec2 texCoord;
  VColorRef color;
};


/// \brief
///   Interface that provides low-level functionality for 2D overlay rendering.
///
/// As rendering geometry through this interface is relatively slow, it should only be used for simple rendering operations, or
/// for debug/in-editor visualization. The Vision UI system uses this interface for rendering 2D UI controls.
class IVRender2DInterface
{
public:
  virtual ~IVRender2DInterface() {}

  /// \brief
  ///   Renders a triangle list using the 2D vertices passed to this function. Renders
  ///   iVertexCount/3 triangles.
  /// 
  /// \param iVertexCount
  ///   Number of vertices to be rendered (number of triangles = iVertexCount/3)
  /// 
  /// \param pVertices
  ///   Pointer to the list of iVertexCount vertices
  /// 
  /// \param pTexture
  ///   The texture used to render the triangles (can be NULL to render in solid color)
  /// 
  /// \param iProperties
  ///   Render state properties (such as blending type, z-buffer test etc.)
  virtual void Draw2DBuffer(int iVertexCount, Overlay2DVertex_t *pVertices, VTextureObject *pTexture, const VSimpleRenderState_t &iProperties) = 0;

  /// \brief
  ///   Renders a triangle list using the 2D vertices passed to this function. The render state
  ///   setup is defined by a shader.
  /// 
  /// \param iVertexCount
  ///   Number of vertices to be rendered (number of triangles = iVertexCount/3)
  /// 
  /// \param pVertices
  ///   Pointer to the list of iVertexCount vertices
  /// 
  /// \param pTexture
  ///   The texture used to render the triangles (can be NULL to render in solid color).
  /// 
  /// \param shader
  ///   Shader that defines the state setup
  virtual void Draw2DBufferWithShader(int iVertexCount, Overlay2DVertex_t *pVertices, VTextureObject *pTexture, VCompiledShaderPass &shader) = 0;

  /// \brief
  ///   Sets a hardware clipping rectangle (specified in screen coordinates)
  /// 
  /// \param pScreenRect
  ///   Pointer to a rectangle that defines the clipping rectangle. Can be NULL to turn off
  ///   clipping.
  virtual void SetScissorRect(const VRectanglef *pScreenRect) = 0;

  /// \brief
  ///   Sets the depth value (z-coordinate) for subsequent render calls on this interface
  /// 
  /// The coordinate is passed in world units, so it has to be taken care that this value is inside
  /// the [nearclip..farclip] range of the current context.
  /// 
  /// This depth can be used to sort GUI elements into the actual z-buffer of the scene (e.g.
  /// status text at entity position).
  /// 
  /// Note that the RENDERSTATEFLAG_ALWAYSVISIBLE should not be set in the VSimpleRenderState_t
  /// if z-sorting should be used.
  /// 
  /// The depth value persists only inside a VisRenderLoopHelper_cl::BeginOverlayRendering() /
  /// VisRenderLoopHelper_cl::EndOverlayRendering block.
  /// 
  /// \param fZCoord
  ///   z-coordinate in world units, or -1 for default depth
  virtual void SetDepth(float fZCoord) = 0;

  /// \brief
  ///   Returns the depth value (z-coordinate) for subsequent render calls on this interface
  /// 
  /// \see
  //    IVRender2DInterface::SetDepth
  /// 
  /// \return
  ///   Current depth.
  virtual float GetDepth() const = 0;

  /// \brief
  ///   Set a custom transformation for subsequent draw calls. The transformation consists of scaling (xy) and offset (zw). NULL can be passed to reset to default
  virtual void SetTransformation(const hkvVec4 *pScaleAndOfs) = 0;

  /// \brief
  ///   Helper function to draw a solid screen quad (uses the Draw2DBuffer function)
  /// 
  /// \param vPos1
  ///   Upper left screen position of the quad
  /// 
  /// \param vPos2
  ///   Bottom right screen position of the quad
  /// 
  /// \param iColor
  ///   The solid color of the quad.
  /// 
  /// \param iProperties
  ///   Render state properties (such as blending type, z-buffer test etc.)
  VBASE_IMPEXP void DrawSolidQuad(const hkvVec2& vPos1, const hkvVec2& vPos2, VColorRef iColor, const VSimpleRenderState_t &iProperties);


  /// \brief
  ///   Helper function to draw a textured screen quad (uses the Draw2DBuffer function)
  /// 
  /// \param vPos1
  ///   Upper left screen position of the quad
  /// 
  /// \param vPos2
  ///   Bottom right screen position of the quad
  /// 
  /// \param pTexture
  ///   Texture used for rendering
  /// 
  /// \param vTexCoord1
  ///   Source texture coordinate of upper left corner
  /// 
  /// \param vTexCoord2
  ///   Source texture coordinate of bottom right corner
  /// 
  /// \param iColor
  ///   The solid color of the quad.
  /// 
  /// \param iProperties
  ///   Render state properties (such as blending type, z-buffer test etc.)
  VBASE_IMPEXP void DrawTexturedQuad(const hkvVec2& vPos1, const hkvVec2& vPos2, VTextureObject *pTexture, const hkvVec2& vTexCoord1, const hkvVec2& vTexCoord2, VColorRef iColor, const VSimpleRenderState_t &iProperties);

  /// \brief
  ///   Helper function to draw a textured screen quad with a shader
  /// 
  /// \param vPos1
  ///   Upper left screen position of the quad
  /// 
  /// \param vPos2
  ///   Bottom right screen position of the quad
  /// 
  /// \param pTexture
  ///   Texture used for rendering
  /// 
  /// \param vTexCoord1
  ///   Source texture coordinate of upper left corner
  /// 
  /// \param vTexCoord2
  ///   Source texture coordinate of bottom right corner
  /// 
  /// \param iColor
  ///   The solid color of the quad.
  /// 
  /// \param shader
  ///   Shader that defines the state setup
  VBASE_IMPEXP void DrawTexturedQuadWithShader(const hkvVec2& vPos1, const hkvVec2& vPos2, VTextureObject *pTexture, const hkvVec2& vTexCoord1, const hkvVec2& vTexCoord2, VColorRef iColor, VCompiledShaderPass &shader);

  /// \brief
  ///   Static helper function that creates 6 vertices that define a quad
  /// 
  /// \param sx1
  ///   target screen position (x1)
  /// 
  /// \param sy1
  ///   target screen position (y1)
  /// 
  /// \param sx2
  ///   target screen position (x2)
  /// 
  /// \param sy2
  ///   target screen position (y2)
  /// 
  /// \param tx1
  ///   source texture coordinate (u1)
  /// 
  /// \param ty1
  ///   source texture coordinate (v1)
  /// 
  /// \param tx2
  ///   source texture coordinate (u2)
  /// 
  /// \param ty2
  ///   source texture coordinate (v2)
  /// 
  /// \param iColor
  ///   Color that is used for all vertices
  /// 
  /// \param pDestBuffer
  ///   destination buffer, receives 6 vertices
  /// 
  /// \return
  ///   int iCount : Number of vertices written to the buffer, always 6
  VBASE_IMPEXP static int CreateQuadVertices(float sx1, float sy1,float sx2, float sy2, float tx1, float ty1,float tx2, float ty2, VColorRef iColor, Overlay2DVertex_t *pDestBuffer);

  /// \brief
  ///   Static helper function that creates 6 vertices that define a quad
  /// 
  /// \param vPos1
  ///   Upper left screen position of the quad
  /// 
  /// \param vPos2
  ///   Bottom right screen position of the quad
  /// 
  /// \param vTexCoord1
  ///   Source texture coordinate of upper left corner
  /// 
  /// \param vTexCoord2
  ///   Source texture coordinate of bottom right corner
  /// 
  /// \param iColor
  ///   Color that is used for all vertices
  /// 
  /// \param pDestBuffer
  ///   destination buffer, receives 6 vertices
  /// 
  /// \return
  ///   int iCount : Number of vertices written to the buffer, always 6
  VBASE_IMPEXP static int CreateQuadVertices(const hkvVec2& vPos1, const hkvVec2& vPos2, const hkvVec2& vTexCoord1, const hkvVec2& vTexCoord2, VColorRef iColor, Overlay2DVertex_t *pDestBuffer);

};

#ifndef _VISION_DOC


class VRender2DCommandStream : public IVRender2DInterface
{
public:
  VBASE_IMPEXP VRender2DCommandStream();

  // IVRender2DInterface overrides
  virtual void Draw2DBuffer(int iVertexCount, Overlay2DVertex_t *pVertices, VTextureObject *pTexture, const VSimpleRenderState_t &iProperties);
  virtual void Draw2DBufferWithShader(int iVertexCount, Overlay2DVertex_t *pVertices, VTextureObject *pTexture, VCompiledShaderPass &shader);
  virtual void SetScissorRect(const VRectanglef *pScreenRect);
  virtual void SetDepth(float fZCoord);
  virtual float GetDepth() const  { return m_fLastDepth; }
  virtual void SetTransformation(const hkvVec4* pScaleAndOfs);

protected:
  void Reset();
  void Append(const void *pData, int iByteCount);
  void AppendCommand(char cmd, const void *pData=NULL, int iByteCount=0);

  int m_iCommandCount;
  DynArray_cl<char> m_Commands;

  // cache:
  char m_iLastCommand;
  int m_iLastCommandBlockPos;
  VTextureObject *m_pLastTex;
  VSimpleRenderState_t m_iLastState;
  VCompiledShaderPass *m_pLastShader;
  float m_fLastDepth;
};
#endif // _VISION_DOC

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
