/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef hkbPhysicsBaseTypes_H
#define hkbPhysicsBaseTypes_H

#include <Common/Base/hkBase.h>

// Typedefs used for indirectly accessing physics-owned objects.
// For now, these are hkReferencedObjects to allow Behavior to add references.
// "Handles" are only okay to use during a step of Behavior. They are not guaranteed to persist outside the Behavior frame.
// "HandleRefs" should be used to ensure a handle persists beyond the Behavior frame. It is not a guarantee, however,
//		that the pointed to object is still valid. It may, therefore, result in a no-op.
typedef hkReferencedObject* hkbPhysicsObjectHandle;
typedef hkbPhysicsObjectHandle hkbRigidBodyHandle;
typedef hkbPhysicsObjectHandle hkbConstraintHandle;
typedef hkbPhysicsObjectHandle hkbCharacterControllerCinfoHandle;
#define hkbPhysicsObjectHandleBase hkReferencedObject
#define hkbRigidBodyHandleBase hkbPhysicsObjectHandleBase
#define hkbConstraintHandleBase hkbPhysicsObjectHandleBase
#define hkbCharacterControllerCinfoHandleBase hkbPhysicsObjectHandleBase
typedef hkRefPtr<hkReferencedObject> hkbPhysicsObjectHandleRef;
typedef hkbPhysicsObjectHandleRef hkbRigidBodyHandleRef;
typedef hkbPhysicsObjectHandleRef hkbConstraintHandleRef;
typedef hkbPhysicsObjectHandleRef hkbCharacterControllerCinfoHandleRef;
#define HKB_INVALID_PHYSICS_HANDLE	HK_NULL

// These defines are just a convenience for deprecated function definitions.
// They may go away in the future.
#define hkbDeprecatedPhysicsWorldPtr class hkpWorld*
#define hkbDeprecatedRagdollPtr class hkaRagdollInstance*
#define hkbDeprecatedConstraintDataPtr class hkpConstraintData*

// Forward declarations of unspecialized physics-related classes.
class hkbSpatialQueryInterface;
class hkbPhysicsInterface;
class hkbRagdollInterface;
class hkbRagdollDriver;
class hkbRagdollController;
struct hkbPoweredRagdollControlData;
struct hkbRigidBodyRagdollControlData;
class hkbCharacterControllerDriver;
class hkbCharacterController;
class hkbFootIkDriver;
class hkbHandIkDriver;

// Forward declarations for contextual information used by physics.
class hkLocalFrame;
class hkbContext;

//////////////////////////////////////////////////////////////////////////
// Behavior supported basic physics setups
//////////////////////////////////////////////////////////////////////////

	/// A struct describing a constraint that must be supported by the underlying physics interface.
	/// This may contain data serialized out from Havok tools.
struct hkbConstraintSetup
{
		/// Type of constraint.
	enum Type {
			/// Typically all positional DOF are constrained, all rotational DOF are unconstrained.
		BALL_AND_SOCKET = 0,
			/// Typically all positional DOF are constrained, and rotational DOF are limited in some fashion.
		RAGDOLL,
	};

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbConstraintSetup );
	HK_DECLARE_REFLECTION();
	HK_DECLARE_POD_TYPE();

		/// Type of the constraint.
	hkEnum<Type, hkInt8> m_type;

	hkbConstraintSetup( Type type ) : m_type(type) {}
};

	/// A struct with runtime data for setting up a constraint instance.
struct hkbConstraintInstanceSetup
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbConstraintSetup );
	HK_DECLARE_POD_TYPE();

		/// Rigid body A to be constrained.
	hkbRigidBodyHandle m_rigidBodyA;

		/// Rigid body B to be constrained.
	hkbRigidBodyHandle m_rigidBodyB;

		/// The local frame for the constraint (this can be null if there is no local frame specified).
	const hkLocalFrame* m_frame;
};

	/// A struct describing a ragdoll that must be supported by the underlying physics interface.
	/// This may contain data serialized out from Havok tools.
struct hkbRagdollControllerSetup
{
		/// Type of ragdoll.
	enum Type {
			/// This is a ragdoll who's driven by powered constraints (Eg. motors).
		POWERED = 1,
			/// This is a ragdoll who's driven by keyframing the rigid bodies.
		RIGID_BODY = 2,
	};

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbRagdollControllerSetup );
	HK_DECLARE_REFLECTION();
	HK_DECLARE_POD_TYPE();

		/// Type of the ragdoll.
	hkEnum<Type,hkInt8> m_type;

	hkbRagdollControllerSetup( Type type ) : m_type(type) {}
};

	/// A struct with runtime data for setting up a ragdoll controller instance.
struct hkbRagdollControllerInstanceSetup
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbConstraintSetup );
	HK_DECLARE_POD_TYPE();

		/// Context in which the controller is being created.
		/// Note: this may be null in some circumstances, it is provided only as extra information when available.
	const hkbContext* m_context;

		/// Ragdoll instance to drive
	hkbRagdollInterface* m_ragdollInterface;
};

	/// A struct with runtime data for driving a ragdoll controller instance.
struct hkbRagdollControllerData
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbRagdollControllerData );
	HK_DECLARE_POD_TYPE();

	union {
			/// Data describing how to drive a POWERED ragdoll.
		const hkbPoweredRagdollControlData* m_poweredControlData;
			/// Data describing how to drive a RIGID_BODY ragdoll.
		const hkbRigidBodyRagdollControlData* m_rigidBodyControlData;
	};

		/// The number of datas provided.
	hkInt8 m_numControlDatas;

		/// A map between bone indices and data indices.
	const hkInt32* m_boneIndexToDataIndex;

		/// The bone weights.
	const hkReal* m_boneWeights;

		/// The local pose that should be driven to.
	const hkQsTransform* m_poseLocalSpace;

		/// The WFM of the pose.
	const hkQsTransform* m_worldFromModel;
};

	/// A struct describing a shape that must be supported by the underlying physics interface.
	/// This may contain data serialized out from Havok tools.
struct hkbShapeSetup
{
		/// Type of shape.
	enum Type {
			/// A capsule shape (this is currently the only supported shape).
		CAPSULE = 0, // CAPSULE == 0 is hard-coded into versioning code
			/// A shape loaded from a file (currently not used).
		FILE,
	};

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbShapeSetup );
	HK_DECLARE_REFLECTION();
	HK_DECLARE_POD_TYPE();

		/// The height of a CAPSULE shape.
	hkReal m_capsuleHeight;	//+default(1.7f)
							//+hk.RangeReal(absmin=0.1,absmax=10.0)
							//+hk.Description("The height of the capsule shape associated with the character controller.")

		/// The radius of a CAPSULE shape.
	hkReal m_capsuleRadius; //+default(0.4f)
							//+hk.RangeReal(absmin=0.1,absmax=10.0)
							//+hk.Description("The radius of the capsule shape associated with the character controller.")

		/// The filename to load the shape from (currently not used).
	hkStringPtr m_fileName;

		/// Type of the shape.
	hkEnum<Type, hkInt8> m_type;

	hkbShapeSetup() : m_capsuleHeight(1.7f), m_capsuleRadius(0.4f), m_type(CAPSULE) {}
	hkbShapeSetup( hkFinishLoadedObjectFlag flag ) : m_fileName( flag ) {}
};

	/// A struct describing a rigid body that must be supported by the underlying physics interface.
	/// This may contain data serialized out from Havok tools.
struct hkbRigidBodySetup
{
		/// Type of rigid body.
		
	enum Type {
			/// An invalid rigid body, used for invalid motion type in the sdk and for the hkbCharacterControllerSetup
		INVALID = -1,
			/// A keyframed rigid body. This body has infinite mass and is integrated by the physics system.
		KEYFRAMED,
			/// A dynamic rigid body. This body has non-infinite mass and is integrated by the physics system.
		DYNAMIC,
			/// A fixed rigid body. This body has infinite mass and is not integrated by the physics system.
		FIXED,
	};

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbRigidBodySetup );
	HK_DECLARE_REFLECTION();
	HK_DECLARE_POD_TYPE();

		/// The collision filter info associated with the character controller.
	hkUint32 m_collisionFilterInfo;	//+default(1)
									//+hk.Description("The collision filter info associated with the character controller.")

		/// Type of the rigid body.
	hkEnum<Type, hkInt8> m_type;

		/// Setup information for the shape.
	struct hkbShapeSetup m_shapeSetup;

	hkbRigidBodySetup() : m_collisionFilterInfo(1), m_type(INVALID) {}
	hkbRigidBodySetup( hkFinishLoadedObjectFlag flag ) : m_shapeSetup( flag ) {}
};

	/// A struct describing a character controller that must be supported by the underlying physics interface.
	/// This may contain data serialized out from Havok tools.
struct hkbCharacterControllerSetup
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbCharacterControllerSetup );
	HK_DECLARE_REFLECTION();
	HK_DECLARE_POD_TYPE();

		/// Setup information about the rigid body used by the character controller.
	struct hkbRigidBodySetup m_rigidBodySetup;

		/// A handle to the implementation-specific controller cinfo exposed to the tool through HavokAssembly.
	hkbCharacterControllerCinfoHandleRef m_controllerCinfo;

	hkbCharacterControllerSetup() : m_controllerCinfo(HKB_INVALID_PHYSICS_HANDLE) {}
	hkbCharacterControllerSetup( hkFinishLoadedObjectFlag flag ) : m_rigidBodySetup( flag ), m_controllerCinfo( flag ) {}
};

	/// A struct with runtime data for setting up a character controller instance.
struct hkbCharacterControllerInstanceSetup
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbConstraintSetup );
	HK_DECLARE_POD_TYPE();

		/// Context in which the controller is being created.
		/// Note: this may be null in some circumstances, it is provided only as extra information when available.
	const hkbContext* m_context;

		/// Initial transform for the controller.
	const hkQsTransform& m_initialTransform;

		/// Behavior world up.
	const hkVector4& m_worldUp;

	hkbCharacterControllerInstanceSetup( const hkQsTransform& initialTransform, hkVector4Parameter worldUp ) : m_initialTransform(initialTransform), m_worldUp(worldUp) {}
};

#endif // hkbPhysicsBaseTypes_H

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
