/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VSoftKeyboardAdapter.hpp

#ifndef VSOFTKEYBOARDADAPTER_HPP_INCLUDED
#define VSOFTKEYBOARDADAPTER_HPP_INCLUDED

/// \brief
///   This interface represents something which can handle input by virtual keyboards
///   By default this is the VTextControl for example but others can be implemented
class IVSoftKeyboardRecipient
{
  public:
  
    /// \brief Inserts text in the recipient - note that this is UTF8 encoded text and may contain more than one char
    ///
    /// \param pUTF8Text the entered character or text
    virtual void EnterText(const char* pUTF8Text) = 0;
  
    /// \brief Should return true if the recipient already has some text, false otherwise
    virtual bool HasText() = 0;

    /// \brief Should return true if the recipient already has some text, false otherwise
    virtual const char* GetUTF8Text() const { return NULL; }

    /// \brief Get a description of the recipient
    virtual const char* GetDescription() const { return NULL; }
  
    virtual bool IsPassword() const { return false; }

    /// \brief Called when a special key is pressed
    ///        (VGLK_BACKSP, VK_LEFT, VK_RIGHT, VK_HOME, VK_END, VK_DELETE)
    ///
    /// \param uiKey The special key which was pressed (Not all softkeyboards support all special keys)
    virtual void OnSpecialKey(unsigned int uiKey) = 0;
  
};

/// \brief
///   Represents an adapter to work with different softkeyboards one implementation is the VIOSSoftkeyboardAdapter
///   which handles input by the iOS virtual keyboard - you can get the current adapter on platforms which
///   support soft keyboards (define SUPPORTS_SOFTKEYBOARD) via VInputManager::GetSoftkeyboardAdapter()
class IVSoftKeyboardAdapter
{
  public:
  
    /// \brief Constructor
    IVSoftKeyboardAdapter()
      : m_pCurrentRecipient(NULL)
    {
    }
  
    /// \brief Destructor
    virtual ~IVSoftKeyboardAdapter()
    {
      m_pCurrentRecipient = NULL;
    }
  
    /// \brief Shows the virtual keyboard
    virtual void Show() = 0;
  
    /// \brief Hides the virtual keyboard
    virtual void Hide() = 0;
  
    /// \brief Sets the current recipient of the input (Used by the focus change of the VTextControl for example)
    ///
    /// \param pRecipient The current recipient of the softkeyboard (e.g. a VTextControl)
    void SetCurrentRecipient(IVSoftKeyboardRecipient* pRecipient)
    {
      m_pCurrentRecipient = pRecipient;
    }
  
    /// \brief Returns the current recipient of the input
    IVSoftKeyboardRecipient* GetCurrentRecipient()
    {
      return m_pCurrentRecipient;
    }
  
  protected:
  
    IVSoftKeyboardRecipient* m_pCurrentRecipient;
  
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
