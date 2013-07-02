/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_VISION_MESH_VERTEX_BUFFER_H
#define HK_VISION_MESH_VERTEX_BUFFER_H

#include <Common/GeometryUtilities/Mesh/hkMeshVertexBuffer.h>
#include <Vision/Tools/Libraries/Geom2/vGeom2.hpp>
#include <Vision/Tools/Libraries/Geom2/Primitives/VGVertex.hpp>

	/// hkMeshVertexBuffer wrapper around Trinigy VisMeshBuffer_cl objects
class hkVisionMeshVertexBuffer : public hkMeshVertexBuffer
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SCENE_DATA);

	public:

			/// Constructor
		hkVisionMeshVertexBuffer(class VGVertexList* buffer, class VGSkeleton* skeleton, bool ownsBuffer);

			/// Destructor
		virtual ~hkVisionMeshVertexBuffer();

	public:

			/// Specifies the groups that should be shared.
		virtual hkMeshVertexBuffer* clone();

			/// Returns true if all of the data is sharable - i.e., if all data is sharable doing a clone will just return a ref count of this object.
			/// NOTE! That a format that says all the members are SHARABLE, doesn't mean this will return true - as an underlying implementation
			/// may require per instance data (for example when software skinning)
		virtual bool isSharable();

			/// Gets the vertex format and stores the result in formatOut
		virtual void getVertexFormat(hkVertexFormat& formatOut);

			/// The total number of vertices in the vertex buffer
		virtual int getNumVertices();

			/// Lock all of the elements in a vertex buffer.
			/// Only one lock can be active on the vertex buffer at any time
		virtual LockResult lock(const LockInput& input, LockedVertices& lockedVerticesOut);

			/// Perform a partial lock - locks a subset of elements.
			/// Only one lock can be active on the vertex buffer at any time.
		virtual LockResult partialLock(const LockInput& input, const PartialLockInput& partialInput, LockedVertices& lockedOut);

			/// Fetches elements into a vector4 array doing conversions as needed. Does not convert integral types.
			/// The elements being extracted must have been previously been locked with lock/partialLock.
		virtual void getElementVectorArray(const LockedVertices& lockedVertices, int elementIndex, hkFloat32* compData);

			/// Sets the elements of an array doing conversions from hkVector4s as needed. Does not convert integral types.
			/// The elements being set must have previously have been locked with lock/partialLock.
		virtual void setElementVectorArray(const LockedVertices& lockedVertices, int elementIndex, const hkFloat32* compData);

			/// Sets the elements of an array doing conversions from integers as needed. Converts only integral types.
			/// The elements being extracted must have been previously been locked with lock/partialLock.
		virtual void getElementIntArray(const LockedVertices& lockedVertices, int elementIndex, int* compData);

			/// Sets the elements of an array doing conversions from integers as needed. Converts only integral types.
			/// The elements being set must have previously have been locked with lock/partialLock.
		virtual void setElementIntArray(const LockedVertices& lockedVertices, int elementIndex, const int* compData);

			/// Unlock a previously locked vertex buffer. The lockedVertices structure that was set in 'lock' or 'partialLock'
			/// should be passed into the unlock. Undefined behavior results if lockedVertices contains any different values than
			/// were returned from the lock/partialLock call.
		virtual void unlock(const LockedVertices& lockedVertices);

	public:

		const VGVertex::VertexMask getVertexMask() const;

		HK_FORCE_INLINE VGVertexList* getVertexList() const { return m_buffer; }

			/// Returns the VGeom2 skeleton, if any
		HK_FORCE_INLINE VGSkeleton* getSkeleton() const { return m_skeleton; }

			/// Adds new element to the underlying vertex mask
		void enhance( VGVertex::VertexMask newMask );

			/// Converts a data type from Vision to Havok format
		static hkVertexFormat::ComponentType HK_CALL getHkComponentType( int visionComponentFormat );

		/// Converts a data type from Vision to Havok format
		static int HK_CALL getHkComponentCount( int visionComponentFormat );

			/// Converts the vertex format from Vision to Havok
		void getHkVertexFormat(class VGVertexList* bufferIn, hkVertexFormat& fmtOut, LockedVertices& offsetsOut);

			/// Creates a vertex buffer from the given vertex buffer
		static hkMeshVertexBuffer* HK_CALL create(const hkVisionMeshVertexBuffer* templateVertexBuffer, int numVertices);

			/// Creates a vertex buffer from the given format
		static hkMeshVertexBuffer* HK_CALL create(const hkVertexFormat& vertexFormat, int numVertices);

			/// Compute vertex mask from hkVertexFormat
		static VGVertex::VertexMask HK_CALL computeVertexMask( const hkVertexFormat& vertexFormat ); 

			/// Searches for the given bone in the skeleton. If the skeleton is null, it will simply cast the bone address to an int and return it
		int getBoneIndex(const class VGBone* bone) const;

			/// Returns the bone at the given index. If the skeleton is null, it will simply return the bone index!
		const class VGBone* getBone(int boneIndex) const;

	protected:

			/// Vision mesh buffer
		class VGVertexList* m_buffer;

			/// Scene skeleton, used to resolve the bone indices
		class VGSkeleton* m_skeleton;

			/// Cached vertex buffer format
		hkVertexFormat m_vtxFormat;

			/// Cached locked vertices, holds the offsets to the vertex components
		LockedVertices m_lockedVerticesDesc;

			/// Array of weights
		hkArray<hkVector4> m_boneWeights;

			/// Array of bone indices
		hkArray<hkUint8> m_boneIndices;

			/// True if we own the buffer
		hkBool m_ownsBuffer;
};

#endif	//	HK_VISION_MESH_VERTEX_BUFFER_H

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
