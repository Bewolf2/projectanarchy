/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokDisplayHandler.hpp

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok for physics
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************
#ifndef VHAVOKDISPLAYHANDLER_HPP_INCLUDED
#define VHAVOKDISPLAYHANDLER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>

#include <Common/Visualize/hkDebugDisplayHandler.h>
#include <Common/Visualize/hkProcess.h>
#include <Common/Visualize/hkProcessFactory.h>
#include <Common/Visualize/Shape/hkDisplayGeometry.h>
#include <Physics2012/Utilities/VisualDebugger/Viewer/Collide/hkpShapeDisplayViewer.h>
#include <Common/Base/Config/hkConfigVersion.h>

// -------------------------------------------------------------------------- //
// HAVOK DISPLAY GEOMETRY                                                     //
// -------------------------------------------------------------------------- //

///
/// \brief
///   Class that represents a display geometry constructed from a Havok geometry to
///   enable debug rendering in Vision. It consists of a mesh instance that has
///   a highlighting shader applied.
///
class vHavokDisplayGeometry: public VRefCounter
{
public:

  ///
  /// \brief
  ///   Simple struct for creating the mesh buffer of the Havok Display Geometry.
  ///
  struct SimpleMesh_t
  {
    float pos[3];   ///< Vertex data (x, y, z)
  };


  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// 
  /// \brief
  ///   Constructor that creates a mesh instance for a Havok geometry to enable debug rendering in Vision.
  /// 
  /// \param geometry
  ///   Pointer to the Havok geometry.
  ///
  /// \param transform
  ///   Reference of original transform of the Havok geometry.
  ///
  /// \param id
  ///   ID of the Havok geometry.
  ///
  /// \param tag
  ///   Tag that can be used for separating display information into different categories. 
  ///
  /// \see
  ///   vHavokDisplayHandler::addGeometry
  /// 
  vHavokDisplayGeometry(hkDisplayGeometry* geometry, const hkTransform &transform, hkUlong id, int tag);
   
  /// 
  /// \brief
  ///   Destructor
  /// 
  ~vHavokDisplayGeometry();


  ///
  /// @}
  ///


  ///
  /// @name Display Helper
  /// @{
  ///


  /// 
  /// \brief
  ///   Creates a compiled shader technique for this Havaok display geometry. Basically
  ///   a wireframe highlighting shader.
  /// 
  /// \param iColor
  ///   Color of the highlighting wireframe effect.
  ///
  VCompiledTechnique *CreateHighlightEffect(VColorRef iColor);


  /// 
  /// \brief
  ///   Sets the visibility state of this Havok Display Geometry.
  /// 
  /// \param bVisible
  ///   If TRUE, display geometry is set to be visible.
  ///
  /// \see
  ///   vHavokDisplayHandler::SetVisible
  /// 
  void SetVisible(BOOL bVisible);


  /// 
  /// \brief
  ///   Gets the visibility state of this Havok Display Geometry.
  /// 
  /// \return 
  ///   BOOL: If TRUE, display geometry is visible.
  ///
  /// \see
  ///   vHavokDisplayHandler::IsVisible
  /// 
  BOOL IsVisible() const;

  
  /// 
  /// \brief
  ///   Sets the wireframe highlighting color of this Havok Display Geometry.
  /// 
  /// \param iColor
  ///   New Color of the highlighting wireframe effect.
  ///
  /// \see
  ///   vHavokDisplayHandler::SetColor
  /// 
  void SetColor(VColorRef iColor);


  /// 
  /// \brief
  ///   Sets the owner object of this Havok Display Geometry.
  /// 
  /// \param pOwner
  ///   Owner object of this Havok Display Geometry.
  ///
  void SetOwner(VisObject3D_cl *pOwner);


  ///
  /// @}
  ///



  ///
  /// @name Update
  /// @{
  ///


  /// 
  /// \brief
  ///   Updates the position and rotation of this Havok dispay geometry. Called by vHavokDisplayHandler.
  /// 
  /// \param transform
  ///   Reference of new transform of the Havok geometry.
  ///
  /// \see
  ///   vHavokDisplayHandler::updateGeometry
  ///
  void UpdateTransform(const hkTransform &transform);


  ///
  /// @}
  ///

  hkUlong m_ID;                                 ///< ID of the geometry (address of the hkpCollidable this geometry was created from)

  VisMeshBufferObject_cl* GetMeshInstance (void) const { return m_spMeshInstance; }

private:

  VisMeshBufferObjectPtr m_spMeshInstance;    ///< Mesh buffer object for the physics shape representation
  hkvVec3 m_vPos;                        ///< Current position of the mesh buffer object
  hkvMat3 m_vRot;                     ///< Current rotation matrix of the mesh buffer object

};



// -------------------------------------------------------------------------- //
// HAVOK DISPLAY GEOMETRY LIST                                                //
// -------------------------------------------------------------------------- //


/// 
/// \brief
///   Collection of all Havok Display Geometries hold by the Havok Display Handler.
/// 
class vHavokDisplayGeometryList
{
public:
  /// \brief
  ///   Default constructor.
  vHavokDisplayGeometryList();
private:
  /// \brief
  ///   Disallows copy construction.
  vHavokDisplayGeometryList(vHavokDisplayGeometryList const&);
  /// \brief
  ///   Disallows assignment.
  vHavokDisplayGeometryList& operator=(vHavokDisplayGeometryList const&);
public:
  /// \brief
  ///   Destructor.
  ~vHavokDisplayGeometryList();

  /// \brief
  ///   Adds a display geometry to this collection.
  /// \param pGeometry
  ///   the geometry to add
  void Add(vHavokDisplayGeometry *pGeometry);

  /// \brief
  ///   Ensures that this collection can be expanded to the given capacity 
  ///   without having to grow the internal list while adding elements.
  /// \param iCapacity
  ///   the capacity to reserve
  ///
  /// Calls to this function are merely hints. Adding elements will succeed as 
  /// well without ensuring sufficient capacity first, but may be slower due to
  /// frequent re-allocations.
  void EnsureCapacity(unsigned int iCapacity);

  /// \brief
  ///   Returns the size of this collection.
  /// \return
  ///   the size of this collection
  unsigned int GetSize() const;

  /// \brief
  ///   returns whether this collection contains a display geometry with the 
  ///   specified ID that is visible.
  /// \param iID
  ///   ID of the display geometries
  /// \return
  ///   \c TRUE if this collection contains one or more display geometries with the
  ///   specified ID, and at least one of these is visible; \c FALSE otherwise
  BOOL IsVisible(unsigned int iID) const;

  /// \brief
  ///   Removes all display geometries with the specified ID from this collection.
  /// \param iID
  ///   ID of the display geometries to remove
  void Remove(unsigned int iID);

  /// \brief
  ///   Sets the color of all display geometries in this collection matching the
  ///   specified ID.
  /// \param iID
  ///   ID of the display geometries
  /// \param iColor
  ///   color to set
  void SetColor(unsigned int iID, VColorRef iColor);

  /// \brief
  ///   Sets the owner of all display geometries in this collection matching the
  ///   specified ID.
  /// \param iID
  ///   ID of the display geometries
  /// \param pOwner
  ///   owner to set
  void SetOwner(unsigned int iID, VisObject3D_cl *pOwner);

  /// \brief
  ///   Sets the visibility of all display geometries in this collection matching the
  ///   specified ID.
  /// \param iID
  ///   ID of the display geometries
  /// \param bVisible
  ///   visibility flag
  void SetVisible(unsigned int iID, BOOL bVisible);

  /// \brief
  ///   Updates the transform of all display geometries in this collection matching the
  ///   specified ID.
  /// \param iID
  ///   ID of the display geometries
  /// \param transform
  ///   transform to set
  void UpdateTransform(unsigned int iID, const hkTransform &transform);

private:
  DynArray_cl<vHavokDisplayGeometry*> m_List; ///< The list of geometries managed by this instance
  unsigned int m_iSize; ///< The number of geometries managed by this instance
};



// -------------------------------------------------------------------------- //
// HAVOK DISPLAY HANDLER                                                      //
// -------------------------------------------------------------------------- //


/// 
/// \brief
///   Vision specific implementation of the hkDebugDisplayHandler. 
/// 
class vHavokDisplayHandler: public VRefCounter, public hkDebugDisplayHandler, public IHavokStepper
{
public:


  ///
  /// @name Constructor / Destructor
  /// @{
  ///
   

  ///
  /// \brief
  ///   Constructor
  ///
  /// Initializes the Havok Display Handler.
  /// 
  /// \param pPhysicsWorld
  ///   The Havok physics world object. Container for the simulation's physical objects.
  ///
  vHavokDisplayHandler(hkpWorld* pPhysicsWorld);
  

  ///
  /// \brief
  ///   Destructor
  ///
  /// De-initializes the Havok Display Handler.
  ///
  VOVERRIDE ~vHavokDisplayHandler();


  ///
  /// @}
  ///


  ///
  /// @name hkDebugDisplayHandler Virtual Overrides
  /// @{
  ///
  

  ///
  /// \brief
  ///   Adds a list of geometries to the display world managed by this display handler. 
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult addGeometry(const hkArrayBase<hkDisplayGeometry *> &geometries, const hkTransform &transform, hkUlong id, int tag, hkUlong shapeIdHint, hkGeometry::GeometryType geomType) HKV_OVERRIDE;

  ///
  /// \brief
  ///   Adds a geometry to the display world managed by this display handler. 
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult addGeometry(hkDisplayGeometry *geometry, hkUlong id, int tag, hkUlong shapeIdHint) HKV_OVERRIDE;

  ///
  /// \brief
  ///   Adds an instance of a geometry that has already been added to 
  ///   the display world managed by this display handler. 
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult addGeometryInstance(hkUlong originalInstanceId, const hkTransform& transform, hkUlong id, int tag, hkUlong shapeIdHint) HKV_OVERRIDE;


  ///
  /// \brief
  ///   Updates the transform of a body in the display world.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult updateGeometry(const hkTransform &transform, hkUlong id, int tag) HKV_OVERRIDE;

#if (HAVOK_SDK_VERSION_MAJOR >= 2010)
  ///
  /// \brief
  ///   Updates the transform of a body in the display world.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult updateGeometry(const hkMatrix4& transform, hkUlong id, int tag) HKV_OVERRIDE;
#endif

  ///
  /// \brief
  ///   Removes a geometry from the display world managed by this display handler. 
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult removeGeometry(hkUlong id, int tag, hkUlong shapeIdHint) HKV_OVERRIDE;

  ///
  /// \brief
  ///   Sets the visibility of a given child geometry in a display object.
  ///
  VOVERRIDE hkResult setGeometryVisibility(int geometryIndex, bool isEnabled, hkUlong id, int tag) HKV_OVERRIDE;

  ///
  /// \brief
  ///   Sets the color of a geometry previously added to the display world. 
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult setGeometryColor( hkColor::Argb color, hkUlong id, int tag) HKV_OVERRIDE;

  /// \brief
  ///   Sets the transparency of a geometry previously added to the display world.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult setGeometryTransparency(float alpha, hkUlong id, int tag) HKV_OVERRIDE;

  /// \brief
  ///   Set whether or not a geometry is pickable.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult setGeometryPickable( hkBool isPickable, hkUlong id, int tag ) HKV_OVERRIDE;

  ///
  /// \brief
  ///   Updates the camera from the display world managed by this display handler.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult updateCamera(const hkVector4& from, const hkVector4& to, const hkVector4& up, hkReal nearPlane, hkReal farPlane, hkReal fov, const char* name) HKV_OVERRIDE;
  

  ///
  /// \brief
  ///   Updates the behavior from the display world managed by this display handler.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult updateBehavior(const hkArrayBase< int > &wordVarIdx, const hkArrayBase< int > &wordStack, const hkArrayBase< int > &quadVarIdx, const hkArrayBase< hkVector4 > &quadStack, const hkArrayBase< char * > &activeNodes, const hkArrayBase< int > &activeStateIds, const hkArrayBase< int > &activeTransitions, const hkArrayBase< hkQsTransform > &transforms);

  ///
  /// \brief
  ///   Puts a display point into the display buffer for display in the next frame.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult displayPoint(const hkVector4& position, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
  

  ///
  /// \brief
  ///   Puts a display line into the display buffer for display in the next frame.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult displayLine(const hkVector4& start, const hkVector4& end, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
  

  ///
  /// \brief
  ///   Puts a display triangle into the display buffer for display in the next frame.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult displayTriangle(const hkVector4& a, const hkVector4& b, const hkVector4& c, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;


  ///
  /// \brief
  ///   Outputs user text to the display.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult displayText(const char* text, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
    

  ///
  /// \brief
  ///   Outputs 3D text. Same as displayText() but with position.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult display3dText(const char* text, const hkVector4& pos, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;

  // compatibility with VisionIntegration branch
#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
  ///
  /// \brief
  /// Puts a display point in 2D [-1..1] normalized windows coordinates into the display buffer for display in the next frame.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult displayPoint2d(const hkVector4& position, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;

  ///
  /// \brief
  /// Puts a display line in 2D [-1..1] normalized windows coordinates into the display buffer for display in the next frame.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult displayLine2d(const hkVector4& start, const hkVector4& end, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;

  ///
  /// \brief
  /// Puts a display triangle in 2D [-1..1] normalized windows coordinates into the display buffer for display in the next frame.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult displayTriangle2d(const hkVector4& a, const hkVector4& b, const hkVector4& c, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;

  ///
  /// \brief
  /// Outputs text in 2D [-1..1] normalized windows coordinates. Also scales the text wrt the font size using sizeScale.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult displayText2d(const char* text, const hkVector4& pos, hkReal sizeScale, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
#endif

  ///
  /// \brief
  ///   Display general annotation data. 
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult displayAnnotation(const char* text, int id, int tag) HKV_OVERRIDE;


  ///
  /// \brief
  ///   Displays the geometries.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult displayGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, const hkTransform& transform, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
    

  ///
  /// \brief
  ///   Displays the geometries without transform.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult displayGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;


  ///
  /// \brief
  ///   Send memory statistics dump.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult sendMemStatsDump(const char* data, int length) HKV_OVERRIDE;


  ///
  /// \brief
  ///   Ensures that the current immediate mode display information will be preserved 
  ///   and merged with all new immediate mode data for the next step/frame.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE hkResult holdImmediate() HKV_OVERRIDE;


  ///
  /// \brief
  ///   Batch update speed increases.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE void lockForUpdate() HKV_OVERRIDE;
  

  ///
  /// \brief
  ///   Unlock for Update.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE void unlockForUpdate() HKV_OVERRIDE;


  /// \brief
  ///   Implements IHavokStepper.
  ///   See Havok documentation for detailed information.
  ///
  VOVERRIDE void Step(float dt) HKV_OVERRIDE;


#if (HAVOK_SDK_VERSION_MAJOR >= 2010)

  /// Adds a mesh to the display world managed by this display handler.
  VOVERRIDE hkResult addMesh(hkMeshBody* mesh, hkUlong id, int tag) { return HK_FAILURE; }

  /// Removes a mesh from the display world managed by this display handler.
  VOVERRIDE hkResult removeMesh(hkUlong id, int tag) { return HK_FAILURE; }

  /// Updates the root transform of a mesh with a hkMatrix4.
  VOVERRIDE hkResult updateMesh( const hkMatrix4& transform, hkUlong id, int tag )  { return HK_FAILURE; }

  /// Updates the skin of a mesh given a pose using hkMatrix4s.
  VOVERRIDE hkResult skinMeshes(hkUlong* ids, int numIds, const hkMatrix4* poseModel, int numPoseModel, const hkMatrix4& worldFromModel, int tag )  { return HK_FAILURE; }

  /// Skins a body in the display world. Only geometries of type hkDisplayMesh can be skinned.
  VOVERRIDE hkResult skinGeometry(hkUlong* ids, int numIds, const hkMatrix4* poseModel, int numPoseModel, const hkMatrix4& worldFromModel, int tag ) { return HK_FAILURE; }
#endif

  ///
  /// @}
  ///


  ///
  /// @name Display Helper
  /// @{
  ///

  
  /// 
  /// \brief
  ///   Sets the visibility state of the Havok Display Geometry identified by the given ID.
  /// 
  /// \param id
  ///   ID of the geometry (address of the hkpCollidable this geometry was created from).
  ///
  /// \param bVisible
  ///   If TRUE, display geometry is set to be visible.
  ///
  /// \see
  ///   vHavokDisplayHandler::SetVisible
  /// 
  void SetVisible(hkUlong id, BOOL bVisible);

  
  /// 
  /// \brief
  ///   Gets the visibility state of the Havok Display Geometry identified by the given ID.
  /// 
  /// \param id
  ///   ID of the geometry (address of the hkpCollidable this geometry was created from).
  ///
  /// \return 
  ///   BOOL: If TRUE, display geometry is visible.
  ///
  /// \see
  ///   vHavokDisplayHandler::IsVisible
  /// 
  BOOL IsVisible(hkUlong id) const;


  /// 
  /// \brief
  ///   Sets the wireframe highlighting color of the Havok Display Geometry identified by the given ID.
  /// 
  /// \param id
  ///   ID of the geometry (address of the hkpCollidable this geometry was created from).
  ///
  /// \param iColor
  ///   New Color of the highlighting wireframe effect.
  ///
  /// \see
  ///   vHavokDisplayHandler::SetColor
  /// 
  void SetColor(hkUlong id, VColorRef iColor);


  /// 
  /// \brief
  ///   Sets the owner object so we can attach the Havok shape to it
  /// 
  /// \param id
  ///   ID of the geometry (address of the hkpCollidable this geometry was created from).
  ///
  /// \param pOwner
  ///   Owner object of the Havok component.
  ///
  /// \see
  ///   vHavokDisplayHandler::SetColor
  /// 
  void SetOwner(hkUlong id, VisObject3D_cl *pOwner);

  inline hkArray<hkProcessContext*>& getContexts() { return m_contexts; }


  ///
  /// @}
  ///

private:

  hkpPhysicsContext* m_pContext;                  ///< Physics Context that holds all physics viewer
  hkArray<hkProcess*> m_pLocalProcesses;          ///< Local Processes handled by this display handler
  vHavokDisplayGeometryList m_Geometries;         ///< List of all geometries in the physics world
  hkArray<hkProcessContext*> m_contexts;

};

#endif // VHAVOKDISPLAYHANDLER_HPP_INCLUDED

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
