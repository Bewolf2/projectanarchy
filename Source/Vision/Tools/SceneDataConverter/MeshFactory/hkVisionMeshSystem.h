/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_VISION_MESH_SYSTEM_H
#define HK_VISION_MESH_SYSTEM_H

#include <Common/GeometryUtilities/Mesh/hkMeshSystem.h>

class hkMeshMaterialRegistry;

/// hkMeshSystem implementation for Vision
class hkVisionMeshSystem : public hkMeshSystem
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SCENE_DATA);

	public:

			/// Constructor
		hkVisionMeshSystem();

	public:

			/// Get the material registry
		virtual hkMeshMaterialRegistry* getMaterialRegistry() const HK_OVERRIDE;

			/// Create a mesh shape.
			/// Returns HK_NULL if not successful.
		virtual hkMeshShape* createShape(const hkMeshSectionCinfo* sections, int numSections) HK_OVERRIDE;

			/// Create a compound shape. The compound will produce a new shape, with each contained
			/// shape, having a transform index to allow control of each 'contained' shape individually.
			/// An implementation can change the structure of the object (for example producing a single shared vertex buffer), but
			/// graphically must look the same. Also note there is no way to find out the shapes used to construct the compound.
			/// It is possible to create a shape that has the same functionality as a shape created this way by using the m_transformIndex in the hkMeshSection.
			/// Returns HK_NULL if not successful.
		virtual hkMeshShape* createCompoundShape(const hkMeshShape*const* shapes, const hkMatrix4* transforms, int numShapes) HK_OVERRIDE;

			/// Create mesh body with the specified local position, and transform set.
			/// The transform set is only needed for compound, or skinned usage. Passing in HK_NULL will disable skinning or per hkMeshSection transforms.
			/// Returns HK_NULL if not successful.
		virtual hkMeshBody* createBody(const hkMeshShape* shape, const hkMatrix4& mat, hkIndexedTransformSetCinfo* transformSet) HK_OVERRIDE;

			/// Create a vertex buffer. Returns HK_NULL if not successful. This implementation will fail, use the one with a template vertex buffer!
		virtual hkMeshVertexBuffer* createVertexBuffer(const hkVertexFormat& vertexFormat, int numVertices) HK_OVERRIDE;

			/// Create a vertex buffer. The vertex format is retrieved from the given sourceVertexBuffer
		virtual hkMeshVertexBuffer* createVertexBuffer(const hkMeshVertexBuffer* templateVertexBuffer, int numVertices) HK_OVERRIDE;

			/// Returns a vertex format which is suitable for this mesh system, based on the format passed in.
		virtual void findSuitableVertexFormat(const hkVertexFormat& format, hkVertexFormat& formatOut) HK_OVERRIDE;

			/// Returns an 'empty' material. Will be of the right type for the mesh system. In order to make the material
			/// appear correctly code for a specific engine implementation needs to set the internal state appropriately.
		virtual hkMeshMaterial* createMaterial() HK_OVERRIDE;

			/// Create a texture with no data
		virtual hkMeshTexture* createTexture() HK_OVERRIDE;

			/// Add a body
		virtual void addBody(hkMeshBody* body) HK_OVERRIDE;

			/// Remove a body
		virtual void removeBody(hkMeshBody* body) HK_OVERRIDE;
		
			/// Called once per frame - required to perform any system wide per frame operations.
		virtual void update() HK_OVERRIDE;

			/// Free resources in mesh system
		virtual void freeResources() HK_OVERRIDE;

	protected:

		hkRefPtr<hkMeshMaterialRegistry> m_materialRegistry;	///< the material registry
};

#endif	//	HK_VISION_MESH_SYSTEM_H

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
