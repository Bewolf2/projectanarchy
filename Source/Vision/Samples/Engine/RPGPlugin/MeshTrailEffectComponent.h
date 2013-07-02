/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_MESH_TRAIL_EFFECT_COMPONENT_H__
#define RPG_PLUGIN_MESH_TRAIL_EFFECT_COMPONENT_H__

struct VTrailHistoryEntry
{
  hkvVec3 m_start;
  hkvVec3 m_end;
  float m_time;
};

///
/// \brief
///   Trail effect that can be used for weapon trail, bullet trails, etc. The component generates a mesh 
///   on the fly that fades out over time. The entity that gets this component assigned needs a skeleton 
///   with a 'trail_start' and 'trail_end' named bones that define the trail spawn region.
/// 
class RPG_MeshTrailEffectComponent : public IVObjectComponent
{
public:

  RPG_MeshTrailEffectComponent(const char* textureFilename, const VColorRef color, const float lifetime, const int numSegments);  
  RPG_MeshTrailEffectComponent();
  ~RPG_MeshTrailEffectComponent();

#ifdef SUPPORTS_SNAPSHOT_CREATION
  void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif

  void SetEnabled(bool bStatus);
  void ResetTrail();
  void PerFrameUpdate();

private:

  BOOL CanAttachToObject(VisTypedEngineObject_cl *object, VString& errorOut) HKV_OVERRIDE;
  void SetOwner(VisTypedEngineObject_cl *owner) HKV_OVERRIDE;
  void Serialize(VArchive& ar) HKV_OVERRIDE;

  void UpdateHistory(float fAbsTime);
  void UpdateMesh(float fAbsTime);

private:

  VString m_textureFilename;
  VColorRef m_color;
  float m_lifetime;
  int m_numSegments;

  bool m_enabled;
  int m_historyPos;
  int m_historyCount;
  VTrailHistoryEntry *m_history;

  VisMeshBufferPtr m_mesh;
  VisMeshBufferObjectPtr m_meshObj;

  hkvVec3 m_relStart;
  hkvVec3 m_relEnd;

  VString m_startBoneName;
  VString m_endBoneName;

  V_DECLARE_SERIAL_DLLEXP(RPG_MeshTrailEffectComponent, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_MeshTrailEffectComponent, RPG_PLUGIN_IMPEXP);
};

typedef VSmartPtr<RPG_MeshTrailEffectComponent> RPG_MeshTrailEffectComponentPtr;
typedef VRefCountedCollection<RPG_MeshTrailEffectComponent> RPG_MeshTrailEffectComponentCollection;

class RPG_MeshTrailEffectComponentManager : public IVisCallbackHandler_cl
{
public:

  void OneTimeInit()
  {
    Vision::Callbacks.OnUpdateSceneFinished += this; 
  }

  void OneTimeDeInit()
  {
    Vision::Callbacks.OnUpdateSceneFinished -= this;
  }

  void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;
  
  inline RPG_MeshTrailEffectComponentCollection &Instances() { return m_components; }

public:

  static RPG_MeshTrailEffectComponentManager s_instance;
  
protected:

  RPG_MeshTrailEffectComponentCollection m_components;
  
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
