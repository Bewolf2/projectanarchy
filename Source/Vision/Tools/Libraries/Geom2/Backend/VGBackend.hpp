/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Backend base class
class VGEOM2_IMPEXP_CLASS VGBackend
{
public:
  typedef VArray<VGProcessor*, VGProcessor*> ProcessorStack;


public:
  VGBackend()         { }
  ~VGBackend()        { }


public:
  /// \brief
  ///   Adds a pointer to a processor to backend's processor stack (run through prior to exporting)
  ///
  /// \param p
  ///   Pointer to processor to add
  inline void                   AddProcessor(VGProcessor* p)            { m_processorStack.Append(p); }

  /// \brief
  ///   Gets a reference to the backend's processor stack
  ///
  /// \return
  ///   Reference to the backend's processor stack
  inline const ProcessorStack&  GetProcessorStack() const throw()       { return m_processorStack; }

  /// \brief
  ///   Gets a reference to the backend's processor stack
  ///
  /// \return
  ///   Reference to the backend's processor stack
  inline void                   ResetProcessorStack()                   { m_processorStack.Reset(); }

  /// \brief
  ///   Loops over the processor stack and executes all processors
  ///
  /// \param scene
  ///   The scene
  ///
  /// \return
  ///   False if processing failed
  bool                          RunProcessors(VGScene& scene) const;



private:
  ProcessorStack m_processorStack;
};

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
