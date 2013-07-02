/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_REFERENCEFRAME_H
#define HKAI_REFERENCEFRAME_H

extern const hkClass hkaiReferenceFrameClass;

/// hkaiReferenceFrame maintains position and velocity data for a particular frame of reference, i.e. a moving
/// navigation mesh.  The velocity information is necessary in order to do local steering and other velocity-based
/// calculations in the proper reference frame.
///
class hkaiReferenceFrame
{
	//+version(0)
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, hkaiReferenceFrame);
	HK_DECLARE_REFLECTION();
	HK_DECLARE_POD_TYPE();

	inline hkaiReferenceFrame();
	inline hkaiReferenceFrame(hkFinishLoadedObjectFlag f);


	/// Get the local-to-world transform (read-only)
	HK_FORCE_INLINE const hkTransform& getTransform() const;

	/// Get the world-space linear velocity of the frame
	HK_FORCE_INLINE const hkVector4& getLinearVelocity() const;

	/// Get the world-space angular velocity of the frame
	HK_FORCE_INLINE const hkVector4& getAngularVelocity() const;


	/// Set the local-to-world transform
	HK_FORCE_INLINE void setTransform( const hkTransform& transform );

	/// Set the world-space linear velocity of the frame
	HK_FORCE_INLINE void setLinearVelocity( hkVector4Parameter linearVelocity );

	/// Set the world-space angular velocity of the frame
	HK_FORCE_INLINE void setAngularVelocity( hkVector4Parameter angularVelocity );


	/// Transform a positional vector from frame-relative space into world space.
	HK_FORCE_INLINE void transformLocalPosToWorldPos( hkVector4Parameter localPos, hkVector4& worldPos ) const;

	/// Transform a directional vector from frame-relative space into world space.
	HK_FORCE_INLINE void transformLocalDirToWorldDir( hkVector4Parameter localDir, hkVector4& worldDir ) const;

	/// Transform a linear velocity vector (from a frame-relative origin point) from frame-relative space into world space.
	/// The transformed velocity includes any velocity of the origin point induced by motion of the frame.
	void transformLocalPosVelToWorldVel( hkVector4Parameter localPos, hkVector4Parameter localVel, hkVector4& worldVel ) const;

	/// Transform an angular velocity (around a world-space up vector) from frame-relative space into world space.
	/// The transformed velocity includes the angular velocity induced by the motion of the frame.
	inline void transformLocalAngVelToWorldAngVel( hkSimdRealParameter localAngVel, hkVector4Parameter worldUp, hkSimdReal& worldAngVel ) const;

	/// Transform a set of (position,direction,velocity) vectors from frame-relative space into world space.
	/// The transformed velocity includes any velocity of the origin point induced by motion of the frame.
	/// \note Velocity vector here includes angular velocity (around a world-space up vector) in the w component.
	void transformLocalPosDirVelToWorldPosDirVel( hkVector4Parameter localPos, hkVector4Parameter localDir, hkVector4Parameter localVel, hkVector4Parameter worldUp, 
		hkVector4& worldPos, hkVector4& worldDir, hkVector4& worldVel ) const;


	/// Transform a positional vector from world space into frame-relative space.
	HK_FORCE_INLINE void transformWorldPosToLocalPos( hkVector4Parameter worldPos, hkVector4& localPos ) const;

	/// Transform a directional vector from world space into frame-relative space into world space.
	HK_FORCE_INLINE void transformWorldDirToLocalDir( hkVector4Parameter worldDir, hkVector4& localDir ) const;

	/// Transform a linear velocity vector (from a world-space origin point) from world space into frame-relative space.
	/// The transformed velocity removes any velocity of the origin point induced by motion of the frame.
	void transformWorldPosVelToLocalVel( hkVector4Parameter worldPos, hkVector4Parameter worldVel, hkVector4& localVel ) const;

	/// Transform an angular velocity (around a world-space up vector) from world space into frame-relative space.
	/// The transformed velocity removes any angular velocity induced motion of the frame.
	inline void transformWorldAngVelToLocalAngVel( hkSimdRealParameter worldAngVel, hkVector4Parameter worldUp, hkSimdReal& localAngVel ) const;

	/// Transform a set of (position,direction,velocity) vectors from world space into frame-relative space.
	/// The transformed velocity removes any velocity of the origin point induced by motion of the frame.
	/// \note Velocity vector here includes angular velocity (around a world-space up vector) in the w component.
	void transformWorldPosDirVelToLocalPosDirVel( hkVector4Parameter worldPos, hkVector4Parameter worldDir, hkVector4Parameter worldVel, hkVector4Parameter worldUp, 
		hkVector4& localPos, hkVector4& localDir, hkVector4& localVel, hkVector4& localUpOut) const;


		/// Test to see if the reference frame is is motion
	bool isMoving() const;

		/// Validate this reference frame's data
	void validate() const;

		/// Validate determinism of data. Does nothing unless determinism checks enabled.
	void checkDeterminism() const;

private:
	/// Local-to-world transform for the frame
	hkTransform		m_transform;

	/// World-space linear velocity for the frame
	hkVector4		m_linearVelocity;

	/// Angular velocity for the frame
	hkVector4		m_angularVelocity;
};

#include <Ai/Pathfinding/Common/hkaiReferenceFrame.inl>

#endif // HKAI_VOLUME_H

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
