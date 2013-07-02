/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiPath.hpp

// VisApiPath.hpp: interface for the VisPath_cl class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DEFINE_VISAPIPATH
#define DEFINE_VISAPIPATH

#include <Vision/Runtime/Engine/SceneElements/VisApiPathNode.hpp>

typedef VSmartPtr<class VisPath_cl> VisPathPtr;

/// \brief
///   Path class that defines a bezier spline path
/// 
/// A single path can have an arbitrary number of path nodes. Each path node can have an in- and
/// out- tangent direction that define the curve behaviour.
/// 
/// The path class provides useful functionality to evaluate positions on the path curve. Thus
/// paths are useful for defining camera scenes or predefined object movement paths.
/// 
/// Paths objects are usually created in vForge, but path instances can also be created via code.
/// See Programmer's Documentation.
/// 
/// All path objects in the scene are added to the global element manager and cleaned up when
/// deinitializing a world.
/// 
/// Since the path class is reference counted, path objects can also retain the cleanup if
/// something has a (smart pointer) reference to it.
/// 
/// \sa VisPathNode_cl
/// \sa VisGame_cl::SearchPath
class VisPath_cl : public VisObject3D_cl, public VisElementManager_cl<VisPath_cl*>
{
  V_DECLARE_SERIAL_DLLEXP( VisPath_cl, VISION_APIDATA )

public:

  ///
  /// @name Constructors, Destructors
  /// @{
  ///

  /// \brief
  ///   Constructor that reserves pointers for a specific number of path nodes
  /// 
  /// \param countPathNodes
  ///   Initial number of path nodes for reserving the pointer array.  This number can be increased
  ///   by adding more nodes, but to avoid re-allocation this should match the exact number. The
  ///   nodes itself still have to be added via AddPathNode.
  /// 
  /// \param pathClosed
  ///   If TRUE, this path is closed, which causes the path parameter value to wrap at the end of
  ///   the path
  /// 
  /// \param newKey
  ///   Optional Key for this path object for finding this path via VisGame_cl::SearchPath.
  /// 
  /// \example
  ///   \code
  ///   VisPath_cl *pNewPath = new VisPath_cl(5, TRUE, "NewPath");
  ///   \endcode
  VISION_APIFUNC VisPath_cl(int countPathNodes, BOOL pathClosed = false, const char* newKey = NULL);

  /// \brief
  ///   Empty constructor for de-serialization.
  VISION_APIFUNC VisPath_cl();


  /// \brief
  ///   Destructor of VisPath_cl, releases the references to the path nodes
  VISION_APIFUNC virtual ~VisPath_cl();

  /// \brief
  ///   Overridden VisTypedEngineObject_cl function to remove this instance from the scene
  VISION_APIFUNC VOVERRIDE void DisposeObject();

  ///
  /// @}
  ///

  ///
  /// @name Add/delete Nodes
  /// @{
  ///

  /// \brief
  ///   Adds one path node to this path.
  /// 
  /// The reference counter of the node will be increased
  /// 
  /// \param node
  ///   Pointer to a VisPathNode_cl instance
  VISION_APIFUNC void AddPathNode(VisPathNode_cl *node);

  /// \brief
  ///   Removes all path nodes from this path.
  /// 
  /// If there are no external (smart pointer) references to the respective nodes, the node objects
  /// will be destroyed.
  VISION_APIFUNC void DeleteAllPathNodes();

  ///
  /// @}
  ///

  ///
  /// @name Access Nodes
  /// @{
  ///

  /// \brief
  ///   Returns the number of path nodes in this path.
  /// 
  /// \return
  ///   int iNodeCount : The number of nodes
  inline int GetPathNodeCount() const;

  /// \brief
  ///   Gets the path node at specified index for read-only access.
  /// 
  /// \param iIndex
  ///   0-based index of the node to get. Must be [0..GetPathNodeCount()-1]
  /// 
  /// \return
  ///   const VisPathNode_cl *pNode : path node at specified index
  VISION_APIFUNC const VisPathNode_cl* GetPathNode(int iIndex) const;

  /// \brief
  ///   Gets the path node at specified index.
  /// 
  /// This version of GetPathNode allows the programmer to modify the returned node, e.g. its
  /// position or tangents.
  /// 
  /// If the node is intended to be modified, the bModify flag should be set to TRUE.
  /// 
  /// This information is important to invalidate cached length information.
  /// 
  /// \param iIndex
  ///   0-based index of the node to get. Must be [0..GetPathNodeCount()-1]
  /// 
  /// \param bModify
  ///   If TRUE, the cached path values will be invalidated (same as Invalidate())
  /// 
  /// \return
  ///   const VisPathNode_cl *pNode : path node at specified index
  VISION_APIFUNC VisPathNode_cl* GetPathNode(int iIndex, BOOL bModify);

  /// \brief
  ///   Returns the first occurrence of a child path node with specified key string. Only searches in child nodes of this path
  /// 
  /// \param szKey
  ///   The key string to search for
  ///
  /// \param bIgnoreCase
  ///   If TRUE, string comparison is not type sensitive
  /// 
  /// \return
  ///   The first node with specified key, or NULL
  inline VisPathNode_cl* FindPathNode(const char *szKey, BOOL bIgnoreCase=TRUE) const;


  /// \brief
  ///   Returns the distance along the spline between two neighboring nodes.
  ///
  /// This function does not perform calculations because distances are ony computed once via VisPathNode_cl::getLength
  /// and then cached.
  ///
  /// \param iFirstNode
  ///   The first node index of the neighboring pairs. This index must be [0..GetPathNodeCount()-2]
  ///
  /// \return
  ///   The distance along the spline (approximated)
  inline float GetDistanceBetweenNeighborNodes(int iFirstNode)
  {
    VASSERT(iFirstNode>=0 && iFirstNode<m_iNodeCount-1);
    GetLen(); // ensure it is cached
    return pathSegmentLength.GetDataPtr()[iFirstNode];
  }

  ///
  /// @}
  ///

  ///
  /// @name Path Properties
  /// @{
  ///

  /// \brief
  ///   Sets the closed status of the path.
  /// 
  /// Closed paths wrap around the path evaluation parameter, whereas non-closed path clamp the
  /// parameter to [0..1]
  /// 
  /// \param pathClosed
  ///   new status
  VISION_APIFUNC void SetClosed(BOOL pathClosed);

  
  /// \brief
  ///   Returns the "closed" status of this path
  /// 
  /// \return
  ///   BOOL pathClosed: TRUE if the path is closed
  inline BOOL IsClosed() const;

    
    
  ///
  /// @}
  ///

  ///
  /// @name Evaluation Functions
  /// @{
  ///

  /// \brief
  ///   Evaluates a point on the path
  /// 
  /// based on the path parameter t. t is a float value ranging from 0.0f (the first node) to 1.0f
  /// (the last node in this path).
  /// 
  /// Each path segement between two nodes is associated with a parameter range of size
  /// 1.f/GetPathNodeCount()
  /// 
  /// Thus the t values are not distributed linearly over the full length of the path.
  /// 
  /// The EvalPointSmooth function compensates for that.
  /// 
  /// \param t
  ///   path parameter between 0.0f and 1.0f. Values outside this range are also handled correctly
  ///   according to the IsClosed() status of this path.
  /// 
  /// \param pos
  ///   Reference to a vector to store the interpolated position.
  /// 
  /// \param firstDerivative
  ///   If this pointer is not NULL, the first derivative of the evaluated  position is stored
  ///   here. This information is useful for movement velocities/lookat direction
  /// 
  /// \param secondDerivative
  ///   If this pointer is not NULL, the second derivative of the evaluated position is stored
  ///   here. This information is useful for the roll Euler angle of the moving object.
  /// 
  /// \sa VisPath_cl::SetClosed()
  /// \sa VisPath_cl::EvalPointSmooth()
  /// 
  /// \example
  ///   \code
  ///   VisVector newPos, firstDer;
  ///   pNewPath->EvalPoint(0.5f, newPos, &firstDer);
  ///   \endcode
  VISION_APIFUNC void EvalPoint(float t, hkvVec3& pos, hkvVec3* firstDerivative = NULL, hkvVec3* secondDerivative = NULL) const;

  /// \brief
  ///   Evaluates a point on the path
  /// 
  /// based on the path parameter t. t is a float value ranging from 0.0f (the first node) to 1.0f
  /// (the last node in this path).
  /// 
  /// Each path segement between two nodes is associated with a parameter range that is
  /// proportional to the segment's length.
  /// 
  /// Thus the t values are distributed linearly over the path segments.
  /// 
  /// \param t
  ///   path parameter between 0.0f and 1.0f. Values outside this range are also handled correctly
  ///   according to the IsClosed() status of this path.
  /// 
  /// \param pos
  ///   Reference to a vector to store the interpolated position.
  /// 
  /// \param firstDerivative
  ///   If this pointer is not NULL, the first derivative of the evaluated  position is stored
  ///   here. This information is useful for movement velocities/lookat direction
  /// 
  /// \param secondDerivative
  ///   If this pointer is not NULL, the second derivative of the evaluated position is stored
  ///   here. This information is useful for the roll Euler angle of the moving object.
  /// 
  /// \sa VisPath_cl::SetClosed()
  /// \sa VisPath_cl::EvalPoint()
  VISION_APIFUNC void EvalPointSmooth(float t, hkvVec3& pos, hkvVec3* firstDerivative = NULL, hkvVec3* secondDerivative = NULL);

  /// \brief
  ///   Returns the path parameter (t) for the specified node
  /// 
  /// Since the EvalPointSmooth function distributes the parameter's [0..1] interval over the full
  /// path according to relative path length, every path node is associated with a parameter value
  /// that is not proportional to its index.
  /// 
  /// This function returns the t value of the path node's position.
  /// 
  /// \param iNodeIndex
  ///   Index of the path node. Must be in valid range [0..GetPathNodeCount()-1]
  /// 
  /// \return
  ///   float t: The path parameter at the node's position. Passing this value to the
  ///   EvalPointSmooth function evaluates the position of that path node
  /// 
  /// \sa VisPath_cl::EvalPointSmooth()
  VISION_APIFUNC float GetNodeParameterSmooth(int iNodeIndex);

  ///
  /// @}
  ///

  ///
  /// @name Misc
  /// @{
  ///
  

  /// \brief
  ///   Invalidates the cached path properties such as path length.
  /// 
  /// This function should be called after a path node has been modified, for instance its control
  /// vertices changed.
  inline void Invalidate() {m_fLength=-1;m_iIndexCache = -1;MarkAsChanged();}

  /// \brief
  ///   Makes the control vertices of all nodes in this path colinear.
  /// 
  /// This avoids sharp corners at node positions.
  /// 
  /// Since each node has an in- and out- tangent, this function can either make "in" colinear to
  /// "out" our vice versa. This is defined by the parameter.
  /// 
  /// \param inOrOutNode
  ///   Determines the source tangent:
  ///   \li IN_VECTOR:   make the in vector colinear to the out vector (Change the first control
  ///     vertex according to the second)
  /// 
  ///   \li OUT_VECTOR:  make the out vector colinear to the in vector (Change the second control
  ///     vertex according to the first)
  /// 
  /// \example
  ///   \code
  ///   pNewPath->MakeControlVerticesColinear( IN_VECTOR );
  ///   \endcode
  VISION_APIFUNC void MakeControlVerticesColinear( int inOrOutNode );

  /// \brief
  ///   Returns the length of the path.
  /// 
  /// The length of a path is cached until path properties are changed that influence the length.
  /// 
  /// Recalculating the length is a performance critical operation.
  /// 
  /// \return
  ///   float fLength: The length of this path in world units.
  inline float GetLen();

  /// \brief
  ///   Returns the key of the path. Wraps around GetObjectKey.
  /// 
  /// \return
  ///   const char *result: key string, or NULL
  inline const char* GetKey() const
  {
    return GetObjectKey();
  }

  /// \brief
  ///   Sets a new path key string. Wraps around SetObjectKey.
  /// 
  /// \param szNewKey
  ///   New key string, can be NULL
  inline void SetKey(const char* szNewKey) 
  {
    SetObjectKey(szNewKey);
  }

  /// \brief
  ///   Returns the frame number the path or one of its nodes changed last time in any way.
  /// 
  /// \return
  ///   int iFrameNumber: Frame number in which path changed last time. \see Vision::Game.GetUpdateSceneCount
  inline int GetLastChangedFrame() const
  {
    return m_iLastChangedFrame;
  }

  /// \brief
  ///   Computes the world space bounding box of this path instance (union of all node positions)
  /// 
  /// The bounding box is not cached, so this operation is performance critical.
  /// 
  /// It takes the union of all node positions and thus does not consider that the spline curve
  /// might be outside this box.
  /// 
  /// \param bbox
  ///   target bounding box reference
  VISION_APIFUNC void ComputeBoundingBox(hkvAlignedBBox &bbox) const;

  ///
  /// @}
  ///

  ///
  /// @name Debugging
  /// @{
  ///
  

  /// \brief
  ///   Puts path information (length/closed/count) into a string for debug purposes
  /// 
  /// \return
  ///   VString result: path information
  /// 
  /// \example
  ///   \code
  ///   Vision::Message.Add(pNewPath->ToString(), 1);
  ///   \endcode
  VISION_APIFUNC VString ToString();


  /// \brief
  ///   Renders this path by approximating it with line segments.
  /// 
  /// For rendering, a segment is subdivided into 10 lines.
  /// 
  /// \param color
  ///   Color used to display the path, default is green
  /// 
  /// \param width
  ///   Width of the line, not supported on all platforms
  /// 
  /// \example
  ///   \code
  ///   pPath->DrawPath(V_RGB(255,0,0), 3.0f);
  ///   \endcode
  VISION_APIFUNC void DrawPath(VColorRef color=V_RGBA_GREEN, float width=2.0f);

  
  /// \brief
  ///   Fully serializes the path instance
  /// 
  /// This function stores all important data into an archive or reads from it to setup an object.
  /// 
  /// The path class supports full serialization, thus path instances can be created from an
  /// archive using ar.ReadObject(NULL).
  /// 
  /// For more information about serialization, see the Vision API Documentation.
  /// 
  /// \param ar
  ///   The archive object to read from or write to
  VISION_APIFUNC VOVERRIDE void Serialize( VArchive &ar );


protected:

  /// \brief
  ///   Overridden VisObject3D_cl function
  VISION_APIFUNC VOVERRIDE void OnObject3DChanged(int iO3DFlags);

  friend class VisPathNode_cl;
  void UpdateLinearTangents();
  void MarkAsChanged();

  ///
  /// @}
  ///

private:
  VISION_APIFUNC void CalcLen();   ///<Calculates the length of the path
  
  int m_iNodeCount;        ///< number of the path nodes
  bool m_bClosed;          ///< true, if last path node is connected with the first path node
  float m_fLength;         ///< length of the path (-1 if uncached)
  int m_iLastChangedFrame; ///< last frame when path or its nodes changed in any way
  int m_iIndexCache;
  float m_ftlen_cache;

  //VString m_key; // path key, now in object3d

  DynArray_cl<VisPathNode_cl*> pathNodes; ///< pointer to the nodes of this path
  SerialDynArray_cl<float> pathSegmentLength;   ///< length of path segments
};


VISION_ELEMENTMANAGER_TEMPLATE_DECL(VisPath_cl)

//Inline functions

float VisPath_cl::GetLen()
{
  if (m_fLength>=0.f) return m_fLength;
  CalcLen();
  return m_fLength;
}

inline int VisPath_cl::GetPathNodeCount() const
{
  return m_iNodeCount;
}

inline BOOL VisPath_cl::IsClosed() const
{
  return m_bClosed;
}

inline VisPathNode_cl* VisPath_cl::FindPathNode(const char *szKey, BOOL bIgnoreCase) const
{
  for (int i = 0; i < m_iNodeCount; i++)
    if (pathNodes.GetDataPtr()[i]->HasObjectKey(szKey,bIgnoreCase))
      return pathNodes.GetDataPtr()[i];
  return NULL;
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
