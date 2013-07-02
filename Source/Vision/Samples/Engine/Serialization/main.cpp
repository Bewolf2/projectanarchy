/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Serialization Demo
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// This sample shows how serialization works with the Vision engine. Serialization allows to
// save a current snapshot of your scene to file. This is typically used for "save game"
// implementations.
// ***********************************************************************************************
#include <Vision/Samples/Engine/Serialization/SerializationPCH.h>

#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Runtime/Base/System/IO/Serialization/VArchive.hpp>
#include <Vision/Runtime/Base/System/IO/Stream/VMemoryStream.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VSceneLoader.hpp>

#include <Vision/Samples/Engine/Serialization/SerializeBaseEntity.hpp>
#include <Vision/Samples/Engine/Serialization/SerializePlayer.hpp>
#include <Vision/Runtime/Common/VisSampleApp.hpp>

////////////////////////////////////////////////////////////////////////////////////
//
// declare our "module", since we need that for serialization support
//
extern VModule g_serializeSampleModule;
//
#define SERIALIZESAMPLE_VERSION_MAJOR 0x01
#define SERIALIZESAMPLE_VERSION_MINOR 0x00
//
DECLARE_THIS_MODULE( g_serializeSampleModule, MAKE_VERSION(SERIALIZESAMPLE_VERSION_MAJOR, SERIALIZESAMPLE_VERSION_MINOR),
                     "Engine Serialization Sample", "Havok", "Vision Engine Serialization Sample", NULL );
//
////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
//
// map-related
//
char g_pszCurrentMap[64];     // the current map name

int  g_iCurrentMap;           // the current map number

#define MAP_1         "Room.vscene"
#define MAP_2         "Crossing.vscene"

#define MAP_ICON_1    "Serialization\\map01.tex"
#define MAP_ICON_2    "Serialization\\map02.tex"
#define MAP_DATA_DIR  "Maps"VFILE_STR_SEPARATOR"SciFi"
const char* g_pszMaps[2]      = { MAP_1, MAP_2 };
const char* g_pszMapIcons[2]  = { MAP_ICON_1, MAP_ICON_2 };
#define SAVE_ICON     "Serialization\\SaveIcon.dds"
#define TTL_IMAGE     "Serialization\\TouchToLoad.dds"
#define HTTS_IMAGE    "Serialization\\HoldTouchToSave.dds"
#define CL_IMAGE      "Serialization\\ChangeLevel.dds"

//
// savegame-related
//
#define INITIAL_SAVEGAME_DIR  MAP_DATA_DIR VFILE_STR_SEPARATOR "Serialization"
#define ARCHIVE_START_TAG "SERIALIZE_API_SAMPLE"
#define ARCHIVE_END_TAG   "END_OF_ARCHIVE"
#define SAVEGAME_NAME     "SaveGames\\save%1d.gam"
#define SAVEGAME_PREVIEW  "SaveGames\\save%1d.bmp"
//
// screen masks
//
#define SAVESLOT_FILE "Serialization\\slot%01i.tex"
#define SAVESLOT_COL_ERROR VColorRef(255,128,128,255)
#define SAVESLOT_COL_LOAD  VColorRef(128,255,128,255)
#define SAVESLOT_COL_SAVE  VColorRef(255,255,128,255)
#define SAVESLOT_COL_NORM  VColorRef(255,255,255,255)
#define SAVESLOT_COL_EMPTY VColorRef(128,128,128,255)
VisScreenMaskPtr g_spMap;
VisScreenMaskPtr g_spSaveIcon;
VisScreenMaskPtr g_spTouchToLoad;
VisScreenMaskPtr g_spHoldToSave;
VisScreenMaskPtr g_spChangeLevel;

#define INFO_SCREEN_TIME    5.0f
#define INFO_FADEOUT_TIME   0.5f
float g_fInfoTime = 0.0f;

static const VColorRef g_iOverlayColor(255,255,255,190);

//
////////////////////////////////////////////////////////////////////////////////////

// sample flags
#if defined(_VISION_MOBILE) || defined( HK_ANARCHY )
  int iSampleFlags = VSAMPLE_INIT_DEFAULTS | VSAMPLE_FORCEMOBILEMODE;
#else
  int iSampleFlags = VSAMPLE_INIT_DEFAULTS;
#endif

//
// *** SaveGameSlot_cl ***
//
// Synopsis:
//    - A single instance of this class manages the HUD overlay for a single save game slot.
//      For instance, it creates a preview screenmask.
//
class SaveGameSlot_cl
{
public:
  // constructor
  SaveGameSlot_cl()
  {
    m_iSlotIndex = 0;
    m_bErrorState = false;
  }

  // called one time after Vision initialisation. Screenmasks are created here
  // Since we have a reference to them, they will survive a LoadWorld
  void OnInitOneTime(int iSlotIndex)
  {
    m_iSlotIndex = iSlotIndex;
    char pszSlotFile[64];
    sprintf(pszSlotFile, SAVESLOT_FILE, m_iSlotIndex + 1);

#if defined( _VISION_MOBILE )
    float fPosX = Vision::Video.GetXRes() - 132.0f;
    float fThumbnailPosY = 8.0f + m_iSlotIndex * 128.0f;
    float fSlotPosY = fThumbnailPosY + 56.0f;

    VTouchArea* pSlotArea = new VTouchArea( VInputManager::GetTouchScreen(), 
      VRectanglef( fPosX, fThumbnailPosY, fPosX + 128.0f, fThumbnailPosY + 96.0f ), 
      0.0f );

    VisSampleApp::GetInputMap()->MapTrigger( SERIALIZE_SLOT_1 + m_iSlotIndex, pSlotArea, CT_TOUCH_ANY, VInputOptions::Once() );
#else
    float fPosX = 8.0f + m_iSlotIndex * 132.0f;
    float fThumbnailPosY = Vision::Video.GetYRes() - 112.0f;
    float fSlotPosY = fThumbnailPosY + 56.0f;
#endif

    // screenmask for the slot number
    m_spSlotNumMask = new VisScreenMask_cl(pszSlotFile);
    m_spSlotNumMask->SetPos( fPosX, fSlotPosY );
    m_spSlotNumMask->SetTransparency(VIS_TRANSP_ALPHA);
    m_spSlotNumMask->SetColor(g_iOverlayColor);
    m_spSlotNumMask->SetOrder(100);
    m_spSlotNumMask->SetVisible(FALSE);
  
    // screenmask for either "No Preview" or "Free"
    m_spBackgroundMask = new VisScreenMask_cl("Serialization\\NoPreview.tex");
    m_spBackgroundMask->SetPos( fPosX, fThumbnailPosY );
    m_spBackgroundMask->SetTargetSize(128.f,96.f);
    m_spBackgroundMask->SetTransparency(VIS_TRANSP_ALPHA);
    m_spBackgroundMask->SetColor(g_iOverlayColor);
    m_spBackgroundMask->SetOrder(110);
    m_spBackgroundMask->SetVisible(FALSE);

    // screenmask for the level preview screenshot
    m_spPreviewMask = new VisScreenMask_cl();
    m_spPreviewMask->SetPos( fPosX, fThumbnailPosY );
    m_spPreviewMask->SetTargetSize(128.f,96.f);
    m_spPreviewMask->SetTransparency(VIS_TRANSP_ALPHA);
    m_spPreviewMask->SetColor(g_iOverlayColor);
    m_spPreviewMask->SetOrder(110);
    m_spPreviewMask->SetVisible(FALSE);
  }

  // called directly before Vision deinitialisation.
  void OnDeInitOneTime()
  {
    // release smart pointer references
    m_spSlotNumMask = NULL;
    m_spBackgroundMask = NULL;
    m_spPreviewMask = NULL;
  }

  // determines if a savegame corresponding to the slot exists
  inline BOOL IsSlotUsed() const
  {
    char pszSaveFileName[FS_MAX_PATH];
    sprintf(pszSaveFileName, SAVEGAME_NAME, m_iSlotIndex + 1);
    return Vision::File.Exists(pszSaveFileName);
  }

  // updates the slot HUD
  void UpdateSlotStatus(int iCurrentSlot)
  {
    BOOL bSlotUsed = IsSlotUsed();
    BOOL bShowPreview = FALSE;
    if (bSlotUsed)
    {
      // try to load preview bmp
      char pszPreviewFileName[FS_MAX_PATH];
      sprintf(pszPreviewFileName, SAVEGAME_PREVIEW, m_iSlotIndex + 1);
      if (Vision::File.Exists(pszPreviewFileName))
      {
        m_spPreviewMask->LoadFromFile(pszPreviewFileName);
        bShowPreview = TRUE;
      }
      else
      {
        m_spBackgroundMask->LoadFromFile("Serialization\\NoPreview.tex");
      }
    }
    else
    {
      m_spBackgroundMask->LoadFromFile("Serialization\\Free.tex");
    }
    // target size gets reset in LoadFromFile 
    m_spBackgroundMask->SetTargetSize(128.f,96.f);
    m_spPreviewMask->SetTargetSize(128.f,96.f);

    // either show preview or "free"
    m_spBackgroundMask->SetVisible(!bShowPreview);
    m_spPreviewMask->SetVisible(bShowPreview);

    // set slot number color according to status 
    VColorRef iColor = SAVESLOT_COL_NORM;
    if (m_bErrorState)
      iColor = SAVESLOT_COL_ERROR;
    else if (!bSlotUsed)
      iColor = SAVESLOT_COL_EMPTY;
    else if (iCurrentSlot==m_iSlotIndex) 
      iColor = SAVESLOT_COL_LOAD;

    m_spSlotNumMask->SetColor(g_iOverlayColor);
    m_spSlotNumMask->SetVisible(TRUE);
  }

  // save a screenshot icon
  void SaveScreenShotPreview()
  {
    const int w = Vision::Video.GetXRes();
    const int h = Vision::Video.GetYRes();
    
    unsigned char *pTempBuffer = new unsigned char [w*h*3];
    char pszPreviewFileName[FS_MAX_PATH];
    sprintf(pszPreviewFileName, SAVEGAME_PREVIEW, m_iSlotIndex + 1);

    // write the screen buffer to memory buffer
    if (Vision::Game.WriteScreenToBuffer(0,0,w,h,(UBYTE *)pTempBuffer))
    {
      // first, flip buffer vertically and swap red<->blue channel
      for (int y=0;y<(h/2);y++)
      {
        unsigned char *pRow1 = &pTempBuffer[y*w*3];
        unsigned char *pRow2 = &pTempBuffer[(h-y-1)*w*3];
        for (int x=0;x<w;x++, pRow1+=3,pRow2+=3)
        {
          unsigned char tmp[3] = {pRow1[0],pRow1[1],pRow1[2]};
          pRow1[0] = pRow2[2];
          pRow1[1] = pRow2[1];
          pRow1[2] = pRow2[0];
          pRow2[0] = tmp[2];
          pRow2[1] = tmp[1];
          pRow2[2] = tmp[0];
        }
      }

      // use VTEX to scale and save the image
      Image_cl image;
      ImageMap_cl colorMap(w,h,24, (UBYTE*)pTempBuffer);
      image.AddColorMap(colorMap);
      image.Scale(128,128);
      image.SaveBMP(pszPreviewFileName,Vision::File.GetOutputManager());
    }
    delete []pTempBuffer;

    // make sure texture is reloaded next time
    if (m_spPreviewMask->GetTextureObject())
      m_spPreviewMask->GetTextureObject()->EnsureUnloaded();
  }

  inline void SetErrorState() {m_bErrorState=true;}
  inline void ResetErrorState() {m_bErrorState=false;}

private:
  int m_iSlotIndex;   // 0-based savegame slot index
  bool m_bErrorState; // indicates whether this slot is currently in error state
  VisScreenMaskPtr m_spSlotNumMask;     // number mask of slot
  VisScreenMaskPtr m_spPreviewMask;     // preview picture
  VisScreenMaskPtr m_spBackgroundMask;  // "free" or "not available"
};


// four global instances of our savegame slots
SaveGameSlot_cl g_SaveSlot[4];



//
// *** GameSnapshot_t ***
//
// Synopsis:
//    - Struct that holds some game state information and is used in the LoadGame
//      function
//
struct GameLoadData_t
{
  int iMapNumber;           // the map number
  float fTime;              // the game timer
  VBool bJustReset;         // TRUE if we just have to reset the current world and not load another one
  int iFullEntityCount;     // number of fully serialized entities
  int iRecreateEntityCount; // number of re-createable entities
};


// ---------------------------------------------------------------------------------
// Method: GetEntityParameters
// Notes: Gets the full parameter string of an entity. This is used for re-creating
//        certain entities after serialization.
// ---------------------------------------------------------------------------------
void GetEntityParameters( VisBaseEntity_cl *pEnt, char *szBuf)
{
  char szValue[1000];
  char szVarString[1000];
  szBuf[0] = 0;
  for (int i = 0; i < pEnt->GetNumVariables(); i++)
  {
    const char *szVarName = pEnt->GetVariableName(i);
    pEnt->GetVariableValue( szVarName, szValue );
    sprintf(szVarString, " %s = \"%s\"", szVarName, szValue );
    szBuf[strlen(szBuf) + strlen(szVarString)] = 0;
    strcat(szBuf, szVarString);
  }
}


// ---------------------------------------------------------------------------------
// Method: DumpEntityCount
// Notes: Helper function to output the current number of entities
// ---------------------------------------------------------------------------------
void DumpEntityCount()
{
  int iEntCount = VisBaseEntity_cl::ElementManagerGetUsedCount();
  int iLitCount = VisLightSource_cl::ElementManagerGetUsedCount();

  Vision::Message.Add(1, "Entity Count: %d -- LightSource Count: %d", iEntCount, iLitCount);
}


// ---------------------------------------------------------------------------------
// Method: LoadMap
// Notes: 
// ---------------------------------------------------------------------------------
void LoadMap(int iMap, BOOL bLoadEntities)
{
  g_spMap = NULL;

  // load map
  strcpy(g_pszCurrentMap, g_pszMaps[iMap]);
  g_iCurrentMap = iMap;
  spApp->LoadScene(g_pszCurrentMap);

  hkvVec3 vStart;
  // try to find the Player_Start placeholder entity
  VisBaseEntity_cl* pStart = Vision::Game.SearchEntity("Player_Start");
  if (pStart)
    vStart = pStart->GetPosition();
  else
    vStart.set (-350.f, 200.f, 200.f);

  if ( bLoadEntities )
  {
    // create player entity (in case the entities are not loaded from the savegame)
    Vision::Game.CreateEntity("SerializePlayer_cl", vStart);

    // create 3 spotlight entities
    hkvVec3 Placement;
    Placement.set ( -200, 500, 200 );
    Vision::Game.CreateEntity("SerializeSpotlight_cl", Placement );
    Placement.set ( -300, 0, 100 );
    Vision::Game.CreateEntity("SerializeSpotlight_cl", Placement );
    Placement.set ( 40, -600, 100 );
    Vision::Game.CreateEntity("SerializeSpotlight_cl", Placement );
  }

  // create some screen masks
  int w = Vision::Video.GetXRes();
  int h = Vision::Video.GetYRes();

  // set up "current map" screen mask
  g_spMap = new VisScreenMask_cl(g_pszMapIcons[iMap]);
  g_spMap->SetPos( w - 48.f - 8.f, (float)h - 48.f - 8.f );
  g_spMap->SetTransparency(VIS_TRANSP_ALPHA);
  g_spMap->SetColor(g_iOverlayColor);
  g_spMap->SetVisible(TRUE);
}


// ---------------------------------------------------------------------------------
// Method: UpdateSlotStatus
// Notes: Update save slot screen masks
// ---------------------------------------------------------------------------------
void UpdateSlotStatus(int iCurrentSlot)
{
  for (int i = 0; i < 4; i++)
    g_SaveSlot[i].UpdateSlotStatus(iCurrentSlot);
}


// ---------------------------------------------------------------------------------
// Method: ResetWorld
// Notes: This function is used by the LoadGame function to get rid of all
//        serializeable entities.
// ---------------------------------------------------------------------------------
void ResetWorld()
{
  int i;
  int iNumOfAllEntities = VisBaseEntity_cl::ElementManagerGetSize();
  
  // send CLEANUP messages to all the entities that are about to be freed, so
  // that they have a chance of cleaning up whatever is necessary before the
  // world is destroyed
  for (i = 0; i < iNumOfAllEntities; i++)
  {
    VisBaseEntity_cl *pEnt = VisBaseEntity_cl::ElementManagerGet(i);
    if ( pEnt )
    {
      if ( pEnt->IsOfType(SerializeBaseEntity_cl::GetClassTypeId()) )
      {
        Vision::Game.SendMsg(pEnt, MSG_SER_CLEANUP, 0, 0 );
      }
    }
  }

  // now actually free all the serializeable entities
  for (i = 0; i < iNumOfAllEntities; i++)
  {
    VisBaseEntity_cl *pEnt = VisBaseEntity_cl::ElementManagerGet(i);
    if ( pEnt )
    {
      if ( pEnt->IsOfType(SerializeBaseEntity_cl::GetClassTypeId()) )
      {
        pEnt->DisposeObject();
      }
    }
  }
}


// ---------------------------------------------------------------------------------
// Method: SaveGame
// Notes: The file format for save games in this example is as follows:
//
//        "SERIALIZE_API_SAMPLE"  magic "number" (a string, really)
//        <version>
//        <map_number>            the current map's number
//        <time>                  the current game timer's value
//        <num_recreate_ent>      number of re-createable entities
//        <num_full_ent>          number of fully serialized entities
//        <recreate_ent>...       re-createable entity data
//        <full_ent>...           fully serialized entities
//        "END_OF_ARCHIVE"        ending key (useful for validating the file)
//
// ---------------------------------------------------------------------------------

VBool SaveGame(int iNum)
{
  if ( (iNum < 1) || (iNum > 4) )
  {
    // we just allow 4 save games
    return FALSE;
  }

  int i;
  char pszSaveFileName[FS_MAX_PATH];

  sprintf(pszSaveFileName,SAVEGAME_NAME, iNum);
  IVFileOutStream* pOut = Vision::File.Create(pszSaveFileName);
  // creating the file didn't work!
  if (!pOut)
  {
    return FALSE;
  }

  VArchive ar( pszSaveFileName, pOut, Vision::GetTypeManager() );

  // serialize global game data
  ar << ARCHIVE_START_TAG; 		      // magic number
  int iSavingVersion = Vision::GetArchiveVersion();
  ar << iSavingVersion;             // archive class version
  ar << g_iCurrentMap;              // current map number
  ar << Vision::GetTimer()->GetTime();		// current time

  // count entities
  SerializeBaseEntity_cl *pSerEnt = NULL;
  VisBaseEntity_cl *pEnt = NULL;
  int iFullCtr = 0;
  int iReCreateCtr = 0;
  int iNumOfAllEntities = VisBaseEntity_cl::ElementManagerGetSize();
  for (i = 0; i < iNumOfAllEntities; i++)
  {
    pEnt = VisBaseEntity_cl::ElementManagerGet(i);
    if ( pEnt )
    {
      if ( pEnt->IsOfType(SerializeBaseEntity_cl::GetClassTypeId()) )
      {
        pSerEnt = static_cast<SerializeBaseEntity_cl*>(pEnt);
        if ( pSerEnt->GetSerializeType() == SERIALIZE_FULL )
          iFullCtr++;
        else
          iReCreateCtr++;
      }
    }
  }

  // serialize number of entities
  ar << iReCreateCtr;
  ar << iFullCtr;

  hkvVec3 vTemp;

  // do ReCreate serialization of entities
  for (i = 0; i < iNumOfAllEntities; i++)
  {
    pEnt = VisBaseEntity_cl::ElementManagerGet(i);
    if ( pEnt )
    {
      if ( pEnt->IsOfType(SerializeBaseEntity_cl::GetClassTypeId()) )
      {
        pSerEnt = (SerializeBaseEntity_cl *) pEnt;
        if ( pSerEnt->GetSerializeType() == SERIALIZE_RECREATE )
        {
          char pszEntityParams[4000];
          GetEntityParameters( pSerEnt, pszEntityParams );
          VDynamicMesh* pMesh = pSerEnt->GetMesh();
          ar << pSerEnt->GetClassFullName() << pSerEnt->GetEntityKey();
          vTemp = pSerEnt->GetPosition();
          vTemp.SerializeAsVisVector (ar);
          vTemp = pSerEnt->GetOrientation();
          vTemp.SerializeAsVisVector (ar);
          const char *szFilename = pMesh ? pMesh->GetFilename() : NULL;
          ar << szFilename << pszEntityParams;
        }
      }
    }
  }
  
  // do full serialization of entities
  for (i = 0; i < iNumOfAllEntities; i++)
  {
    pEnt = VisBaseEntity_cl::ElementManagerGet(i);
    if ( pEnt )
    {
      if ( pEnt->IsOfType(SerializeBaseEntity_cl::GetClassTypeId()) )
      {
        pSerEnt = (SerializeBaseEntity_cl *) pEnt;
        if ( pSerEnt->GetSerializeType() == SERIALIZE_FULL )
        {
          ar << pSerEnt;
        }
      }
    }
  }
  
  // store end tag - useful to verify a valid archive
  ar << ARCHIVE_END_TAG;

  ar.Close();
  pOut->Close();

  g_SaveSlot[iNum-1].SaveScreenShotPreview();

  return TRUE;
}


// ---------------------------------------------------------------------------------
// Method: LoadGame
// Notes: The file format for save games in this example is as follows:
//
//        "SERIALIZE_API_SAMPLE"  magic "number" (a string, really)
//        <version>
//        <map_number>            the current map number
//        <time>                  the current game timer's value
//        <num_recreate_ent>      number of re-creatable entities
//        <num_full_ent>          number of fully serialized entities
//        <recreate_ent>...       re-creatable entity data
//        <full_ent>...           fully serialized entities
//        "END_OF_ARCHIVE"        ending key (useful for validating the file)
//
// ---------------------------------------------------------------------------------
VBool LoadGame(int iNum)
{
  if ( (iNum < 1) || (iNum > 6) )
  {
    return FALSE;
  }

  GameLoadData_t snap;

  int i;
  char pszLoadFileName[FS_MAX_PATH];
  char szArchiveTag[FS_MAX_PATH];
  
  sprintf(pszLoadFileName,SAVEGAME_NAME, iNum);

  IVFileInStream* pIn = Vision::File.Open(pszLoadFileName);
  // opening the file didn't work!
  if (!pIn)
    return FALSE;

  VArchive ar( pszLoadFileName, pIn, Vision::GetTypeManager() );

  ar.ReadStringBinary(szArchiveTag, FS_MAX_PATH);
  if (strcmp(szArchiveTag, ARCHIVE_START_TAG) != 0)
  { 
    return FALSE;
  }

  ar >> i;                        // loading version
  ar.SetLoadingVersion(i);
  if (! Vision::IsValidArchive(ar))
  {
    // TODO
    return FALSE;
  }
    
  ar >> snap.iMapNumber;          // map number
  if (snap.iMapNumber == g_iCurrentMap)
    snap.bJustReset = TRUE;
  else
    snap.bJustReset = FALSE;

  ar >> snap.fTime;               // game timer
  ar >> snap.iRecreateEntityCount;// number of re-createable entities
  ar >> snap.iFullEntityCount;    // number of fullly serialized entities

  //snap.bJustReset = FALSE;
  
  if (snap.bJustReset == TRUE)
  {
    // as it's the same map, we just clean up and re-use it :-)
    ResetWorld();
  }
  else
  {
    // load the other map (without entities)
    LoadMap(snap.iMapNumber,FALSE);
  }

  Vision::GetTimer()->SetTime(snap.fTime);

  // now re-create all the "RECREATE" entities
  VMemoryTempBuffer<1024> modelFileName;
  VMemoryTempBuffer<1024> entityParams;
  VMemoryTempBuffer<1024> entityKey;
  VMemoryTempBuffer<1024> className;

  // now re-create all the "RECREATE" entities
  for (i = 0; i < snap.iRecreateEntityCount; i++)
  {
    char* pszClassName;
    char* pszEntityKey;
    hkvVec3 vOrigin;
    hkvVec3 vOrientation;
    char* pszModelFile;
    char* pszEntityParams;
    // get entity data
    pszClassName = className.ReadStringBinary(ar);
    pszEntityKey = entityKey.ReadStringBinary(ar);
    vOrigin.SerializeAsVisVector (ar);
    vOrientation.SerializeAsVisVector (ar);
    pszModelFile = modelFileName.ReadStringBinary(ar);
    pszEntityParams = entityParams.ReadStringBinary(ar);
    SerializeBaseEntity_cl* pSerEnt = static_cast<SerializeBaseEntity_cl*>(
      Vision::Game.CreateEntity(pszClassName, vOrigin, pszModelFile,
      pszEntityParams));
    pSerEnt->SetOrientation(vOrientation);
    pSerEnt->SetEntityKey(pszEntityKey);
  }

  // finally, re-create all the "FULL" entities
  for (i = 0; i < snap.iFullEntityCount; i++)
  {
    SerializeBaseEntity_cl *pSerEnt;
    // get an entity from the archive...
    ar >> pSerEnt;
  }

  // check if the ARCHIVE_END_TAG exists
  ar.ReadStringBinary(szArchiveTag, FS_MAX_PATH);
  if (strcmp(szArchiveTag, ARCHIVE_END_TAG) != 0)
  {
    Vision::Error.FatalError("No (or invalid) end tag found in save game!");
    ar.Close();
    pIn->Close();
    return FALSE;
  }
  
  ar.Close();
  pIn->Close();

  return TRUE;
}

VisSampleAppPtr spApp = NULL;

// ---------------------------------------------------------------------------------
// Save Game handler
// ---------------------------------------------------------------------------------

class SaveGameHandler : public IVisCallbackHandler_cl
{
public:
  // Constructor
  SaveGameHandler() : m_bSavePending(false), m_iGameNum(1) {}

  // Interface
  void TriggerSave(int iGameNum)
  {
    m_bSavePending = true;
    m_iGameNum = iGameNum;
  }

  // Callback interface
  VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData)
  {
    if (pData->m_pSender == &Vision::Callbacks.OnBeforeSwapBuffers)
    {
      if (m_bSavePending)
      {
        // Perform Save
        if (SaveGame(m_iGameNum) == TRUE)
        {
          g_SaveSlot[m_iGameNum-1].ResetErrorState();
          Vision::Message.Add(1, "Saved game in slot %d.", m_iGameNum);
          UpdateSlotStatus(-1);
        }
        else
        {
          g_SaveSlot[m_iGameNum-1].SetErrorState();
          Vision::Message.Add(1, "ERROR: Couldn't save game in slot %d!", m_iGameNum);
          UpdateSlotStatus(-1);
        }

        // Re-set
        m_bSavePending = false;
      }
    }
  }

private:
  // Member variables
  bool m_bSavePending;
  int m_iGameNum;
};

SaveGameHandler g_saveGameHandler;


// ---------------------------------------------------------------------------------
// Method: main
// ---------------------------------------------------------------------------------

VISION_INIT
{
  VISION_SET_DIRECTORIES(false);

  // register our module with the vision type manager
  Vision::GetTypeManager()->RegisterModule( &g_serializeSampleModule );

  // include the vision engine plugin
  VisionAppHelpers::MakeEXEDirCurrent();
  VisSampleApp::LoadVisionEnginePlugin();

  VISION_PLUGIN_LOAD_STATIC(vHavok);

  // Create and init an application
  spApp = new VisSampleApp();
  if (!spApp->InitSample(MAP_DATA_DIR /*DataDir*/, NULL /*SampleScene*/, iSampleFlags&~VSAMPLE_HAVOKLOGO))
    return false;

#if defined( _VISION_MOBILE )
  VString outputDir;

#if defined( _VISION_ANDROID )
  outputDir = VisSampleApp::GetCacheDirectory();

  // On Android, we have to re-set our output directory to be the SDCard one
  // (instead of the cache directory, which is used by default).
  if ( !Vision::File.SetOutputDirectory( outputDir.AsChar() ) )
  {
    Vision::Error.Warning( "Unable to set output directory '%s'\n", outputDir.AsChar() );
    return false;
  }
  Vision::File.AddDataDirectory( outputDir );
#elif defined( _VISION_IOS )
  outputDir = VisSampleApp::GetDocumentsDirectory();
#endif

  // On mobile platforms, we store our savegame data in another folder than our assets (where initial savegame 
  // data might be sitting). So, we have to make sure that our output directory actually exists.
  VString saveGameDir = VPathHelper::CombineDirAndDir( outputDir.AsChar(), "SaveGames" );
  if ( !VFileHelper::ExistsDir( saveGameDir.AsChar() ) )
  {
    printf( "creating savegame directory '%s'\n", saveGameDir.AsChar() );
    VFileHelper::MkDirRecursive( saveGameDir.AsChar() );
  }

#else
  // On all other platforms, we use the initial savegame dir for storing new savegames as well.
  VString outputDir = INITIAL_SAVEGAME_DIR;
#endif

  // Finally, add the initial savegame directory
  spApp->AddSampleDataDir( INITIAL_SAVEGAME_DIR );

  // The following lines set the output file manager to the same instance as the sample data directory
  // This is necessary because we have directory caching enabled (by default). If the output manager was using
  // its own instance of VDiskFileStreamManager, then new created files won't trigger a notification in the file
  // manager used for Vision::File.Open. That's why it is so important that we share the same 
  // VDiskFileStreamManager.
  // Alternatively directory caching could be globally disabled by this sample using VDiskFileStreamManager::SetEnableDirectoryCaching.
  VisionFileStreamManager *pFileManager = (VisionFileStreamManager *)Vision::File.GetManager();
  const int iDirCount = pFileManager->GetNumDataDirectories();
  int iLen = outputDir.GetSize();
  // find our data directory and set the file manager that represents this directory as file output manager
  for (int i=0;i<iDirCount;i++)
  {
    const char *szDir = pFileManager->GetDataDirectory(i);
    if (!szDir || !szDir[0])
      continue;
    int iDirLen = (int)strlen(szDir);
    if (iDirLen>=iLen && !strcmp(&szDir[iDirLen-iLen], outputDir.AsChar()))
    {
      // share the disk file manager instance
      Vision::File.SetOutputManager(pFileManager->GetSubManager(i));
#if defined( _VISION_IOS )
      // On iOS, the document directory is not cached by default, which has 
      // implications on the search directory order. To avoid this causing
      // issues, we enable directory caching for our output directory as well.
      pFileManager->SetEnableDirectoryCaching( true, i );
#endif
      break;
    }
  }

  g_iCurrentMap = 0;

  // Create Save Game Callback Handler and register it at OnBeforeSwapBuffers to be 
  // able to get the back buffer just before it is presented.
  Vision::Callbacks.OnBeforeSwapBuffers += g_saveGameHandler;

  for ( int i = 0; i < 4; i++ )
  {
    g_SaveSlot[i].OnInitOneTime(i);
  }
  g_spMap = NULL;

  // first initialization
  LoadMap(g_iCurrentMap, TRUE);
  UpdateSlotStatus(-1);

  return true;
}

VISION_SAMPLEAPP_AFTER_LOADING
{
  // Free help text generated from previous loads.
  spApp->FreeHelpText();

  // define help text
  spApp->AddHelpText("");
  spApp->AddHelpText("How to use this demo :");
  spApp->AddHelpText("");
  spApp->AddHelpText("The icons at the bottom left show which save game slots hold a saved game,");
  spApp->AddHelpText("while the icon at the bottom right shows which map you are currently in.");
  spApp->AddHelpText("");

#ifdef SUPPORTS_MOUSE
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_LOOK_CHANGED, V_MOUSE, CT_MOUSE_DATA_CHANGED);

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_HORIZONTAL_LOOK, V_MOUSE, CT_MOUSE_NORM_DELTA_X, VInputOptions::Sensitivity(2.0f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_VERTICAL_LOOK,   V_MOUSE, CT_MOUSE_NORM_DELTA_Y, VInputOptions::Sensitivity(2.0f));

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_RUN,        V_MOUSE, CT_MOUSE_RIGHT_BUTTON);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_RUN_FASTER, V_MOUSE, CT_MOUSE_MIDDLE_BUTTON);
#endif

#ifdef SUPPORTS_KEYBOARD
  // On the PC, register key controls that
  // differ from the defaults
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_1,   V_KEYBOARD, CT_KB_1, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_2,   V_KEYBOARD, CT_KB_2, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_3,   V_KEYBOARD, CT_KB_3, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_4,   V_KEYBOARD, CT_KB_4, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_NEXT_MAP, V_KEYBOARD, CT_KB_M, VInputOptions::Once());

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SAVE_MODIFIER,  V_KEYBOARD, CT_KB_LSHIFT);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SAVE_MODIFIER,  V_KEYBOARD, CT_KB_RSHIFT);

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_ACTION_OCCURED, V_KEYBOARD, CT_KB_UNKNOWN);

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,   V_KEYBOARD, CT_KB_UP);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD,  V_KEYBOARD, CT_KB_DOWN);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,      V_KEYBOARD, CT_KB_LEFT);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,     V_KEYBOARD, CT_KB_RIGHT);

  spApp->AddHelpText("KEYBOARD - Shift + 1...4 : Save game to slot 1...4");
  spApp->AddHelpText("KEYBOARD - 1...4         : Load game from slot 1...4");
  spApp->AddHelpText("KEYBOARD - M : switch to next map");
  spApp->AddHelpText("");

#endif

#if defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_1,   V_XENON_PAD(0), CT_PAD_A, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_2,   V_XENON_PAD(0), CT_PAD_B, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_3,   V_XENON_PAD(0), CT_PAD_X, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_4,   V_XENON_PAD(0), CT_PAD_Y, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_NEXT_MAP, V_XENON_PAD(0), CT_PAD_LEFT_SHOULDER, VInputOptions::Once());

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SAVE_MODIFIER,  V_XENON_PAD(0), CT_PAD_LEFT_TRIGGER, VInputOptions::DeadZone(0.25f));

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_ACTION_OCCURED, V_XENON_PAD(0), CT_PAD_ANY_KEY);

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,  V_XENON_PAD(0), CT_PAD_UP);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD, V_XENON_PAD(0), CT_PAD_DOWN);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,     V_XENON_PAD(0), CT_PAD_LEFT);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,    V_XENON_PAD(0), CT_PAD_RIGHT);

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,  V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP, VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD, V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_DOWN, VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,     V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT, VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,    V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_LOOK_CHANGED,  V_XENON_PAD(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  VisSampleApp::GetInputMap()->MapTriggerAxis(SERIALIZE_HORIZONTAL_LOOK, V_XENON_PAD(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f, true));
  VisSampleApp::GetInputMap()->MapTriggerAxis(SERIALIZE_VERTICAL_LOOK,   V_XENON_PAD(0), CT_PAD_RIGHT_THUMB_STICK_UP, CT_PAD_RIGHT_THUMB_STICK_DOWN, VInputOptions::DeadZone(0.25f, true));
 
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_RUN,        V_XENON_PAD(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_RUN_FASTER, V_XENON_PAD(0), CT_PAD_RIGHT_THUMB);

  spApp->AddHelpText("PAD1 - Left Trigger + PAD1 - A, B, X, Y : Save game to slot 1...4");
  spApp->AddHelpText("PAD1 - A, B, X, Y     : Load game from slot 1...4");
  spApp->AddHelpText("PAD1 - Left Shoulder : Switch to next map");

#elif defined (_VISION_PS3)
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_1,   V_PS3_PAD(0), CT_PAD_CROSS, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_2,   V_PS3_PAD(0), CT_PAD_CIRCLE, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_3,   V_PS3_PAD(0), CT_PAD_SQUARE, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_4,   V_PS3_PAD(0), CT_PAD_TRIANGLE, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_NEXT_MAP, V_PS3_PAD(0), CT_PAD_LEFT_SHOULDER, VInputOptions::Once());

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SAVE_MODIFIER,  V_PS3_PAD(0), CT_PAD_LEFT_TRIGGER, VInputOptions::DeadZone(0.25f));

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_ACTION_OCCURED, V_PS3_PAD(0), CT_PAD_ANY_KEY);

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,  V_PS3_PAD(0), CT_PAD_UP);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD, V_PS3_PAD(0), CT_PAD_DOWN);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,     V_PS3_PAD(0), CT_PAD_LEFT);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,    V_PS3_PAD(0), CT_PAD_RIGHT);

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,  V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD, V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,     V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,    V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_LOOK_CHANGED,  V_PS3_PAD(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  VisSampleApp::GetInputMap()->MapTriggerAxis(SERIALIZE_HORIZONTAL_LOOK, V_PS3_PAD(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f, true));
  VisSampleApp::GetInputMap()->MapTriggerAxis(SERIALIZE_VERTICAL_LOOK,   V_PS3_PAD(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.25f, true));
 
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_RUN,        V_PS3_PAD(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_RUN_FASTER, V_PS3_PAD(0), CT_PAD_RIGHT_THUMB);

  spApp->AddHelpText("PAD1 - Left Trigger + PAD1 - CROSS/CIRCLE/TRIANGLE/SQUARE : Save game to slot 1...4");
  spApp->AddHelpText("PAD1 - CROSS/CIRCLE/TRIANGLE/SQUARE     : Load game from slot 1...4");
  spApp->AddHelpText("PAD1 - Left Shoulder : Switch to next map");

#elif defined (_VISION_PSP2)
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_1,   V_PSP2_PAD(0), CT_PAD_CROSS,         VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_2,   V_PSP2_PAD(0), CT_PAD_CIRCLE,        VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_3,   V_PSP2_PAD(0), CT_PAD_SQUARE,        VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_4,   V_PSP2_PAD(0), CT_PAD_TRIANGLE,      VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_NEXT_MAP, V_PSP2_PAD(0), CT_PAD_LEFT_SHOULDER, VInputOptions::Once());

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SAVE_MODIFIER,  V_PSP2_PAD(0), CT_PAD_LEFT_TRIGGER, VInputOptions::DeadZone(0.25f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_ACTION_OCCURED, V_PSP2_PAD(0), CT_PAD_ANY_KEY);

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,   V_PSP2_PAD(0), CT_PAD_UP);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD,  V_PSP2_PAD(0), CT_PAD_DOWN);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,      V_PSP2_PAD(0), CT_PAD_LEFT);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,     V_PSP2_PAD(0), CT_PAD_RIGHT);

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,   V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD,  V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,      V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,     V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_LOOK_CHANGED,   V_PSP2_PAD(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  VisSampleApp::GetInputMap()->MapTriggerAxis(SERIALIZE_HORIZONTAL_LOOK, V_PSP2_PAD(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f, true));
  VisSampleApp::GetInputMap()->MapTriggerAxis(SERIALIZE_VERTICAL_LOOK,   V_PSP2_PAD(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.25f, true));
 
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_RUN,        V_PSP2_PAD(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_RUN_FASTER, V_PSP2_PAD(0), CT_PAD_RIGHT_THUMB);

  spApp->AddHelpText("PAD1 - Left Trigger + PAD1 - CROSS/CIRCLE/TRIANGLE/SQUARE : Save game to slot 1...4");
  spApp->AddHelpText("PAD1 - CROSS/CIRCLE/TRIANGLE/SQUARE     : Load game from slot 1...4");
  spApp->AddHelpText("PAD1 - Left Shoulder : Switch to next map");

#elif defined (_VISION_WIIU)
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_1,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_B, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_2,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_A, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_3,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_Y, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SLOT_4,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_X, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_NEXT_MAP, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_SHOULDER, VInputOptions::Once());

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_SAVE_MODIFIER,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_TRIGGER, VInputOptions::DeadZone(0.25f));

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_ACTION_OCCURED, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_ANY_KEY);
  
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_UP);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_DOWN);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,     VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT);

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,     VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_LOOK_CHANGED,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  VisSampleApp::GetInputMap()->MapTriggerAxis(SERIALIZE_HORIZONTAL_LOOK, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f, true));
  VisSampleApp::GetInputMap()->MapTriggerAxis(SERIALIZE_VERTICAL_LOOK,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.25f, true));

  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_RUN,        VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_RUN_FASTER, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB);

  spApp->AddHelpText("DRC - Left Trigger + DRC - B/A/Y/X : Save game to slot 1...4");
  spApp->AddHelpText("DRC - B/A/Y/X     : Load game from slot 1...4");
  spApp->AddHelpText("DRC - Left Shoulder : Switch to next map");

#elif defined( _VISION_MOBILE )
  
  int iResX = Vision::Video.GetXRes();
  int iResY = Vision::Video.GetYRes();
  int iWidth, iHeight;

  g_spSaveIcon = new VisScreenMask_cl( SAVE_ICON );
  g_spSaveIcon->GetTextureSize( iWidth, iHeight );
  g_spSaveIcon->SetPos( 8.0f, iResY - iHeight - 8.0f );
  g_spSaveIcon->SetTransparency( VIS_TRANSP_ALPHA );
  g_spSaveIcon->SetColor( g_iOverlayColor );
  g_spSaveIcon->SetVisible( TRUE );

  // create some touch areas
  VTouchArea* pSaveIconArea = new VTouchArea( VInputManager::GetTouchScreen(), VRectanglef( 0.0f, iResY - iHeight - 16.0f, 16.0f + iWidth, float(iResY) ), 0.0f );
  VisSampleApp::GetInputMap()->MapTrigger( SERIALIZE_SAVE_MODIFIER, pSaveIconArea, CT_TOUCH_ANY );

  VTouchArea* pNextMapArea = new VTouchArea( VInputManager::GetTouchScreen(), VRectanglef( iResX - 56.0f, iResY - 56.0f, float(iResX), float(iResY) ), 0.0f );
  VisSampleApp::GetInputMap()->MapTrigger( SERIALIZE_NEXT_MAP, pNextMapArea, CT_TOUCH_ANY, VInputOptions::Once() );

  // finally, add in those info overlays
  int iWidth2, iHeight2;
  g_spHoldToSave = new VisScreenMask_cl( HTTS_IMAGE );
  g_spHoldToSave->GetTextureSize( iWidth2, iHeight2 );
  g_spHoldToSave->SetPos( 4.0f, iResY - iHeight - iHeight2 - 12.0f );
  g_spHoldToSave->SetTransparency( VIS_TRANSP_ALPHA );
  g_spHoldToSave->SetColor( g_iOverlayColor );
  g_spHoldToSave->SetVisible( TRUE );

  g_spTouchToLoad = new VisScreenMask_cl( TTL_IMAGE );
  g_spTouchToLoad->GetTextureSize( iWidth2, iHeight2 );
  g_spTouchToLoad->SetPos( iResX - iWidth2 - 140.0f, 264.0f - ( iHeight2 / 2 ) );
  g_spTouchToLoad->SetTransparency( VIS_TRANSP_ALPHA );
  g_spTouchToLoad->SetColor( g_iOverlayColor );
  g_spTouchToLoad->SetVisible( TRUE );

  g_spChangeLevel = new VisScreenMask_cl( CL_IMAGE );
  g_spChangeLevel->GetTextureSize( iWidth2, iHeight2 );
  g_spChangeLevel->SetPos( iResX - iWidth2 - 60.0f, iResY - iHeight2 - 60.0f );
  g_spChangeLevel->SetTransparency( VIS_TRANSP_ALPHA );
  g_spChangeLevel->SetColor( g_iOverlayColor );
  g_spChangeLevel->SetVisible( TRUE );

  g_fInfoTime = INFO_SCREEN_TIME + INFO_FADEOUT_TIME;

#endif
}

VISION_SAMPLEAPP_RUN
{
  if (spApp->Run())
  {
    int iGameNum = 0;
    bool bPerformSave = false;
    
    if (VisSampleApp::GetInputMap()->GetTrigger(SERIALIZE_SLOT_1)) iGameNum = 1;
    if (VisSampleApp::GetInputMap()->GetTrigger(SERIALIZE_SLOT_2)) iGameNum = 2;
    if (VisSampleApp::GetInputMap()->GetTrigger(SERIALIZE_SLOT_3)) iGameNum = 3;
    if (VisSampleApp::GetInputMap()->GetTrigger(SERIALIZE_SLOT_4)) iGameNum = 4;
    if (VisSampleApp::GetInputMap()->GetTrigger(SERIALIZE_SAVE_MODIFIER)) bPerformSave = true;

    if (iGameNum != 0)
    {
      if ( bPerformSave )
      {
        // save (wait for next back buffer swap)
        g_saveGameHandler.TriggerSave(iGameNum);
      }
      else // load slot
      {
        if (g_SaveSlot[iGameNum-1].IsSlotUsed())
        {
          // load
          if (LoadGame(iGameNum) == TRUE)
          {
            UpdateSlotStatus(iGameNum - 1);
            Vision::Message.Add(1, "Loaded game from slot %d.", iGameNum);
          }
          else
          {
            g_SaveSlot[iGameNum-1].SetErrorState();
            Vision::Message.Add(1, "ERROR: Couldn't load game from slot %d!", iGameNum);
            UpdateSlotStatus(iGameNum - 1);
          }
        }
        else
        {
          Vision::Message.Add(1, "Save game slot %d is empty.", iGameNum);
        }
      }
      //DumpEntityCount();
    }

    if (VisSampleApp::GetInputMap()->GetTrigger(SERIALIZE_NEXT_MAP))
    {
      g_iCurrentMap = (g_iCurrentMap + 1) % 2;
      // load the next map (including entities)
      LoadMap(g_iCurrentMap,TRUE);
      UpdateSlotStatus(-1);
    }

    if ( g_fInfoTime > 0.0f )
    {
      if ( g_fInfoTime < INFO_FADEOUT_TIME )
      {
        VColorRef col( g_iOverlayColor );
        col.a = UBYTE( col.a * ( g_fInfoTime / INFO_FADEOUT_TIME ) );
        g_spTouchToLoad->SetColor( col );
        g_spHoldToSave->SetColor( col );
        g_spChangeLevel->SetColor( col );
      }
      g_fInfoTime -= Vision::GetTimer()->GetTimeDifference();

      if ( g_fInfoTime <= 0.0f )
      {
        g_spTouchToLoad->SetVisible( FALSE );
        g_spHoldToSave->SetVisible( FALSE );
        g_spChangeLevel->SetVisible( FALSE );
      }
    }

    return true;
  }

  return false;
}

VISION_DEINIT
{
  g_spMap = NULL;
  g_spSaveIcon = NULL;
  g_spHoldToSave = NULL;
  g_spTouchToLoad = NULL;
  g_spChangeLevel = NULL;

  // deinit the slots (screenmasks etc.)
  for ( int i = 0; i < 4; i++ )
  {
    g_SaveSlot[i].OnDeInitOneTime();
  }

  // unregister our module
  Vision::GetTypeManager()->UnregisterModule( &g_serializeSampleModule );

  // unregister callbacks
  Vision::Callbacks.OnBeforeSwapBuffers -= g_saveGameHandler;

  // Deinit the application
  spApp->DeInitSample();
  spApp = NULL;
  Vision::Plugins.UnloadAllEnginePlugins();
  
  return 0;
}

#if !defined( _VISION_IOS )
  VISION_MAIN_DEFAULT
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
