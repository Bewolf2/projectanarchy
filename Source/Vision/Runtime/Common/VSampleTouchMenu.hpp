/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VSampleTouchMenu.hpp

#ifndef DEFINE_VSAMPLETOUCHMENU
#define DEFINE_VSAMPLETOUCHMENU

#include <Vision/Runtime/Base/Container/VArray.hpp>

class VTouchArea;
class VSampleTouchMenu;
class VInputMap;

/// \brief
///   Base class for buttons in the sample touch menu.
class VSampleTouchButton
{
    friend class VSampleTouchMenu;
public:
  /// \brief 
  ///   Returns the width of the button.
  inline float GetWidth() const { return m_fWidth; }
  
  /// \brief 
  ///   Returns the height of the button.
  inline float GetHeight() const { return m_fHeight; }

protected:
    VSampleTouchButton(VSampleTouchMenu *pFather, float fRelPosX, float fRelPosY, float fWidth, float fHeight, int iControlIndex,
                       bool bSingleHit);
    virtual ~VSampleTouchButton() {}
    void RecalculatePosition(VInputMap& InputMap);
    
    virtual void Update(bool bTriggered) = 0;
    
    VSampleTouchMenu *m_pFather;
    float m_fWidth,m_fHeight, m_fRelPosX, m_fRelPosY,m_fPosX,m_fPosY;
    VisScreenMaskPtr m_spActiveScreenMask;

    int m_iControlIndex;
    int m_iAlternative;

    VTouchAreaPtr m_spTouchArea;
    bool m_bSingleHit;
};

/// \brief
///   this class implements a simple button
///
class VSimpleSampleTouchButton : public VSampleTouchButton
{
    friend class VSampleTouchMenu;
protected:
    VSimpleSampleTouchButton(VSampleTouchMenu* pFather, float fRelPosX, float fRelPosY, const char* szIconPath, int iControlIndex,
                             bool bSingleHit);
    
    virtual void Update(bool bTriggered) HKV_OVERRIDE;
    
    VisScreenMaskPtr m_spIcon;
    float m_fActiveSize;
    float m_fGrowSpeed;
    float m_fTimeSinceLastToggle;
};

/// \brief
///   This class implements a toggle button
///
class VToggleSampleTouchButton : public VSimpleSampleTouchButton
{
  friend class VSampleTouchMenu;
public:
  /// \brief
  ///   Returns the toggle state of the button
  /// \return
  ///   \c true if the button is currently toggled; \c false if not.
  inline bool GetState() const { return m_bState; }
  
  /// \brief
  ///   Sets the toggle state of the button
  /// \param bState
  ///   \c true if the button should be toggled; \c false if not.
  void SetState(bool bState);
  
protected:
  VToggleSampleTouchButton(VSampleTouchMenu* pFather, float fRelPosX, float fRelPosY, const char* szIconPathOn, const char* szIconPathOff, 
                             int iControlIndex);
    
  virtual void Update(bool bTriggered) HKV_OVERRIDE;
    
  VisScreenMaskPtr m_spIconOff;
  bool m_bLastTriggerState,m_bState;
    
};

/// \brief
///   A simple touch menu which can be easily used by samples to add functionality to touch based devices
///
class VSampleTouchMenu : public IVisCallbackHandler_cl
{
public:
    /// \brief Orientation of the menu
    enum VOrientation {
        VO_TOP, ///< align the menu to the top of the screen
        VO_BOTTOM ///< align the menu to the bottom of the screen
    };
  
    /// \brief Horizontal orientation of the menu when sharing space with another menu
    enum VHorizontalOrientation {
      VHO_LEFT, ///< align the menu on the left
      VHO_RIGHT ///< align the menu on the right
    };
    
    /// \brief
    ///   Constructor
    ///
    /// \param InputMap
    ///   The input map to be used for input handling
    ///
    /// \param orientation
    ///   How the menu should be oriented
    ///
    ///   \see VOrientation enum
    VSampleTouchMenu(VInputMap& InputMap, VOrientation orientation);
    
    virtual ~VSampleTouchMenu();
  
    /// \brief Sets another menu to share the screen space with
    /// 
    /// \param pShareSpaceWith 
    ///   The other menu
    ///
    /// \param eHorizontalOrientation
    ///   The horizontal orientation of this menu
    ///
    void SetShareSpaceWith(VSampleTouchMenu *pShareSpaceWith, VHorizontalOrientation eHorizontalOrientation);
    
    /// \brief 
    ///   Creates a simple button and adds it to the menu
    ///
    /// \param szIconPath
    ///   path to a image file to be used as icon for the button
    ///   
    /// \param iIndex
    ///   The number of the trigger to assign the button to
    ///
    /// \param bSingleHit
    ///   if the button should trigger once (true), or continiously (false)
    ///
    /// \return
    ///   The newly created simple button
    ///
    VSimpleSampleTouchButton* AddSimpleButton(const char* szIconPath,int iIndex, bool bSingleHit = false);
    
    /// \brief
    ///   Creates a toggle button and adds it to the menu.
    ///   A toggle button is always single hit
    ///
    /// \param szIconPathOn
    ///   path to a image file to be used as the icon for the button in a on state
    /// 
    /// \param szIconPathOff
    ///   path to a image file to be used as the icon for the button in a off state
    ///
    /// \param iIndex
    ///   The number of the trigger to assign the button to
    ///
    /// \return
    ///   The newly created toggle button
    ///
    VToggleSampleTouchButton* AddToggleButton(const char* szIconPathOn, const char* szIconPathOff, int iIndex);
    
    /// \brief
    ///   Recalculates the position of all buttons, has to be called at least once
    ///
    void RecalculatePosition();
    
    virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;
    
    /// \brief Gets the X position of the menu
    inline float GetPosX() const { return m_fPosX; }
  
    /// \brief Gets the Y position of the menu
    inline float GetPosY() const { return m_fPosY; }
  
    /// \brief Gets the size of the menu
    ///
    /// \param fWidth
    ///   Contains the with after the function finished
    ///
    /// \param fHeight
    ///   Contains the height after the function finished
    ///
    void GetSize(float& fWidth, float& fHeight);
    
private:
    VInputMap& m_InputMap;
    float m_fPosX,m_fPosY;
    VOrientation m_Orientation;
    VArray<VSampleTouchButton*> m_Children;
    VSampleTouchMenu* m_pShareSpaceWith;
    VHorizontalOrientation m_eHorizontalOrientation;
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
