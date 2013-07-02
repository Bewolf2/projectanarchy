/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_ATTACHMENT_H__
#define RPG_PLUGIN_ATTACHMENT_H__

class VSkeletalBoneProxyObject;

class RPG_Attachment : public VRefCounter
{
public:
  RPG_Attachment(VisBaseEntity_cl* parent);
  virtual ~RPG_Attachment();

  void Attach(VisObject3D_cl* object, const VString& boneName, const hkvVec3& positionOffset = hkvVec3(0.f, 0.f, 0.f), const hkvVec3& orientationOffset = hkvVec3(0.f, 0.f, 0.f));
  void Detach(bool disposeObject);

protected:
  VString m_boneName;
  hkvVec3 m_positionOffset;
  hkvVec3 m_orientationOffset;

  VSkeletalBoneProxyObject* m_proxy;
  VisObject3D_cl* m_object;
  VisBaseEntity_cl* m_parent;
};

#endif // RPG_PLUGIN_ATTACHMENT_H__

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
