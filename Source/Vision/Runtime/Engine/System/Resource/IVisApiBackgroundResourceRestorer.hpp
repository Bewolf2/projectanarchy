/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file IVisApiBackgroundResourceRestorer.hpp

#ifndef IVISAPIBACKGROUNDRESOURCERESTORER_HPP_INCLUDED
#define IVISAPIBACKGROUNDRESOURCERESTORER_HPP_INCLUDED

//-----------------------------------------------------------------------------------

/// \brief
///   Handles the resource restoration process when the application is restored after
///   having been sent to the background.
///
/// An object of this type is passed in the data object of the Vision::Callbacks.OnBackgroundRestore
/// callback.
class IVisBackgroundResourceRestorer_cl
{
public:
  virtual ~IVisBackgroundResourceRestorer_cl(){}

  /// \brief
  ///   Checks if the resource restoration process after leaving the background state
  ///   has finished.
  ///
  /// After resource restoration has finished the Run() function can be called again.
  virtual bool IsFinished() const = 0;

  /// \brief
  ///   Proceeds with restoring the resources.
  ///
  /// Should be called every frame if a resource restoration process is in progress.
  virtual void Tick() = 0;
};

//-----------------------------------------------------------------------------------

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
