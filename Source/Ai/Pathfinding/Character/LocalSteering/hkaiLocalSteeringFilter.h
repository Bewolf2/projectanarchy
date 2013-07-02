/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_AI_LOCAL_STEERING_FILTER_H
#define HK_AI_LOCAL_STEERING_FILTER_H

class hkaiCharacter;
class hkaiObstacleGenerator;

	/// A class used to enable/disable interactions between hkaiCharacters and hkaiObstacleGenerators during local steering
class hkaiLocalSteeringFilter : public hkReferencedObject
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

	hkaiLocalSteeringFilter();
	~hkaiLocalSteeringFilter();

		/// Called immediately after AABB overlap with hkaiCharacter is detected. If the method returns false, the hkaiCharacter is ignored.
	hkBool32 virtual isCharacterEnabled( const hkaiCharacter* currentCharacter, const hkaiCharacter* otherCharacter ) const = 0;

		/// Called immediately after AABB overlap with hkaiObstacleGenerator is detected. If the method returns false, the hkaiObstacleGenerator is ignored.
	hkBool32 virtual isObstacleEnabled( const hkaiCharacter* currentCharacter, const hkaiObstacleGenerator* otherObstacle) const = 0;
};
#endif // HK_AI_LOCAL_STEERING_FILTER_H

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
