/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file IVisApiPhysicsModule.hpp

#ifndef FR_DEFINE_VISAPIPHYSICSMODULE
#define FR_DEFINE_VISAPIPHYSICSMODULE


class IVisPhysicsObject_cl;
class VisObject3D_cl;
class VTerrainSector;
class VTerrainDecorationInstance;
class IVisPhysicsModule_cl;


/// \brief
///   Physics-API agnostic class for a physics material.
class VPhysicsMaterial: public VColMeshMaterial
{
public:
  VPhysicsMaterial() 
  {
    pNativeMaterial = NULL;
    bIsDefaultMaterial = true;
  }

  void *pNativeMaterial; // pointer to the physics-API specific native material
  bool bIsDefaultMaterial; // determines, whether the native material is a default material, which the corresponding physics-API automatically generates
};

/// \brief
///   Raycast result structure for physics raycast operations.
struct VisPhysicsHit_t
{
  VisPhysicsHit_t()
  {
    fHitFraction = 1.0f; eHitType = VIS_TRACETYPE_UNKNOWN; pHitObject = NULL;
  }

  inline void Reset()
  {
    fHitFraction = 2.0f; // ensure the next fraction is smaller
  }

  float fHitFraction;                   ///< Fraction between start and end of the trace at which the hit occurred.
  hkvVec3 vImpactPoint;            ///< Point (position) of impact
  hkvVec3 vImpactNormal;           ///< Surface normal at the point of impact
  VColMeshSceneElement_e eSceneElement; ///< Scene element that was hit (entity / static mesh / custom geometry)
  VisTraceHitType_e eHitType;           ///< Type of geometry that was hit; see VisTraceHitType_e for details.
  VisTypedEngineObject_cl *pHitObject;  ///< If an entity was hit, the corresponding VisObject3D_cl pointer is stored here. If a static mesh instance was hit, the corresponding VisStaticMeshInstance_cl is stored here.
  VPhysicsMaterial hitMaterial;         ///< The hit collision material. 
                                        ///< If available, a triangle based material will be returned, otherwise a shape based material.
                                        ///< If no materials were explicitly assigned to the corresponding collider, a default material will be returned.
};

/// \brief
///   Abstract base class for defining physics raycast behavior.
class VisPhysicsRaycastBase_cl
{
public:

  /// \brief
  ///   Accuracy of the raycast operation
  enum RAYCAST_ACCURACY
  {
    RAYCAST_ACC_SHAPE,
    RAYCAST_ACC_BOUNDING    
  };

  /// \brief
  ///   Constructor of the physics raycast base class.
  VisPhysicsRaycastBase_cl()
  {
    Reset();
  }

  /// \brief
  ///   Resets the data in this object.
  virtual void Reset()
  {
    iCollisionBitmask = 0xFFFFFFFF;
    bFinished = false;
    eAccuracy = RAYCAST_ACC_SHAPE;
  }

  /// \brief
  ///   Destructor of the physics raycast base class.
  virtual ~VisPhysicsRaycastBase_cl() {}

  /// \brief
  ///   This function is called whenever the underlying physics module encounters a trace hit with
  ///   any type of geometry.
  /// 
  /// The passed VisPhysicsHit_t object contains the detail information about the hit that
  /// occurred.
  /// 
  /// \param hit
  ///   Information about the hit that occurred.
  /// 
  /// \return
  ///   bool: The returned value is a hint to the physics system: If false is returned, this
  ///   indicates that the physics module can stop looking for further hits. If true is returned,
  ///   the physics system has to continue looking for further hits.
  virtual bool onHit(VisPhysicsHit_t &hit) = 0;

  /// \brief
  ///   This function is called when a raycast operation finishes. For asynchronous operation, it
  ///   is crucial to set the bFinished to true.
  virtual void onFinished() { bFinished = true; }

  /// \brief
  ///   This function should be implemented to indicate if this struct will contain all the hits (true)
  ///   or only the closest (false).
  virtual bool allHits() = 0;

  hkvVec3 vRayStart;                   ///< Start of the ray
  hkvVec3 vRayEnd;                     ///< End of the ray
  unsigned int iCollisionBitmask;           ///< Collision bitmask
  bool bFinished;                           ///< Member indicating whether the raycast operation finished.
  RAYCAST_ACCURACY eAccuracy;               ///< Member indicating accuracy for raycast(BoundingBox / AllShape). Not necessarily supported by all physics systems.
  
};


/// \brief
///   Raycast class for raycast operations determining the closest point of impact. See base class
///   (VisPhysicsRaycastBase_cl) for details.
class VisPhysicsRaycastClosestResult_cl : public VisPhysicsRaycastBase_cl
{
public:
  VisPhysicsRaycastClosestResult_cl() : VisPhysicsRaycastBase_cl()
  {
    bHit = false;
  }

  virtual void Reset()
  {
    VisPhysicsRaycastBase_cl::Reset();
    bHit = false;
    closestHit.Reset(); // reset the fHitFraction for the next hit
  }

  virtual bool onHit(VisPhysicsHit_t &hit)
  {
    if (hit.fHitFraction < closestHit.fHitFraction)
    {
      closestHit = hit;
      bHit = true;
    }
    return true;
  }

  virtual bool allHits(){ return false; }

  bool bHit;
  VisPhysicsHit_t closestHit;
};


/// \brief
///   Raycast class for raycast operations determining whether point A can "see" point B (line of
///   sight checks). Simplest (and fastest) form of raycasts. See base class
///   (VisPhysicsRaycastBase_cl) for details.
class VisPhysicsRaycastFirstResult_cl : public VisPhysicsRaycastBase_cl
{
public:
  VisPhysicsRaycastFirstResult_cl() : VisPhysicsRaycastBase_cl()
  {
    bHit = false;
  }

  virtual void Reset()
  {
    VisPhysicsRaycastBase_cl::Reset();
    this->hit.Reset();
    bHit = false;
  }

  virtual bool onHit(VisPhysicsHit_t &h)
  {
    bHit = true;
    this->hit = h;
    return false;
  }

  virtual bool allHits(){ return false; }

  VisPhysicsHit_t hit;
  bool bHit;
};


/// \brief
///   Raycast class for raycast operations returning all hits (up to iMaxResults). See base class
///   (VisPhysicsRaycastBase_cl) for details.
class VisPhysicsRaycastAllResults_cl : public VisPhysicsRaycastBase_cl
{
public:
  VisPhysicsRaycastAllResults_cl(int iMaxResults, VisPhysicsHit_t *pResultsOut) : VisPhysicsRaycastBase_cl()
  {
    pPhysicsHits = pResultsOut;
    iMaxResultCount = iMaxResults;
    iNumResults = 0;
  }

  virtual void Reset()
  {
    VisPhysicsRaycastBase_cl::Reset();
    iNumResults = 0;
  }

  virtual bool onHit(VisPhysicsHit_t &hit)
  {
    if (iNumResults < iMaxResultCount)
    {
      pPhysicsHits[iNumResults++] = hit;
      if (iNumResults < iMaxResultCount)
        return true;
    }
    return false;
  }

  virtual bool allHits(){ return true; }

  VisPhysicsHit_t *pPhysicsHits;
  int iMaxResultCount;
  int iNumResults;
};


/// \brief
///   Simple pure virtual interface to provide access to an array of decoration objects. Used for the physics interface (see OnDecorationCreated)
class IVisDecorationGroup_cl
{
public:
  IVisDecorationGroup_cl()
  {
    m_pDecorationPhysicsUserData = NULL;
  }

  virtual ~IVisDecorationGroup_cl()
  {
  }


  /// \brief
  ///   Pure virtual function to return the number of objects in this group (each of type VTerrainDecorationInstance)
  virtual int GetDecorationObjectCount() const = 0;

  /// \brief
  ///   Pure virtual function to return the decoration instance with specified index
  virtual VTerrainDecorationInstance* GetDecorationObject(int iIndex) const = 0;

  /// \brief
  ///   Stores a user data object in this group. Physics module implementations can use it store a custom object that holds physics shapes for each of the object in the group
  inline void SetDecorationPhysicsUserData(void *pUserData)
  {
    m_pDecorationPhysicsUserData = pUserData;
  }

  /// \brief
  ///   Returns the user data object that associates physics with objects in this group
  inline void* GetDecorationPhysicsUserData() const
  {
    return m_pDecorationPhysicsUserData;
  }

protected:
  void *m_pDecorationPhysicsUserData;
};


/// \brief
///   Interface for physics modules
/// 
/// \note
///   Several parts of the Vision Physics API are obsolete without explicitly being marked as such
///   and should not be used directly. When in doubt, use the functionality provided by the
///   vHavokPhysicsPlugin directly instead of relying on the generic interface.
/// 
/// This is the abstract interface for physics modules. Each physics module needs to implement the
/// virtual functions declared in this interface.
/// 
/// The IVisPhysicsModule_cl interface provides a convenient way to handle the physics simulation
/// and the creation of physics objects. It is strongly recommended to implement custom physics
/// bindings as physics modules. 
class IVisPhysicsModule_cl : public VRefCounter
{
public:

  /// \brief
  ///   Enumeration of available physics module types
  enum eMODULETYPE
  {
    UNKNOWN = 0,
    VISION = 1,
    HAVOK = 2,
    PHYSX = 3,
    CUSTOM = 10
  };


  VISION_APIFUNC virtual ~IVisPhysicsModule_cl()
  {
  }



  ///
  /// @name Main Functions
  /// @{
  ///


  /// \brief
  ///   Override this function to initialize the physics module
  /// 
  /// The IVisApp_cl::SetPhysicsModule function takes care of calling this function when a new
  /// physics module gets installed.
  /// 
  /// Note that this function is just called once when the physics module gets installed.
  /// 
  /// \return
  ///   BOOL: TRUE if the initialization was successful
  /// 
  /// \note
  ///   Refer to one of the existing physics modules (e.g. Havok) as
  ///   an example for implementing a custom physics module for Vision
  /// 
  /// \sa IVisPhysicsModule_cl::OnDeInitPhysics
  virtual BOOL OnInitPhysics() = 0;


  /// \brief
  ///   Override this function to deinitialize the physics module
  /// 
  /// The IVisApp_cl::SetPhysicsModule function takes care of calling this function when the
  /// physics module is set to NULL. This happens latest in the destructor of the IVisApp_cl class.
  /// 
  /// \return
  ///   BOOL: TRUE if the deinitialisation was sucessful
  /// 
  /// \note
  ///   Refer to one of the existing physics modules (e.g. Havok) as
  ///   an example for implementing a custom physics module for Vision
  /// 
  /// \sa IVisPhysicsModule_cl::OnInitPhysics
  virtual BOOL OnDeInitPhysics() = 0;


  /// \brief
  ///   Override this function to get notified when a new world has been loaded (DEPRECATED)
  /// 
  /// You can query the collision meshes of the world primitives within this function (via the
  /// VisPhysics_cl class). 
  /// 
  /// The VisionApp_cl class takes care of calling this function. 
  virtual void OnNewWorldLoaded() = 0;

  /// \brief
  ///   Override this function to run your physics simulation once per frame
  /// 
  /// The passed time since the last simulation is passed as a parameter.
  /// 
  /// The OnRunPhysics function is called by VisionApp_cl::OnUpdateScene function once per tick.
  /// 
  /// Please note the following when using OnRunPhysics within vForge: the OnRunPhysics function
  /// will   only be executed if the play-the-game mode is active (see VisEditorManager_cl).
  /// 
  /// Note that, for physics modules supporting asynchronous execution, the exact point where this
  /// function is called may differ. Refer to the VisionSceneManager_cl class for details.
  /// 
  /// \param fDuration
  ///   passed time since the last simulation in seconds.
  /// 
  /// \note
  ///   Refer to one of the existing physics modules (e.g. Havok) as
  ///   an example for implementing a custom physics module for Vision
  /// 
  /// \sa IVisPhysicsModule_cl::FetchPhysicsResults
  /// \sa IVisPhysicsModule_cl::SetUseAsynchronousPhysics
  /// \sa IVisPhysicsModule_cl::GetUseAsynchronousPhysics
  virtual void OnRunPhysics( float fDuration ) = 0;

  /// \brief
  ///   Fetches new physics results after a call to OnRunPhysics.
  /// 
  /// Physics modules which allow asynchronous execution (typically for improving performance
  /// through multithreading) rely on some time passing between OnRunPhysics (which kicks off the
  /// physics simulation) and FetchPhysicsResults (which typically picks up the simulation results
  /// and feeds them back to the Vision Engine).
  /// 
  /// Note that, for physics modules supporting asynchronous execution, the exact point where this
  /// function is called may differ. Refer to the VisionSceneManager_cl class for details.
  /// 
  /// \note
  ///   Refer to one of the existing physics modules (e.g. Havok) as
  ///   an example for implementing a custom physics module for Vision
  /// 
  /// \sa IVisPhysicsModule_cl::OnRunPhysics
  /// \sa IVisPhysicsModule_cl::SetUseAsynchronousPhysics
  /// \sa IVisPhysicsModule_cl::GetUseAsynchronousPhysics
  virtual void FetchPhysicsResults() = 0;

  /// \brief
  ///   Returns the type of the physics module (abstract)
  /// 
  /// Custom physics modules have to return IVisPhysicsModule_cl::CUSTOM.
  /// 
  /// \return
  ///   virtual eMODULETYPE: type of the physics module
  /// 
  /// \sa IVisPhysicsModule_cl::Init
  virtual eMODULETYPE GetType() = 0;
  

  ///
  /// @}
  ///

  ///
  /// @name Factory Functions
  /// @{
  ///

  /// \brief
  ///   Override this function to create a new physics object. 
  /// 
  /// CreatePhysicsObject creates and initializes a new physics object and returns the interface
  /// pointer. It is called by the VisBaseEntity_cl::CreatePhysicsObject function. 
  /// 
  /// Use the VisObject3D_cl::GetType function to ensure that the passed object is a
  /// VisBaseEntity_cl instance. You can then cast the object to a VisBaseEntity_cl instance and
  /// query the collision mesh via the VisBaseEntity_cl::GetCollisionMesh() function. The
  /// bounding box of the object can be retrieved with the VisBaseEntity_cl::GetCollisionBoundingBox().
  /// function.
  /// 
  /// \param pObject3D
  ///   pointer to the VisObject3D_cl instance the physics object belongs to
  /// 
  /// \param bStatic
  ///   Indicates whether the object is static and thus never moves (i.e. passive collider).
  /// 
  /// \return
  ///   virtual IVisPhysicsObject_cl *: pointer to the new physics object.
  /// 
  /// \sa IVisPhysicsModule_cl::DeletePhysicsObject
  virtual IVisPhysicsObject_cl *CreatePhysicsObject( VisObject3D_cl *pObject3D, bool bStatic=false ) = 0;  

  /// \brief
  ///   Override this function to update the module when a physics object is deleted
  ///
  /// This function is called by the base entity class when the user deletes a physics object
  /// via the VisBaseEntity_cl::DeletePhysicsObject function.
  ///
  /// \param pObject3D
  ///   Owner of the deleted physics object 
  virtual void OnPhysicsObjectDeleted(VisObject3D_cl *pObject3D) {}

  /// \brief
  ///   Returns whether the physics module uses asynchronous physics.
  /// 
  /// Used e.g. by the Vision engine to determine when to schedule the calls to OnRunPhysics and
  /// FetchPhyicsResults.
  /// 
  /// \return
  ///   bool if asynchronous physics are used.
  virtual bool GetUseAsynchronousPhysics() = 0;

  /// \brief
  ///   Used to specify whether the physics module uses asynchronous physics.
  /// 
  /// Since asynchronous physics can take advantage of multithreading on consoles and multicore
  /// PCs, it typically leads to significant performance advantages.
  /// 
  /// \param bAsyncPhysics
  ///   true to use asynchronous physics.
  virtual void SetUseAsynchronousPhysics(bool bAsyncPhysics) {}

  /// \brief
  ///   Method called by the Vision engine whenever a new static mesh instance has been created.
  virtual void OnStaticMeshInstanceCreated(VisStaticMeshInstance_cl *pMeshInstance) {}

  /// \brief
  ///   Method called by the Vision engine whenever a static mesh instance has been removed.
  virtual void OnStaticMeshInstanceRemoved(VisStaticMeshInstance_cl *pMeshInstance) {}

  /// \brief
  ///   Method called by the Vision engine whenever a static mesh instance has moved. Typically
  ///   only used by vForge.
  virtual void OnStaticMeshInstanceMoved(VisStaticMeshInstance_cl *pMeshInstance) {}

  /// \brief
  ///   Method called by the Vision engine whenever the collision bitmask of a static mesh instance
  ///   has changed. Typically only used by vForge.
  virtual void OnStaticMeshInstanceCollisionBitmaskChanged(VisStaticMeshInstance_cl *pMeshInstance) {}

#ifdef SUPPORTS_TERRAIN

  /// \brief
  ///   Method called by the Vision engine when a new terrain sector has been created.
  virtual void OnTerrainSectorCreated(VTerrainSector *pTerrainSector) {}

  /// \brief
  ///   Method called by the Vision engine when a terrain sector has been removed.
  virtual void OnTerrainSectorRemoved(VTerrainSector *pTerrainSector) {}

  /// \brief
  ///   Method called by the Vision engine when a terrain sector has been saved.
  virtual void OnTerrainSectorSaved(VTerrainSector *pTerrainSector) {}
  
  /// \brief
  ///	Method called by the Vision engine when terrain decorations have been created
  ///
  /// The engine currently supported decoration collisions for painted SpeedTree instances
  /// and for SpeedTree groups. The decoration objects are passed in groups, so that the
  /// physics module can bundle them efficiently.
  ///
  /// \param group
  ///	Group with multiple decoration objects that shall be included in the physics simulation
  virtual void OnDecorationCreated(IVisDecorationGroup_cl &group) {}

  /// \brief
  ///	Method called by the Vision engine when terrain decorations have been removed
  virtual void OnDecorationRemoved(IVisDecorationGroup_cl &group) {}

#endif

  /// \brief
  ///	Overridable called by the Vision engine when gathering additional resource/file dependencies for streaming for a instance.
  ///
  /// \param snapshot
  ///	  The snapshot that collects dependencies
  /// 
  /// \param pInstance
  ///	  The instance that queries additional files. The implementation is responsible to check the instance's type (e.g. static mesh) 
  ///   and then evaluate the additional files that are needed (for instance pre-cooked mesh files)
  virtual void GetDependenciesForObject(VResourceSnapshot &snapshot, VTypedObject *pInstance)
  {
  }

  /// \brief
  ///   Overridable called by the Vision engine when gathering additional resource/file dependencies for streaming for a resource.
  ///
  /// \param snapshot
  ///	  The snapshot that collects dependencies
  /// 
  /// \param pResource
  ///	  The managed resource that queries additional files. The implementation is responsible to check the resource's type 
  ///   and then evaluate the additional files that are needed.
  virtual void GetDependenciesForResource(VResourceSnapshot &snapshot, VManagedResource *pResource)
  {
  }

  /// \brief
  ///   Convenience function for performing line of sight tests
  /// 
  /// For best performance, it is recommended to instead use the batched raycast functionality
  /// (EnqueueRaycast) with asynchronous physics whenever the results are not immediately required.
  /// 
  /// \param vRayStart
  ///   Start of the ray
  /// 
  /// \param vRayEnd
  ///   End of the ray
  /// 
  /// \param iCollisionMask
  ///   Collision bitmask.
  /// 
  /// \return
  ///   bool: true if something intersected the ray.
  inline bool Raycast(const hkvVec3& vRayStart, const hkvVec3& vRayEnd, unsigned int iCollisionMask = 0xFFFFFFFF)
  {
    VisPhysicsRaycastFirstResult_cl rc;
    rc.vRayStart = vRayStart;
    rc.vRayEnd = vRayEnd;
    rc.iCollisionBitmask = iCollisionMask;
    PerformRaycast(&rc);
    return rc.bHit;
  }

  /// \brief
  ///   Covenience function for performing a raycast and determining the closest intersection
  ///   point.
  /// 
  /// For best performance, it is recommended to instead use the batched raycast functionality
  /// (EnqueueRaycast) with asynchronous physics whenever the results are not immediately required.
  /// 
  /// \param vRayStart
  ///   Start of the ray
  /// 
  /// \param vRayEnd
  ///   End of the ray
  /// 
  /// \param closestImpact
  ///   VisPhysicsHit_t structure describing the closest hit.
  /// 
  /// \param iCollisionMask
  ///   Collision bitmask. Currently not supported.
  /// 
  /// \return
  ///   bool: true if something intersected the ray.
  inline bool Raycast(const hkvVec3& vRayStart, const hkvVec3& vRayEnd, VisPhysicsHit_t &closestImpact, unsigned int iCollisionMask = 0xFFFFFFFF)
  {
    VisPhysicsRaycastClosestResult_cl rc;
    rc.vRayStart = vRayStart;
    rc.vRayEnd = vRayEnd;
    rc.iCollisionBitmask = iCollisionMask;
    PerformRaycast(&rc);
    closestImpact = rc.closestHit;
    return rc.bHit;
  }

  /// \brief
  ///   Covenience function for performing a raycast and determining all intersection points.
  /// 
  /// For best performance, it is recommended to instead use the batched raycast functionality
  /// (EnqueueRaycast) with asynchronous physics whenever the results are not immediately required.
  /// 
  /// \param vRayStart
  ///   Start of the ray
  /// 
  /// \param vRayEnd
  ///   End of the ray
  /// 
  /// \param iMaxHits
  ///   Maximum number of hits to be stored.
  /// 
  /// \param pImpactListOut
  ///   Array of VisPhysicsHit_t structures in which to store the hits.
  /// 
  /// \param iCollisionMask
  ///   Collision bitmask. Currently not supported.
  /// 
  /// \return
  ///   int: Number of hits found (<= iMaxHits).
  inline int Raycast(const hkvVec3& vRayStart, const hkvVec3& vRayEnd, int iMaxHits, VisPhysicsHit_t *pImpactListOut, unsigned int iCollisionMask = 0xFFFFFFFF)
  {
    VisPhysicsRaycastAllResults_cl rc(iMaxHits, pImpactListOut);
    rc.vRayStart = vRayStart;
    rc.vRayEnd = vRayEnd;
    rc.iCollisionBitmask = iCollisionMask;
    PerformRaycast(&rc);
    return rc.iNumResults;
  }

  /// \brief
  ///   Immediately performs a raycast operation.
  /// 
  /// The exact behavior is determined by the VisPhysicsRaycastBase_cl implementation passed to
  /// this method. See VisPhysicsRaycastAllResults_cl, VisPhysicsRaycastClosestResult_cl, and
  /// VisPhysicsRaycastFirstResult_cl for details.
  /// 
  /// For best performance, it is recommended to instead use the batched raycast functionality
  /// (EnqueueRaycast) with asynchronous physics.
  /// 
  /// \param pRaycastData
  ///   Raycast implementation defining the raycast behavior. This object is also used for storing
  ///   the results.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC virtual void PerformRaycast(VisPhysicsRaycastBase_cl *pRaycastData) = 0;

  /// \brief
  ///   Enqueues a raycast operation for asynchronous execution.
  /// 
  /// The result is available as soon as the bFinished member of the passed pRaycastData object is
  /// set to true.
  /// 
  /// The exact behavior is determined by the VisPhysicsRaycastBase_cl implementation passed to
  /// this method. See VisPhysicsRaycastAllResults_cl, VisPhysicsRaycastClosestResult_cl, and
  /// VisPhysicsRaycastFirstResult_cl for details.
  /// 
  /// \param pRaycastData
  ///   Raycast implementation defining the raycast behavior. This object is also used for storing
  ///   the results.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC virtual void EnqueueRaycast(VisPhysicsRaycastBase_cl *pRaycastData) = 0;

  ///
  /// @}
  ///

};


typedef VSmartPtr<IVisPhysicsModule_cl> IVisPhysicsModulePtr;

#endif   // FR_DEFINE_VISAPIPHYSICSMODULE

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
