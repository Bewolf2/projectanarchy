/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace ManagedFramework;
using namespace CSharpFramework::Scene;
using namespace TerrainBase;
using namespace TerrainBase::Editing;
using namespace TerrainBase;
using namespace TerrainBase::Actions;
using namespace System::Runtime::Serialization;

class TerrainConfig;
class VEditableTerrain;

#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/EngineInstanceTerrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/EditableTerrain.hpp>

namespace TerrainManaged
{

  [Serializable]
  public ref class TerrainChangedAction : public TerrainChangedBaseAction
  {
  public:
	  TerrainChangedAction(EngineInstanceTerrain ^pTerrain, String ^ActionName);
    virtual ~TerrainChangedAction();
    VEditableTerrain *m_pTerrain;
    VUndoableSectorActionCollection* m_pSectorActions;

    VOVERRIDE void Do() override;
    VOVERRIDE void Undo() override;
    VOVERRIDE void OnDispose() override
    {
      V_SAFE_DELETE(m_pSectorActions);
    }
   
    // ISerializable Members
    TerrainChangedAction(SerializationInfo ^info, StreamingContext context);
    VOVERRIDE void GetObjectData(SerializationInfo ^info, StreamingContext context) override;

    void DeserializeCustomDataBlock();

    /*
    VOVERRIDE bool get_Valid() override;

    VOVERRIDE int get_SectorX1() override {return m_pSectorActions->m_iSectorRect[0];}
    VOVERRIDE int get_SectorY1() override {return m_pSectorActions->m_iSectorRect[1];}
    VOVERRIDE int get_SectorX2() override {return m_pSectorActions->m_iSectorRect[2];}
    VOVERRIDE int get_SectorY2() override {return m_pSectorActions->m_iSectorRect[3];}
    */
    
    VOVERRIDE property bool Valid
    {
      bool get() override 
      { 
        return true; // m_pSectorActions->Count()>0; otherwise messing with OnDispose
      }
    }

    VOVERRIDE property int SectorX1 { int get() override {return m_pSectorActions->m_iSectorRect[0];} }
    VOVERRIDE property int SectorY1 { int get() override {return m_pSectorActions->m_iSectorRect[1];} }
    VOVERRIDE property int SectorX2 { int get() override {return m_pSectorActions->m_iSectorRect[2];} }
    VOVERRIDE property int SectorY2 { int get() override {return m_pSectorActions->m_iSectorRect[3];} }

  };

  [Serializable]
  public ref class TerrainClipboardObject : public ITerrainClipboardObject
  {
  public:
    TerrainClipboardObject(System::Object ^obj, String ^name) : ITerrainClipboardObject(obj,name)
    {
      m_pHeightValues = NULL;
      m_pDetailTextures = NULL;
      m_pDecorationChannels = NULL;
      m_psDetailTextureNames = NULL;
      m_psDecorationChannelNames = NULL;
      m_iDetailTextureCount = m_iDecorationChannelCount = 0;
    }

    ~TerrainClipboardObject()
    {
      V_SAFE_DELETE(m_pHeightValues);
      V_SAFE_DELETE_ARRAY(m_pDetailTextures);
      V_SAFE_DELETE_ARRAY(m_pDecorationChannels);
      V_SAFE_DELETE_ARRAY(m_psDetailTextureNames);
      V_SAFE_DELETE_ARRAY(m_psDecorationChannelNames);
    }

    // overridden ITerrainClipboardObject
    VOVERRIDE void OnRemovedFromClipboard()
    {
      V_SAFE_DELETE(m_pHeightValues);
      V_SAFE_DELETE_ARRAY(m_pDetailTextures);
      V_SAFE_DELETE_ARRAY(m_pDecorationChannels);
      V_SAFE_DELETE_ARRAY(m_psDetailTextureNames);
      V_SAFE_DELETE_ARRAY(m_psDecorationChannelNames);
    }

    // overridden ITerrainClipboardObject
    VOVERRIDE bool TryPaste(System::Object ^opHint) override;

    VTerrainLockObject* m_pHeightValues; // single object
    int m_iDetailTextureCount;
    VTerrainLockObject *m_pDetailTextures; // array of m_iDetailTextureCount objects
    VString *m_psDetailTextureNames;  // array of m_iDetailTextureCount objects. Used to address detail textures by name, not by ID (when pasting into a new scene)

    int m_iDecorationChannelCount;
    VTerrainLockObject *m_pDecorationChannels; // array of m_iDecorationChannelCount objects
    VString *m_psDecorationChannelNames;  // array of m_iDetailTextureCount objects. Used to address decoration by name, not by ID (when pasting into a new scene)
  };


  public ref class TerrainClipboardAction : public TerrainChangedBaseAction
  {
  public:
    TerrainClipboardAction(EngineInstanceTerrain ^pTerrain, String ^ActionName);
    VOVERRIDE void OnDispose() override;

    VOVERRIDE void Do() override;
    VOVERRIDE void Undo() override;

    // ISerializable Members
    TerrainClipboardAction(SerializationInfo ^info, StreamingContext context);
    VOVERRIDE void GetObjectData(SerializationInfo ^info, StreamingContext context) override;

    /*
    VOVERRIDE int get_SectorX1() override {return sx1;}
    VOVERRIDE int get_SectorY1() override {return sy1;}
    VOVERRIDE int get_SectorX2() override {return sx2;}
    VOVERRIDE int get_SectorY2() override {return sy2;}
    */
    VOVERRIDE property int SectorX1 { int get() override { return sx1; } }
    VOVERRIDE property int SectorY1 { int get() override { return sy1; } }
    VOVERRIDE property int SectorX2 { int get() override { return sx2; } }
    VOVERRIDE property int SectorY2 { int get() override { return sy2; } }

    void CopyTerrainData(VTerrainLockObject &targetRect, VTerrainLockObject &newData);
    void CopyDetailTextures(VTerrainLockObject *pNewArray);
    void CopyDecoration(VTerrainLockObject *pNewArray);

    int sx1,sy1,sx2,sy2;
    VEditableTerrain *m_pTerrain;

    VTerrainLockObject* m_pOldHeightValues;
    VTerrainLockObject* m_pNewHeightValues;

    int m_iDetailTextureCount;
    VTerrainLockObject *m_pOldDetailTextures;
    VTerrainLockObject *m_pNewDetailTextures;

    int m_iDecorationChannelCount;
    VTerrainLockObject *m_pOldDecorationChannels;
    VTerrainLockObject *m_pNewDecorationChannels;
  };
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
