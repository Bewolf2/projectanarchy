/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_VISION_MESH_SHAPE_H
#define HK_VISION_MESH_SHAPE_H

#include <Common/GeometryUtilities/Mesh/hkMeshShape.h>
#include <Common/SceneData/Blob/hkxBlobMeshShape.h>

extern const hkClass hkVisionMeshShapeClass;

	/// hkMeshShape wrapper around Trinigy VBaseMesh objects
class hkVisionMeshShape : public hkxBlobMeshShape
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SCENE_DATA);
		HK_DECLARE_REFLECTION();

	public:

			/// Constructor
		hkVisionMeshShape(class VGScene* scene, bool ownsScene, bool shouldHaveSortedTriangles);
		
			/// Serialization constructor
		hkVisionMeshShape(hkFinishLoadedObjectFlag flag);

			/// Destructor
		virtual ~hkVisionMeshShape();

	public:

			/// Returns the amount of sections
		virtual int getNumSections() const;

			/// Gets information about a section
		virtual void lockSection(int sectionIndex, hkUint8 accessFlags, hkMeshSection& sectionOut) const;

			/// Unlocks a mesh section. Must be given exactly the same structure contents as was returned from a lockSection
			/// otherwise behavior is undefined.
		virtual void unlockSection(const hkMeshSection& section) const;

			/// hkReferencedObject Implementation
		virtual const hkClass* getClassType() const { return &hkVisionMeshShapeClass; }

	public:

			/// Creates a mesh by merging all vertex and index buffers in the given sections into one
		static hkVisionMeshShape* HK_CALL create(const hkMeshSectionCinfo* sections, int numSections);

			/// Computes and updates collision vertices
		void updateCollisionMesh();

			/// REturns true if the mesh has been baked
		HK_FORCE_INLINE bool isBaked() const		{ return m_isBaked; }

			/// Sets the mesh baked status
		HK_FORCE_INLINE void setBaked(bool baked)	{	m_isBaked = baked; }

			/// Bakes the mesh into the blob
		void bake(const class hkVisionMeshFactory* factory);
		
			/// Returns the Vision mesh
		HK_FORCE_INLINE const class VGMesh* getMesh() const { return m_mesh.m_visionMesh; }
	
			/// Returns the Vision scene
		HK_FORCE_INLINE const class VGScene* getScene() const { return m_mesh.m_visionScene; }

	protected:

			/// Converts a Vision primitive type to Havok format
		static hkMeshSection::PrimitiveType HK_CALL getHkPrimitiveType(int visionPrimitiveType, bool& isIndexed);

			/// Computes the number of primitives
		static int HK_CALL calcNumPrimitives(int numIndices, hkMeshSection::PrimitiveType primitiveType);

			/// Converts a Vision index type to Havok format
		static hkMeshSection::MeshSectionIndexType HK_CALL getHkIndexType(int visionIndexType);

	protected:

			/// Finds a (mesh, sub-mesh) pair from the section index
		void findMeshFromSectionIndex(int sectionIdx, int& meshIdxOut, int& subMeshIdxOut) const;

			/// Look for material boundaries and create sections
		void createSections();

			/// Deletes everything except the blob data
		void disposeMesh();

	public:

			// Mesh struct
		struct Mesh
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SCENE_DATA, hkVisionMeshShape::Mesh);
			HK_DECLARE_REFLECTION();

				/// Constructor
			Mesh();

			HK_FORCE_INLINE class hkVisionMeshVertexBuffer* getVertexBuffer()				{ return reinterpret_cast<hkVisionMeshVertexBuffer*>(m_vb.val()); }
			HK_FORCE_INLINE const class hkVisionMeshVertexBuffer* getVertexBuffer() const	{ return reinterpret_cast<hkVisionMeshVertexBuffer*>(m_vb.val()); }

			HK_FORCE_INLINE class hkVisionMeshIndexBuffer* getIndexBuffer()				{ return reinterpret_cast<hkVisionMeshIndexBuffer*>(m_ib.val()); }
			HK_FORCE_INLINE const class hkVisionMeshIndexBuffer* getIndexBuffer() const	{ return reinterpret_cast<hkVisionMeshIndexBuffer*>(m_ib.val()); }

			class VGMesh* m_visionMesh;				// +nosave
			class VGScene* m_visionScene;			// +nosave
			hkRefPtr<hkReferencedObject> m_vb;		// +nosave
			hkRefPtr<hkReferencedObject> m_ib;		// +nosave
		};

	protected:

			/// Vision meshes
		Mesh m_mesh;							// +nosave
		hkArray<hkMeshMaterial*> m_materials;	// +nosave

			/// Sections
		hkArray<hkMeshSection> m_sections;		// +nosave

			/// True if the mesh has been baked
		hkBool m_isBaked;						// +nosave
};

#endif	//	HK_VISION_MESH_SHAPE_H

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
