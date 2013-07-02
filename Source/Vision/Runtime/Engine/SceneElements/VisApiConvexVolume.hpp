/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiConvexVolume.hpp

#ifndef DEFINE_VISAPICONVEXVOLUME
#define DEFINE_VISAPICONVEXVOLUME

#include <string.h>

#include <Vision/Runtime/Engine/System/ModuleSystem/VisApiModuleSystemChild.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiObject3D.hpp>
#include <Vision/Runtime/Engine/System/ElementManager/VisApiElementManager.hpp>


struct AreaPolygon_t;
class VisParticleGroup_cl;
class VisConvexVolume_cl;



/// \brief
///   Represents a convex volume defined by a set of planes.
/// 
/// At least four planes are required to form a valid convex shape. The resulting volume represents
/// the smallest convex shape enclosed by the planes.
/// 
/// The VisConvexVolume_cl class can, for instance, be used for all kinds of intersection and
/// overlap tests. The various render collection classes provide functionality for filtering
/// collections of entities, primitives etc. by their spatial relationship to a convex volume.
class VisConvexVolume_cl : public VisTypedEngineObject_cl, public VUserDataObj, public VisObjectKey_cl, public VisModuleSystemChild_cl, public VisElementManager_cl<class VisConvexVolume_cl *>
{
  friend class SerializationTest;
  V_DECLARE_SERIAL_DLLEXP( VisConvexVolume_cl, VISION_APIDATA )
public:

  ///
  /// @name Constructors / Destructor
  /// @{
  ///
  
  /// \brief
  ///   Constructor of the convex volume class
  /// 
  /// The constructor initialises an empty volume. No planes are defined, and therefore, the volume
  /// is empty.
  /// 
  /// \param volumeKey
  ///   key to identify this convex volume
  VISION_APIFUNC VisConvexVolume_cl(const char *volumeKey);
  
  /// \brief
  ///   Default constructor which is only uses by the serialization code
  /// 
  /// Do not use this constructor in your game code
  VISION_APIFUNC VisConvexVolume_cl();
  
  /// \brief
  ///   Destructor of the convex volume class
  /// 
  /// Use the remove function instead of the destructor.
  VISION_APIFUNC virtual ~VisConvexVolume_cl();

  /// \brief
  ///   Dispose the object and remove it from scene
  VISION_APIFUNC VOVERRIDE void DisposeObject();

  /// \brief
  ///   Removes the volume from the scene and destroy it.
  /// 
  /// Use this function instead of simply calling delete.
  VISION_APIFUNC void Remove();


  ///
  /// @}
  ///

  ///
  /// @name Geometry Related Functions
  /// @{
  ///
  
  /// \brief
  ///   Adds another plane to the volume.
  /// 
  /// At least four planes are required to form a valid convex volume.
  /// 
  /// Each plane's normal has to point out of the event area, otherwise the event area is invalid.
  /// 
  /// \param plane
  ///   plane to be added
  /// 
  /// \return
  ///   int planeNumber: internal 0-based index of the plane, can be used to replace the plane
  VISION_APIFUNC int AddPlane(const hkvPlane& plane);


  /// \brief
  ///   Adds another plane to the volume.
  /// 
  /// At least four planes are required to form a valid convex volume.
  /// 
  /// Each plane's normal has to point out of the event area, otherwise the event area is invalid.
  /// 
  /// \param dotA
  ///   first of three vertices which describe the plane
  /// 
  /// \param dotB
  ///   second of three vertices which describe the plane
  /// 
  /// \param dotC
  ///   third of three vertices which describe the plane
  /// 
  /// \return
  ///   int planeNumber: internal 0-based index of the plane, can be used to replace the plane
  VISION_APIFUNC int AddPlaneByPoints(const hkvVec3& dotA, const hkvVec3& dotB, const hkvVec3& dotC);


  /// \brief
  ///   Replaces an already existing plane with a new plane
  /// 
  /// \param planeNr
  ///   internal 0-based plane index of the plane to be replaced
  /// 
  /// \param plane
  ///   new plane 
  /// 
  /// \return
  ///   BOOL result: TRUE if the plane has been replaced successfully
  VISION_APIFUNC BOOL ReplacePlane(int planeNr, const hkvPlane& plane);
  

  /// \brief
  ///   Returns the number of planes defined for this convex volume
  /// 
  /// GetNumberOfPlanes returns the number of planes which have been added to this volume.
  /// 
  /// \return
  ///   int numOfPlanes:   number of planes
  inline unsigned int GetNumberOfPlanes() const {return m_iNumPlanes;}
  
  /// \brief
  ///   Gets a pointer to a specific plane of the convex volume.
  /// 
  /// The index must be in the range of [0..GetNumberOfPlanes()-1]
  /// 
  /// To modify a plane, use the ReplacePlane function.
  /// 
  /// \param iIndex
  ///   0-based index of the plane. This index is returned by AddPlane.
  /// 
  /// \return
  ///   const hkvPlane* pPlane : Pointer to the plane with given index.
  const hkvPlane* GetPlane(unsigned int iIndex) const {VASSERT(iIndex<m_iNumPlanes && m_pPlane.GetDataPtr()[iIndex]);return m_pPlane.GetDataPtr()[iIndex];}

  /// \brief
  ///   Returns whether this volume is valid
  /// 
  /// A convex volume is considered valid if the planes defining it fully enclose a space with a
  /// volume larger than 0.
  /// 
  /// \return
  ///   BOOL valid: TRUE if this event area is valid
  VISION_APIFUNC BOOL IsValid() const;

  /// \brief
  ///   Gets the axis-aligned bounding box of the volume.
  /// 
  /// The bounding box is calculated from the planes that describe a convex volume
  /// 
  /// \param box
  ///   Reference to the bounding box object in which the values will be stored.
  VISION_APIFUNC void GetBoundingBox(hkvAlignedBBox &box) const;

  /// \brief
  ///   Get a pointer to a bounding box
  /// 
  /// This method returns a const pointer because the bounding box can only be changed by
  /// modifying the volume's plane definitions.
  /// 
  /// \return
  ///   const hkvAlignedBBox* : Pointer to the bounding box
  VISION_APIFUNC const hkvAlignedBBox* GetSimpleBoundingBox() const;
 

  ///
  /// @}
  ///

  ///
  /// @name Information Queries
  /// @{
  ///

  /// \brief
  ///   Returns the volume's key string 
  /// 
  /// which was originally passed to the constructor.
  /// 
  /// This function wraps around GetObjectKey of the base class. Inputs
  /// 
  /// Nothing
  /// 
  /// \return
  ///   const char: the key
  inline const char *GetKey() const {return GetObjectKey();}

  
  ///
  /// @}
  ///

  ///
  /// @name Serialisation
  /// @{
  ///
  

  /// \brief
  ///   Serializes the object
  /// 
  /// This function serializes/deserializes a convex volume instance.
  /// 
  /// For more information about serialization, see the Vision API Documentation.
  /// 
  /// \param ar
  ///   The archive to read from or write to
  VISION_APIFUNC VOVERRIDE void Serialize( VArchive& ar );



  ///
  /// @}
  ///

  ///
  /// @name Overlap/intersection Tests
  /// @{
  ///
  

  /// \brief
  ///   Returns whether the volume overlaps the passed Bounding Box.
  /// 
  /// \param box
  ///   Reference to the bounding box
  /// 
  /// \return
  ///   BOOL: TRUE if the volume overlaps the bounding box.
  VISION_APIFUNC BOOL Overlaps(const hkvAlignedBBox& box) const;

  /// \brief
  ///   Returns whether the volume overlaps the passed Bounding Sphere.
  /// 
  /// \param sphere
  ///   Reference to the bounding sphere
  /// 
  /// \return
  ///   BOOL: TRUE if the volume overlaps bounding sphere.
  VISION_APIFUNC BOOL Overlaps(const hkvBoundingSphere& sphere) const;


  /// \brief
  ///   Returns the distance of an arbitrary point to the volume's surface
  /// 
  /// A positive return value indicates that the position is outside, a negative value indicates
  /// that the position is outside the volume
  /// 
  /// The distance value for positions outside the volume is only an approximation, since the
  /// distance computation is limited to point-plane-checks for performance reasons.
  /// 
  /// For positions outside the volume, the returned value represents a lower bound of the actual
  /// distance.
  /// 
  /// \param pos
  ///   Position in world space to calculate the distance
  /// 
  /// \return
  ///   float fDistance : The distance in world units.
  VISION_APIFUNC float GetPointDistance(const hkvVec3& pos) const;

 
  /// \brief
  ///   Prepares a list of all entities which are currently in the volume.
  /// 
  /// ResetQuery looks for all the entities which currently overlap the volume, and adds them to a
  /// stack. This stack can be accessed with the QueryEntitiesInVolume method in order to get all
  /// the entities which currently overlap the volume.
  /// 
  /// For the overlap calculation, the entity's axis-aligned bounding box is used.
  VISION_APIFUNC void ResetQuery();
  

  /// \brief
  ///   Gets the next entity on the stack which is currently in the volume
  /// 
  /// QueryEntitiesInVolume returns the next entity on the queried entity stack. 
  /// 
  /// It requires that the entiy stack has previously been set up using the ResetQuery method.
  /// 
  /// Use this method to iterate over all entities on the query stack.
  /// 
  /// \return
  ///   VisBaseEntity_cl *nextEntity: is either an entity in the volume, or NULL if there are no
  ///   more entities on the stack.
  VISION_APIFUNC VisBaseEntity_cl *QueryEntitiesInArea();
  

protected:
  // [INTERNAL]
  void UpdateBoundingBox() const;
  void UpdateConvexVolume() const;
  void EnsureUpdated() const {if (areaOutdated) {UpdateConvexVolume();areaOutdated=false;}}
  void DeleteAllPlanes();

  ///
  /// @}
  ///

private:
  friend int DefineNewConvexVolume(char *key);
  
  /// \brief
  ///   Initialises the event area
  void Init(const char *AreaKey);
  

  /// \brief
  ///   Initialization code common to the constructors as well as serialization
  void CommonInit();



  /// \brief
  ///   Set the event area matrix, doesn't work for event areas which are attached to entities or
  ///   lightsources.
  VISION_APIFUNC void SetMatrix(hkvMat4& matrix);


  // general variables
  //VString m_key;                        // event area key (serialized), now in base class
  int *entityQueryTable;                  ///< dynamic allocated stack for the entity queries
  int entityQueryTablePos;                ///< current position in the entity queries stack

  unsigned int   m_iNumPlanes;
  DynArray_cl<hkvPlane* >m_pPlane;

  char  validity;
  char  active;
  mutable bool areaOutdated;                      ///< if TRUE then the event area has to be updated (for bBox etc.)
  mutable hkvAlignedBBox m_BoundingBox;


  
protected:
  friend class VisEntityCollection_cl;
  friend class VisLightSrcCollection_cl;
  friend class VisVisibilityObjectCollection_cl;

};


VISION_ELEMENTMANAGER_TEMPLATE_DECL(VisConvexVolume_cl)


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
