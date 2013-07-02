/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// VArabicSupport_cl Source
//***********************************************************************

#include <Vision/Samples/Engine/Unicode/UnicodePCH.h>
#include <Vision/Samples/Engine/Unicode/VArabicConverter.hpp>
#include <Vision/Samples/Engine/Unicode/VArrayHelper.hpp>

//setup mapping
//-------------
//1==contextual representation has connection to the following character
//0==contextual representation has no connection to the following character (so it's isolated as start charcter)
//-1==gap in the unicode table of the contextual representation
const char VArabicSupport_cl::s_pArabicLetterMap[41] = 
{
  0, //'alif madda 0x0622 (maps to 0xFE82)
  0, 0, 0, 1, //unnamed characters 0x0623-0x0626
  0, //'alif
  1, //ba'
  0, //ta' marbuta
  1, //ta'
  1, //tha'
  1, //gjim
  1, //cha'
  1, //chra'
  0, //dal
  0, //dhal
  0, //ra
  0, //zay
  1, //sin
  1, //schin
  1, //sad
  1, //dad
  1, //tach
  1, //zach
  1, //'ayn
  1, //rayn
  (char)-1, (char)-1, (char)-1, (char)-1, (char)-1, (char)-1, //gap 0x63B-0x640
  1, //fa'
  1, //qaf
  1, //kaf
  1, //lam
  1, //mim
  1, //nun
  1, //ha
  0, //waw
  0, //'alif maqsura
  1  //ya
};

VRTLSupport_cl::VRTLSupport_cl(VisFont_cl * pFont, const wchar_t *pText) : m_pFont(pFont), m_ppTextLines(NULL), m_iTextLines(0)  {

  VASSERT(pFont != NULL);

  //set the text to display (if present)
  if(pText != NULL)
    SetText(pText);
}

VRTLSupport_cl::VRTLSupport_cl(VisFont_cl * pFont, const VString & utf8Text) : m_pFont(pFont), m_ppTextLines(NULL), m_iTextLines(0) {

  VASSERT(pFont != NULL);

  //set the text to display (if present)
  if(!utf8Text.IsEmpty())
    SetText(utf8Text);
}

VRTLSupport_cl::~VRTLSupport_cl() {
  //delete text block an line pointers
  if(m_ppTextLines!=NULL) 
  {
    for(int i=0;i<m_iTextLines;i++) 
    {
      V_SAFE_FREE(m_ppTextLines[i]);
    }
  }
  V_SAFE_FREE(m_ppTextLines);
}

int VRTLSupport_cl::DistanceToRightAlignment(VisFont_cl * pFont, const char * pText) {

  VASSERT(pFont);

  VRectanglef requiredSpace;
  pFont->GetTextDimension(pText, requiredSpace);

  return (int)-requiredSpace.GetSizeX();
}

void VRTLSupport_cl::PrintText(IVRender2DInterface *pRI, hkvVec2 vPos, VColorRef iColor ,
                                  VSimpleRenderState_t &iState, float fScaling, VRectanglef *pClipRect, float fZCoord) const
{
  //render line per line in order to adjust the x-value for RTL font
  for(int i=0;i<m_iTextLines;i++) 
  {
    //skip empty lines
    if(m_ppTextLines[i]!=NULL)
    {
      m_pFont->PrintText(pRI, hkvVec2(vPos.x+DistanceToRightAlignment(m_pFont, m_ppTextLines[i]), vPos.y),
        m_ppTextLines[i], iColor, iState, fScaling, pClipRect, fZCoord);
    }

    //inc value for next line
    vPos.y += m_pFont->GetFontHeight();
  }
}

void VRTLSupport_cl::SetText(const VString & utf8String)
{
  // Convert to wide char as RTL support helper requires wide char strings.
  int iBufSize = utf8String.GetLen() + 1;
  wchar_t *pWCharBuf = new wchar_t[iBufSize];
  V_UTF8_TO_WCHAR(utf8String.AsChar(), pWCharBuf, iBufSize);

  SetText(pWCharBuf);

  V_SAFE_DELETE(pWCharBuf);
}

void VRTLSupport_cl::SetText(const wchar_t *pText)
{
  VASSERT(pText!=NULL);

  //clear text vars for multiple SetText calls
  if(m_ppTextLines!=NULL) {
    for(int i=0;i<m_iTextLines;i++) {
      V_SAFE_FREE(m_ppTextLines[i]);
    }
  }
  V_SAFE_FREE(m_ppTextLines);

  //create a tokenizer in order to separate the text lines
  VArrayHelper_cl<wchar_t> tokenizer(pText, L"\n");

  //create line pointers
  m_iTextLines = tokenizer.CalcNumberOfTokens();
  m_ppTextLines = (char **)vMemAlloc(m_iTextLines * sizeof(char *));

  int iLineIndex=0;

  int iWLen, iSize;

  while( tokenizer.HasNextToken() ) {
    
    //get the current text line
    const wchar_t *pCurrentLine = tokenizer.NextToken();

    //length of the current line
    int iLength = VArrayHelper_cl<wchar_t>::Length(pCurrentLine);

    //allocate the target buffer (original length + terminator)
    wchar_t *pReversed = (wchar_t *) vMemAlloc(sizeof(wchar_t)*(iLength+1));

    //pre-terminate buffer
    pReversed[iLength] = 0;

    //transform LTR to RTL
    for(int i=0;i<iLength;i++) 
    {
      pReversed[iLength-i-1] = pCurrentLine[i];
    }

    // Convert to UTF8 and assign the reversed line
    iWLen = (int)wcslen(pReversed);
    iSize = VString::ConvertWCharToUTF8String(pReversed, iWLen, NULL, 0);
    m_ppTextLines[iLineIndex] = new char[iSize+1];
    VString::ConvertWCharToUTF8String(pReversed, iWLen, m_ppTextLines[iLineIndex], iSize);
    m_ppTextLines[iLineIndex][iSize] = '\0';

    V_SAFE_DELETE(pReversed);
    iLineIndex++;
  }
}




VArabicSupport_cl::VArabicSupport_cl(VisFont_cl * pArabicFont,
                                     const wchar_t *pArabicText,
                                     bool isContextualArabic,
                                     bool duplicateSpacesDuringConversion) : VRTLSupport_cl()  {

  VASSERT(pArabicFont != NULL);
  m_pFont = pArabicFont;

  //set the text to display (if present)
  if(pArabicText != NULL)
    SetText(pArabicText, isContextualArabic, duplicateSpacesDuringConversion);
}

VArabicSupport_cl::VArabicSupport_cl(VisFont_cl * pArabicFont,
                                     const VString & utf8ArabicText,
                                     bool isContextualArabic,
                                     bool duplicateSpacesDuringConversion) : VRTLSupport_cl()  {

   VASSERT(pArabicFont != NULL);
   m_pFont = pArabicFont;

   //set the text to display (if present)
   if(!utf8ArabicText.IsEmpty())
     SetText(utf8ArabicText, isContextualArabic, duplicateSpacesDuringConversion);
}

VRectanglef VArabicSupport_cl::GetCharacterDimension( wchar_t wcChar, VisFont_cl * pFont )
{
  VASSERT_MSG(pFont!=NULL, "Specified font is NULL");

  VRectanglef requiredSpace(0,0,0,0);

  // Convert to UTF8
  const wchar_t singleCharacter[2] = {wcChar, 0};
  char utf8Representation[8];

  int iSize = VString::ConvertWCharToUTF8String(singleCharacter, 1, NULL, 0);
  VString::ConvertWCharToUTF8String(singleCharacter, 1, utf8Representation, iSize);
  utf8Representation[iSize] = '\0';

  //check if the end-representation is present
  if(pFont!=NULL) pFont->GetTextDimension(utf8Representation,requiredSpace,1);

  return requiredSpace;
}

wchar_t* VArabicSupport_cl::UniformToContextualLine(VisFont_cl * pArabicFont, const wchar_t * pUniformArabic, bool duplicateSpaces) {

  if(pUniformArabic==NULL) return NULL;

  //length of the current line
	int iLength = VArrayHelper_cl<wchar_t>::Length(pUniformArabic);

  //count spaces
  int iNumSpaces = 0;
  if(duplicateSpaces) 
  {
    for(int i=0;i<iLength;i++) 
    {
      if(pUniformArabic[i]==L' ') iNumSpaces++;
    }
  }

  //allocate the target buffer (original length + number of duplicates space + terminator)
  wchar_t *pContextualArabic = (wchar_t *) vMemAlloc(sizeof(wchar_t)*(iLength+iNumSpaces+1));

  //pre-terminate buffer
  pContextualArabic[iLength+iNumSpaces] = 0;

  wchar_t wcCurLetter = pUniformArabic[0];	//start with the first letter
  wchar_t wcPrevLetter = 0;					//init wcPrevLetter, because the assignment is done at the end of the for loop

  //is the current (first) letter an Arabic letter?
  bool bCurIsArabic = isUniformArabic(pUniformArabic[0]);

  bool bPrevIsArabic = false;	//at the beginning the previous letter is never Arabic
  bool bNextIsArabic;			//calculated inside the loop

  //transform and terminate the target buffer
  for(int i=0;i<iLength;i++) 
  {

    //start at the right side with the first character at index 0 (because Arabic is RTL)
    wcCurLetter = pUniformArabic[i];
    bNextIsArabic = i<(iLength-1) ? isUniformArabic(pUniformArabic[i+1]) : false;

    if(bCurIsArabic) 
    {
      //first uniform letter is 'alif madda: 0x0622 (maps to contextual representation 0xFE81)

      int iIndex = wcCurLetter-0x0622;	//number of iterations
      wchar_t wcContextual = 0xFE81;		//contextual representation of 'alif madda

      //calculate contextual representation of current uniform representation
      //(count up transformation steps) 
      for(int j=0;j<iIndex;j++) {
        if(s_pArabicLetterMap[j]==0) wcContextual +=2;
        else if(s_pArabicLetterMap[j]==1) wcContextual +=4;
        //else skip (-1)
      }

      //now adjust character according to the in-word position

      wchar_t wcCandidate = wcContextual; //we experiment with a candidate character since it's not sure that the candidate exists!

      //end character
      if( bPrevIsArabic && isInnerLetter(wcPrevLetter) && (!bNextIsArabic || isEndLetter(wcCurLetter)) ) {
        wcCandidate+=1;

        //start character
      } else if( bNextIsArabic && (!bPrevIsArabic || isEndLetter(wcPrevLetter)) ) {
        if(s_pArabicLetterMap[iIndex]!=0)
          wcCandidate+=2;

        //inner character
      } else if( i>0 && bPrevIsArabic && bNextIsArabic && isInnerLetter(wcPrevLetter) && isInnerLetter(wcCurLetter) ) {
        if(s_pArabicLetterMap[iIndex]!=0)
          wcCandidate+=3;
      }
      //else isolated (default)

      //candidate check (do we have this character in the font..?)
      VRectanglef requiredSpace = GetCharacterDimension(wcCandidate, pArabicFont);
      if(requiredSpace.GetSizeX()<=0)
      {
        wcCandidate = wcContextual; //restore from backup (because the character is missing)

        //just in case: fall-back to non-contextual
        VRectanglef requiredSpace = GetCharacterDimension(wcCandidate, pArabicFont);
        if(requiredSpace.GetSizeX()<=0)	wcCandidate = wcCurLetter;
      }

      #ifdef HK_DEBUG_SLOW
        requiredSpace = GetCharacterDimension(wcCandidate, pArabicFont);
        VASSERT_MSG(requiredSpace.GetSizeX()>0, "Your text contains a character which is not present in your font!")
      #endif

      //candidate approved:
      pContextualArabic[iLength+iNumSpaces-i-1] = wcCandidate;

    } else {
      //invert directional symbols
      switch(wcCurLetter) {
        case L'(': wcCurLetter = L')'; break;
        case L')': wcCurLetter = L'('; break;
        case L'<': wcCurLetter = L'>'; break;
        case L'>': wcCurLetter = L'<'; break;
        case L'{': wcCurLetter = L'}'; break;
        case L'}': wcCurLetter = L'{'; break;
        case L'[': wcCurLetter = L']'; break;
        case L']': wcCurLetter = L'['; break;
        case L'/': wcCurLetter = L'\\'; break;
        case L'\\': wcCurLetter = L'/'; break;
        case L' ':
          if(duplicateSpaces) {
            pContextualArabic[iLength+iNumSpaces-i-1] = L' ';
            iNumSpaces--;
          }
          break;
        default: break;
      }
      pContextualArabic[iLength+iNumSpaces-i-1] = wcCurLetter;
    }

    //prepare for next loop iteration
    wcPrevLetter = wcCurLetter;

    bPrevIsArabic = bCurIsArabic; 
    bCurIsArabic = bNextIsArabic;

  }

  return pContextualArabic;
}

void VArabicSupport_cl::SetText(const VString & utf8ArabicText, bool isContextualArabic, bool duplicateSpacesDuringConversion)
{
  // Convert to wide char as Arabic support helper requires wide char strings.
  int iBufSize = utf8ArabicText.GetLen() + 1;
  wchar_t *pWCharBuf = new wchar_t[iBufSize];
  V_UTF8_TO_WCHAR(utf8ArabicText.AsChar(), pWCharBuf, iBufSize);

  SetText(pWCharBuf, isContextualArabic, duplicateSpacesDuringConversion);

  V_SAFE_DELETE(pWCharBuf);
}

void VArabicSupport_cl::SetText(const wchar_t *pArabicText, bool isContextualArabic, bool duplicateSpacesDuringConversion)
{

  VASSERT(pArabicText!=NULL);

  //clear text vars for multiple SetText calls
  if(m_ppTextLines!=NULL) {
    for(int i=0;i<m_iTextLines;i++) {
      V_SAFE_FREE(m_ppTextLines[i]);
    }
  }
  V_SAFE_FREE(m_ppTextLines);

  //create a tokenizer in order to separate the text lines
  VArrayHelper_cl<wchar_t> tokenizer(pArabicText, L"\n");

  //create line pointers
  m_iTextLines = tokenizer.CalcNumberOfTokens();
  m_ppTextLines = (char **)vMemAlloc(m_iTextLines * sizeof(char *));

  int iWLen, iSize;

  int iLineIndex=0;
  while( tokenizer.HasNextToken() ) {

    wchar_t *pConverted = NULL;
    if(isContextualArabic) 
    {
      //duplicate line
      pConverted = VArrayHelper_cl<wchar_t>::Copy(tokenizer.NextToken());
    } 
    else 
    {
      //convert to contextual
      pConverted = UniformToContextualLine(m_pFont, tokenizer.NextToken(), duplicateSpacesDuringConversion);
    }

    // Convert to UTF8 and assign the reversed line
    iWLen = (int)wcslen(pConverted);
    iSize = VString::ConvertWCharToUTF8String(pConverted, iWLen, NULL, 0);
    m_ppTextLines[iLineIndex] = new char[iSize+1];
    VString::ConvertWCharToUTF8String(pConverted, iWLen, m_ppTextLines[iLineIndex], iSize);
    m_ppTextLines[iLineIndex][iSize] = '\0';

    V_SAFE_DELETE(pConverted);

    iLineIndex++;
  }
}

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
