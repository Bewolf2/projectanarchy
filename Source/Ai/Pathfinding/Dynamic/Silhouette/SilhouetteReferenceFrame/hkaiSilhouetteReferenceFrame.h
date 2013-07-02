/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_SILLOUETTE_REFERENCE_FRAME_H
#define HK_SILLOUETTE_REFERENCE_FRAME_H

#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>

/// Reference frame for silhouette projection.
/// The 'up' axis is the axis along (down) which the 3D objects will be projected.
/// The 'reference' axis is an axis orthogonal to the 'up axis used to fully define the plane of projection.
/// 'Up' and 'reference' then implicitly define an 'orthogonal' axis as 'up' CROSS 'reference'.
///
/// The projection of a 3D vector v is then calculated as pair (a,b) where:
///     - a = component of v in the 'reference' direction
///     - b = component of v in the 'orthogonal' direction
class hkaiSilhouetteReferenceFrame
{
public:
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, hkaiSilhouetteReferenceFrame);

		/// Constructor - leaves 'up' and 'reference' invalid
	inline hkaiSilhouetteReferenceFrame();
	hkaiSilhouetteReferenceFrame( hkFinishLoadedObjectFlag f) {}

		/// Constructor - sets 'up' and 'reference'
	hkaiSilhouetteReferenceFrame( hkVector4Parameter up, hkVector4Parameter referenceAxis );

		/// Sets 'up' and 'reference'
	void set( hkVector4Parameter up, hkVector4Parameter referenceAxis );

		/// Get the 'first' component of a vector where this is the component in the reference direction
	hkSimdReal getFirstComponent(hkVector4Parameter v) const;

		/// Get the 'second' component of a vector where this is the component in the orthogonal direction
	hkSimdReal getSecondComponent(hkVector4Parameter v) const;
	
		/// Project the vector v to the 2D vector ( firstComponent, secondComponent, 0, 0)
	inline void project(hkVector4& vInOut) const;

		/// 'unproject' first and second to the 3D vector v - inverse of project()
	void setUnprojected(hkSimdRealParameter first, hkSimdRealParameter second, hkVector4& vOut) const;

		/// 'unproject' the 2D vector ( firstComponent, secondComponent, 0, 0) to the 3D vector v - inverse of project()
	inline void unproject(hkVector4& vInOut, hkSimdRealParameter missingComponent) const;

		/// Project the AABB to the 2D AABB with 3rd component zero.
	inline void projectAabb(hkAabb& aabbInOut) const;

		/// Accessor for 'up'
	inline const hkVector4& getUp() const;

		/// Accessor for 'reference'
	inline const hkVector4& getReferenceAxis() const;

		/// Accessor for 'orthogonal'
	inline const hkVector4& getOrthogonalAxis() const;
	
		/// Check is valid - all three axes are normalized.
	bool isValid() const;

private:
		/// Up
	hkVector4 m_up;

		/// Reference
	hkVector4 m_referenceAxis;

		/// Orthogonal
	hkVector4 m_orthogonalAxis;
};

#include <Ai/Pathfinding/Dynamic/Silhouette/SilhouetteReferenceFrame/hkaiSilhouetteReferenceFrame.inl>

#endif // HK_SILLOUETTE_REFERENCE_FRAME_H

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
