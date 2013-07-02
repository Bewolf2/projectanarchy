/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file LuminanceChannel.hpp

#ifndef LUMINANCECHANNEL_HPP_INCLUDED
#define LUMINANCECHANNEL_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/TerrainModule.hpp>

#include <Vision/Runtime/Base/Math/Rect/hkvRect.h>

/// \brief
///   2-dimensional array of byte values. Can be represented as a luminance texture.
class VLuminanceChannel : public VRefCounter, public VUserDataObj
{
public:
  /// \brief
  ///   Constructor: Allocates iSizeX*iSizeY bytes
  TERRAIN_IMPEXP VLuminanceChannel(int iSizeX, int iSizeY, UBYTE fillVal=0);

  /// \brief
  ///   Destructor: Releases the memory
  TERRAIN_IMPEXP virtual ~VLuminanceChannel()
  {
    V_SAFE_DELETE_ARRAY(m_pBuffer);
  }

  /// \brief
  ///   Returns m_iSizeX*m_iSizeY
  inline int GetSampleCount() const {return m_iSizeX*m_iSizeY;}

  /// \brief
  ///   Fills the whole array with random values (between min and max)
  inline void InitRandom(UBYTE iMin=0, UBYTE iMax=255)
  {
    int iMod = iMax-iMin+1;
    VASSERT(iMod>0);
    int iCount = m_iSizeX*m_iSizeY;
    for (int i=0;i<iCount;i++)
      m_pBuffer[i] = iMin + rand()%iMod;
    SetDirtyFlag();
  }

  /// \brief
  ///   Clears the array with the passed clear value (e.g. 0)
  inline void Clear(UBYTE fillVal=0)
  {
    memset(m_pBuffer,fillVal,GetSampleCount());
    m_bDirty = true;
    m_bIsZero = fillVal==0;
  }

  /// \brief
  ///   Marks the array as dirty so the bitmap needs to be re-uploaded when used as a luminance texture
  inline void SetDirtyFlag()
  {
    m_bDirty = true;
    m_bIsZero = -1;
  }

  /// \brief
  ///   Return the value at given coordinates
  inline UBYTE GetValue(int x, int y) const
  {
    VASSERT(x>=0 && x<m_iSizeX && y>=0 && y<m_iSizeY);
    return m_pBuffer[y*m_iSizeX+x];
  }

  /// \brief
  ///   Return the value pointer at given coordinates
  inline UBYTE* GetValuePtr(int x, int y) const
  {
    VASSERT(x>=0 && x<m_iSizeX && y>=0 && y<m_iSizeY);
    return &m_pBuffer[y*m_iSizeX+x];
  }

  /// \brief
  ///   Checks whether all array elements have the same value
  TERRAIN_IMPEXP bool CheckSolidColor(UBYTE val) const;

  /// \brief
  ///   Checks whether the array holds only 0 values. The result is cached so this function is fast
  inline bool IsBlack() const
  {
    if (m_bIsZero<0)
      m_bIsZero = CheckSolidColor(0) ? 1 : 0;
    return m_bIsZero==1;
  }

  /// \brief
  ///   Returns an interpolated value. The return value is i [0..1.0] range (rather than 0..255) and the passed coordinates also have to be in 0..1 range
  TERRAIN_IMPEXP float GetValueI(float x, float y) const;

  /// \brief
  ///   Copies the array from another one. Re-allocates to new sizes if sizes do not match
  TERRAIN_IMPEXP virtual void CopyFrom(const VLuminanceChannel *pOther);

  /// \brief
  ///   Blends a region passed array onto a region of this one using the passed channel as an opacity map
  TERRAIN_IMPEXP void Blend(const hkvRect& destRect, const hkvRect& srcRect, const VLuminanceChannel *pOther);
  
  /// \brief
  ///   Returns a value that measures how much the destRect region of this channel 'shines through' the srcRect region of pOther using the passed values as opacity
  TERRAIN_IMPEXP virtual float AlphaMaskCompare(const hkvRect& destRect, const hkvRect& srcRect, const VLuminanceChannel *pOther) const;

  /// \brief
  ///   Returns a cached instance of a texture that represents this array as luminance texture. The passed parameters are just used to build a filename for the resource viewer
  TERRAIN_IMPEXP VTextureObject *GetLuminanceTexture(int iSectorX, int iSectorY, int iDbgID);

  V_DECLARE_SERIALX( VLuminanceChannel, TERRAIN_IMPEXP );
  TERRAIN_IMPEXP virtual void SerializeX( VArchive &ar, int iWantedSizeX=-1, int iWantedSizeY=-1);

  TERRAIN_IMPEXP static int SortByAlphaVisibility(int iListCount, VLuminanceChannel **pSourceList, int iMaxCount, VLuminanceChannel **pDestList, float fTolerance=1.f/255.f);

  TERRAIN_IMPEXP bool ReadScaled(VChunkFile &file, int iSizeXInFile, int iSizeYInFile);
  TERRAIN_IMPEXP void Rescale(int iNewSizeX, int iNewSizeY);
public:
  int m_iSizeX, m_iSizeY;
  UBYTE *m_pBuffer;

  bool m_bDirty;
  mutable signed char m_bIsZero; ///< -1,0 or 1

  VTextureObjectPtr m_spLuminanceTexture;
};

typedef VSmartPtr<VLuminanceChannel> VLuminanceChannelPtr;


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
