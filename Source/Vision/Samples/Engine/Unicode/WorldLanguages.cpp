/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// WorldLanguages_cl Source
//***********************************************************************

#include <Vision/Samples/Engine/Unicode/UnicodePCH.h>
#include <Vision/Samples/Engine/Unicode/WorldLanguages.hpp>
#include <Vision/Samples/Engine/Unicode/VArabicConverter.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


// ReadPlacesFile: Reads in the PLACES_FILE and sets up all the WorldPlace_t
//                 objects.
void WorldLanguages_cl::ReadPlacesFile()
{
  IVFileInStream* pIn = Vision::File.Open(PLACES_FILE);
  char *buffer = NULL;
  
  if (pIn)
  {
    int fsize= pIn->GetSize();
    
    buffer = new char[fsize+1];
    pIn->Read(buffer, fsize);
    buffer[fsize] = 0;
    
    pIn->Close();
  }
  // if we couldn't read anything, return here!
  if (!buffer)
    return;

  static const char Newline[] = "\r\n";
  // tokenize buffer line-wise
  char *line = strtok(buffer, Newline);
  
  // skip first character, it only marks the UTF8 file (Byte Order Mark)
  line += 3;
  // read number of places
  sscanf(line, "%d", &m_iNumPlaces);
  // no places -> return without doing anything
  if (m_iNumPlaces < 1)
  {
    V_SAFE_DELETE_ARRAY(buffer);
    return;
  }
  // create m_iNumPlaces WorldPlace_t structs
  m_pPlaces = new WorldPlace_t[m_iNumPlaces];
  m_iCurrentPlace = 0;

  // get all places
  while (m_iCurrentPlace < m_iNumPlaces)
  {
    WorldPlace_t &pThisPlace = m_pPlaces[m_iCurrentPlace];
    // get next line
    line = strtok(NULL, Newline);
    if (line == NULL)
    {
      V_SAFE_DELETE_ARRAY(buffer);
      return;
    }

    // skip comments
    if (line[0] == ';')
      continue;
    // skip empty lines
    if (line[0] == '\0')
      continue;
    // find [
    if (line[0] == '[')
    {
      // the next line contains the name of the place
      line = strtok(NULL, Newline);
      pThisPlace.m_PlaceName = line;

      //is it Arabic?
      bool bArabic = strcmp(line, "UAE - Arabic") == 0;

      // the next line contains the font type
      line = strtok(NULL, Newline);

      // load font type with resource manager
      pThisPlace.m_spFont = Vision::Fonts.LoadFont(line);

      //left to right or right to left?
      line = strtok(line + strlen(line) + 1, Newline); // LoadFont() seems to use strtok too -> reinitialize
      bool bIsRightToLeft = strcmp(line,"RTL") == 0;

      // the next line contains the alphabet index, x position and y position
      // read alphabet, x_pos, y_pos;
      line = strtok(NULL, Newline);
      float x,y;
      sscanf(line, "%f,%f", &x, &y);
      
      // fix up angles (convert from long/lat to yaw/pitch)
      pThisPlace.m_angleY = -x;
      pThisPlace.m_angleZ = -y-90;
      // the next line(s) contain(s) the text
      // read text
      line = strtok(NULL, Newline);
      pThisPlace.m_Text.Reset();
      // until text is ]
      while (line[0] != ']')
      {
        pThisPlace.m_Text += line;
        pThisPlace.m_Text += "\n";
        line = strtok(NULL, Newline);
      }

      // Init Arabic support helper and set text which gets transformed from uniform to contextual Arabic
      if(bArabic)
      {
        pThisPlace.m_pRTLSupport = new VArabicSupport_cl(pThisPlace.m_spFont, pThisPlace.m_Text);
      }
      else if(bIsRightToLeft)
      {
        pThisPlace.m_pRTLSupport = new VRTLSupport_cl(pThisPlace.m_spFont, pThisPlace.m_Text);
      }

    }

    m_iCurrentPlace++;
  }
  V_SAFE_DELETE_ARRAY(buffer);

}


// SetupMasks: Loads screen masks for the alphabets and the curly brace.

void WorldLanguages_cl::SetupMasks()
{
  // Load Curly Brace
  m_pBrace = new VisScreenMask_cl("curlybrace.dds");
  float xPos = (Vision::Video.GetXRes() *   5) / 10.f;
  float yPos = (Vision::Video.GetYRes() - 128) /  2.f;
  m_pBrace->SetTransparency(VIS_TRANSP_ALPHA);
  m_pBrace->SetPos(xPos, yPos);
  m_pBrace->SetOrder(20);
  m_pBrace->SetVisible(FALSE);

  // Load Hot Spot
  m_pHotSpot = new VisScreenMask_cl("hotspot.dds");
  m_pHotSpot->SetTransparency(VIS_TRANSP_ALPHA);
  m_pHotSpot->SetOrder(0);
  m_pHotSpot->SetVisible(FALSE);
}

  
// SwitchPlace: Sets the target angles and updates the text lines
//              for the new place.
void WorldLanguages_cl::SwitchPlace(int iNewIndex)
{
  WorldPlace_t *pNewPlace = &m_pPlaces[iNewIndex];
//  WorldPlace_t *pOldPlace = NULL;
//  if (m_iOldPlace>=0) 
//    pOldPlace = &m_pPlaces[m_iOldPlace];
  m_iOldPlace = iNewIndex;

  // set new rotation target
  m_fTargetAngle[0] = pNewPlace->m_angleY;
  m_fTargetAngle[1] = pNewPlace->m_angleZ;
}


// InitFunction: Reads places file and sets up initial screen masks,
//               including the used fonts.
void WorldLanguages_cl::InitFunction()
{
  // load one global font
  spGlobalFont = Vision::Fonts.LoadFont("\\Fonts\\Arial_22.fnt");

  // initialize miscellaneous variables
  m_pBrace = NULL;
  m_pPlaces = NULL;
  m_pHotSpot = NULL;
  m_iNumPlaces = 0;

  // read places file and set up places array
  ReadPlacesFile();
  
  // set up position data for the first place
  m_iCurrentPlace = Vision::Game.GetRand()%m_iNumPlaces;
  m_iOldPlace = -1;

  // find the earth globe
  m_pTheWorld = Vision::Game.SearchEntity("theWorld");
  
  // set up screen masks
  SetupMasks();
  
  // set up some keys
#ifdef SUPPORTS_KEYBOARD
  VisSampleApp::GetInputMap()->MapTrigger(WORLD_LANG_PREV, V_KEYBOARD, CT_KB_BACKSP, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(WORLD_LANG_NEXT, V_KEYBOARD, CT_KB_SPACE,  VInputOptions::Once());
#endif

#if defined(SUPPORTS_MULTITOUCH)
  VTouchArea *pTouchArea
#if defined(WIN32) && !defined(_VISION_WINRT)
    = new VTouchArea(VInputManager::GetMultitouch(),  VRectanglef(0,0,(float)Vision::Video.GetXRes(),(float)Vision::Video.GetYRes()));
#else
    = new VTouchArea(VInputManager::GetTouchScreen());
#endif

  VisSampleApp::GetInputMap()->MapTrigger(WORLD_LANG_NEXT, pTouchArea, CT_TOUCH_ANY, VInputOptions::Once());
#endif

#if defined(_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  VisSampleApp::GetInputMap()->MapTrigger(WORLD_LANG_PREV, V_XENON_PAD(0), CT_PAD_A, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(WORLD_LANG_NEXT, V_XENON_PAD(0), CT_PAD_B, VInputOptions::Once());

#elif defined(_VISION_PS3)
  VisSampleApp::GetInputMap()->MapTrigger(WORLD_LANG_PREV, V_PS3_PAD(0), CT_PAD_CROSS, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(WORLD_LANG_NEXT, V_PS3_PAD(0), CT_PAD_CIRCLE, VInputOptions::Once());

#elif defined(_VISION_PSP2)
  VisSampleApp::GetInputMap()->MapTrigger(WORLD_LANG_PREV, V_PSP2_PAD(0), CT_PAD_CROSS, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(WORLD_LANG_NEXT, V_PSP2_PAD(0), CT_PAD_CIRCLE, VInputOptions::Once());

#elif defined(_VISION_MOBILE)
  int width = Vision::Video.GetXRes();
  int height = Vision::Video.GetYRes();
  
  VTouchArea* prevArea = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(0, 0, 100, height), -950.0f);
  VisSampleApp::GetInputMap()->MapTrigger(WORLD_LANG_PREV, prevArea, CT_TOUCH_ANY, VInputOptions::Once());    
  
  VTouchArea* nextArea = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(width - 100, 0, width, height), -950.0f);
  VisSampleApp::GetInputMap()->MapTrigger(WORLD_LANG_NEXT, nextArea, CT_TOUCH_ANY, VInputOptions::Once());

#elif defined(_VISION_WIIU)
  VisSampleApp::GetInputMap()->MapTrigger(WORLD_LANG_PREV, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_B, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(WORLD_LANG_NEXT, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_A, VInputOptions::Once());
  
#endif


  m_fCurrentAngle[0] = 0.0f;
  m_fCurrentAngle[1] = 0.0f;

  SwitchPlace(m_iCurrentPlace);
}


// DeInitFunction: Cleans up all the resources this class used.
void WorldLanguages_cl::DeInitFunction()
{
  if (m_pPlaces)
    delete[] m_pPlaces;

  spGlobalFont = NULL;
}


// ShowOverlays: Shows or hides the text overlays.
void WorldLanguages_cl::ShowOverlays(VBool bShow)
{
  // calculate the hotspot position in screen space:
  float globeRad = 90;
  
  // get the world entity position and move towards the camera
  hkvVec3 wpos = m_pTheWorld->GetPosition();
  wpos.x -= globeRad;
  
  // transform 3d to 2d
  float screenX,screenY;
  Vision::Contexts.GetMainRenderContext()->Project2D(wpos, screenX, screenY);

  int sizeX, sizeY;
  m_pHotSpot->GetTextureSize(sizeX, sizeY);

  // center screenmask on hotspot (and tweak a bit :-):
  screenX-=0.2f*sizeX;
  screenY-=0.5f*sizeY;

  m_pHotSpot->SetPos(screenX,screenY);
  m_pHotSpot->SetVisible(bShow);

  // show/hide curly brace
  m_pBrace->SetVisible(bShow);
  
  // show/hide text
  // set up initial x and y position
  float xPos, yPos;
  m_pBrace->GetPos(xPos, yPos);
  xPos += 56.f;
  yPos -= 16.f;

  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_ALWAYSVISIBLE);

  // print place name
  if (bShow) 
  {
    spGlobalFont->PrintText(NULL, hkvVec2(xPos, yPos), m_pPlaces[m_iCurrentPlace].m_PlaceName.AsChar(), VColorRef(160,160,160,255), state);
 
    if(m_pPlaces[m_iCurrentPlace].m_pRTLSupport!=NULL)
    { 
       //print "left to right" text
       m_pPlaces[m_iCurrentPlace].m_pRTLSupport->PrintText(NULL, hkvVec2(xPos+300, yPos + 24), VColorRef(160,160,160,255), state);
    }
    else
    {
      // print place text
      m_pPlaces[m_iCurrentPlace].m_spFont->PrintText(NULL, hkvVec2(xPos, yPos + 28), m_pPlaces[m_iCurrentPlace].m_Text.AsChar(), VColorRef(160,160,160,255), state);
    }
   }
}


// ThinkFunction: Updates globe rotation and text display, as well as checking
//                the input.
void WorldLanguages_cl::ThinkFunction()
{
  float fDeltaY = m_fTargetAngle[0] - m_fCurrentAngle[0];
  float fDeltaZ = m_fTargetAngle[1] - m_fCurrentAngle[1];

  VSimpleRenderState_t state = VisFont_cl::DEFAULT_STATE;

  // if distance is smaller than an arbitrary threshold, destination is reached
  // (since rotation speed decreases exponentially, destination will never be reached)
  if ( ((fDeltaY * fDeltaY) + (fDeltaZ * fDeltaZ)) < 0.5f ) 
  {
    // if we reached the target angles, show the text
    ShowOverlays(TRUE);

    float x = Vision::Video.GetXRes()-300.f;
    float y = Vision::Video.GetYRes()-60.f;

    // display current rotation angles
    static float fTempScale = 1.0f;
    VString currentAngle;
    float currentLon = -m_fCurrentAngle[1]-90;
    char currentLonC = 'E';
    if(currentLon<0)
    {
      currentLon = -currentLon;
      currentLonC = 'W';
    }
    currentAngle.Format("Current  N %02.3f, %c %03.3f", -m_fCurrentAngle[0], currentLonC, currentLon);
    spGlobalFont->PrintText(NULL, hkvVec2(8.f, Vision::Video.GetYRes() - 40.f), currentAngle, VColorRef(255,255,255,160), state, fTempScale);
      
    // display target rotation angles
    VString targetAngle;
    float targetLon = -m_fTargetAngle[1]-90;
    char targetLonC = 'E';
    if(targetLon<0)
    {
      targetLon = -targetLon;
      targetLonC = 'W';
    }
    targetAngle.Format("Target N %02.3f, %c %03.3f", -m_fTargetAngle[0], targetLonC, targetLon); 
    spGlobalFont->PrintText(NULL, hkvVec2(8.f, Vision::Video.GetYRes() - 80.f), targetAngle, VColorRef(255,255,255,160), state, fTempScale);

    // "Destination reached" text
    spGlobalFont->PrintText(NULL, hkvVec2(x, y), "Destination reached!", VColorRef(255,255,255,160), state);
  }
  else
  {
    // rotate globe towards the target angles
    float fDiff = Vision::GetTimer()->GetTimeDifference();

    // no linear speed, but a fraction of the distance to go
    float distFraction = 1.f - powf(0.03f,fDiff); // framerate independent fraction
    m_fCurrentAngle[0] += distFraction*fDeltaY;
    m_fCurrentAngle[1] += distFraction*fDeltaZ;
    
    // set globe's rotation
    hkvMat3 mat1, mat2;

    mat2.setRotationMatrixY(m_fCurrentAngle[0]);
    mat1.setRotationMatrixZ(m_fCurrentAngle[1]);
    mat1 = mat1.multiplyReverse(mat2);
    m_pTheWorld->SetRotationMatrix(mat1);

    
    // display current rotation angles
    static float fTempScale = 1.0f;
    VString currentAngle;
    float currentLon = -m_fCurrentAngle[1]-90;
    char currentLonC = 'E';
    if(currentLon<0)
    {
      currentLon = -currentLon;
      currentLonC = 'W';
    }
    currentAngle.Format("Current  N %02.3f, %c %03.3f", -m_fCurrentAngle[0], currentLonC, currentLon);
    spGlobalFont->PrintText(NULL, hkvVec2(8.f, Vision::Video.GetYRes() - 40.f), currentAngle, VColorRef(255,255,255,160), state, fTempScale);
      
    // display target rotation angles
    VString targetAngle;
    float targetLon = -m_fTargetAngle[1]-90;
    char targetLonC = 'E';
    if(targetLon<0)
    {
      targetLon = -targetLon;
      targetLonC = 'W';
    }
    targetAngle.Format("Target N %02.3f, %c %03.3f", -m_fTargetAngle[0], targetLonC, targetLon); 
    spGlobalFont->PrintText(NULL, hkvVec2(8.f, Vision::Video.GetYRes() - 80.f), targetAngle, VColorRef(255,255,255,160), state, fTempScale);
    
    // "Searching..." text
    spGlobalFont->PrintText(NULL, hkvVec2(Vision::Video.GetXRes()-300.f, Vision::Video.GetYRes() - 60.f), "Searching...", VColorRef(255,255,255,160), state);       
  }

  if (VisSampleApp::GetInputMap()->GetTrigger(WORLD_LANG_NEXT))
  {
    // switch to the next place
    if (++m_iCurrentPlace >= m_iNumPlaces)
      m_iCurrentPlace = 0;
    ShowOverlays(FALSE);
    SwitchPlace(m_iCurrentPlace);
  }

  if (VisSampleApp::GetInputMap()->GetTrigger(WORLD_LANG_PREV))
  {
    // switch to the previous place
    if (--m_iCurrentPlace < 0)
      m_iCurrentPlace = m_iNumPlaces - 1;
    ShowOverlays(FALSE);
    SwitchPlace(m_iCurrentPlace);
  }
}


V_IMPLEMENT_SERIAL( WorldLanguages_cl, VisBaseEntity_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(WorldLanguages_cl, VisBaseEntity_cl, "WorldLanguages_cl", 0, "")
END_VAR_TABLE

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
