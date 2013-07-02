/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */



inline int VisParticleConstraintList_cl::GetConstraintCount() const 
{
  return m_iConstraintCount;
}


inline VisParticleConstraint_cl *VisParticleConstraintList_cl::GetConstraint(int iIndex) const
{
  VASSERT(iIndex>=0 && iIndex<m_iConstraintCount);
  return m_Constraint.GetDataPtr()[iIndex];
}



inline void VisParticleConstraintList_cl::HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, unsigned int iAffectMask)
{
  int i;
  VisParticleConstraint_cl **pConstraint = m_Constraint.GetDataPtr();
  VIS_CONSTRAINT_REFLECT_BEHAVIOR_e *pForceBehavior = m_ForceBehavior.GetDataPtr();
  int iCount = m_iConstraintCount;
  m_iConstraintCount = 0;
  for (i=0;i<iCount;i++)
  {
    if (!pConstraint[i]) continue;
    if (pConstraint[i]->IsFlaggedForRemoval())
    {
      pConstraint[i]->Release();
      pConstraint[i] = NULL;
      continue;
    }

    m_iConstraintCount = i+1;
    if (pConstraint[i]->IsActive() && (pConstraint[i]->m_iAffectBitMask & iAffectMask))
      pConstraint[i]->HandleParticles(pGroup,fTimeDelta,pForceBehavior[i]);
  }
}

inline void VisParticleConstraintList_cl::HandleParticlesNoRemove(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, unsigned int iAffectMask)
{
  int i;
  VisParticleConstraint_cl **pConstraint = m_Constraint.GetDataPtr();
  VIS_CONSTRAINT_REFLECT_BEHAVIOR_e *pForceBehavior = m_ForceBehavior.GetDataPtr();
  for (i=0;i<m_iConstraintCount;i++)
    if (pConstraint[i]!=NULL && pConstraint[i]->IsActive() && (pConstraint[i]->m_iAffectBitMask & iAffectMask))
      pConstraint[i]->HandleParticles(pGroup,fTimeDelta,pForceBehavior[i]);
}

inline void VisParticleConstraintList_cl::RenderConstraints(IVRenderInterface *pRenderer, bool bActiveOnly) const
{
  int i;
  VisParticleConstraint_cl **pConstraint = m_Constraint.GetDataPtr();
  for (i=0;i<m_iConstraintCount;i++)
  {
    if (!pConstraint[i]) continue;
    if (pConstraint[i]->IsFlaggedForRemoval()) continue;
    if (!bActiveOnly || pConstraint[i]->IsActive())
      pConstraint[i]->DebugRender(pRenderer, hkvVec3(0,0,0));
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
