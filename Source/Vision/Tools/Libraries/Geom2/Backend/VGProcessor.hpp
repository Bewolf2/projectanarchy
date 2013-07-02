/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once



/// \brief
///   Interface for configurable classes
class VGEOM2_IMPEXP_CLASS VGConfigurable
{
public:
  VGConfigurable()                          { }
  virtual ~VGConfigurable()                 { }

public:
  //virtual void    GetConfigName() = 0;

  //virtual VString GetOptions() = 0;
  //virtual void    SetOptions() = 0;
};



/// \brief
///   Abstract mesh processor base class
///
/// Describes interface for geometry processors to be used to preprocess a mesh. Processors
/// can be used manually, but are usually stacked in a backend, running the scene through it prior
/// to writing out the mesh data.
class VGEOM2_IMPEXP_CLASS VGProcessor : public VGConfigurable
{
public:
  VGProcessor()                         : m_log(NULL) { }
  VGProcessor(const VGProcessor& other) : m_log(NULL) { }
  virtual ~VGProcessor()                              { }



  /// \brief
  ///   Processes a mesh - this might alter the passed-in mesh, of course
  ///
  /// \param scene
  ///   Mesh containing data to preprocess
  ///
  /// \return
  ///   False if preprocessing failed
  virtual bool  Process(VGScene& scene) const = 0;



  /// \brief
  ///   Sets a pointer to a log used for logging
  ///
  /// \param log
  ///   Pointer to log to be used
  ///
  /// \return
  ///   Pointer to previously used log
  inline IVLog* SetLog(IVLog* log)                    { IVLog* oldLog = m_log; m_log = log; return oldLog; }

  /// \brief
  ///   Gets pointer to log currently in use
  ///
  /// \return
  ///   Pointer to log currently in use
  inline IVLog* GetLog() const                        { return m_log; }


private:
  IVLog* m_log;
};



/// \brief
///   Helper struct to lookup all processor types available for creation
struct VGAvailableProcessors
{
  /// \brief
  ///   Helper struct to create and destroy dynamic processors
  struct ProcessorCreator
  {
    virtual VGProcessor* CreateInstance() const = 0;
    virtual void         DestroyInstance(VGProcessor* p) const = 0;
    virtual const char*  GetProcessorName() const = 0;
  };

  static inline void RegisterProcessor(const ProcessorCreator& c)   { c.GetProcessorName(); }
  //@@@ continue implementing...
};


/// \brief
///   Processor base class for processors that can be created dynamically
template<class CRTP>
class VGEOM2_IMPEXP_CLASS VGDynamicProcessor : public VGProcessor
{
public:
  VGDynamicProcessor()            { }
  virtual ~VGDynamicProcessor()   { }

  /// \brief
  ///   Helper struct to create and destroy dynamic processors
  struct Creator : public VGAvailableProcessors::ProcessorCreator
  {
    Creator()                                                   { VGAvailableProcessors::RegisterProcessor(*this); }
    virtual VGProcessor* CreateInstance() const                 { return new CRTP(); }
    virtual void         DestroyInstance(VGProcessor* p) const  { delete p; }
    virtual const char*  GetProcessorName() const;
  };

private:
  static Creator ms_creator;
};



// All vGeom2 processors that should have reflection support, should use this macro in their translation unit.
#define VG2_DEFINE_PROCESSOR(className) \
  const char* VGDynamicProcessor<className>::Creator::GetProcessorName() const { return #className; } \
  VGDynamicProcessor<className>::Creator VGDynamicProcessor<className>::ms_creator;

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
