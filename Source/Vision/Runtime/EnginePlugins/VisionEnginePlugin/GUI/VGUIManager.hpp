/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VGUIManager.hpp

#ifndef VGUIMANAGER_HPP_INCLUDED
#define VGUIMANAGER_HPP_INCLUDED

#include <Vision/Runtime/Base/ThirdParty/tinyXML/tinyxml.h>
#include <Vision/Runtime/Base/ThirdParty/tinyXML/TinyXMLHelper.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VDialog.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VGUIContext.hpp>

extern VModule g_VisionEngineModule;
class VMenuEventDataObject;
class IVMenuSystemResource;
class VDialogResource;
class VGUIManager;
class VDialog;
class VisFont_cl;

typedef VSmartPtr<VCursor> VCursorPtr;
typedef VSmartPtr<VDialog> VDialogPtr;
typedef VSmartPtr<VDialogResource> VDialogResourcePtr;




#define BUTTON_LMOUSE     0x00000001
#define BUTTON_RMOUSE     0x00000002
#define BUTTON_MMOUSE     0x00000004
#define BUTTON_NEXT       0x00000100
#define BUTTON_PREV       0x00000200

#define BUTTONMASK_CLICKEVENTS (BUTTON_LMOUSE|BUTTON_RMOUSE|BUTTON_MMOUSE)


#define KEYMODIFIER_LSHIFT  0x00000001
#define KEYMODIFIER_RSHIFT  0x00000002
#define KEYMODIFIER_LCTRL   0x00000004
#define KEYMODIFIER_RCTRL   0x00000008
#define KEYMODIFIER_LALT    0x00000010
#define KEYMODIFIER_RALT    0x00000020
#define KEYMODIFIER_SHIFT   (KEYMODIFIER_LSHIFT|KEYMODIFIER_RSHIFT)
#define KEYMODIFIER_CTRL    (KEYMODIFIER_LCTRL|KEYMODIFIER_RCTRL)
#define KEYMODIFIER_ALT     (KEYMODIFIER_LALT|KEYMODIFIER_RALT)

#define MAX_CLIPSTACK_DEPTH 16


#ifdef _VISION_PS3
#pragma diag_push
#pragma diag_suppress=1011
#endif

#if defined(__ghs__)
#pragma ghs nowarning 997
#endif

/// \brief
///   Resource manager that manages all GUI related resources. The global GUI manager can be
///   accessed by VGUIManager::GlobalManager()
class VGUIManager : public VisResourceManager_cl, public IVisCallbackHandler_cl, public VUserDataObj
{
public:
  enum ResourceType
  {
    UNDEFINED = -1,
    CURSOR,
    DIALOG,
    FONT
  };

  /// \brief
  ///   Constructor
  GUI_IMPEXP VGUIManager(const char *szManagerName = "GUI");

  /// \brief
  ///   Called by the plugin
  GUI_IMPEXP void OneTimeInit();

  /// \brief
  ///   Called by the plugin
  GUI_IMPEXP void OneTimeDeInit();


  /// \brief
  ///   Purges all resources and closes all contexts
  GUI_IMPEXP void CleanupResources();


  ///
  /// @name Resources
  /// @{
  ///

  /// \brief
  ///   Finds an existing resource by its filename
  GUI_IMPEXP VManagedResource *FindResource(const char *szFilename, ResourceType type, const char *szPath=NULL) const;

  /// \brief
  ///   Creates a resource by parsing an XML node
  GUI_IMPEXP VManagedResource *CreateResource(TiXmlElement *pNode, const char *szPath);

  /// \brief
  ///   Loads a cursor resource (texture filename or XML filename)
  GUI_IMPEXP VCursor* LoadCursorResource(const char *szFilename, const char *szPath=NULL);

  /// \brief
  ///   Loads a dialog resource from XML file
  GUI_IMPEXP VDialogResource *LoadDialog(const char *szFilename, const char *szPath=NULL);

  /// \brief
  ///   Creates a dialog instance using the resource
  GUI_IMPEXP VDialog* CreateDialogInstance(const char *szDialogResource, IVGUIContext *pContext, VDialog *pParent=NULL, int iDlgFlags=DIALOGFLAGS_NONE);

  /// \brief
  ///   Static helper function to load a texture file via the engine. Sets GUI suitable flags such
  ///   as no compression etc.
  GUI_IMPEXP static VTextureObject *LoadTexture(const char *szFilename, const char *szPath);

  /// \brief
  ///   Loads a font resource from a .def/.tga file
  GUI_IMPEXP VisFont_cl *LoadFont(const char *szFilename, const char *szPath=NULL);

  /// \brief
  ///   Sets the default font that is used where no font is specified
  inline void SetDefaultFont(VisFont_cl *pFont) {m_spDefaultFont=pFont;}

  /// \brief
  ///   Gets the default font
  GUI_IMPEXP VisFont_cl *GetDefaultFont() const;

  /// \brief
  ///   Sets the default cursor that is used if no specific cursor is defined
  GUI_IMPEXP void SetDefaultCursor(VCursor *pCursor);

  /// \brief
  ///   Gets the default cursor
  GUI_IMPEXP VCursor* GetDefaultCursor() const;

  /// \brief
  ///   Loads an XML file that can contain multiple resources
  GUI_IMPEXP bool LoadResourceFile(const char *szFilename);

  /// \brief
  ///   Static helper function used for parsing
  GUI_IMPEXP static ResourceType GetResourceType(const char *szTypeStr);

  /// \brief
  ///   Static helper function used for parsing
  GUI_IMPEXP static const char* GetResourceTypeStr(ResourceType eType);

  /// \brief
  ///   This function triggers the OnTextLabelCallback callback to translate a text label
  GUI_IMPEXP const char *TranslateString(VWindowBase *pItem, const char *szIn) const;


  VCursorPtr m_spDefaultCursor;
  VisFontPtr m_spDefaultFont;
  VRefCountedCollection<VisFont_cl> m_LoadedFonts; ///< keep them in a separate collection so the engine does not purge them

  GUI_IMPEXP VGUIContextCollection &Contexts() {return m_Contexts;} ///< context instances that use this manager

  
  ///
  /// @}
  ///

  ///
  /// @name Menu Ids
  /// @{
  ///


  /// \brief
  ///   Converts a string into int ID, e.g. "OK" returns 1. Can be used for yet unknown ID strings
  ///   to create a new ID
  GUI_IMPEXP static int GetID(const char *szIDName);

  /// \brief
  ///   Returns the name of an ID, e.g. for ID_OK it returns "OK"
  GUI_IMPEXP static const char* GetIDName(int iID);

  GUI_IMPEXP_DATA static int ID_STATIC; ///< pre-defined ID for static items (no ID)
  GUI_IMPEXP_DATA static int ID_OK;     ///< pre-defined ID for 'OK'
  GUI_IMPEXP_DATA static int ID_CANCEL; ///< pre-defined ID for 'Cancel'
  GUI_IMPEXP_DATA static int ID_YES;    ///< pre-defined ID for 'Yes'
  GUI_IMPEXP_DATA static int ID_NO;     ///< pre-defined ID for 'No'
  GUI_IMPEXP_DATA static int ID_ACCEPT; ///< pre-defined ID for 'Accept'
  GUI_IMPEXP_DATA static int ID_IGNORE; ///< pre-defined ID for 'Ignore'


// profiling:
  static int PROFILING_RENDERING;     ///< Internal profiling ID
  static int PROFILING_FONTRENDERING; ///< Internal profiling ID
  static int PROFILING_TICKFUNCTIONS; ///< Internal profiling ID
  static int PROFILING_BUILD;         ///< Internal profiling ID

  ///
  /// @}
  ///

  ///
  /// @name Global
  /// @{
  ///


  /// \brief
  ///   Returns one global instance of a GUI manager, initialized in the plugin
  GUI_IMPEXP static VGUIManager& GlobalManager();

  /// \brief
  ///   Returns a valid VModule that can be used to register custom GUI controls. However this module should not be used when adding classes for a vForge plugin. Plugins should use their own module
  GUI_IMPEXP static VModule& GUIModule();

  /// \brief
  ///   Returns the defaul GUI render state contant with relevant flags properly set
  GUI_IMPEXP static const VSimpleRenderState_t& DefaultGUIRenderState(VIS_TransparencyType transp=VIS_TRANSP_ALPHA);


  /// \brief
  ///   This callback is triggered whenever a text label is gathered from a resource.
  /// 
  /// Use this callback whenever you need your text items to be translated
  /// 
  /// The callback data object can be converted to VTextLabelDataObject. The m_szLabel member can
  /// be set to a translated string
  GUI_IMPEXP static VisCallback_cl OnTextLabelCallback;

protected:
  GUI_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);
  VGUIContextCollection m_Contexts; ///< context instances that use this manager

  ///
  /// @}
  ///

private:

  static VStrList g_IDList;
  static VGUIManager g_GlobalManager;
};


#ifdef _VISION_PS3
#pragma diag_pop
#endif

#if defined(__ghs__)
#pragma ghs endnowarning
#endif


/// \brief
///   Class that represents a stack of 2D clipping rectangles for hierarchical clipping.
class VClipRectStack
{
public:

  /// \brief
  ///   Constructor
  inline VClipRectStack(IVRender2DInterface *pRI) 
  {
    m_pRI = pRI; 
    m_iStackDepth=0;
    m_pStackValues = (VRectanglef *)m_fStackBuffer;
  }

  /// \brief
  ///   Adds a rectangle to the stack that gets intersected with the current result.
  inline void Push(const VRectanglef &newRect, bool bIntersect)
  {
    if (bIntersect)
      CurrentRect = CurrentRect.GetIntersection(newRect);
    else
      CurrentRect = newRect;
    VASSERT_MSG(m_iStackDepth<MAX_CLIPSTACK_DEPTH, "Max clip stack size exceeded");
    m_pStackValues[m_iStackDepth++] = CurrentRect;
    UpdateScissorRect();
  }

  /// \brief
  ///   Pops the last rectangle
  inline void Pop()
  {
    m_iStackDepth--;
    int iLastIndex = m_iStackDepth-1;
    if (iLastIndex>=0)
      CurrentRect = m_pStackValues[iLastIndex];
    else 
      CurrentRect = FullViewRect;
    UpdateScissorRect();
  }

  /// \brief
  ///   Reset the stack
  inline void Reset()
  {
    m_iStackDepth = 0;
    CurrentRect.Reset();
    UpdateScissorRect();
  }

  /// \brief
  ///   Assignment operator
  inline VClipRectStack& operator = (const VClipRectStack &other)
  {
    CurrentRect = other.CurrentRect;
    FullViewRect = other.FullViewRect;
    m_iStackDepth = other.m_iStackDepth;
    memcpy(m_fStackBuffer,other.m_fStackBuffer,sizeof(m_fStackBuffer));
    return *this;
  }

  VRectanglef CurrentRect;  ///< Defines the current intersection result
  VRectanglef FullViewRect; ///< Defines the full context client area


private:
  friend struct VGraphicsInfo;

  inline void UpdateScissorRect()
  {
    m_pRI->SetScissorRect(&CurrentRect);
  }
  IVRender2DInterface *m_pRI;
  int m_iStackDepth;
  float m_fStackBuffer[sizeof(VRectanglef)*MAX_CLIPSTACK_DEPTH/sizeof(float)];
  VRectanglef *m_pStackValues;

};



/// \brief
///   Paint info that gets passed to the OnPaint function. For instance provides the render interface
struct VGraphicsInfo
{
  /// \brief
  ///   Constructor
  GUI_IMPEXP VGraphicsInfo(IVRender2DInterface &renderer, IVGUIContext &context) 
    : Renderer(renderer), ClippingStack(&renderer), GUIContext(context)
  {
    ClippingStack.FullViewRect = context.GetClientRect();
    ClippingStack.Push(ClippingStack.FullViewRect,false); ///< start with full screen
    m_pCurrentUser = NULL;
  }

  IVRender2DInterface &Renderer;  ///< The IVRender2DInterface that can be used for painting screen quads
  VClipRectStack ClippingStack;   ///< The current clipping stack for hierarchical rectangle clipping
  IVGUIContext &GUIContext;       ///< The current context that is rendered
  VGUIUserInfo_t *m_pCurrentUser; ///< In some cases user specific data is needed (e.g. cursor pos)
};


/// \brief
///   Additional render state info that is passed to the control in the OnPaint function
struct VItemRenderInfo
{
  /// \brief
  ///   Constructor
  inline VItemRenderInfo(IVGUIContext *pContext, VWindowBase *pWnd=NULL)
  {
    VASSERT(pContext!=NULL);
    VASSERT(pWnd==NULL || pWnd->GetContext()==pContext);
    m_pWindow = pWnd;
    m_pContext = pContext;

    fFadeVal = 1.f;
    iFadeColor = iColor = V_RGBA_WHITE;
    bForceDisabled = pWnd!=NULL && !pWnd->IsEnabled();
  }

  /// \brief
  ///   Constructor
  inline VItemRenderInfo(const VItemRenderInfo &copyInfo, VWindowBase *pNewParent, float fFadePos)
  {
    m_pContext = copyInfo.m_pContext;
    m_pWindow = pNewParent;
    fFadeVal = fFadePos*copyInfo.fFadeVal;
    iColor = copyInfo.iColor;
    iFadeColor = iColor;
    int a = (int)((float)iFadeColor.a * fFadeVal);
    VASSERT(a>=0);
    iFadeColor.a = a>255 ? 255:a;
    bForceDisabled = copyInfo.bForceDisabled;
    if (pNewParent && !pNewParent->IsEnabled())
      bForceDisabled = true;
  }

  IVGUIContext *m_pContext; ///< Parent context
  VWindowBase *m_pWindow;   ///< Parent window control (can be a dialog)
  VColorRef iColor;         ///< Parent color state, should be considered when painting
  VColorRef iFadeColor;     ///< Fading result
  float fFadeVal;           ///< Parent fading value [0..1]
  bool bForceDisabled;      ///< If true, force this item to be rendered in disabled state
};




/// \brief
///   Menu event data object for event callbacks for events such as button pressed
class VMenuEventDataObject : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor
  inline VMenuEventDataObject(VisCallback_cl *pSender, IVGUIContext *pContext, VDialog *pDialog, VDlgControlBase *pItem, VGUIUserInfo_t &user, int iButtons)
    : IVisCallbackDataObject_cl(pSender), m_User(user)
  {
    m_pContext = pContext;
    m_pDialog = pDialog;
    m_pItem = pItem;
    m_iButtons = iButtons;
    m_bProcessed = false;
    m_vMousePos = user.m_vMousePos;
  }

  IVGUIContext *m_pContext; ///< the context
  VDialog *m_pDialog;       ///< the dialog that received the click event
  VWindowBase *m_pItem;     ///< the item control that was clicked/double clicked
  VGUIUserInfo_t &m_User;   ///< the user that triggered the event
  hkvVec2 m_vMousePos;    ///< the absolute mouse position when clicked
  int m_iButtons;           ///< the button bitmask when clicked (BUTTON_LMOUSE,..)
  bool m_bProcessed;        ///< internal flag
};


/// \brief
///   Menu event data object for event callbacks that gets triggered when a menu control changed
///   its value
class VItemValueChangedEvent : public IVisCallbackDataObject_cl
{
public:
  enum ValueType_e
  {
    VALUE_CUSTOM,
    VALUE_BOOL,
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_DOUBLE,
    VALUE_STRING,
    VALUE_VCOLORREF,
    VALUE_VECTOR3,
    VALUE_LISTITEM,
  };

  GUI_IMPEXP VItemValueChangedEvent(VDlgControlBase *pItem, ValueType_e eType, void *pNewVal, bool bChanging);

  ///
  /// @name Access the New Value
  /// @{
  ///

  inline bool              AsBool()     const { VASSERT(m_eValueType == VALUE_BOOL);      return *(bool*)             m_pNewValue;}
  inline int               AsInt()      const { VASSERT(m_eValueType == VALUE_INT);       return *(int*)              m_pNewValue;}
  inline float             AsFloat()    const { VASSERT(m_eValueType == VALUE_FLOAT);     return *(float*)            m_pNewValue;}
  inline double            AsDouble()   const { VASSERT(m_eValueType == VALUE_DOUBLE);    return *(double*)           m_pNewValue;}
  inline const char*       AsString()   const { VASSERT(m_eValueType == VALUE_STRING);    return (const char*)        m_pNewValue;}
  inline VColorRef         AsColorRef() const { VASSERT(m_eValueType == VALUE_VCOLORREF); return *(VColorRef*)        m_pNewValue;}
  inline const hkvVec3&    AsVector3f() const { VASSERT(m_eValueType == VALUE_VECTOR3);   return *(hkvVec3*)          m_pNewValue;}
  inline VListControlItem* AsListItem() const { VASSERT(m_eValueType == VALUE_LISTITEM);  return (VListControlItem*)  m_pNewValue;}

  VDlgControlBase *m_pItem;   ///< The item control that changed its value
  ValueType_e m_eValueType;   ///< determines the data type (VALUE_BOOL,...)
  void *m_pNewValue;          ///< points to data of type ValueType_e
  bool m_bChanging;           ///< indicates whether value is currently changing (true) or finished changing (false)
  ///
  /// @}
  ///

};


/// \brief
///   Data object sent by the VGUIManager::OnTextLabelCallback callback. Can be used to translate
///   text items
class VTextLabelDataObject : public IVisCallbackDataObject_cl
{
public:
  inline VTextLabelDataObject(VWindowBase *pTextItem, const char *szLabel) : IVisCallbackDataObject_cl(&VGUIManager::OnTextLabelCallback)
  {
    m_pTextItem = pTextItem;
    m_szLabel = szLabel;
    m_bTranslated = false;
  }

  VWindowBase *m_pTextItem;       ///< the item control that owns the text
  const char *m_szLabel;          ///< old resp. new text label. The string will be copied.
  bool m_bTranslated;             ///< internal flag that indicates whether the label already has been translated by another listener
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
