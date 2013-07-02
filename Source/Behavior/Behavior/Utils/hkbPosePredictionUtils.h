/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_POSE_PREDICTION_UTILS_H
#define HKB_POSE_PREDICTION_UTILS_H

class hkbCharacter;

	/// A set of utilities that simplify the prediction of future pose related quantities
class hkbPosePredictionUtils
{
public:

	/// Predict the characters future worldFromModel.
	static void predictWorldFromModelWithBone(	const hkbCharacter& character, 
												hkReal timeOffset, 
												hkQsTransform& predictedWorldFromModel,
												hkInt32 boneIndex,
												hkQsTransform* boneFromModel);

		/// Predict the characters future worldFromModel.
	static void predictWorldFromModel(	const hkbCharacter& character, 
										hkReal timeOffset, 
										hkQsTransform& predictedWorldFromModel);

		/// Predict the error between the future worldFromModel, and a desired transform.
		/// If you multiply the worldFromModel on the right by the predicted error, the result should be the desired transform.
	static void predictWorldFromModelError(	const hkbCharacter& character, 
											hkReal timeOffset, 
											const hkQsTransform& desiredTransform, 
											const hkQsTransform& modelSpaceOffset,
											hkQsTransform& predictedError,
											hkQsTransform* predictedWorldFromModelOut = HK_NULL);

		/// Predict the incremental error between the future worldFromModel, and a desired transform, given a blend parameter.
		/// If you multiply the worldFromModel on the right by the blended error, the result should be the appropriate blend
		/// from the worldFromModel to the desired transform
	static void predictWorldFromModelIncrementalError(	const hkbCharacter& character, 
													hkReal timeOffset, 
													hkReal blendValue,
													const hkQsTransform& desiredTransform, 
													const hkQsTransform& modelSpaceOffset,
													hkQsTransform& blendedError,
													hkQsTransform* predictedWorldFromModelOut = HK_NULL);
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
