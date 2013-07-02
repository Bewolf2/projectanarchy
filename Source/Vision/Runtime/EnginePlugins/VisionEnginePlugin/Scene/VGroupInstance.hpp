/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef _VISION_DOC

class VGroupInstance
{
public:

  VGroupInstance()
  {
    m_fScale = 1.f;
    m_fConeAngleX = m_fConeAngleY = m_fFarClip = m_fSeed = 0.f;
    m_iSplitKey = 0;
    m_iAnimationIndex = -1;
    m_fAnimationOffset = 0.f;
    m_iColor.SetRGBA(255,255,255,255);
  }

  enum AvailableFlags_e
  {
    INSTANCE_UNDEFINED  = 0,
    INSTANCE_POSITION   = V_BIT(0),
    INSTANCE_ROTATION   = V_BIT(1),
    INSTANCE_SCALE      = V_BIT(2),
    INSTANCE_COLOR      = V_BIT(3),
    INSTANCE_ANIMATION  = V_BIT(4),
    INSTANCE_FARCLIP    = V_BIT(5),
    INSTANCE_SEED       = V_BIT(6),
    INSTANCE_SPLITKEY   = V_BIT(7),
    INSTANCE_CONEANGLE  = V_BIT(8),

    // predefined
    INSTANCE_TYPE_LIGHTPOINT  = INSTANCE_POSITION|INSTANCE_ROTATION|INSTANCE_SCALE|INSTANCE_COLOR|INSTANCE_ANIMATION|INSTANCE_FARCLIP|INSTANCE_SEED|INSTANCE_CONEANGLE|INSTANCE_SPLITKEY,
    INSTANCE_TYPE_DECORATION  = INSTANCE_POSITION|INSTANCE_ROTATION|INSTANCE_SCALE|INSTANCE_SEED|INSTANCE_SPLITKEY|INSTANCE_COLOR,
    INSTANCE_TYPE_SPEEDTREE6  = INSTANCE_POSITION|INSTANCE_ROTATION|INSTANCE_SCALE|INSTANCE_SEED|INSTANCE_SPLITKEY

  };

  inline void Read(IVFileInStream *pIn, AvailableFlags_e eFlags);
  inline void Write(IVFileOutStream *pOut, AvailableFlags_e eFlags) const;

// members
  hkvVec3 m_vPosition;    ///< Position
  hkvMat3 m_Rotation;     ///< Rotation matrix
  float m_fScale;         ///< Size for light points
  VColorRef m_iColor;     ///< Tint color
  float m_fConeAngleX, m_fConeAngleY; ///< For light points
  short m_iAnimationIndex;  ///< for lightpoints: Index of animation templates (-1 for no animation)
  float m_fAnimationOffset; ///< offset phase [0..1] of the animation
  float m_fFarClip;       ///< m_fVisibilityRange for light points
  float m_fSeed;          ///< m_fActivationThreshold for light points
  int m_iSplitKey;        ///< arbitrary int value to define splits into new sub-groups (vegetation models)
};


class VGroupInstanceFile
{
public:
  inline VGroupInstanceFile();
  inline void Open(IVFileInStream *pIn, int &iInstanceCount, VGroupInstance::AvailableFlags_e &eFlags);
  inline void Create(IVFileOutStream *pOut, int iInstanceCount, VGroupInstance::AvailableFlags_e eFlags);

  inline void WriteInstance(const VGroupInstance &instance);
  inline void ReadInstance(VGroupInstance &instance);

  inline void Close(bool bCloseStream=true);
protected:
  int m_iInstanceCount;
  VGroupInstance::AvailableFlags_e m_eFlags;
  IVFileInStream *m_pIn;
  IVFileOutStream *m_pOut;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline VGroupInstanceFile::VGroupInstanceFile() 
{
  m_iInstanceCount = 0;
  m_eFlags = VGroupInstance::INSTANCE_UNDEFINED;
  m_pIn = NULL;
  m_pOut = NULL;
}


inline void VGroupInstance::Read(IVFileInStream *pIn, AvailableFlags_e eFlags)
{
  if ((eFlags&INSTANCE_POSITION)>0) pIn->Read(&m_vPosition,sizeof(m_vPosition),"fff");
  if ((eFlags&INSTANCE_ROTATION)>0) pIn->Read(m_Rotation.getPointer(),sizeof(m_Rotation),"9f");
  if ((eFlags&INSTANCE_SCALE)>0) pIn->Read(&m_fScale,sizeof(m_fScale),"f");
  if ((eFlags&INSTANCE_COLOR)>0) {pIn->Read(&m_iColor.r,1);pIn->Read(&m_iColor.g,1);pIn->Read(&m_iColor.b,1);pIn->Read(&m_iColor.a,1);}
  if ((eFlags&INSTANCE_ANIMATION)>0) {pIn->Read(&m_iAnimationIndex,sizeof(m_iAnimationIndex),"s");pIn->Read(&m_fAnimationOffset,sizeof(m_fAnimationOffset),"f");}
  if ((eFlags&INSTANCE_FARCLIP)>0) pIn->Read(&m_fFarClip,sizeof(m_fFarClip),"f");
  if ((eFlags&INSTANCE_SEED)>0) pIn->Read(&m_fSeed,sizeof(m_fSeed),"f");
  if ((eFlags&INSTANCE_SPLITKEY)>0) pIn->Read(&m_iSplitKey,sizeof(m_iSplitKey),"i");
  if ((eFlags&INSTANCE_CONEANGLE)>0) {pIn->Read(&m_fConeAngleX,sizeof(m_fConeAngleX),"f");pIn->Read(&m_fConeAngleY,sizeof(m_fConeAngleY),"f");}
}

inline void VGroupInstance::Write(IVFileOutStream *pOut, AvailableFlags_e eFlags) const
{
  if ((eFlags&INSTANCE_POSITION)>0) pOut->Write(&m_vPosition,sizeof(m_vPosition),"fff");
  if ((eFlags&INSTANCE_ROTATION)>0) pOut->Write(m_Rotation.getPointer(),sizeof(m_Rotation),"9f");
  if ((eFlags&INSTANCE_SCALE)>0) pOut->Write(&m_fScale,sizeof(m_fScale),"f");
  if ((eFlags&INSTANCE_COLOR)>0) {pOut->Write(&m_iColor.r,1);pOut->Write(&m_iColor.g,1);pOut->Write(&m_iColor.b,1);pOut->Write(&m_iColor.a,1);}
  if ((eFlags&INSTANCE_ANIMATION)>0) {pOut->Write(&m_iAnimationIndex,sizeof(m_iAnimationIndex),"s");pOut->Write(&m_fAnimationOffset,sizeof(m_fAnimationOffset),"f");}
  if ((eFlags&INSTANCE_FARCLIP)>0) pOut->Write(&m_fFarClip,sizeof(m_fFarClip),"f");
  if ((eFlags&INSTANCE_SEED)>0) pOut->Write(&m_fSeed,sizeof(m_fSeed),"f");
  if ((eFlags&INSTANCE_SPLITKEY)>0) pOut->Write(&m_iSplitKey,sizeof(m_iSplitKey),"i");
  if ((eFlags&INSTANCE_CONEANGLE)>0) {pOut->Write(&m_fConeAngleX,sizeof(m_fConeAngleX),"f");pOut->Write(&m_fConeAngleY,sizeof(m_fConeAngleY),"f");}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


inline void VGroupInstanceFile::Open(IVFileInStream *pIn, int &iInstanceCount, VGroupInstance::AvailableFlags_e &eFlags)
{
  VASSERT(pIn!=NULL);
  m_pIn = pIn;
  char iLocalVersion = 0;
  pIn->Read(&iLocalVersion,1);
  VASSERT_MSG(iLocalVersion==0, "invalid version number");
  pIn->Read(&m_iInstanceCount,sizeof(m_iInstanceCount),"i");
  pIn->Read(&m_eFlags,sizeof(m_eFlags),"i");
  iInstanceCount = m_iInstanceCount;
  eFlags = m_eFlags;
}

inline void VGroupInstanceFile::Create(IVFileOutStream *pOut, int iInstanceCount, VGroupInstance::AvailableFlags_e eFlags)
{
  VASSERT(pOut!=NULL);
  m_pOut = pOut;
  m_iInstanceCount = iInstanceCount;
  m_eFlags = eFlags;
  char iLocalVersion = 0;
  pOut->Write(&iLocalVersion,1);
  pOut->Write(&m_iInstanceCount,sizeof(m_iInstanceCount),"i");
  pOut->Write(&m_eFlags,sizeof(m_eFlags),"i");  
}


inline void VGroupInstanceFile::WriteInstance(const VGroupInstance &instance)
{
  VASSERT_MSG(m_iInstanceCount>0, "written too many instances");
  m_iInstanceCount--;
  instance.Write(m_pOut,m_eFlags);
}

inline void VGroupInstanceFile::ReadInstance(VGroupInstance &instance)
{
  VASSERT_MSG(m_iInstanceCount>0, "read too many instances");
  m_iInstanceCount--;
  instance.Read(m_pIn,m_eFlags);
}

inline void VGroupInstanceFile::Close(bool bCloseStream)
{
  VASSERT_MSG(m_iInstanceCount==0, "mismatch number of instances read or written");
  if (m_pIn) {if (bCloseStream) m_pIn->Close();m_pIn=NULL;}
  if (m_pOut) {if (bCloseStream) m_pOut->Close();m_pOut=NULL;}
  m_iInstanceCount = 0;
  m_eFlags = VGroupInstance::INSTANCE_UNDEFINED;
}

#endif // _VISION_DOC

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
