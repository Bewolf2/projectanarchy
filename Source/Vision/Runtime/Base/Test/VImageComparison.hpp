/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VImageComparison.hpp

// VImageComparison.hpp: interface for the VImageComparison class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIMAGECOMPARISON_HPP__027D583A_5E25_4ABD_B250_04DBD39EA8CF__INCLUDED_)
#define AFX_VIMAGECOMPARISON_HPP__027D583A_5E25_4ABD_B250_04DBD39EA8CF__INCLUDED_

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class ImageMap_cl;

/// \brief
///   This class can be used to compare two 24 bit images. It supports max and average tolerance
///   values, returns the average and maximum difference which has been detected and allows you to
///   save a difference bitmap. The input data can be from file or memory. The two compared bitmaps
///   are called "source" and "target" in this class.
class VBASE_IMPEXP_CLASS VImageComparison  
{
public:
  VImageComparison();
  virtual ~VImageComparison();

  
  /// \brief
  ///   Load the source bitmap from file
  VBool LoadSourceBitmapFromFile( const char *szFileName);

  
  /// \brief
  ///   Load the source bitmap from memory
  VBool LoadSourceBitmapFromMemory( void *pMem, int iSizeX, int iSizeY, int iBPP );

  
  /// \brief
  ///   Load the target bitmap from file
  VBool LoadTargetBitmapFromFile( const char *szFileName );

  
  /// \brief
  ///   Load the target bitmap from memory
  VBool LoadTargetBitmapFromMemory( void *pMem, int iSizeX, int iSizeY, int iBPP );
  

  /// \brief
  ///   Compare the two bitmaps. This function fails if the two bitmaps have not been loaded yet.
  /// 
  /// \param storeAverage
  ///   the average difference will be stored there
  /// 
  /// \param storeMax
  ///   the maximum difference which has been found will be stored there
  /// 
  /// \param iValidAverage
  ///   average difference which is allowed. If the found average difference is higher than that
  ///   value, the comparison returns FALSE.
  /// 
  /// \param iValidMax
  ///   maximum difference which is allowed. If the found maximum difference is higher than that
  ///   value, the comparison returns FALSE.
  /// 
  /// \return
  ///   VBool: returns TRUE if the two bitmaps are the same (considering the passed tolerances
  ///   iValidAverage and iValidMax)
  VBool CompareBitmaps(int &storeAverage, int &storeMax, int iValidAverage = 8, int iValidMax = 768);

  
  /// \brief
  ///   Compares the two bitmaps and saves the difference file in case the comparison failed. The
  ///   function returns TRUE if the two bitmaps are the same (considering the passed tolerances
  ///   iValidAverage and iValidMax)
  VBool CompareBitmapsAndSave( const char *szFileName, int &storeAverage, int &storeMax, int iValidAverage = 8, int iValidMax = 768 );


  /// \brief
  ///   Save the source bitmap as a BMP to the specified file
  VBool SaveSourceBitmap( const char *szFileName );


  /// \brief
  ///   Save the target bitmap as a BMP to the specified file
  VBool SaveTargetBitmap( const char *szFileName );
  

protected:
  VBool SaveComparisionBitmap( const char *szFileName );
  VBool MakeDifferenceImage ( ImageMap_cl &diffImgMap );
  
  ImageMap_cl *m_pSourceBMP;
  ImageMap_cl *m_pTargetBMP;
};

#endif // !defined(AFX_VIMAGECOMPARISON_HPP__027D583A_5E25_4ABD_B250_04DBD39EA8CF__INCLUDED_)

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
