/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef vHavokProfiler_h__
#define vHavokProfiler_h__

#ifndef _VISION_DOC

class vHavokProfiler : public hkExternalJobProfiler
{
public:

  HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DEMO, vHavokProfiler);	

  vHavokProfiler(VProfilingNode* pGroup)
  {
    m_jobTypeStrings[HK_JOB_TYPE_DYNAMICS]                      =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_COLLIDE]                       =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_COLLISION_QUERY]               =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_RAYCAST_QUERY]                 =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_ANIMATION_SAMPLE_AND_COMBINE]  =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_ANIMATION_SAMPLE_AND_BLEND]    =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_ANIMATION_MAPPING]             =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_BEHAVIOR]                      =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_CLOTH]                         =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_AI_PATHFINDING]                =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_AI_VOLUME_PATHFINDING]         =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_AI_DYNAMIC]                    =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_AI_LOCAL_STEERING]             =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_AI_GENERATION]                 =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_DESTRUCTION]                   =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_UNIT_TEST]                     =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_CHARACTER_PROXY]               =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_VEHICLE]                       =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_COLLIDE_STATIC_COMPOUND]       =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_NP_CONSTRAINT_SETUP]           =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_NP_SOLVER]                     =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_NP_COLLIDE]                    =  Vision::Profiling.GetFreeElementID();
    m_jobTypeStrings[HK_JOB_TYPE_USER_0]                        =  Vision::Profiling.GetFreeElementID();

    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_DYNAMICS]                      , "Dynamics", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_COLLIDE]                       , "Collide", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_COLLISION_QUERY]               , "CollisionQuery", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_RAYCAST_QUERY]                 , "RayCastQuery", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_ANIMATION_SAMPLE_AND_COMBINE]  , "AnimSampleAndComb", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_ANIMATION_SAMPLE_AND_BLEND]    , "AnimSampleAndBlend", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_ANIMATION_MAPPING]             , "AnimMapping", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_BEHAVIOR]                      , "Behavior", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_CLOTH]                         , "Cloth", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_AI_PATHFINDING]                , "AiPathfinding", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_AI_VOLUME_PATHFINDING]         , "AiVolumePathfinding", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_AI_DYNAMIC]                    , "AiDynamic", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_AI_LOCAL_STEERING]             , "AiLocalSteering", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_AI_GENERATION]                 , "AiGeneration", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_DESTRUCTION]                   , "Destruction", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_UNIT_TEST]                     , "UnitTest", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_CHARACTER_PROXY]               , "CharacterProxy", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_VEHICLE]                       , "Vehicle", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_COLLIDE_STATIC_COMPOUND]       , "CollideStaticCompound", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_NP_CONSTRAINT_SETUP]           , "NpConstraintSetup", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_NP_SOLVER]                     , "NpSolver", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_NP_COLLIDE]                    , "NpCollide", TRUE, pGroup);
    Vision::Profiling.AddElement(m_jobTypeStrings[HK_JOB_TYPE_USER_0]                        , "User", TRUE, pGroup);
  }

#endif // _VISION_DOC

  virtual ~vHavokProfiler()
  {
  }

  inline int getTaskId( hkJobType t ) 
  {
    if (t < HK_JOB_TYPE_USER_0) 
      return m_jobTypeStrings[(int)t];
    else
      return m_jobTypeStrings[(int)HK_JOB_TYPE_USER_0];
  }

  virtual void onStartJob( hkJobType t, hkUint32 jobSubType ) HK_OVERRIDE
  {
    VISION_START_PROFILING(getTaskId(t));
  }

  virtual void onEndJob( hkJobType t ) HK_OVERRIDE
  {
    VISION_STOP_PROFILING(getTaskId(t));
  }

  int m_jobTypeStrings[HK_JOB_TYPE_HAVOK_MAX + 1];
};

HK_COMPILE_TIME_ASSERT(HK_JOB_TYPE_DYNAMICS == 0);
HK_COMPILE_TIME_ASSERT(HK_JOB_TYPE_HAVOK_MAX == 22); // If this fails then there are new job types to add to the above table

#endif // vHavokProfiler_h__

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
