/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Base/Graphics/Textures/VTextureManager.hpp>

/// \brief
///  A texture provider that always returns a plain white texture rather than loading the texture from file.
class VNullTextureProvider : public IVTextureFormatProvider
{
public:
  virtual const char** GetSupportedFileExtensions(int &iListCount) HKV_OVERRIDE
  {
    static const char* formats[] = {
      "dds", "png", "jpg", "jpeg", "tga", "pvr", "etc", "rgba", "tex", "gfd", "bmp"  // Extend list as necessary
    };

    iListCount = V_ARRAY_SIZE(formats);
    return formats;
  }

  virtual VTextureObject* CreateTexture(const char *szFilename, int &iFlags) HKV_OVERRIDE
  {
    return Vision::TextureManager.GetPlainWhiteTexture();
  }
};

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
