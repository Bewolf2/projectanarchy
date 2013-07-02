/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_REFERENCE_FRAME_AND_EXTRUSION
#define HK_AI_REFERENCE_FRAME_AND_EXTRUSION

	/// AABB expansion information
class hkaiReferenceFrameAndExtrusion
{
	public:
		//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiReferenceFrameAndExtrusion );

			/// How the world up vector is used to project silhouettes for nav meshes transformed out of the horizontal
			/// plane. Wall climbing meshes always use USE_FACE_NORMAL.
		enum UpVectorTransformMethod
		{
				/// Always use the world up vector. This is generally recommneded but can cause artifacts
				/// if transformed faces are nearly vertical with respsect to the world up.
			USE_GLOBAL_UP,

				/// Use the world up vector, rotated by the instance's tranform.
			USE_INSTANCE_TRANSFORM,

				/// Ignore the world up vector and use the face's normal for the projection direction.
				/// This is only recommended for wall climbing nav meshes.
			USE_FACE_NORMAL,
		};

		/// Constructor
		inline hkaiReferenceFrameAndExtrusion();
		inline hkaiReferenceFrameAndExtrusion( hkFinishLoadedObjectFlag f );

		/// Check members have been set
		inline bool isValid() const;

		/// Axis of projection
		hkVector4 m_up;

		/// Height for cell extrusion along m_up direction
		hkReal m_cellExtrusion;
		/// Radius expansion for silhouette generators in the non-up directions
		hkReal m_silhouetteRadiusExpasion;
		/// How the world up vector is used to project silhouettes for nav meshes transformed out of the horizontal plane.
		hkEnum<UpVectorTransformMethod, hkUint8> m_upTransformMethod; //+default(hkaiReferenceFrameAndExtrusion::USE_GLOBAL_UP);
};

#include <Ai/Pathfinding/Dynamic/Partitioning/hkaiReferenceFrameAndExtrusion.inl>

#endif // HK_AI_REFERENCE_FRAME_AND_EXTRUSION

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
