/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once
#ifndef CUSTOM_VOLUME_OBJECT_HPP
#define CUSTOM_VOLUME_OBJECT_HPP

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

class VisStaticMesh_cl;

/// \brief a custom volume object that can be defined by the user
class VCustomVolumeObject : public VisObject3D_cl, public IVisCallbackHandler_cl
{
private:
  enum VVersion_e 
  {
    VV_Version1 = 1,
    VV_CurrentVersion = VV_Version1
  };

  VStaticString<FS_MAX_PATH> m_sStaticMeshPath;
  VisStaticMeshPtr m_spStaticMesh;
  bool m_bCustomStaticMesh;
  bool m_bCreatedFromEditor;
  hkvVec3 m_vScale;
  int m_iManagerIndex;

  //copying not allowed
  VCustomVolumeObject(const VCustomVolumeObject&);
  void operator=(const VCustomVolumeObject&);

public:

  /// \brief constructor
  EFFECTS_IMPEXP VCustomVolumeObject();
  /// \brief destructor
  EFFECTS_IMPEXP virtual ~VCustomVolumeObject();

  /// \brief disposes the object
  EFFECTS_IMPEXP virtual void DisposeObject() HKV_OVERRIDE;

  /// \brief
  ///   init function. Needs to be called manually when dynamically creating an object of this type
  EFFECTS_IMPEXP VOVERRIDE void Init();

  /// \brief overrides the OnHandleCallback function of IVisCallbackHandler_cl
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  //serialization
  V_DECLARE_SERIAL_DLLEXP( VCustomVolumeObject,  EFFECTS_IMPEXP );
  /// \brief serialization
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;
  /// \brief for initialization after deserialization
  EFFECTS_IMPEXP virtual void OnSerialized( VArchive& ar ) HKV_OVERRIDE;

  /// \brief returns the static mesh in use
  inline VisStaticMesh_cl* GetStaticMesh() { return m_spStaticMesh.GetPtr(); }

  /// \brief 
  ///   sets the path of the static mesh to be used
  ///
  /// \param szPath
  ///   
  EFFECTS_IMPEXP void SetStaticMeshPath(const char* szPath);

  /// \brief returns if this volume uses a custom static mesh (.vmesh file) or not  
  inline bool GetCustomStaticMesh() const { return m_bCustomStaticMesh; }
  /// \brief sets if this volume uses a custom static mesh (.vmesh file) or not
  EFFECTS_IMPEXP void SetCustomStaticMesh(bool bValue);

  /// \brief sets the scale for this volume
  inline void SetScale(hkvVec3 vScale) { m_vScale = vScale; }
  /// \brief gets the scale for this volume
  inline hkvVec3 GetScale() const { return m_vScale; }

  /// \brief sets the loaded from editor flag
  EFFECTS_IMPEXP void SetCreatedFromEditor();

  IMPLEMENT_OBJ_CLASS(VCustomVolumeObject)

private:
  void LoadStaticMesh();
  void Deregister();
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
