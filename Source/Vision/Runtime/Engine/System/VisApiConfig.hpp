/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiConfig.hpp

#ifndef DEFINE_VISAPICONFIG
#define DEFINE_VISAPICONFIG

typedef unsigned char UCHAR;

#define V_DEFAULT_XRES VVIDEO_DEFAULTWIDTH
#define V_DEFAULT_YRES VVIDEO_DEFAULTHEIGHT

/// \brief
///   Resource configuration structure for defining the un-/reloading behavior
///   when the application goes to the background.
///
/// Background handling of certain resource types can be specified here.
/// The default values define the minimum/suggested requirements on specific platforms.
struct VisResourceConfig_t
{
  VisResourceConfig_t()
  {
#if defined(WIN32) && defined(_VR_DX9) // on lost device
    // restore textures and buffers (only unmanaged)
    restoreTexturesOnBackgrounding = true;
    restoreBuffersOnBackgrounding = true;
    restoreShadersOnBackgrounding = false;

#elif defined(_VISION_ANDROID)
    // restore all
    restoreTexturesOnBackgrounding = true;
    restoreBuffersOnBackgrounding = true;
    restoreShadersOnBackgrounding = true;

#elif defined(_VISION_IOS)
    // restore textures
    restoreTexturesOnBackgrounding = true;
    restoreBuffersOnBackgrounding = false;
    restoreShadersOnBackgrounding = false;

#elif (defined(WIN32) && defined(_VR_DX11)) || defined(_VISION_PS3) || defined(_VISION_XENON) || defined(_VISION_WIIU) || defined(_VISION_PSP2)
  // TODO: Metro, Apollo

    restoreTexturesOnBackgrounding = false;
    restoreBuffersOnBackgrounding = false;
    restoreShadersOnBackgrounding = false;

#else

#warning undefined platform!

#endif
  }

  // enable resource background handling for specific resource types
  bool restoreTexturesOnBackgrounding;
  bool restoreBuffersOnBackgrounding;           ///< Implicates reload of static/dynamic meshes if necessary
  bool restoreShadersOnBackgrounding;
};

/// \brief
///   Vision Engine Configuration Structure which is passed to the Vision::Init method
/// 
/// This configuration structure is used to initialize a set of parameters which are typically not
/// changed while the application is running. In fact, it is only rarely necessary to modify the
/// default settings.
struct VisConfig_t
{
public:

  VisConfig_t()
  {
    version = 7;                                    
    size = sizeof (VisConfig_t);                   
     
    // visibility  
    visibilitySubDivideAreas = VIS_ENABLED;             ///<DEPRECATED
    visibilityMinKdTreeLeafSize = 384;                  ///<DEPRECATED
    visibilityMaxPrimitivesPerKdTreeNode = 256;         ///<DEPRECATED
    octreeLeafSize = 4096; 

    // geometry caches
    maxShadowVertices = 64000; 
    maxShadowIndices = 128000;
    
    // Maximum item count values
    globalMaxDynamicLights = 220;
    globalMaxPolygonChunks = 512;                       ///<DEPRECATED
    globalMaxObjectTypes = 512;                         ///<DEPRECATED

    // adjacency information (required for stencil shadow casting)
    computeAdjacencyForModels = true;                   ///<DEPRECATED
    computeAdjacencyForWorld = true;                    ///<DEPRECATED

    m_pszAssetProfile = NULL;
  }
  

  // members
  int version;                                  ///< Structure version, version must be either 3 or 4. the current version is 6 (!)
  int size;                                     ///< Size of this structure.
   
  // visibility  
  HKV_DEPRECATED_2013_2 VIS_CFG_OnOff visibilitySubDivideAreas;       ///<DEPRECATED
  HKV_DEPRECATED_2013_2 int visibilityMinKdTreeLeafSize;              ///<DEPRECATED
  HKV_DEPRECATED_2013_2 int visibilityMaxPrimitivesPerKdTreeNode;     ///<DEPRECATED
  HKV_DEPRECATED_2013_2 int octreeLeafSize;                           ///<DEPRECATED

  // geometry caches
  HKV_DEPRECATED_2013_2 int maxShadowVertices;                        ///<DEPRECATED
  HKV_DEPRECATED_2013_2 int maxShadowIndices;                         ///<DEPRECATED
  
  // dynamic array sizes, replaces constants in older engine versions
  int globalMaxDynamicLights;                   ///< maximum number of dynamic lightsources (excluding those in .v3d). default is 200
  HKV_DEPRECATED_2013_2 int globalMaxPolygonChunks;                   ///< DEPRECATED. Maximum number of polygon chunks. default is 512. you should only increase it when the engine is complaining about the value being too small.
  HKV_DEPRECATED_2013_2 int globalMaxObjectTypes;                     ///< IGNORED

  // adjacency computation
  HKV_DEPRECATED_2013_2 bool computeAdjacencyForModels;               ///< DEPRECATED
  HKV_DEPRECATED_2013_2 bool computeAdjacencyForWorld;                ///< DEPRECATED

  // background resource handling
  VisResourceConfig_t resourceConfig;

  // Asset library profile
  const char* m_pszAssetProfile;                ///< The Asset profile to be used, can be NULL. The engine will then use the name of the platform (pcdx9, pcdx11, xbox360, ps3, android, psvita, ios)

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
