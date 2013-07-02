/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// WorldLanguages_cl Class
//***********************************************************************

// you must first include the vision.hpp file to use this header
#ifndef _WorldLanguages_cl_HEADER_
#define _WorldLanguages_cl_HEADER_

#include <Vision/Samples/Engine/Unicode/VArabicConverter.hpp>

// defines
#define PLACES_FILE "places.utf8"
#define MAX_TEXTLINES 8
#define MAX_ALPHABETS 4
#define DATADIR_PREFIX "..\\Data\\UnicodeDemo"

// key for printer
#define PLACENAME     1
#define PLACEDESC     2
#define SEARCH        3
#define REACHED       4
#define ROTANGLESDEST 5
#define ROTANGLESCUR  6

enum WORLD_LANG_CONTROL {
  WORLD_LANG_NEXT = VISION_INPUT_CONTROL_LAST_ELEMENT+1,
  WORLD_LANG_PREV
};

// structure that holds information about a place on earth
struct WorldPlace_t
{
  WorldPlace_t() : m_PlaceName(), m_spFont(NULL), m_angleY(0), m_angleZ(0), m_Text(), m_pRTLSupport(NULL) {} // constructor
  ~WorldPlace_t() 
  {
    m_spFont = NULL;
    V_SAFE_DELETE(m_pRTLSupport);
  } // destructor

  VString m_PlaceName;            // the name of the place/alphabet (e.g. "Cyrillic (Russian)")
  VisFontPtr m_spFont;            // pointer on unicode font needed for this place
  float m_angleY;                 // the x angle the earth globe should turn to
  float m_angleZ;                 // the y angle the earth globe should turn to
  VString m_Text;                 // the text written in the foreign alphabet
  VRTLSupport_cl * m_pRTLSupport; // optional class to support right-to-left text rendering
};


//
// *** WorldLanguages_cl  ***
//
// Synopsis:
//    - Entity class that writes unicode text and rotates the globe.
//
// Description:
//    - This class contains all the vital parts of this demo. The
//      ReadPlacesFile() function reads in the file "places.uni", which
//      is a unicode text file that defines all the places. The alphabets
//      were created using the fontgrabber utility.
//
// Notes:
//    - 
//
class WorldLanguages_cl : public VisBaseEntity_cl
{
private:
  ////////////////////////////////////////////////////////////////////////////////
  // private variables
  VisScreenMask_cl *m_pHotSpot;                     // the Hot spot texture on the globe
  VisScreenMask_cl *m_pBrace;                       // the curly brace next to text
  WorldPlace_t *m_pPlaces;                          // stores all the places' info
  VisBaseEntity_cl *m_pTheWorld;                    // pointer to the earth globe entity

  VisFontPtr spGlobalFont;      // Font used for nearly everything
  
  int m_iNumPlaces;             // the total number of places
  int m_iCurrentPlace;          // the index of the current place
  int m_iOldPlace;              // the index of the old place
  float m_fTargetAngle[2];      // the angles the target place has
  float m_fCurrentAngle[2];     // the current angles of the globe
  
  ////////////////////////////////////////////////////////////////////////////////
  // private functions

  // read places file and set up WorldPlace_t list
  void ReadPlacesFile();
  // set up (static) screen masks
  void SetupMasks();
  // set up new text box from WorldPlace_t object
  void SwitchPlace(int iNewIndex);
  // show or hide the overlay brace and text
  void ShowOverlays(VBool bShow);
 
public:
  
  ////////////////////////////////////////////////////////////////////////////////
  // virtual functions derived from VisBaseEntity_cl  

  virtual void InitFunction();
  virtual void DeInitFunction();
  virtual void ThinkFunction();

  V_DECLARE_SERIAL( WorldLanguages_cl, )
  IMPLEMENT_OBJ_CLASS(WorldLanguages_cl);
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
