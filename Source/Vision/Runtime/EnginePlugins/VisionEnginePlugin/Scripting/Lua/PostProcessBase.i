/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor VPostProcessingBaseComponent;
%nodefaultdtor VPostProcessingBaseComponent;

class VPostProcessingBaseComponent : public IVObjectComponent
{
public:

  virtual bool IsValid() const;
  virtual void SetActive(bool bStatus);
  inline bool IsActive() const;
};

VSWIG_CREATE_CONCAT(VPostProcessingBaseComponent, 128, "%s: %p", self->GetComponentName()==NULL?self->GetTypeId()->m_lpszClassName:self->GetComponentName(), self )
VSWIG_CREATE_TOSTRING(VPostProcessingBaseComponent, "%s (%s)", self->GetTypeId()->m_lpszClassName, self->IsActive() ? "Active" : "Inactive" )

#else


/// \brief Wrapper class of the post-processing base.
/// 
/// This represents the base class for post-processing effects in the Vision Engine. Post-processing effects are
/// components which can be attached to a renderer node. There is no limit regarding the number of post-processing
/// effects a single renderer node can have. Within a single renderer node, the post-processors are rendered in the
/// order of their respective render contexts' priorities. 
class VPostProcessingBaseComponent : public IVObjectComponent
{
public:

  /// \brief Returns whether the post processor is valid and may be activated.
  /// \return Returns true if valid, otherwise false.
  boolean IsValid();

  /// \brief Enables/disables this post-processor.
  /// \param status Set to true to activate the post processor, to false to disable.
  void SetActive(boolean status);

  /// \brief Returns whether this post-processor is currently enabled or disabled.
  /// \return Returns true if active, otherwise false.
  boolean IsActive();
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
