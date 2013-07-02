/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiGame.hpp

#ifndef DEFINE_VISAPIGAME
#define DEFINE_VISAPIGAME

#include <Vision/Runtime/Engine/SceneElements/VisApiBaseEntity.hpp>


class VisLightSource_cl;
class VisBaseEntity_cl;
class VisPath_cl;
class VisStaticMeshInstance_cl;
class VisMsg_cl;

/// \brief
///  Options on what to render when calling VisGame_cl::RenderDebugGeometry
///
enum VDebugGeometryRenderMode_e {
  VDGRM_2D = 0x1, ///< Renders 2D debugging geometry only (2D lines)
  VDGRM_3D = 0x2, ///< Renders 3D debugging geometry only (3D lines, triangles and sprites)
  VDGRM_ALL = 0x3 ///< Renders all debugging geometry
};

/// \brief
///   Class for game related functions like entity creation. One global instance of this class can
///   be accessed via Vision::Game.
class VisGame_cl
{
public:
  
  ///
  /// @name Constructors / Destructor
  /// @{
  ///
  
  /// \brief
  ///   Constructor of the game class
  VisGame_cl();


  /// \brief
  ///   Destructor of the game class
  virtual ~VisGame_cl();  

  ///
  /// @}
  ///

  ///
  /// @name Light Related Functions
  /// @{
  ///

  
  /// \brief
  ///   Creates a new light source
  /// 
  /// Lights created with Vision::Game.CreateLight will use the new attenuation mode
  /// VIS_LIGHT_ATTEN_CUSTOM.
  /// 
  /// \param pos
  ///   Position of the new light
  /// 
  /// \param eLightType
  ///   Type of the light (VIS_LIGHT_POINT/VIS_LIGHT_SPOTLIGHT)
  /// 
  /// \param fRadius
  ///   Radius of the light's influence
  /// 
  /// \return
  ///   VisLightSource_cl *: Pointer to the new light
  /// 
  /// \example
  ///   \code
  ///   VisLightSource_cl *pLight = Vision::Game.CreateLight( hkvVec3(200,50,0), VIS_LIGHT_POINT, 300.f  );
  ///   pLight->SetMultiplier( 2.f );
  ///   pLight->SetColor(V_RGB(255,0,0));
  ///   \endcode
  VISION_APIFUNC VisLightSource_cl * CreateLight( const hkvVec3& pos, VisLightSourceType_e eLightType = VIS_LIGHT_POINT, float fRadius = 100.f );
  
  ///
  /// @}
  ///

  ///
  /// @name Entity Related Functions
  /// @{
  ///

  
  /// \brief
  ///   Create a new entity by classname
  /// 
  /// CreateEntity creates a new entity of the class "classname", initialises the variables and
  /// calls the InitFunction of the entity. If the model has not been used before, then the model
  /// will also be loaded.
  /// 
  /// \param className
  ///   The class name of the new entity, e.g. "Player_cl". Must be a registered entity class.
  /// 
  /// \param origin
  ///   Spawn position of the new entity.
  /// 
  /// \param customModelFile
  ///   Filename of the model file.
  /// 
  /// \param szVarString
  ///   Variable string which define a set of properties of the entity (see
  ///   VisBaseEntity_cl::SetVariablesByString). The "origin" can't be set via the variable string,
  ///   the function parameter has to be used instead
  /// 
  /// \return
  ///   VisBaseEntity_cl *newEntity: Pointer to the new entity, or NULL if the entity could not be
  ///   created
  /// 
  /// \sa VisBaseEntity_cl::SetModel
  /// \sa VisBaseEntity_cl::SetVariablesByString
  /// \sa VisObject3D_cl::AttachToParent
  /// 
  /// \example
  ///   \code
  ///   hkvVec3 playerOrigin;
  ///   Player_cl *newPlayer = (Player_cl *) Vision::Game.CreateEntity("Player_cl", playerOrigin);
  ///   Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3(0,0,0) , "chair.model", " orientation=\"70.000/0.000/0.000\" ,entityKey=\"\" ");
  ///   \endcode
  VISION_APIFUNC VisBaseEntity_cl *CreateEntity(const char *className, const hkvVec3& origin, const char *customModelFile = NULL, const char *szVarString = NULL);

  /// \brief
  ///   Create a new entity from a template
  VISION_APIFUNC VisBaseEntity_cl *CreateEntity(VisEntityTemplate_cl &templ);

  /// \brief
  ///   Frees all the entities that are flagged as "removed", but not freed yet. Usually called by
  ///   the scene implementation.
  VISION_APIFUNC void FreeRemovedEntities();
 

  /// \brief
  ///   Search for an entity (or multiple entities) with a given entity key
  /// 
  /// SearchEntity looks for all the entities with a given entity key, and returns the first one
  /// which has the given key. If the storageArray parameter is defined, then this function will
  /// additionally store all the entities which have been found in that array.
  /// 
  /// This function wraps around VisElementManager_cl::FindByKey for the element manager of the
  /// entity class .
  /// 
  /// \param entityKey
  ///   Entity Key to search for.
  /// 
  /// \param storageArray
  ///   Pointer to an dynamic array of VisBaseEntity_cl pointers.
  /// 
  /// \return
  ///   VisBaseEntity_cl *firstResult: First entity which the search function has found
  VISION_APIFUNC VisBaseEntity_cl *SearchEntity(const char *entityKey, DynArray_cl<VisBaseEntity_cl *> *storageArray = NULL);


  /// \brief
  ///   Removes an entity. Just wraps around entity->Remove()
  VISION_APIFUNC void RemoveEntity(VisBaseEntity_cl *entity);


  /// \brief
  ///   Loads a dynamic mesh file (.model) file and its animations (.anim).
  /// 
  /// This function is useful to load a dynamic mesh file without creating an entity.
  /// 
  /// If a mesh resource with the specified name is already created, the function will return that
  /// instance.
  /// 
  /// This function optionally loads the corresponding .anim file.
  /// 
  /// \param szFilename
  ///   File name of the model file.
  /// 
  /// \param bForceLoad
  ///   If TRUE, the returned resource is guaranteed to be loaded (calls EnsureLoaded).
  /// 
  /// \param bLoadAnim
  ///   If TRUE, the .anim file will also be loaded.
  /// 
  /// \return
  ///   VDynamicMesh* pMesh : Pointer to the mesh resource. 
  /// 
  /// \sa VDynamicMesh
  VISION_APIFUNC VDynamicMesh* LoadDynamicMesh(const char *szFilename, bool bForceLoad, bool bLoadAnim = true);

  /// \brief
  ///   Deprecated. Use Vision::Game.LoadDynamicMesh instead
  HKV_DEPRECATED_2012_1 inline VDynamicMesh* LoadModel(const char *szFilename, bool bForceLoad, bool bLoadAnim = true)
  {
    return LoadDynamicMesh(szFilename, bForceLoad, bLoadAnim);
  }


  /// \brief
  ///   Deprecated function, use VDynamicMesh::GetResourceManager().PurgeUnusedResources instead
  HKV_DEPRECATED_2012_1 VISION_APIFUNC int PurgeUnusedModels(float fSeconds);



  /// \brief
  ///   Removes one or more entities specified by key 
  /// 
  /// The found entities are removed via VisBaseEntity_cl::Remove
  /// 
  /// \param entityKey
  ///   The entity key.
  /// 
  /// \return
  ///   \c true if at least one entity has been removed.
  /// 
  /// \sa VisBaseEntity_cl::Remove()
  /// 
  /// \example
  ///   \code
  ///   Vision::Game.RemoveEntity("scientist");  
  ///   \endcode
  VISION_APIFUNC BOOL RemoveEntity(const char *entityKey);

  
  

  ///
  /// @}
  ///

  ///
  /// @name Line Drawing Functions
  /// @{
  ///

  /// \brief
  ///   Adds a single line to the buffer for deferred line rendering in the render loop.
  /// 
  /// This function should only be used for debug output
  /// 
  /// \param startPoint
  ///   Position in world space where the line starts.
  /// 
  /// \param endPoint
  ///   Position in world space where the line ends.
  /// 
  /// \param color
  ///   Color value used for rendering, alpha channel is ignored.
  /// 
  /// \param width
  ///   Width of the line, the default value is 1.0f. Ignored for DX9 based platforms.
  /// 
  /// \sa VisGame_cl::DrawSingleLine2D
  /// \sa VColorRef
  VISION_APIFUNC void DrawSingleLine(const hkvVec3& startPoint, const hkvVec3& endPoint, VColorRef color = V_RGBA_WHITE, float width = 1.0f);

  /// \brief
  ///   Adds a single line to the buffer for deferred line rendering in the render loop.
  /// 
  /// This function should only be used for debug output
  /// 
  /// \param startPoint
  ///   Position in world space where the line starts.
  /// 
  /// \param endPoint
  ///   Position in world space where the line ends.
  /// 
  /// \param color
  ///   Color value used for rendering, alpha channel is ignored.
  /// 
  /// \param state
  ///   Constants that describe the render state, such as transparency, z-buffer tests etc.
  /// 
  /// \sa VisGame_cl::DrawSingleLine2D
  /// \sa VColorRef
  VISION_APIFUNC void DrawSingleLine(const hkvVec3& startPoint, const hkvVec3& endPoint, VColorRef color, const VSimpleRenderState_t &state);

 
  /// \brief
  ///   This version of DrawSingleLine takes the coordinates directly
  /// 
  /// \param startX
  ///   X position in world space where the line starts.
  /// 
  /// \param startY
  ///   Y position in world space where the line starts.
  /// 
  /// \param startZ
  ///   Z position in world space where the line starts.
  /// 
  /// \param endX
  ///   X position in world space where the line ends.
  /// 
  /// \param endY
  ///   Y position in world space where the line ends.
  /// 
  /// \param endZ
  ///   Z position in world space where the line ends.
  /// 
  /// \param color
  ///   Color value used for rendering, alpha channel is ignored.
  /// 
  /// \param width
  ///   Width of the line, the default value is 1.0f. Ignored for DX9 based platforms.
  /// 
  /// \sa VisGame_cl::DrawSingleLine2D
  VISION_APIFUNC void DrawSingleLine(float startX, float startY, float startZ, float endX, float endY, float endZ, VColorRef color = V_RGBA_WHITE, float width = 1.0f);


  /// \brief
  ///   Draw a 2D line on screen. No z-test is used for rendering these lines
  /// 
  /// \param startX
  ///   X position in screen space where the line starts.
  /// 
  /// \param startY
  ///   Y position in screen space where the line starts.
  /// 
  /// \param endX
  ///   X position in screen space where the line ends.
  /// 
  /// \param endY
  ///   Y position in screen space where the line ends.
  /// 
  /// \param color
  ///   Color value used for rendering, alpha channel is ignored.
  /// 
  /// \param width
  ///   Width of the line, the default value is 1.0f. Ignored for DX9 based platforms.
  /// 
  /// \sa VisGame_cl::DrawSingleLine
  /// 
  /// \example
  ///   \code
  ///   Vision::Game.DrawSingleLine2D(30.0f, 30.0f, 200.0f, 200.0f, 255.0f, 0.0f, 0.0f, 3.0f);
  ///   Vision::Game.DrawSingleLine2D(300.0f, 400.0f, 200.0f, 300.0f, 255.0f, 0.0f, 0.0f, 1.0f);
  ///   \endcode
  VISION_APIFUNC void DrawSingleLine2D(float startX, float startY, float endX, float endY, VColorRef color = V_RGBA_WHITE, float width=1.f);




  /// \brief
  ///   Draws an axis aligned bounding box based on six float values. This function wraps around
  ///   the VisGame_cl::DrawSingleLine function
  /// 
  /// \param minX
  ///   Minimum x value of the bounding box.
  /// 
  /// \param minY
  ///   Minimum y value of the bounding box.
  /// 
  /// \param minZ
  ///   Minimum z value of the bounding box.
  /// 
  /// \param maxX
  ///   Maximum x value of the bounding box.
  /// 
  /// \param maxY
  ///   Maximum y value of the bounding box.
  /// 
  /// \param maxZ
  ///   Maximum z value of the bounding box.
  /// 
  /// \param color
  ///   Color value used for rendering, alpha channel is ignored.
  /// 
  /// \param lineWidth
  ///   Width of the lines, the default value is 1.0f. Ignored for DX9 based platforms.
  /// 
  /// \sa VisGame_cl::DrawBoundingBox
  /// \sa VisGame_cl::DrawOrientedBoundingBox
  VISION_APIFUNC void DrawSimpleBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, VColorRef color = V_RGBA_WHITE, float lineWidth = 1.0f);

  /// \brief
  ///   Draws a cube at a certain position
  /// 
  /// \param center
  ///   Center of the cube.
  /// 
  /// \param fSize
  ///   Specifies the length of the cube sides.
  /// 
  /// \param color
  ///   Color value used for rendering, alpha channel is ignored.
  /// 
  /// \example
  ///   \code
  ///   Vision::Game.DrawCube( center, 1.f, V_RGB(255,255,0) );
  ///   \endcode
  VISION_APIFUNC void DrawCube(const hkvVec3& center, float fSize, VColorRef color = V_RGBA_WHITE );

  
  /// \brief
  ///   Draws an axis aligned bounding box. This function wraps around the
  ///   VisGame_cl::DrawSingleLine function
  /// 
  /// \param bBox
  ///   Bounding box to be rendered.
  /// 
  /// \param color
  ///   Color value used for rendering, alpha channel is ignored.
  /// 
  /// \param lineWidth
  ///   Width of the lines, the default value is 1.0f. Ignored for DX9 based platforms.
  /// 
  /// \sa VisGame_cl::DrawSimpleBox
  /// \sa VisGame_cl::DrawOrientedBoundingBox
  /// \sa VisBaseEntity_cl::DrawBoundingBox
  VISION_APIFUNC void DrawBoundingBox(const hkvAlignedBBox &bBox, VColorRef color = V_RGBA_WHITE, float lineWidth = 1.0f);


  /// \brief
  ///   Draws an oriented bounding box specified by a local box and its position/orientation in the
  ///   world
  /// 
  /// \param bBox
  ///   Local bounding box around the center.
  /// 
  /// \param bBoxRotMatrix
  ///   3x3 rotation matrix that defines the orientation of the box.
  /// 
  /// \param bBoxPosition
  ///   World position of the box center.
  /// 
  /// \param color
  ///   Color value used for rendering, alpha channel is ignored.
  /// 
  /// \param lineWidth
  ///   Width of the lines, the default value is 1.0f. Ignored for DX9 based platforms.
  /// 
  /// \sa VisGame_cl::DrawSimpleBox
  /// \sa VisGame_cl::DrawSimpleBox
  /// \sa VisBaseEntity_cl::DrawBoundingBox
  VISION_APIFUNC void DrawOrientedBoundingBox(const hkvAlignedBBox &bBox, const hkvMat3& bBoxRotMatrix, const hkvVec3& bBoxPosition, VColorRef color = V_RGBA_WHITE, float lineWidth = 1.0f);

  /// \brief
  ///   Draws a frustum
  /// 
  /// Useful function for debugging custom visibility tests
  /// 
  /// \param frustum
  ///   Frustum to be rendered.
  /// 
  /// \param fDisplayLength
  ///   The extrusion length of the frustum in world units.
  /// 
  /// \param iColor
  ///   Color value used for rendering, alpha channel is ignored.
  /// 
  /// \param fLineWidth
  ///   Width of the lines, the default value is 1.0f. Ignored for DX9 based platforms.
  /// 
  /// \param bDrawNormals
  ///   If set to true, plane normals will be rendered.
  /// 
  /// \sa VisFrustum_cl
  VISION_APIFUNC void DrawFrustum(const VisFrustum_cl &frustum, float fDisplayLength, VColorRef iColor=V_RGBA_WHITE, float fLineWidth=1.f, bool bDrawNormals = false);

  /// \brief
  ///   Draws a portal
  /// 
  /// Useful function for debugging custom visibility tests
  /// 
  /// \param portal
  ///   Portal to be rendered.
  /// 
  /// \param iColor
  ///   Color value used for rendering, alpha channel is ignored.
  /// 
  /// \param bDrawNormal
  ///   If set to true, the portal's plane normal will be rendered.
  /// 
  /// \sa VisPortal_cl
  VISION_APIFUNC void DrawPortal(const VisPortal_cl &portal, VColorRef iColor, bool bDrawNormal);

  /// \brief
  ///   Renders a single solid colored triangle. For debugging purposes only.
  /// 
  /// Like rendering single lines, this function is not particularly fast since the triangles are
  /// batched for deferred rendering in the render loop. Thus it should only be used for debugging
  /// purposes.
  /// 
  /// \param vPos0,vPos1,vPos2
  ///   Three world space positions that define the triangle.
  /// 
  /// \param iColor
  ///   The triangle's solid color. For alpha transparency, the alpha channel is used.
  /// 
  /// \param transp
  ///   The transparency mode.
  /// 
  /// \param iRenderFlags
  ///   Additional render flags. The following bit constants are supported:
  ///   - RENDERSTATEFLAG_FRONTFACE: The frontface is rendered
  ///   - RENDERSTATEFLAG_BACKFACE: The backface is rendered
  ///   - RENDERSTATEFLAG_DOUBLESIDED: A combination of RENDERSTATEFLAG_FRONTFACE and RENDERSTATEFLAG_BACKFACE
  ///   - RENDERSTATEFLAG_ALWAYSVISIBLE: Does not consider the z-buffer when rendering.
  ///   - RENDERSTATEFLAG_WRITETOZBUFFER: Writes to z-buffer
  ///   - RENDERSTATEFLAG_ALPHATEST: Uses alpha test (does not make sense for solid color rendering though)
  VISION_APIFUNC void DrawSingleTriangle(const hkvVec3& vPos0, const hkvVec3& vPos1, const hkvVec3& vPos2, VColorRef iColor, VIS_TransparencyType transp=VIS_TRANSP_ALPHA, int iRenderFlags=RENDERSTATEFLAG_FRONTFACE);

  /// \brief
  ///   See IVRenderInterface::DrawSprite
  VISION_APIFUNC void DrawSprite(const hkvVec3 &vCenter, VTextureObject *pTexture, VColorRef iColor, const VSimpleRenderState_t &iState, float fSizeX, float fSizeY, const hkvVec2& uv0=hkvVec2(0,0), const hkvVec2& uv1=hkvVec2(1,1) );

  /// \brief
  ///   Returns a render interface that renders lines, debug triangles etc. into all render contexts
  VISION_APIFUNC IVRenderInterface* GetDebugRenderInterface();


  /// \brief
  ///   Draws a text at the screen position that represents the specified 3D world position
  ///
  /// The message position will be transformed to camera space and then to screen space with a
  /// perspective divide. No scaling according to distance will take place. The camera of the current render context will be used.
  /// 
  /// Same as Vision::Message.DrawMessage3D.
  /// 
  /// \param pszMsg
  ///   Message string.
  /// 
  /// \param pos
  ///   Position in world space.
  /// 
  /// \param iFont
  ///   Ignored
  /// 
  /// \return
  ///   \c TRUE if the string is valid (not NULL)
  VISION_APIFUNC BOOL DrawMessage3D ( const char *pszMsg, const hkvVec3& pos, short iFont=1 );


  /// \brief
  ///   Deprecated, please use VisRenderContext_cl::Project2D instead.
  ///   This function wraps around VisRenderContext_cl::GetCurrentContext()->Project2D.
  HKV_DEPRECATED_2013_1 VISION_APIFUNC BOOL Project2D ( const hkvVec3& pos, float &x, float &y );

  /// \brief
  ///   Deprecated, please use VisRenderContext_cl::GetTraceDirFromScreenPos instead.
  ///   This function wraps around VisRenderContext_cl::GetCurrentContext()->GetTraceDirFromScreenPos.
  HKV_DEPRECATED_2013_1 VISION_APIFUNC void GetTraceDirFromScreenPos(float fScreenX,float fScreenY, hkvVec3& destDir, float fDirLength=1.f);

  
  ///
  /// @}
  ///

  ///
  /// @name Custom Functions
  /// @{
  ///
  

  /// \brief
  ///   Search for a path (or multiple paths) with a given key
  /// 
  /// SearchPath searches for paths with a given key, and returns the first one which has the given
  /// key. If the storageArray parameter has been defined, then this function will  additionally
  /// store all the paths which have been found in that array.
  /// 
  /// This function wraps around VisElementManager_cl::FindByKey for the element manager of the
  /// path class.
  /// 
  /// \param key
  ///   PathKey to search for.
  /// 
  /// \param storageArray
  ///   Optional pointer to an dynamic array of VisPath_cl pointers.
  /// 
  /// \return
  ///   VisPath_cl *firstResult: First path which the search function has found (or NULL).
  /// 
  /// \example
  ///   \code
  ///   VisPath_cl *pPath1 = Vision::Game.SearchPath("Path1");
  ///   \endcode
  VISION_APIFUNC VisPath_cl *SearchPath(const char *key, DynArray_cl<VisPath_cl *> *storageArray = NULL);
    

  /// \brief
  ///   Search for a light source (or multiple light sources) with a given key
  /// 
  /// SearchLightSource searches for light sources with a given key, and returns the first one which
  /// has the given key. If the storageArray parameter has been defined, then this function will
  /// additionally store all the light sources which have been found in that array.
  /// 
  /// This function wraps around VisElementManager_cl::FindByKey for the element manager of the
  /// light source class.
  /// 
  /// \param key
  ///   Light source key to search for.
  /// 
  /// \param storageArray
  ///   Optional pointer to an dynamic array of VisLightSource_cl pointers.
  /// 
  /// \return
  ///   First light source which the search function has found.
  /// 
  /// \example
  ///   \code
  ///   VisLightSource_cl *pLight = Vision::Game.SearchLightSource("LIGHT_1");
  ///   \endcode
  VISION_APIFUNC VisLightSource_cl *SearchLightSource(const char *key, DynArray_cl<VisLightSource_cl *> *storageArray = NULL);


  /// \brief
  ///   Search for a convex volume (or multiple convex volumes) with a given key
  /// 
  /// SearchConvexVolume looks for convex volumes with a given key, and returns the first one which
  /// has the given key. If the storageArray parameter has been defined, then this function will
  /// additionally store all the convex volumes which have been found in that array.
  /// 
  /// This function wraps around VisElementManager_cl::FindByKey for the element manager of the
  /// convex volume class.
  /// 
  /// \param szKey
  ///   Convex volume key to search for.
  /// 
  /// \param storageArray
  ///   Optional pointer to an dynamic array of VisConvexVolume_cl pointers.
  /// 
  /// \return
  ///   First convex volume the search function has found.
  /// 
  /// \example
  ///   \code
  ///   VisConvexVolume_cl *pVol = Vision::Game.SearchConvexVolume("WATER");
  ///   \endcode
  VISION_APIFUNC VisConvexVolume_cl *SearchConvexVolume(const char *szKey, DynArray_cl<VisConvexVolume_cl*> *storageArray = NULL);

  /// \brief
  ///   Finds a static mesh instance with specified key string
  /// 
  /// Wraps around VisStaticMeshInstance_cl::FindByKey(key,storageArray)
  /// 
  /// \param key
  ///   Mesh instance key to search for.
  /// 
  /// \param storageArray
  ///   Optional pointer to an dynamic array of VisStaticMeshInstance_cl pointers.
  /// 
  /// \return
  ///   the first match, if any.
  /// 
  /// \example
  ///   \code
  ///   VisStaticMeshInstance_cl *pMesh = Vision::Game.SearchStaticMeshInstance("House01");
  ///   \endcode
  VISION_APIFUNC VisStaticMeshInstance_cl *SearchStaticMeshInstance(const char *key, DynArray_cl<VisStaticMeshInstance_cl *> *storageArray = NULL);


  /// \brief
  ///   Wraps around rand()
  VISION_APIFUNC int GetRand();

  
  /// \brief
  ///   Returns rand()*2-RAND_MAX
  VISION_APIFUNC int GetRandNeg();
  
  
  /// \brief
  ///   Get a random float value between 0.0 and 1.0. Uses rand()
  VISION_APIFUNC float GetFloatRand();

  
  /// \brief
  ///   Get a random float value between -1.0 and 1.0. Uses rand()
  VISION_APIFUNC float GetFloatRandNeg();


  /// \brief
  ///   Saves a screen shot of the current frame buffer to file
  /// 
  /// If no filename is specified, this function saves to a filename of type "grab000X.bmp",
  /// increasing the digit each time.
  /// 
  /// For creating the file, the installed file manager is used; the first data directory is used
  /// that supports writing.
  ///
  /// \param pszFileName
  ///   Name of the screenshot file. Can be NULL.
  ///
  /// \param bSaveAsJPG
  ///   If TRUE then the screenshot will be saved as a JPG file, otherwise a BMP file will be created
  ///   JPG is not supported on consoles and will be ignored.
  ///
  /// \param bUseFrontBuffer
  ///   If true, image data will be retrieved from the front buffer, otherwise from the back buffer.
  ///   Even though both buffers are supported, there might be problems with some drivers in rare cases when using the
  ///   front buffer.
  ///
  /// \returns
  ///   \c true if the screenshot was successfully saved; \c false if not.
  ///
  /// \note
  ///   You can use this function in combination with the IVTimer::ForceFrameRate function in order to
  ///   produce a trailer of your application. Set the timer to produce a constant frame rate by calling
  ///   the IVTimer::ForceFrameRate and generate a screen shot in the main loop each frame. Just
  ///   keep care which framerate you use for the trailer bitmaps, since certain video software and
  ///   video recorders might only work with certain frame rates.
  ///   To set the quality of generated JPG images, use the VisGame_cl::SetScreenShotJPGQuality function.
  VISION_APIFUNC bool SaveScreenShot(const char *pszFileName = NULL, bool bSaveAsJPG = false, bool bUseFrontBuffer = true);

  /// \brief
  ///   Sets the quality (1..100) of JPG images when saveAsJPG is set to TRUE in the SaveScreenShot
  ///   function
  /// 
  /// \param iPercentage
  ///   The quality level, where 100 means full quality.
  /// 
  /// \note
  ///   The default quality is 75. You should not use JPG files for releasing screenshots that 
  ///   require a high quality. Instead, use BMP format and convert with dedicated graphics software.
  /// 
  /// \sa VisGame_cl::SaveScreenShot
  VISION_APIFUNC void SetScreenShotJPGQuality(int iPercentage=75);

  /// \brief
  ///   Copies the frontbuffer-data to a system memory buffer
  /// 
  /// To calculate the buffer's size, use the formula sizeX*sizeY*3.
  /// 
  /// Use this function only when image readback (e.g. for screenshots) is required.
  /// 
  /// \param x
  ///   X coordinate of upper left corner of the screen-rectangle to be copied.
  /// 
  /// \param y
  ///   Y coordinate of upper left corner of the screen-rectangle to be copied.
  /// 
  /// \param sizeX
  ///   Width of the screen-rectangle to be copied.
  /// 
  /// \param sizeY
  ///   Height of the screen-rectangle to be copied.
  /// 
  /// \param memPtr
  ///   Pointer to destination-buffer.
  /// 
  /// \param pSrcRT
  ///   Optional pointer to a render target texture to read from. Can be NULL to read from the main render target.
  /// 
  /// \param bUseFrontBuffer
  ///   DEPRECATED
  /// 
  /// \return
  ///   \c true if successful.
  /// 
  /// \sa VisGame_cl::SaveScreenShot
  VISION_APIFUNC bool WriteScreenToBuffer(int x, int y, int sizeX, int sizeY, UBYTE *memPtr, VisRenderableTexture_cl *pSrcRT=NULL, bool bUseFrontBuffer = true);

  /// \brief
  ///   Renders the debugging geometry (triangles, lines and sprites) (but doesn't empty the buffer). May only be
  ///   called inside the render loop
  /// \param mode
  ///  If all or only a selection of debugging geometry should be rendered (\see VDebugGeometryRenderMode_e enum)
  ///
  VISION_APIFUNC void RenderDebugGeometry(char mode);

  /// \brief
  ///   Clears the debugging geometry buffer. Gets called in the VisVideo_cl::UpdateScreen()
  ///   function to reset all contexts
  VISION_APIFUNC void ResetDebugGeometryBuffer();

  
  /// \brief
  ///   Send a message to an entity
  /// 
  /// SendMsg can be used to send a message to another entity. The message ID and the data is
  /// passed is parameters.
  /// 
  /// SendMsg currently just directly calls the virtual message function on the specified object
  /// 
  /// \param pDest
  ///   Destination object the message will be sent to.
  /// 
  /// \param iID
  ///   Message ID.
  /// 
  /// \param iParamA
  ///   Message data parameter 1.
  /// 
  /// \param iParamB
  ///   Message data parameter 2.
  /// 
  /// \note
  ///   Make sure that you use valid message IDs when sending messages (see
  ///   VisTypedEngineObject_cl::MessageFunction).
  /// 
  /// \sa VisTypedEngineObject_cl::MessageFunction
  /// \sa VisTypedEngineObject_cl::SendMsg
  /// 
  /// \example
  ///   \code
  ///   const int GAME_MSG_TRIGGER = VIS_MSG_USER + 1;
  ///   Vision::Game.SendMsg( pDestEntity, GAME_MSG_TRIGGER, data[1], data[2] );
  ///   \endcode
  VISION_APIFUNC void SendMsg(VisTypedEngineObject_cl *pDest, int iID, INT_PTR iParamA = 0, INT_PTR iParamB = 0);

  /// \brief
  ///   Queues the message to be processed during the game loop
  /// 
  /// The scene manager implementation is responsible for sending the queue (see
  /// VisionApp_cl::OnUpdateScene())
  /// 
  /// \param pDest
  ///   Destination object the message will be sent to.
  /// 
  /// \param iID
  ///   Message ID.
  /// 
  /// \param iParamA
  ///   Message data parameter 1.
  /// 
  /// \param iParamB
  ///   Message data parameter 2.
  /// 
  /// \note
  ///   Make sure that you use valid message IDs when sending messages (see
  ///   VisTypedEngineObject_cl::MessageFunction).
  /// 
  /// \sa VisGame_cl::SendMsg
  /// \sa VisTypedEngineObject_cl::MessageFunction
  /// 
  /// \example
  ///   \code
  ///   const int GAME_MSG_TRIGGER = VIS_MSG_USER + 1;
  ///   Vision::Game.PostMsg( pDestEntity, GAME_MSG_TRIGGER, data[1], data[2] );
  ///   \endcode
  VISION_APIFUNC void PostMsg(VisTypedEngineObject_cl *pDest, int iID, INT_PTR iParamA, INT_PTR iParamB);

  /// \brief
  ///   Processes all queued entity messages, usually called from application implementations (see
  ///   VisionApp_cl::OnUpdateScene).
  VISION_APIFUNC void ProcessMessageQueue();

  /// \brief
  ///   Removes any messages from a concrete object from the message queue. This method is used by 
  ///   VisTypedEngineObject_cl to prevent messages from disposed /deleted objects to be processed 
  ///   and therefore cause an access violation.
  VISION_APIFUNC void RemoveObjectFromMessageQueue(VisTypedEngineObject_cl *pObj);

  /// \brief
  ///   Gets the number of queued message that are still pending (still to be sent)
  VISION_APIFUNC int GetNumPendingMessages() const;  

  /// \brief
  ///   Returns the global settings map.
  /// 
  /// Returns the settings map of the VisGame_cl class. 
  /// 
  /// The settings map is a useful helper for storing and retrieving global game-specific settings.
  /// It is a string dictionary that you can use to store key-value pairs.
  /// 
  /// \return
  ///   VStrDictionary settingsMap: The settings map string dictionary.
  inline VStrDictionary &GetSettingsMap() 
  { 
    return m_SettingsDictionary; 
  }

  
  /// \brief
  ///   Sets the internal update counter
  /// 
  /// It is very important that application implementations increment this counter each game tick
  /// (see VisionApp_cl::OnUpdateScene()), since animation state refresh functions use this value
  /// for early out.
  ///
  /// \param iFrame
  ///   Internal update counter value to set.
  /// 
  /// \example
  ///   \code
  ///   // Advance the scene update counter
  ///   Vision::Game.SetUpdateSceneCount( Vision::Game.GetUpdateSceneCount() + 1 );
  ///   \endcode
  inline void SetUpdateSceneCount(int iFrame)
  {
    m_iUpdateSceneCount = iFrame;
  }
  
  /// \brief
  ///   Returns the current counter set via SetUpdateSceneCount
  inline int GetUpdateSceneCount()
  {
    return m_iUpdateSceneCount;
  }

  /// \brief
  ///   Purges all non-managed resources. 
  ///
  /// Returns true if any resource has been freed.
  ///
  /// \note
  ///   Resets the active sky, so that the active sky object might get deleted too.
  VISION_APIFUNC bool PurgeAllResources();

  /// \brief
  ///   Remove all objects (entities, lights, particle groups,...) that are currently unused. 
  ///
  /// Returns true if any object has been removed.
  VISION_APIFUNC bool RemoveAllUnRefObjects();

  ///
  /// @}
  ///

  ///
  /// @name Scene Collections
  /// @{
  ///

  /// \brief
  ///   Returns a collection that contains all entities that have to perform a PreThinkFunction
  ///   call during game loop.
  /// 
  /// The collection is always kept up-to-date by the respective entity member function
  /// (VisBaseEntity_cl::SetPreThinkFunctionStatus)
  /// 
  /// See VisionSceneManager_cl implementation for an example.
  /// 
  /// \return
  ///   Collection containing all entities that need to perform PreThinkFunction.
  VISION_APIFUNC VisEntityCollection_cl &GetPreThinkingEntities();

  /// \brief
  ///   Returns a collection that contains all entities that have to perform a ThinkFunction call
  ///   during game loop.
  /// 
  /// The collection is always kept up-to-date by the respective entity member function
  /// (VisBaseEntity_cl::SetThinkFunctionStatus)
  /// 
  /// See VisionSceneManager_cl implementation for an example.
  /// 
  /// \return
  ///   Collection containing all entities that need to perform ThinkFunction.
  VISION_APIFUNC VisEntityCollection_cl &GetThinkingEntities();

  /// \brief
  ///   Returns a collection that contains all entities that have a valid animation config and thus
  ///   need animation to be handled in the game loop
  /// 
  /// The collection is always kept up-to-date by the respective entity member function
  /// (VisBaseEntity_cl::SetAnimConfig)
  /// 
  /// See VisionSceneManager_cl implementation for an example.
  /// 
  /// \return
  ///   Collection containing all entities that need to be animated.
  VISION_APIFUNC VisEntityCollection_cl &GetAnimatedEntities();

  /// \brief
  ///   Returns a collection that contains all entities that have the "always in foreground" option enabled.
  /// 
  /// All entities which have the "always in foreground" property enabled will be added to this list. At the
  /// same time, calling VisBaseEntity_cl::SetAlwaysInForeground(TRUE) will result in the entity being ignored
  /// for regular visibility determination.
  /// You do not have to manage this collection manually.
  /// 
  /// \return
  ///   Collection containing all foreground entities.
  VISION_APIFUNC VisEntityCollection_cl &GetForegroundEntities();

  /// \brief
  ///   Returns a collection that contains all entities that have been updated in the last game
  ///   tick.
  /// 
  /// Entities are added to this collection during the game loop whenever
  /// VisBaseEntity_cl::OnObject3DChanged is called. The collection is guaranteed to be free of
  /// duplicate entries. You do not have to manage this collection manually.
  /// 
  /// Note that the contents of this collection typically change every game tick.
  /// 
  /// See VisionSceneManager_cl implementation for an example.
  /// 
  /// \return
  ///   Collection containing all updated entities.
  VISION_APIFUNC VisEntityCollection_cl &GetUpdatedEntities();

  /// \brief
  ///   Resets the list of updated entities.
  /// 
  /// Since the list of updated entities is created from scratch every game tick, this method needs
  /// to be called every tick before the simulation is run.
  /// 
  /// See VisionSceneManager_cl implementation for an example.
  VISION_APIFUNC void ResetUpdatedEntitiesList();

  ///
  /// @}
  ///

private:
  friend class Vision;
  friend class VisionRenderInterface_cl;

  // initialisation / deinitialisation
  
  /// \brief
  ///   Initialises the random tables
  void InitRandTable();


  /// \brief
  ///   Removes all the entities, models, lightsources .... Called for each LoadWorld
  void Clear();

  /// \brief
  ///   Initialise the game class
  void Init();

  
  /// \brief
  ///   Deinitialise the game class
  void DeInit();



  /// \brief
  ///   Deletes all the lightsources
  int RemoveLightSources();


  /// \brief
  ///   Removes all the particle groups
  int RemoveParticleGroups();


  /// \brief
  ///   Removes all the skies and sky layers
  int RemoveSkies();

  /// \brief
  ///   Removes all the screenmask instances
  int RemoveScreenMasks();

  /// \brief
  ///   Removes all the event areas
  int RemoveConvexVolumes();
   
  /// \brief
  ///   Removes all the paths and path nodes
  int RemovePaths();
  
  /// \brief
  ///   Removes all the vertex buffers and vertex buffer objects
  int RemoveVertexBuffers();

  /// \brief
  ///   Removes all visibility objects
  int RemoveVisibilityObjects();

  //in-game functions

  // line drawing
  VisDebugRenderQueue_cl m_DebugQueue;

  int m_iUpdateSceneCount;
    
  VStrDictionary m_SettingsDictionary;
  
};




inline int VisGame_cl::GetRand()
{
  return rand();
}


inline int VisGame_cl::GetRandNeg()
{
  return rand()*2-RAND_MAX;
}

inline float VisGame_cl::GetFloatRand()
{
  return ((float)rand()*(1.f/(float)RAND_MAX));
}


inline float VisGame_cl::GetFloatRandNeg()
{
  return (((float)rand()*(2.f/(float)RAND_MAX))-1.f);
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
