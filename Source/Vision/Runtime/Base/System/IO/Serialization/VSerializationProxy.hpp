/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VSerializationProxy.hpp

#ifndef VSERIALIZATIONPROXY_HPP_INCLUDED
#define VSERIALIZATIONPROXY_HPP_INCLUDED

#include <Vision/Runtime/Base/Container/VCollection.hpp>
#include <Vision/Runtime/Base/System/IO/Stream/IVFileStream.hpp>

class IVSerializationProxyCreator;
class IVSerializationProxy;


/// \brief
///   Interface of a serialization proxy object
/// 
/// A proxy object is a typed object that performs full serialization and creates the real object
/// in the serialization code.
/// 
/// This mechanism is well suited for serializing shared resources, because full serialization only
/// shares the pointer references inside the binary archive. For example, if an entity model should
/// be de-serialized, the deserialization should look for an already loaded model in the resource
/// manager.
/// 
/// This class consists of a single virtual overridable, GetInstance(). This function is called in
/// VArchive::ReadProxyObject to get to the real object. The GetInstance() implementation could for
/// instance perform a lookup into the resource manager before actually creating a new resource.
/// 
/// \sa VArchive
/// \sa VArchive::WriteProxyObject
/// \sa VArchive::ReadProxyObject
/// \sa IVSerializationProxyCreator
class IVSerializationProxy : public VTypedObject, public VRefCounter
{
public:

  /// \brief
  ///   Overridable that returns the actual instance of the deserialized object
  /// 
  /// The return value of this function is returned by the VArchive::ReadProxyObject function.
  /// 
  /// This function is the counterpart to the IVSerializationProxyCreator::CreateProxy function.
  /// 
  /// \return
  ///   IVSerializationProxyCreator *pObj: pointer to the actual object.
  /// 
  /// \sa IVSerializationProxyCreator
  /// \sa IVSerializationProxyCreator::CreateProxy
  /// \sa VArchive::WriteProxyObject
  /// \sa VArchive::ReadProxyObject
  virtual IVSerializationProxyCreator *GetInstance() = 0;
};


/// \brief
///   Interface of a serialization proxy creator
/// 
/// A proxy creator is a pure interface that allows to create a proxy object through its one and
/// only CreateProxy() function.
/// 
/// This allows to share resources in an archive with resources that have been loaded already and
/// even allows to serialize objects that are not derived from VTypedObject.
/// 
/// If the resources would deserialize themselfes directly, unique resource instances would be
/// created even if the resource has already been loaded before the archive has been opened.
class IVSerializationProxyCreator
{
public:
  VBASE_IMPEXP virtual ~IVSerializationProxyCreator() {}

  /// \brief
  ///   Overridable that returns a proxy object that is able to serialize this class
  /// 
  /// The VArchive::WriteProxyObject function calls this function to gather a proxy object for
  /// serialization.
  /// 
  /// Since the IVSerializationProxy class is reference counted, the VArchive class takes care of
  /// releasing all proxy object instances that have been created during a serialization session.
  /// 
  /// This function is the counterpart to the IVSerializationProxy::GetInstance function.
  /// 
  /// \return
  ///   IVSerializationProxy *pObj: New proxy object
  /// 
  /// \note
  ///   The VManagedResource class is derived from IVSerializationProxyCreator because shared
  ///   resources (models, animation files, bitmaps ...) rely on this feature.
  /// 
  /// \sa IVSerializationProxy
  /// \sa IVSerializationProxy::GetInstance
  /// \sa VArchive::WriteProxyObject
  /// \sa VArchive::ReadProxyObject
  /// 
  /// \example
  ///   \code
  ///   virtual IVSerializationProxy *CreateProxy() {return new VMyClassSerializationProxy(this);} // don't care about freeing
  ///   \endcode
  VBASE_IMPEXP virtual IVSerializationProxy *CreateProxy() = 0;
};


/// \brief
///   Collection of reference counted proxy objects. Used by VArchive class to clean up.
class VSerializationProxyCollection : public VRefCountedCollection<IVSerializationProxy>
{
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
