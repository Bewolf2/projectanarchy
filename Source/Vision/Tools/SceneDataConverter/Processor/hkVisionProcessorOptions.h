/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */



#ifndef VISION_EXPORT_OPTIONS__H
#define VISION_EXPORT_OPTIONS__H

#pragma managed(push, off)

struct VExporterOptions
{
	struct Option;

  // Animation info structure.
  struct AnimInfo
  {
    // Ctor.
    AnimInfo();

    enum Type { INVALID, BONE, MUSCLE, MORPHTARGET };  //TODO: RENAME to  { INVALID, BONE_ANIM, VERTEX_ANIM, VERTEX_POSE }
    Type type;

    VString name;
    VString source;           //@@@ (timeline, clip, files...)
    bool    useActiveRange;   // If on, timeline range has precedence over 'range[]'.
    int     range[2];         // Specifies the animation range, by start [0] and end frame [1]
    bool    sample;           // Sample or not. @@@ rename? Better name?
    float   samplingDensity;  // Precision @@@ rename
    bool    extractMotionDelta;
    int     offsetDeltaAxes;  // The 3 LSBs stand for x=4, y=2 and z=1.
    VString rotationDelta;    // "No", "X", "Y" or "Z".


		// To/From string conversion of all anim-option.
		void ToString(VString& anim) const;
		//@@@void FromString(const VString& s);
		void FromOptionsTree(const VExporterOptions::Option& o);
  };


  //--------------------------------------------------------------------------------------
  // Options    NOTE: In order to add new options, add them here and assign default values
  // -------          in the constructor (VExporterOptions::VExporterOptions). Then expand
  //                  VExporterOptions::ToString and VExporterOptions::FromString methods,
  //                  accordingly. There shouldn't be anything else to do.
  //--------------------------------------------------------------------------------------

  // Harvesting.
  bool                    grabBones;
  bool                    grabLights;
  bool                    grabMaterials;
  bool                    grabColors;
  bool                    grabNormals;
  bool                    grabTangents;
  bool                    grabUVs;
  bool                    grabBezierCurves;
  VString                 uvChannel_diffuse;
  VString                 uvChannel_lightmap;
  LinkedList_cl<AnimInfo> anims;

  // Miscellaneous stuff.
  enum ColMeshType { COL_NOT_COLLIDING, COL_CONVEX_HULL, COL_COLLISION_MESH };
  ColMeshType colMeshType;
  bool        replaceSpacesInBoneNames;
  VString     trajectoryBoneName;
  bool        exportMaterialAlongWithMeshOrModel;
  bool        exportAnimAlongWithModel;
  bool        exportColmeshAlongWithMesh;
  bool        exportMeshAlongWithPrefab;  
  bool		  exportHiddenNodes; 

  // Optimization options.
  bool removeUnskinnedLeafBones;

  // Pivot.
  enum PivotType { PIVOT_T, PIVOT_TRS };  // Translation, Translation+Rotation+Scale
  VString   pivotNodeName;
  PivotType pivotAffects;
  float     scaleMesh;

  // Visibility processing.
  float maxSubmeshSize;
  bool  computeVisibility;
  bool  writeVisibilityDebugInfo;

  // Post export.
  bool    displayViewer;
  bool    extractAndCopyTextures;
  bool    projRelativeTexturePath; // True = export textures to project relative path. False = export textures to model relative path. 
  VString textureSubdir;    // @@@ allow something like $(ProjDir), etc.

  //Spatial grouping options as a use-case for more complex options we need in this stage?




  //--------------------------------------------------------------------------------------
  // Convenience functions, etc..
  //--------------------------------------------------------------------------------------

  // Presets for different usage cases of export options.
  enum Presets
  {
    VEOP_DEFAULT,
    VEOP_3DSMAX_DEFAULT,
    VEOP_MAYA_DEFAULT
    // @@@ think of splitting this up by export type, as well... e.g. animations, meshes, etc.
  };

  // Ctor - options will set their defaults according to a passed-in preset.
  VExporterOptions(Presets preset);

  // To/From string conversion of all options.
  void ToString(VString& s) const;
  bool FromString(const VString& s);


  struct Option
  {
    // Info about ptions.
    VString key;
    VString val;                                // Optional.
    char typeSig;                               // Optional.
    VString info;                               // Optional.
    VString infoExt;                            // Optional.
    VGVisionImporterExporter::DataFormat mask;  // Optional.

    // Suboptions (optional).
    LinkedList_cl<Option>* subOptions;
    Option* parent;

    // Ctors/dtor/assignment.
    Option();
    Option(const Option& other);
    ~Option();
    Option& operator=(const Option& rhs);

		// Minimal string representation (without typeSig, info(Ext), mask; recursive).
		void ToSimpleString(VString& s);
  };

  // Get a tree of options (as a simplified way to enumerator options, compared to 'ToString') or the inverse.
  void ToOptionsTree(Option& o) const;
  void FromOptionsTree(const Option& o);

  // Checks whether animation names are unique. Fills optional list with non-unique names.
  bool AreAnimationNamesUnique(VArray<VString>* nonUnique = NULL) const;

  // Compute the correct pivot transform taking into acccount current options
  void BuildPivotTransformMatrix(hkvMat4 &out, const hkvMat4 &pivot) const;
};


#endif

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
