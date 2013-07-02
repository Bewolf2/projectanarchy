/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#ifndef HK_HCT_VISION_MESH_FACTORY_H
#define HK_HCT_VISION_MESH_FACTORY_H

// Mesh factory to create hkVisionMeshShapes from hkxScene data
// and to embed Vision .model files for the dynamically created
// meshes in the root container

#include <Common/SceneData/Mesh/hkxMeshFactory.h>
#include <Common/GeometryUtilities/Mesh/Skin/hkSkinnedMeshShape.h>

class hkVisionMeshShape;
class hkVisionMeshSystem;
class hctCreateVisionDataFilter;
class VGMaterial;
class VGScene;
class VGMesh;

class hkxScene; 
class hkxNode;
class hkMeshShape;

class hkVisionMeshFactory : public hkxMeshFactory
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT);

	public:

			/// Construction / Destruction
		hkVisionMeshFactory( hctCreateVisionDataFilter* owner );
		virtual ~hkVisionMeshFactory();

			/// hkxMeshFactory methods
		virtual hkMeshSystem* getMeshSystem();

		virtual void extractShapes( hkRootLevelContainer* rootLevelContainer, hkStringMap<int>& extraGraphicsNodes, hkStringMap<hkMeshShape*>& shapesOut );

			/// Stores all the given shapes in the given root level container
		virtual void storeShapes(hkRootLevelContainer* rootLevelContainer, hkArray<const hkMeshShape*>& shapes);

			/// Removes all the given shapes from the root level container
		virtual void removeShapes(hkRootLevelContainer* rootLevelContainer, hkArray<const hkMeshShape*>& shapes);

		virtual const hkMatrix3& getTransform() const;
		virtual void setTransform( const hkMatrix3& transform );

			/// Extracts all hkxMeshes
		void extractAllHkxMeshes(hkRootLevelContainer* rootLevelContainer, hkArray<const hkMeshShape*>& shapesOut);

			/// Locates a material in the list of all known materials
		const VGMaterial* findMaterialByName(const char* materialName) const;

			/// Sets vision exporter options
		void setVisionExportOptions( const hkStringPtr& options ) { m_visionExportOptions = options; }

			/// Flattens all the bones in the given scene
		static void flattenSkeleton(class VGScene* visionScene);

	protected:

			/// Recursively loads vision meshes based on custom property info in the hkxNode and creates hkcVisionMeshShapes
		void extractChildShapes(hkxScene* scene, hkxNode* node, hkArray<const hkMeshShape*>& shapesOut);
		
			/// Creates Vision .model file from hkVisionMeshShapes and embeds them in rootLevelContainer as blobs
		void createVisionMesh(hkRootLevelContainer* rootLevelContainer, hkVisionMeshShape* visionMeshShape, const char* name);

			/// Ensures the Vision container is created
		class hkResourceContainer* ensureExistingVisionContainer(hkRootLevelContainer* rootLevelContainer);

			/// Stores a Vision skin ref in the root level container
		void storeSkinRef(hkRootLevelContainer* rootLevelContainer, const class hkSkinnedRefMeshShape* skinRef, const char* name);

			/// Transforms mesh from Vision to Havok coordinate-system
		void scaleMesh(VGScene* scene);

			/// Creates a skinned mesh shape from the given mesh shapes and transforms. A single bone will drive each of the provided shapes.
			/// Currently, the implementation returns HK_NULL.
		virtual hkSkinnedMeshShape* createSkinnedMesh(const char* skinnedMeshName, const hkMeshShape** meshShapesIn, const hkQTransform* transformsIn, int numMeshes, int maxBonesPerSection);

			/// Registers all materials in a scene
		void registerSceneMaterials(const VGScene& scene);

			/// Loads the vision mesh based on the custom property of the node
		void loadMesh( hkxScene* havokScene, hkxNode* node, hkArray<const hkMeshShape*>& shapesOut );

	protected:

			/// Vision mesh system
		hkVisionMeshSystem* m_meshSystem;

			/// All known materials
		hkArray<VGMaterial> m_materials;

			/// Vision -> Havok transform
		hkMatrix3 m_transform;

			/// Vision exporter options
		hkStringPtr m_visionExportOptions;
};

#endif // HK_HKD_VISION_MESH_FACTORY_H

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
