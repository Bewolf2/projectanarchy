/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// VArabicSupport_cl and VRTLSupport_cl Class
//***********************************************************************

// you must first include the vision.hpp file to use this header
#ifndef _VArabicSupport_cl_HEADER_
#define _VArabicSupport_cl_HEADER_

/// \brief
///   Container for Right-To-Left character rendering (RTL).
///
///  At the rendering process (Method PrintText) VRTLSupport_cl draws the assigned text (or text block) from right to left.
class VRTLSupport_cl
{
public:

  ///
  /// @name Static Methods
  /// @{
  ///


  /// \brief
  ///   Gets the distance difference from left to right alignment.
  ///
  /// Calculates the required space an negates the value (result can be added to the text origin at the right top).
  ///
  /// \param pFont
  ///   The font, which has to be present.
  /// \param pText
  ///   The string, as base of the distance calculation.
  ///
  /// \return
  ///   The horizontal space requirement.
  static int DistanceToRightAlignment(VisFont_cl * pFont, const char * pText);

  ///
  /// @}
  ///


  ///
  /// @name Non-static method for initialized VRTLSupport_cl instances
  /// @{
  ///

  /// \brief
  ///   Constructor. Creates a new VRTLSupport_cl instance.
  ///
  /// Calculates the required space an negates the value (result can be added to the text origin at the right top).
  ///
  /// \param pFont
  ///   The font, used for text rendering.
  /// \param pText
  ///   The text to be displayed (Later assignment with SetText possible, multiple lines valid).
  VRTLSupport_cl(VisFont_cl * pFont, const wchar_t *pText = NULL);

  VRTLSupport_cl(VisFont_cl * pFont, const VString & utf8Text);

  /// \brief
  ///   Destructor. Releases internal resources (wchar_t buffers created on conversion).
  virtual ~VRTLSupport_cl();

  /// \brief
  ///   Prints the text of this instance on the screen.
  ///
  /// Renders each line from right to left (RTL) starting at the right top.
  ///
  /// \param pRI
  ///   Target interface. Can be NULL to batch the text and render it later (automatically).
  /// \param vPos
  ///   Text position on screen (right top!).
  /// \param iColor
  ///   The text color (transparent texts can cause artifacts if the symbols of the font is not that accurate).
  /// \param iState
  ///   Text render state (e.g. transparency mode), can be left to VisFont_cl::DEFAULT_STATE.
  /// \param fScaling
  ///   Optional scaling of the text.
  /// \param pClipRect
  ///   Optional clipping rectangle for software clipping. This function only clips away characters.
  /// \param fZCoord
  ///   Optional z-coordinate for sorting into z-buffer.
  void PrintText(IVRender2DInterface *pRI, hkvVec2 vPos, VColorRef iColor , VSimpleRenderState_t &iState = VisFont_cl::DEFAULT_STATE, float fScaling = 1.0f, VRectanglef *pClipRect = 0, float fZCoord = -1.0f) const;

  /// \brief
  ///   Sets the text to be displayed.
  ///
  /// The text which is getting displayed on the PrintText call.
  ///
  /// \param pText
  ///   The text to be displayed (multiple lines valid).
  virtual void SetText(const wchar_t *pText);

  /// \brief
  ///   Sets the text to be displayed.
  ///
  /// The text which is getting displayed on the PrintText call.
  ///
  /// \param utf8String
  ///   The text to be displayed (multiple lines valid).
  virtual void SetText(const VString & utf8String);

  ///
  /// @}
  ///

protected:

  VRTLSupport_cl() :m_pFont(NULL), m_ppTextLines(NULL), m_iTextLines(0) { }


  // Members
  VisFont_cl * m_pFont;          ///< Font used for rendering
  char ** m_ppTextLines;         ///< Line pointers of the assigned text
  int m_iTextLines;              ///< Number of text lines

};


/// \brief
///   Container for Arabic strings, supporting proper rendering of contextual representation from right to left inheriting from VRTLSupport_cl.
///
/// Every unicode file containing Arabic text, stores the text as uniform Arabic (which means that every single letter is isolated).
//  The VArabicSupport_cl class provides a conversion for you to the contextual representation and keeps track of the specified font:  
//  Some fonts have no extra end representation of the characters, because they look similar to the isolated representation. If this 
//  is the case the VArabicSupport_cl adjusts the character for you (from end to isolated representation). Resulting in a correct view 
//  on the screen.
//  At the rendering process (Method PrintText) VArabicSupport_cl draws the assigned text (or text block) from right to left.
class VArabicSupport_cl: public VRTLSupport_cl
{
public:

  ///
  /// @name Static Methods
  /// @{
  ///

  /// \brief
  ///   Converts a uniform Arabic string a contextual Arabic string (Just a single line, otherwise the lines appear upside down).
  ///
  /// Allocates a new wchar_t buffer containing the proper converted Arabic string.
  ///
  /// \param pArabicFont
  ///   The Arabic font, which has to contain the contextual representation. NULL invokes a conversion, regardless the font specification.
  /// \param pUniformArabicText
  ///   The Arabic string, which should be converted.
  /// \param duplicateSpaces
  ///   Duplicates spaces if set to true (Some fonts appear more readable if the white space is expanded, default is false).
  ///
  /// \return
  ///   pContextualArabic: The contextual Arabic string.
  static wchar_t* UniformToContextualLine(VisFont_cl * pArabicFont, const wchar_t * pUniformArabicText, bool duplicateSpaces=false);

  /// \brief
  ///  Calculates the required space of a specified character
  /// \param wcChar
  ///   The character used to calculate the space requirement
  /// \param pFont
  ///   The font used for measurement
  /// \return
  ///   A rectangle telling you the required space for the character. Will be 0,0 if the character is not contained in the specified font!
  static VRectanglef GetCharacterDimension( wchar_t wcChar, VisFont_cl * pFont );

  ///
  /// @}
  ///


  ///
  /// @name Non-static method for initialized VArabicSupport_cl instances
  /// @{
  ///

  /// \brief
  ///   Constructor. Creates a new VArabicSupport_cl instance - An instance handles the conversion from uniform to contextual Arabic and can perform the proper rendering of the assigned text.
  ///
  /// Calculates the required space an negates the value (result can be added to the text origin at the right top).
  ///
  /// \param pArabicFont
  ///   The Arabic font, used for text rendering.
  /// \param pArabicText
  ///   The text to be displayed (Later assignment with SetText possible, multiple lines valid).
  /// \param isContextualArabic
  ///   Specify the type of string (A uniform representation forces a conversion to contextual).
  /// \param duplicateSpacesDuringConversion
  ///   Duplicates spaces if set to true and a conversion is performed (Some fonts appear more readable if the white space is expanded, default is false).
  VArabicSupport_cl(VisFont_cl * pArabicFont, const wchar_t *pArabicText = NULL, bool isContextualArabic = false, bool duplicateSpacesDuringConversion = false);

  VArabicSupport_cl(VisFont_cl * pArabicFont, const VString &utf8ArabicText, bool isContextualArabic = false, bool duplicateSpacesDuringConversion = false);

  /// \brief
  ///   Sets the text to be displayed.
  ///
  /// The text which is getting displayed on the PrintText call.
  ///
  /// \param pArabicText
  ///   The text to be displayed (multiple lines valid).
  /// \param isContextualArabic
  ///   Specify the type of string (A uniform representation forces a conversion to contextual).
  /// \param duplicateSpacesDuringConversion
  ///   Duplicates spaces if set to true and a conversion is performed (Some fonts appear more readable if the white space is expanded, default is false).
  void SetText(const wchar_t *pArabicText, bool isContextualArabic = false, bool duplicateSpacesDuringConversion = false);

  /// \brief
  ///   Sets the text to be displayed.
  ///
  /// The text which is getting displayed on the PrintText call.
  ///
  /// \param utf8ArabicText
  ///   The text to be displayed (multiple lines valid).
  /// \param isContextualArabic
  ///   Specify the type of string (A uniform representation forces a conversion to contextual).
  /// \param duplicateSpacesDuringConversion
  ///   Duplicates spaces if set to true and a conversion is performed (Some fonts appear more readable if the white space is expanded, default is false).
  void SetText(const VString & utf8ArabicText, bool isContextualArabic = false, bool duplicateSpacesDuringConversion = false);

  ///
  /// @}
  ///

private:

  // Static class vars  
  static const char s_pArabicLetterMap[41];   ///< Map contains information which letter connects to the following letter (in the contextual representation)

  // Static helper methods

  /// \brief
  ///   Checks weather a single symbol is uniform or not
  static inline bool isUniformArabic(wchar_t letter) { return (letter >= 0x0622) && (letter <= 0x064A); }

  /// \brief
  ///   Checks whether a single letter is an inner letter (contextual)
  static inline bool isInnerLetter(wchar_t letter) { return s_pArabicLetterMap[letter-0x0622]!=0; }

  /// \brief
  ///   Check whether a single letter is an end letter (contextual)
  static inline bool isEndLetter(wchar_t letter) { return s_pArabicLetterMap[letter-0x0622]==0; }
};

#endif //_VArabicSupport_cl_HEADER_

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
