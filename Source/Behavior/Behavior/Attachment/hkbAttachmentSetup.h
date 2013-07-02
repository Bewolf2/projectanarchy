/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_ATTACHMENT_SETUP_H
#define HK_ATTACHMENT_SETUP_H


extern const class hkClass hkbAttachmentSetupClass;

/// The template for an attachment scenario.  These are referenced objects
/// so that you can choose how to manage the memory for them.
class hkbAttachmentSetup : public hkReferencedObject
{
	public:
		//+version(2)
		//+vtable(1)
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Set to default values.
		hkbAttachmentSetup();

			/// Copy constructor
		hkbAttachmentSetup( const hkbAttachmentSetup& setup );

			/// How the attachment will be implemented.
		enum AttachmentType
		{
				/// Keyframe the attached rigid body.
			ATTACHMENT_TYPE_KEYFRAME_RIGID_BODY,

				/// Use a ball and socket constraint.
			ATTACHMENT_TYPE_BALL_SOCKET_CONSTRAINT,

				/// Use a ragdoll constraint.
			ATTACHMENT_TYPE_RAGDOLL_CONSTRAINT,

				/// Set the world-from-model transform of the attachee.
			ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL,

				/// No attachment will be done.
			ATTACHMENT_TYPE_NONE,
		};

			/// The attachment is blended in over this many seconds.  This is only used with ATTACHMENT_TYPE_KEYFRAME_RIGID_BODY or ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL.
		hkReal m_blendInTime;	//+default(0.0)
								//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")
								//+hk.Description("The attachment is blended in over this many seconds. This is only used with ATTACHMENT_TYPE_KEYFRAME_RIGID_BODY or ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL.")

			/// How much to move the attacher instead of the attachee.
			/// 
			/// During the blend time, when the attacher and the attachee are being
			/// moved to each other, you can choose how much to move each one.
			/// Normally, you'll want this set to the default of zero so that the
			/// attachee is moved to the attacher.  But for dual character interactions,
			/// you may want to set this to 0.5 so that both characters move equally.
			/// This is only used when m_attachmentType is ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL.
		hkReal m_moveAttacherFraction;	//+default(0.0)
										//+hk.RangeReal(absmin=0.0,absmax=1.0)
										//+hk.Description("How much to move the attacher instead of the attachee. During the blend time, when the attacher and the attachee are being moved into registration, you can choose how much to move each one. Normally, you'll want this set to the default of zero so that the attachee is moved to the attacher.  But for dual character interactions, you may want to set this to 0.5 so that both characters move equally. Note that this is only used when using ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL.")

			/// Of the motion that is required to enforce the constraint, only this
			/// fraction will be applied at each frame.  This is after the initial blend in period.
			/// During the blend, whatever fraction is implied by m_blendInTime is used.
			/// Note the values larger than the default of 0.3 can cause oscillations.
			/// This is only used when using ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL, in which case it must be non-zero.
		hkReal m_gain;	//+default(0.3f)
						//+hk.RangeReal(absmin=0.0,absmax=1.0)
						//+hk.Description("Of the motion that is required to enforce the constraint, only this fraction will be applied at each frame.  This is after the initial blend in period. During the blend, whatever fraction is implied by m_blendInTime is used. Note the values larger than the default of 0.3 can cause oscillations. This is only used when using ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL, in which case it must be non-zero.")

			/// If the attacher or the attachee has a corresponding rigid body, then extrapolation can be used to
			/// estimate the configuration of the rigid body after the physics step by setting this to the simulation timestep.
			/// This is only used when the AttachmentType is ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL.
		hkReal m_extrapolationTimeStep; //+default(0.0f)
										//+hk.Description("If the attacher or the attachee has a corresponding rigid body, then extrapolation can be used to estimate the configuration of the rigid body after the physics step by setting this to the simulation timestep. This is only used when using ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL.")

			/// When doing attachment fixup we use this gain.
			/// This is only used when using ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL.
		hkReal m_fixUpGain; //+default(0.0f)
							//+hk.RangeReal(absmin=0.0,absmax=1.0)
							//+hk.Description("When doing attachment fixup we use this gain.")

			/// The maximum linear distance between the attacher and the attachee cannot be more than m_maxLinearDistance. 
			/// This is used only during fixup and only when using ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL.
		hkReal m_maxLinearDistance; //+default(1.0f)
									//+hk.Description("The maximum linear distance between the attacher and the attachee cannot be more than m_maxLinearDistance. This is used only during fixup and only when using ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL.")

			/// The maximum angular distance between the attacher and the attachee cannot be more than m_maxAngularDistance. 
			/// This is used only during fixup and only when usingATTACHMENT_TYPE_SET_WORLD_FROM_MODEL.
		hkReal m_maxAngularDistance;	//+default(180.0f)
										//+hk.RangeReal(absmin=0.0,absmax=180.0)
										//+hk.Description("The maximum angular distance between the attacher and the attachee cannot be more than m_maxAngularDistance. This is used only during fixup and only when using ATTACHMENT_TYPE_SET_WORLD_FROM_MODEL.")

			/// How to enforce the attachment.
		hkEnum<AttachmentType,hkInt8> m_attachmentType;	//+default(hkbAttachmentSetup::ATTACHMENT_TYPE_KEYFRAME_RIGID_BODY)
														//+hk.Description("How to enforce the attachment.")

	public:

		hkbAttachmentSetup( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag) {}

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
