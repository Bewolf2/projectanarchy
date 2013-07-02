/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#ifndef VISION_EXPORT_CONVERTER__H
#define VISION_EXPORT_CONVERTER__H

#include <Common/Base/Container/Set/hkSet.h>

class hkxMaterial;
class hkxMesh;
class hkxNode;
class hkxSpline;

struct VExporterOptions;

class hkVisionConverter : public hkReferencedObject
{

public: 

	hkVisionConverter( hkxScene* scene);
	virtual ~hkVisionConverter();

	struct ConvertOptions
	{
		ConvertOptions();
		bool m_printSummary;
		bool m_undoInputSceneTransform; // will alter input scene. Give a copy if you want unmutable
		bool m_assumeDDSTextures; // assume all textures already in DDS format even if HKT says otherwise
		bool m_flipV; // Vision uses 1-v by default from Max. But some Havok DDS files are already inverted, so can be handy not to invert :)
		bool m_filePerMesh; // Make a .model say for each mesh node
		bool m_stripPhysicsEtc; // Take out geoms that are not graphics related (or at least look that way)
		bool m_skipPrefabs; // normally true if doing whole scene export, false if doing exportselected
		bool m_flattenBones;	// flatten bone hierarchy to all root bones
		bool m_disableBoneSorting;	// dont reorder bones in the skeletons
		const char* m_assetPath; // so that textures can

		VExporterOptions* m_processorOptions; // so that we can only produce as much as requested, know the animation ranges etc 
	} m_options;

	// Full scene convert
	hkResult convert( VGScene& destScene, VGVisionImporterExporter::DataFormat assumedOutputformat );

	// Prefabs are empty nodes that reference external files, and then lights and splines
	hkResult convertPrefab( VGScene& destScene );


protected:

	hkxScene* m_scene;

	VGScene* m_gscene;
	VGVisionImporterExporter::DataFormat m_outputformat; 

	hkResult convertMesh(  const hkxNode* n, const hkxMesh* m, const hkMatrix4& transform, const hkxSkinBinding* curSkinBinding );
	int      convertMaterial( const hkxMaterial* m );
	hkResult convertLight( const hkxNode* n, const hkxLight* l, const hkMatrix4& transform);
	hkResult convertNode( const hkxNode* n, const hkMatrix4& parentTransform); 
	hkResult convertNodePrefab( const hkxNode* n, const hkMatrix4& parentTransform );
	hkResult convertSpline( const hkxNode* n, const hkxSpline* s, const hkMatrix4& parentTransform );

	void	 convertBoneAnims( const hkxNode* hnode, const hkSet< float >* hints );
	void     convertBindPose( const hkxNode* hnode, VGBone& vbone, const hkArray<const hkxSkinBinding*>& bindings );
	void	 convertBone( const hkxNode* hnode, VGBone& vbone, const hkvMat4& transformation );

	const VGBone* getBoneByIndex(hkUint32 i, const hkxSkinBinding* binding);
	int		 getBoneIndex(const VGBone* bone, const hkxSkinBinding* binding);

	void	 cleanupTempTextures();
	const char* saveInplaceTextureToFile( const hkxTextureInplace* t );

	void	 recomputeMatrixToLocalSpace( const hkMatrix4& parentWorldTransform, const hkMatrix4& in, hkvMat4& out );
	
	hkPointerMap< const hkxMaterial*, int > m_materialMap;
	hkPointerMap< const hkxMesh*, int > m_meshMap;

	hkPointerMap< const hkxNode*, int > m_nodeToBoneMap;
	hkArray<VGBone*> m_bones; // as per indexed from skins
	hkPointerMap< const hkxTextureInplace*, char* > m_inplaceTextureFiles;

private:

	void	 convertBoneAnimsRecursive(const hkxNode* hnode, const hkSet< float >* hints, hkArray<const hkxNode*>& interNodePath );
	void	 convertBindPoseRecursive( const hkxNode* hnode, VGBone& vbone, const hkArray<const hkxSkinBinding*>& bindings, const hkMatrix4& parentWorldTransform, hkArray<const hkxNode*>& interNodePath );
	void	 getBoneAnimFromKeyFrame( VGBone& vbone, VGAnimation_Bone& vboneAnim, const hkxNode* hnode, hkArray<const hkxNode*>& interNodePath, int startFrame, int keyframe, bool rootScale = false );
	bool	 getBoneAnimFromKeyTime( VGBone& vbone, VGAnimation_Bone& vboneAnim, const hkxNode* hnode, hkArray<const hkxNode*>& interNodePath, float startTime, float keyTime, bool rootScale = false );
};

#endif //VISION_EXPORT_CONVERTER__H

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
