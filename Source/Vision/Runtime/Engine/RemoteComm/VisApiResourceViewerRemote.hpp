/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiResourceViewerRemote.hpp

#ifndef _VISAPIRESOURCEVIEWERREMOTE_HPP_INCLUDED
#define _VISAPIRESOURCEVIEWERREMOTE_HPP_INCLUDED

#include <Vision/Runtime/Engine/System/VisApiCallbacks.hpp>

/// \brief
///   The remote resource viewer which handles updates to the resource viewer.
class VisResourceViewerRemote : IVisCallbackHandler_cl
{
  public:

    /// \brief Default constructor. 
    VisResourceViewerRemote();

    /// \brief Destructor. 
    ~VisResourceViewerRemote();

    /// \brief Handles the callbacks by VTarget and the engine. 
    virtual void OnHandleCallback(IVisCallbackDataObject_cl* pData) HKV_OVERRIDE;

    /// \brief Registers callbacks with VTarget and the engine. 
    void RegisterCallbacks();

    /// \brief Unregisters callbacks with VTarget and the engine. 
    void UnregisterCallbacks();

    /// \brief 
    ///   Callback called by the engine when the engine wants to print a message. These messages will be sent over the remote connection and displayed in the remote debugger.
    ///
    /// \param szMessage The engine message.
    /// \param messageType Type of the message. 
    /// \param pUserData User-data pointer supplied back by the engine. In this case it is a pointer to the client.
    ///
    /// \return Returns whether the function succeeded.
    static BOOL EngineMessageCallback(const char* szMessage, VisApiMessageType_e messageType, void* pUserData);

  private:

    /// \brief  Updates the resource viewer. 
    void UpdateResourceViewer();

    friend class VResourceSystem_cl;

    /// Stores the connection of the resource viewer. 
    VConnection* m_pRemoteConnection;
    
    /// Stores percentage of loading progress so unnecessary updates are not performed. 
    float m_fOldPercentage;
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
