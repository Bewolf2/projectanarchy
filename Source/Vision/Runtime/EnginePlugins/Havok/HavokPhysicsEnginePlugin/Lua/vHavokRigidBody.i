
#ifndef VLUA_APIDOC

%nodefaultctor vHavokRigidBody;
%nodefaultdtor vHavokRigidBody;

class vHavokRigidBody : public IVObjectComponent
{
public:

  %rename(Remove) DisposeObject();
  void DisposeObject();

  %extend{
    VSWIG_CONVERT_BOOL_SETTER(SetDebugRendering);
	
    bool InitBox(hkvVec3* pBoxSize, hkvVec3* pScale = NULL, int iMotionType = MotionType_DYNAMIC, int iQualityType = HK_COLLIDABLE_QUALITY_MOVING)
	  {
	    if(!pBoxSize) return false;
  	  
	    vHavokRigidBody::InitTemplate initTempl;
	    initTempl.m_motionType =  (hkpMotion::MotionType) iMotionType;
	    initTempl.m_qualityType = (hkpCollidableQualityType)iQualityType;
	    self->InitBoxRb(*pBoxSize, pScale?*pScale:hkvVec3(1,1,1), initTempl);
  	  
	    return true;
	  }
  	
	  bool InitSphere(float fRadius, float fScale = 1.f, int iMotionType = MotionType_DYNAMIC, int iQualityType = HK_COLLIDABLE_QUALITY_MOVING)
	  {	  
	    vHavokRigidBody::InitTemplate initTempl;
	    initTempl.m_motionType =  (hkpMotion::MotionType) iMotionType;
	    initTempl.m_qualityType = (hkpCollidableQualityType)iQualityType;
	    self->InitSphereRb(fRadius, fScale, initTempl);
  	  
	    return true;
	  }
  	
	  bool InitCapsule(hkvVec3* pVecA, hkvVec3* pVecB, float fRadius, float fScale = 1.f, int iMotionType = MotionType_DYNAMIC, int iQualityType = HK_COLLIDABLE_QUALITY_MOVING)
	  {
	    if(!pVecA||!pVecB||fRadius<0) return false;
  	  
	    vHavokRigidBody::InitTemplate initTempl;
	    initTempl.m_motionType =  (hkpMotion::MotionType) iMotionType;
	    initTempl.m_qualityType = (hkpCollidableQualityType)iQualityType;
	    self->InitCapsuleRb(*pVecA, *pVecB, fRadius, fScale, initTempl);
  	  
	    return true;
	  }
  	
	  bool InitCylinder(hkvVec3* pVecA, hkvVec3* pVecB, float fRadius, float fScale = 1.f, int iMotionType = MotionType_DYNAMIC, int iQualityType = HK_COLLIDABLE_QUALITY_MOVING)
	  {
	    if(!pVecA||!pVecB||fRadius<0) return false;
  	  
	    vHavokRigidBody::InitTemplate initTempl;
	    initTempl.m_motionType =  (hkpMotion::MotionType) iMotionType;
	    initTempl.m_qualityType = (hkpCollidableQualityType)iQualityType;
	    self->InitCylinderRb(*pVecA, *pVecB, fRadius, fScale, initTempl);
  	  
	    return true;
	  }
  	
	  bool InitConvex(VDynamicMesh *pMesh, hkvVec3* pScale = NULL, int iMotionType = MotionType_DYNAMIC, int iQualityType = HK_COLLIDABLE_QUALITY_MOVING)
	  {
	    if(!pMesh) return false;
  	  
	    vHavokRigidBody::InitTemplate initTempl;
	    initTempl.m_motionType =  (hkpMotion::MotionType) iMotionType;
	    initTempl.m_qualityType = (hkpCollidableQualityType)iQualityType;
	    self->InitConvexRb(pMesh, pScale?*pScale:hkvVec3(1,1,1), initTempl);
  	  
	    return true;
	  }
  	
	  bool InitMesh(VDynamicMesh *pMesh, hkvVec3* pScale = NULL, int iMotionType = MotionType_DYNAMIC, int iQualityType = HK_COLLIDABLE_QUALITY_MOVING)
	  {
	    if(!pMesh) return false;
  	  
	    vHavokRigidBody::InitTemplate initTempl;
	    initTempl.m_motionType =  (hkpMotion::MotionType) iMotionType;
	    initTempl.m_qualityType = (hkpCollidableQualityType)iQualityType;
	    self->InitMeshRb(pMesh, pScale?*pScale:hkvVec3(1,1,1), initTempl);
  	  
	    return true;
	  }
  	
	  bool InitFromFile(const char* filename, float fScale = 1.f)
	  {
	    if(!filename || *filename == '\0') return false;
  	  
	    return self->InitRbFromFile(filename, TRUE, fScale, NULL);
	  }
  	
  	void SetMotionType(int iType)
  	{
  	  self->SetMotionType((hkpMotion::MotionType)iType);
  	}

    hkReal PerformSweep(const hkvVec3& vDir, float fDistance)
    {
      vHavokSweepResult result;
      int iNumHits = ((vHavokPhysicsModule*)Vision::GetApplication()->GetPhysicsModule())->PerformSweep(&result, 1, self, vDir, fDistance);
      if(iNumHits > 0)
        return result.m_fDistance; // return distance of first hit 
      return hkReal(-1);
    }

    bool DropToFloor(float fDistance=10000.0f)
    {
      return ((vHavokPhysicsModule*)Vision::GetApplication()->GetPhysicsModule())->DropToFloor(self, fDistance);
    }

	  VSWIG_CREATE_CAST(vHavokRigidBody)
  }
  
  void SetPosition(const hkvVec3& value);
  void SetOrientation(const hkvVec3& value);

  void SetDebugColor(VColorRef color);

  void SetLinearDamping(float fDamping);
  float GetLinearDamping() const;
  
  void SetDamageMultiplier(float fMultiplier);
  float GetDamageMultiplier() const;

  void SetAngularDamping(float fDamping);
  float GetAngularDamping() const;

  void SetLinearVelocity(const hkvVec3& value);
  hkvVec3 GetLinearVelocity() const;

  void SetAngularVelocity(const hkvVec3& value);
  hkvVec3 GetAngularVelocity() const;

  void ApplyForce(hkvVec3& value, float deltaT);

  void ApplyLinearImpulse(hkvVec3& value);

  void SetMass(float fMass);

  void SetRestitution(float fRestitution);

  void SetFriction(float fFriction);

  void SetCollisionInfo(int iLayer, int iGroup, int iSubsystem, int iSubsystemDontCollideWith);
 
};

#else

/// \brief Physics module class (Havok): Object component wrapper class that provides a rigid body for an entity.
class vHavokRigidBody : public IVObjectComponent
{
public:
	
	/// @}
  /// @name Initialization & Deletion
  /// @{

  /// \brief Init a rigid body that uses a box as shape.
  /// \param boxSize Size of the box shape.
  /// \param scale (\b optional) Vector that defines the scaling of this rigid body.
  /// \param motionType (\b optional) Sets the motion type of this rigid body. Default is Physics.MOTIONTYPE_DYNAMIC.
  ///   The following values are possible (look at the Havok Physics documetation for more details!):
  ///    - MOTIONTYPE_DYNAMIC: A fully-simulated, movable rigid body.
  ///    - MOTIONTYPE_KEYFRAMED: Simulation is not performed as a normal rigid body, can be moved manually by code.
  ///    - MOTIONTYPE_FIXED: This motion type is used for the static elements of a game scene.
  ///    - MOTIONTYPE_SPHERE_INERTIA: Simulation is performed using a sphere inertia tensor.
  ///    - MOTIONTYPE_BOX_INERTIA: Simulation is performed using a box inertia tensor.
  ///    - MOTIONTYPE_THIN_BOX_INERTIA: A box inertia motion which is optimized for thin boxes and has less stability problems.
  ///    - MOTIONTYPE_CHARACTER: A specialized motion used for character controllers.
  /// \param qualityType (\b optional) The quality type for use continuous physics. Default is Physics.QUALITY_MOVING.
  ///   The following values are possible (look at the Havok Physics documetation for more details!):
  ///    - QUALITY_FIXED: Use this for fixed bodies.
  ///    - QUALITY_KEYFRAMED: Use this for moving objects with infinite mass.
  ///    - QUALITY_DEBRIS: Use this for all your debris objects.
  ///    - QUALITY_DEBRIS_SIMPLE_TOI: Use this for debris objects that should have simplified TOI collisions.
  ///    - QUALITY_MOVING: Use this for moving bodies, which 'should' not leave the world.
  ///    - QUALITY_CRITICAL: Use this for all objects, which you cannot afford to tunnel through the world at all.
  ///    - QUALITY_BULLET: Use this for very fast objects.
  ///    - QUALITY_CHARACTER: Use this for rigid body character controllers.
  ///    - QUALITY_KEYFRAMED_REPORTING: Use this for moving objects with infinite mass which should report contact points.   
  boolean InitBox(hkvVec3 boxSize, hkvVec3 scale = nil, number motionType = MOTIONTYPE_DYNAMIC, number qualityType = QUALITY_MOVING);

	
  /// \brief Init a rigid body that uses a sphere as shape.
  /// \param radius Radius of the sphere shape.
  /// \param scale (\b optional) Uniform scaling value of this rigid body.
  /// \param motionType (\b optional) Sets the motion type of this rigid body. Default is Physics.MOTIONTYPE_DYNAMIC.
  ///   The following values are possible (look at the Havok Physics documetation for more details!):
  ///    - MOTIONTYPE_DYNAMIC: A fully-simulated, movable rigid body.
  ///    - MOTIONTYPE_KEYFRAMED: Simulation is not performed as a normal rigid body, can be moved manually by code.
  ///    - MOTIONTYPE_FIXED: This motion type is used for the static elements of a game scene.
  ///    - MOTIONTYPE_SPHERE_INERTIA: Simulation is performed using a sphere inertia tensor.
  ///    - MOTIONTYPE_BOX_INERTIA: Simulation is performed using a box inertia tensor.
  ///    - MOTIONTYPE_THIN_BOX_INERTIA: A box inertia motion which is optimized for thin boxes and has less stability problems.
  ///    - MOTIONTYPE_CHARACTER: A specialized motion used for character controllers.
  /// \param qualityType (\b optional) The quality type for use continuous physics. Default is Physics.QUALITY_MOVING.
  ///   The following values are possible (look at the Havok Physics documetation for more details!):
  ///    - QUALITY_FIXED: Use this for fixed bodies.
  ///    - QUALITY_KEYFRAMED: Use this for moving objects with infinite mass.
  ///    - QUALITY_DEBRIS: Use this for all your debris objects.
  ///    - QUALITY_DEBRIS_SIMPLE_TOI: Use this for debris objects that should have simplified TOI collisions.
  ///    - QUALITY_MOVING: Use this for moving bodies, which 'should' not leave the world.
  ///    - QUALITY_CRITICAL: Use this for all objects, which you cannot afford to tunnel through the world at all.
  ///    - QUALITY_BULLET: Use this for very fast objects.
  ///    - QUALITY_CHARACTER: Use this for rigid body character controllers.
  ///    - QUALITY_KEYFRAMED_REPORTING: Use this for moving objects with infinite mass which should report contact points.   
  boolean InitSphere(number radius, number scale = 1, number motionType = MOTIONTYPE_DYNAMIC, number qualityType = QUALITY_MOVING);

  /// \brief Init a rigid body that uses a capsule as shape.
  /// \param pointA Position of the lower end of the capsule shape.
  /// \param pointB Position of the upper end of the capsule shape.
  /// \param radius Radius of the capsule shape.
  /// \param scale (\b optional) Uniform scaling value of this rigid body.
  /// \param motionType (\b optional) Sets the motion type of this rigid body. Default is Physics.MOTIONTYPE_DYNAMIC.
  ///   The following values are possible (look at the Havok Physics documetation for more details!):
  ///    - MOTIONTYPE_DYNAMIC: A fully-simulated, movable rigid body.
  ///    - MOTIONTYPE_KEYFRAMED: Simulation is not performed as a normal rigid body, can be moved manually by code.
  ///    - MOTIONTYPE_FIXED: This motion type is used for the static elements of a game scene.
  ///    - MOTIONTYPE_SPHERE_INERTIA: Simulation is performed using a sphere inertia tensor.
  ///    - MOTIONTYPE_BOX_INERTIA: Simulation is performed using a box inertia tensor.
  ///    - MOTIONTYPE_THIN_BOX_INERTIA: A box inertia motion which is optimized for thin boxes and has less stability problems.
  ///    - MOTIONTYPE_CHARACTER: A specialized motion used for character controllers.
  /// \param qualityType (\b optional) The quality type for use continuous physics. Default is Physics.QUALITY_MOVING.
  ///   The following values are possible (look at the Havok Physics documetation for more details!):
  ///    - QUALITY_FIXED: Use this for fixed bodies.
  ///    - QUALITY_KEYFRAMED: Use this for moving objects with infinite mass.
  ///    - QUALITY_DEBRIS: Use this for all your debris objects.
  ///    - QUALITY_DEBRIS_SIMPLE_TOI: Use this for debris objects that should have simplified TOI collisions.
  ///    - QUALITY_MOVING: Use this for moving bodies, which 'should' not leave the world.
  ///    - QUALITY_CRITICAL: Use this for all objects, which you cannot afford to tunnel through the world at all.
  ///    - QUALITY_BULLET: Use this for very fast objects.
  ///    - QUALITY_CHARACTER: Use this for rigid body character controllers.
  ///    - QUALITY_KEYFRAMED_REPORTING: Use this for moving objects with infinite mass which should report contact points.   
  boolean InitCapsule(hkvVec3 pointA, hkvVec3 pointB, number radius, number scale = 1, number motionType = MOTIONTYPE_DYNAMIC, number qualityType = QUALITY_MOVING);
	
	/// \brief Init a rigid body that uses a cylinder as shape.
  /// \param pointA Position of the lower end of the cylinder shape.
  /// \param pointB Position of the upper end of the cylinder shape.
  /// \param radius Radius of the cylinder shape.
  /// \param scale (\b optional) Uniform scaling value of this rigid body.
  /// \param motionType (\b optional) Sets the motion type of this rigid body. Default is Physics.MOTIONTYPE_DYNAMIC.
  ///   The following values are possible (look at the Havok Physics documetation for more details!):
  ///    - MOTIONTYPE_DYNAMIC: A fully-simulated, movable rigid body.
  ///    - MOTIONTYPE_KEYFRAMED: Simulation is not performed as a normal rigid body, can be moved manually by code.
  ///    - MOTIONTYPE_FIXED: This motion type is used for the static elements of a game scene.
  ///    - MOTIONTYPE_SPHERE_INERTIA: Simulation is performed using a sphere inertia tensor.
  ///    - MOTIONTYPE_BOX_INERTIA: Simulation is performed using a box inertia tensor.
  ///    - MOTIONTYPE_THIN_BOX_INERTIA: A box inertia motion which is optimized for thin boxes and has less stability problems.
  ///    - MOTIONTYPE_CHARACTER: A specialized motion used for character controllers.
  /// \param qualityType (\b optional) The quality type for use continuous physics. Default is Physics.QUALITY_MOVING.
  ///   The following values are possible (look at the Havok Physics documetation for more details!):
  ///    - QUALITY_FIXED: Use this for fixed bodies.
  ///    - QUALITY_KEYFRAMED: Use this for moving objects with infinite mass.
  ///    - QUALITY_DEBRIS: Use this for all your debris objects.
  ///    - QUALITY_DEBRIS_SIMPLE_TOI: Use this for debris objects that should have simplified TOI collisions.
  ///    - QUALITY_MOVING: Use this for moving bodies, which 'should' not leave the world.
  ///    - QUALITY_CRITICAL: Use this for all objects, which you cannot afford to tunnel through the world at all.
  ///    - QUALITY_BULLET: Use this for very fast objects.
  ///    - QUALITY_CHARACTER: Use this for rigid body character controllers.
  ///    - QUALITY_KEYFRAMED_REPORTING: Use this for moving objects with infinite mass which should report contact points.   
  boolean InitCylinder(hkvVec3 pointA, hkvVec3 pointB, number radius, number scale = 1, number motionType = MOTIONTYPE_DYNAMIC, number qualityType = QUALITY_MOVING);
	 
  /// \brief Init a rigid body that uses a convex hull as shape.
  /// \param mesh The mesh data as template for the convex hull.
  /// \param scale (\b optional) Vector that defines the scaling of this rigid body.
	/// \param motionType (\b optional) Sets the motion type of this rigid body. Default is Physics.MOTIONTYPE_DYNAMIC.
  ///   The following values are possible (look at the Havok Physics documetation for more details!):
  ///    - MOTIONTYPE_DYNAMIC: A fully-simulated, movable rigid body.
  ///    - MOTIONTYPE_KEYFRAMED: Simulation is not performed as a normal rigid body, can be moved manually by code.
  ///    - MOTIONTYPE_FIXED: This motion type is used for the static elements of a game scene.
  ///    - MOTIONTYPE_SPHERE_INERTIA: Simulation is performed using a sphere inertia tensor.
  ///    - MOTIONTYPE_BOX_INERTIA: Simulation is performed using a box inertia tensor.
  ///    - MOTIONTYPE_THIN_BOX_INERTIA: A box inertia motion which is optimized for thin boxes and has less stability problems.
  ///    - MOTIONTYPE_CHARACTER: A specialized motion used for character controllers.
  /// \param qualityType (\b optional) The quality type for use continuous physics. Default is Physics.QUALITY_MOVING.
  ///   The following values are possible (look at the Havok Physics documetation for more details!):
  ///    - QUALITY_FIXED: Use this for fixed bodies.
  ///    - QUALITY_KEYFRAMED: Use this for moving objects with infinite mass.
  ///    - QUALITY_DEBRIS: Use this for all your debris objects.
  ///    - QUALITY_DEBRIS_SIMPLE_TOI: Use this for debris objects that should have simplified TOI collisions.
  ///    - QUALITY_MOVING: Use this for moving bodies, which 'should' not leave the world.
  ///    - QUALITY_CRITICAL: Use this for all objects, which you cannot afford to tunnel through the world at all.
  ///    - QUALITY_BULLET: Use this for very fast objects.
  ///    - QUALITY_CHARACTER: Use this for rigid body character controllers.
  ///    - QUALITY_KEYFRAMED_REPORTING: Use this for moving objects with infinite mass which should report contact points.   
  boolean InitConvex(VDynamicMesh mesh, hkvVec3 scale = nil, number motionType = MOTIONTYPE_DYNAMIC, number qualityType = QUALITY_MOVING);
	
	/// \brief Init a rigid body that uses the model's mesh as shape.
  /// \param mesh The mesh data to be used.
  /// \param scale (\b optional) Vector that defines the scaling of this rigid body.
	/// \param motionType (\b optional) Sets the motion type of this rigid body. Default is Physics.MOTIONTYPE_DYNAMIC.
  ///   The following values are possible (look at the Havok Physics documetation for more details!):
  ///    - MOTIONTYPE_DYNAMIC: A fully-simulated, movable rigid body.
  ///    - MOTIONTYPE_KEYFRAMED: Simulation is not performed as a normal rigid body, can be moved manually by code.
  ///    - MOTIONTYPE_FIXED: This motion type is used for the static elements of a game scene.
  ///    - MOTIONTYPE_SPHERE_INERTIA: Simulation is performed using a sphere inertia tensor.
  ///    - MOTIONTYPE_BOX_INERTIA: Simulation is performed using a box inertia tensor.
  ///    - MOTIONTYPE_THIN_BOX_INERTIA: A box inertia motion which is optimized for thin boxes and has less stability problems.
  ///    - MOTIONTYPE_CHARACTER: A specialized motion used for character controllers.
  /// \param qualityType (\b optional) The quality type for use continuous physics. Default is Physics.QUALITY_MOVING.
  ///   The following values are possible (look at the Havok Physics documetation for more details!):
  ///    - QUALITY_FIXED: Use this for fixed bodies.
  ///    - QUALITY_KEYFRAMED: Use this for moving objects with infinite mass.
  ///    - QUALITY_DEBRIS: Use this for all your debris objects.
  ///    - QUALITY_DEBRIS_SIMPLE_TOI: Use this for debris objects that should have simplified TOI collisions.
  ///    - QUALITY_MOVING: Use this for moving bodies, which 'should' not leave the world.
  ///    - QUALITY_CRITICAL: Use this for all objects, which you cannot afford to tunnel through the world at all.
  ///    - QUALITY_BULLET: Use this for very fast objects.
  ///    - QUALITY_CHARACTER: Use this for rigid body character controllers.
  ///    - QUALITY_KEYFRAMED_REPORTING: Use this for moving objects with infinite mass which should report contact points.   
  boolean InitMesh(VDynamicMesh mesh, hkvVec3 scale = nil, number motionType = MOTIONTYPE_DYNAMIC, number qualityType = QUALITY_MOVING);
	
	/// \brief Init a rigid body that uses a shape defined in a Havok file.
  /// \param filename Filename of the Havok file.
  /// \param scale (\b optional) Uniform scaling factor.
	boolean InitFromFile(string filename, number scale = 1);

  /// \brief Removes the rigid body from the simulation without necessarily deleting this instance.
  void Remove();
  
  /// @}
  /// @name Velocity & Property Wrapper Functions
  /// @{
  
  /// \brief Set the position of this rigid body, in world space - don't use this for motions.
  /// \param position The new position vector.
  void SetPosition(hkvVec3 position);

  /// \brief Set the orientation of this rigid body.
  /// \param eulerAngles The new orientation specified by Euler angles.
  void SetOrientation(hkvVec3 eulerAngles);
    
  /// \brief Sets the motion type of this rigid body.
  /// \param type The new motion type, possible values are:
  ///    - MOTIONTYPE_DYNAMIC: A fully-simulated, movable rigid body.
  ///    - MOTIONTYPE_KEYFRAMED: Simulation is not performed as a normal rigid body, can be moved manually by code.
  ///    - MOTIONTYPE_FIXED: This motion type is used for the static elements of a game scene.
  ///    - MOTIONTYPE_SPHERE_INERTIA: Simulation is performed using a sphere inertia tensor.
  ///    - MOTIONTYPE_BOX_INERTIA: Simulation is performed using a box inertia tensor.
  ///    - MOTIONTYPE_THIN_BOX_INERTIA: A box inertia motion which is optimized for thin boxes and has less stability problems.
  ///    - MOTIONTYPE_CHARACTER: A specialized motion used for character controllers.
  ///   (look at the Havok Physics documetation for more details!)
  void SetMotionType(number type);
  
  /// \brief Sets the linear damping of this rigid body.
  /// \param damping The new linear damping.
  void SetLinearDamping(number damping);
  
  /// \brief Gets the linear damping of this rigid body.
  /// \return The linear damping.
  number GetLinearDamping() const;
  
  /// \brief Sets the damage multiplier of this rigid body.
  /// \param fMultiplier The new damage multiplier.
  void SetDamageMultiplier(float fMultiplier);
  
  /// \brief Gets the damage multiplier of this rigid body.
  /// \return The damage multiplier.
  float GetDamageMultiplier() const;

  /// \brief Sets the angular damping of this rigid body.
  /// \param damping The new angular damping.
  void SetAngularDamping(number damping);
  
  /// \brief Gets the angular damping of this rigid body
  /// \return The angular damping.
  number GetAngularDamping() const;

  /// \brief Set the movement velocity of this rigid body.
  /// \param velocity Linear velocity vector.
  void SetLinearVelocity(hkvVec3 velocity);
  
  /// \brief Get the movement velocity of this rigid body.
  /// \return Linear velocity vector.
  hkvVec3 GetLinearVelocity();

  /// \brief Set the rotation velocity of this rigid body.
  /// \param velocity Angular velocity vector.
  void SetAngularVelocity(hkvVec3 velocity);
  
  /// \brief Get the rotation velocity of this rigid body.
  /// \return Angular velocity vector.
  hkvVec3 GetAngularVelocity();

  /// \brief
  ///   Apply a force to this rigid body for a given time interval.
  ///
  /// \param value
  ///   Force vector.
  ///
  /// \param deltaT
  ///   The time interval over which the force is applied.
  ///
  /// To instantaneously accelerate a rigid body upon a single event (e.g. when struck by an explosion),
  /// it is recommended to use ApplyLinearImpulse. To apply a continuous force to a rigid body (e.g. a changing
  /// gravity which is evaluated every frame), it is recommended to use ApplyForce and pass the time interval
  /// between each application for the deltaT parameter.
  void ApplyForce(hkvVec3 force, float deltaT);

  /// \brief
  ///   Apply a linear impulse to this rigid body.
  ///
  /// \param impulse
  ///   Impulse vector.
  ///
  /// To instantaneously accelerate a rigid body upon a single event (e.g. when struck by an explosion),
  /// it is recommended to use ApplyLinearImpulse. To apply a continuous force to a rigid body (e.g. a changing
  /// gravity which is evaluated every frame), it is recommended to use ApplyForce and pass the time interval
  /// between each application for the deltaT parameter.
  void ApplyLinearImpulse(hkvVec3 impulse);

  /// \brief Set the mass of this rigid body.
  /// \param mass The mass specified in kilo.
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       self:AddComponentOfType("vHavokRigidBody", "RigidBody")
  ///       self.RigidBody:SetMass(80)
  ///     end
  ///   \endcode
  void SetMass(number mass);

  /// \brief Sets the restitution of this rigid body.
  /// \param restitution The new restitution; should be between 0.0 and 1.0.
  void SetRestitution(number restitution);

  /// \brief Sets the friction of this rigid body.
  /// \param friction The new friction; should be between 0.0 and 1.0.
  void SetFriction(number friction);

  /// \brief Sets the collision parameters of this rigid body.
  /// \param layer The collision layer as integer number.
  /// \param group The collision group.
  /// \param subsystem The collision subsystem.
  /// \param subsystemDontCollideWith The collision subsystem this body shouldn't collide with.
  /// \note
  ///   See the Havok documentation on rigid body collisions for more 
  ///   information about what values to specify for these parameters.
  void SetCollisionInfo(number layer, number group, number subsystem, number subsystemDontCollideWith);

  /// @}
  /// @name Debug Rendering
  /// @{
  
  /// \brief Enable / Disable debug rendering for this rigid body's shape.
  /// \param enable If true, the shape of this rigid body will be displayed in Vision.
  void SetDebugRendering(boolean enable);
  
  /// \brief Set the color of the debug rendering representation of this rigid body's shape.
  /// \param color New Color of the debug rendering representation.
  void SetDebugColor(VColorRef color);
  
  /// @}
  /// @name Sweep Testing
  /// @{

  /// \brief Performs a linear sweep through space with the rigid body. 
  /// \param dir Normalized motion vector for the sweep.
  /// \param distance  Distance along the dir vector.
  /// \return A positive number (= distance to collider), if the object would collide with another object within the specified distance, 
  ///         or a negative number, if no collision would occur.
  number PerformSweep(hkvVec3 dir, number distance);

  /// \brief Drops the specified rigid body to the floor by performing a linear sweep.
  /// \param distance Test for a floor in distance units below the rigid body.
  /// \return Returns true on success, otherwise false.
  boolean DropToFloor(number distance=10000);

  /// @}

};

#endif
