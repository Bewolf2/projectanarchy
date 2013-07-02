/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_TRANSFORMATION_STEP_IMAGE_TO_DDS_HPP_INCLUDED
#define HKV_TRANSFORMATION_STEP_IMAGE_TO_DDS_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Transformation/hkvFileTransformationStep.hpp>

class hkvMipImage;
class hkvTextureTransformationSettings;

struct DDS_HEADER;

class hkvTransformationStepImageToDds : public hkvFileTransformationStep
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  hkvTransformationStepImageToDds(const hkvTextureTransformationSettings& settings,
    const char* sourceFile, const char* targetFile);
private:
  hkvTransformationStepImageToDds(const hkvTransformationStepImageToDds&);
  hkvTransformationStepImageToDds& operator=(const hkvTransformationStepImageToDds&);

  static inline DWORD makeBitMask(DWORD width, DWORD shift)
  {
    return width > 0 ? (0xffffffffu >> (32 - width)) << shift : 0;
  }

  hkResult writeImageBlock(hkStreamWriter& writer, const DDS_HEADER& ddsHeader, hkvMipImage* image);

public:
  virtual hkResult run() HKV_OVERRIDE;
  virtual void cancel() HKV_OVERRIDE;

private:
  DWORD m_rgbaBits;
  DWORD m_rWidth;
  DWORD m_rShift;
  DWORD m_gWidth;
  DWORD m_gShift;
  DWORD m_bWidth;
  DWORD m_bShift;
  DWORD m_aWidth;
  DWORD m_aShift;
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
