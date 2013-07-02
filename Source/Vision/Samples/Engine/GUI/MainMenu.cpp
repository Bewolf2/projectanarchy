/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/GUI/GUIPCH.h>
#include <Vision/Samples/Engine/GUI/MainMenu.hpp>

// register our custom menu dialog class
VModule &GUIModule = VGUIManager::GUIModule();
V_IMPLEMENT_SERIAL( SubDialog, VDialog, 0, &GUIModule );
V_IMPLEMENT_SERIAL( MyMainMenu, VDialog, 0, &GUIModule );
V_IMPLEMENT_SERIAL( ListControlDialog, VDialog, 0, &GUIModule );
V_IMPLEMENT_SERIAL( SliderDialog, VDialog, 0, &GUIModule );

///////////////////////////////////////////////////////////////////////////////
// Subdialog class
///////////////////////////////////////////////////////////////////////////////

void SubDialog::OnItemClicked(VMenuEventDataObject *pEvent)
{
  VDialog::OnItemClicked(pEvent);

  int iDlgResult = GetDialogResult();
  if ((iDlgResult == VGUIManager::ID_CANCEL) || (iDlgResult == VGUIManager::ID_OK))
  {
    GetContext()->CloseDialog(this);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Main menu dialog class
///////////////////////////////////////////////////////////////////////////////

// Override this function to prompt a message box before actually closing the dialog
void MyMainMenu::SetDialogResult(int iResult)
{
#if !defined(_VISION_ANDROID)
  // When the main dialog wants to quit, prompt a modal message box
  if (iResult==VGUIManager::ID_CANCEL)
  {
    m_pExitDialog = GetContext()->ShowDialog("GUI\\Dialogs\\MessageBox_Abort.xml");
    return;
  }
#endif

  VDialog::SetDialogResult(iResult);
}

void MyMainMenu::OnItemClicked(VMenuEventDataObject *pEvent)
{
  VDialog::OnItemClicked(pEvent); // important: forward this event to the base class

  IVGUIContext* pContext = GetContext();

  for (int i=0; i<pContext->m_OpenDialogs.Count(); ++i) 
  {
    if (pContext->m_OpenDialogs.GetAt(i) == this)
      continue;

    if ((m_pExitDialog != NULL) && (pContext->m_OpenDialogs.GetAt(i) == m_pExitDialog))
      continue;
    
    pContext->CloseDialog(pContext->m_OpenDialogs.GetAt(i));
  }

  // Open push buttons dialog
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("DLG_PUSHBUTTONS"))
  {
    GetContext()->ShowDialog("GUI\\Dialogs\\Dialog_PushButtons.xml");
    return;
  }

  // Open list control dialog
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("DLG_LISTCONTROL"))
  {
#if defined(_VISION_PSP2) || defined(_VISION_MOBILE) || defined(HK_ANARCHY)
    GetContext()->ShowDialog("GUI\\Dialogs\\Dialog_ListControl_Mobile.xml");
#else
    GetContext()->ShowDialog("GUI\\Dialogs\\Dialog_ListControl.xml");
#endif
    return;
  }

  // Open slider control dialog
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("DLG_SLIDER"))
  {
    GetContext()->ShowDialog("GUI\\Dialogs\\Dialog_Slider.xml");
    return;
  }

  // Open text input control dialog
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("DLG_TEXTINPUT"))
  {
    GetContext()->ShowDialog("GUI\\Dialogs\\Dialog_TextInput.xml");
    return;
  }

  // Open click sensitive map control dialog
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("DLG_MAP"))
  {
    GetContext()->ShowDialog("GUI\\Dialogs\\Dialog_Map.xml");
    return;
  }

  // Open character preview control dialog
  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("DLG_RENDERTOTEXTURE"))
  {
    GetContext()->ShowDialog("GUI\\Dialogs\\Dialog_RenderToTexture.xml");
    return;
  }
}

void MyMainMenu::OnTick(float dtime)
{
  VDialog::OnTick(dtime);

  if (m_pExitDialog)
  {
    int iDlgResult = m_pExitDialog->GetDialogResult();

    if (iDlgResult != 0)
    {
      GetContext()->CloseDialog(m_pExitDialog);
      m_pExitDialog = NULL;

      if (iDlgResult == VGUIManager::ID_YES)
        VDialog::SetDialogResult(VGUIManager::ID_CANCEL);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// List control dialog class
///////////////////////////////////////////////////////////////////////////////

void ListControlDialog::OnItemClicked(VMenuEventDataObject *pEvent)
{
  SubDialog::OnItemClicked(pEvent); // important: forward this event to the base class

  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("ADD_LISTITEM"))
  {
    VListControl *pListCtrl = (VListControl *)Items().FindItem(VGUIManager::GetID("LISTCTRL"));
    VASSERT(pListCtrl);
    char szItemName[512];
    sprintf(szItemName,"New item #%i",pListCtrl->Items().Count()+1);
    VListControlItem *pNewItem = pListCtrl->AddItem(szItemName, -1); // add (to the end)

    pListCtrl->EnsureVisible(pNewItem); // make sure the new item is visible
    return;
  }

  if (pEvent->m_pItem->GetID()==VGUIManager::GetID("CLEAR_LISTITEMS"))
  {
    VListControl *pListCtrl = (VListControl *)Items().FindItem(VGUIManager::GetID("LISTCTRL"));
    VASSERT(pListCtrl);
    pListCtrl->Reset();
    return;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Slider control dialog class
///////////////////////////////////////////////////////////////////////////////

void SliderDialog::OnInitDialog()
{
  m_pImageScrollH = (VSliderControl *)Items().FindItem(VGUIManager::GetID("HSCROLL_IMAGE"));
  m_pImageScrollV = (VSliderControl *)Items().FindItem(VGUIManager::GetID("VSCROLL_IMAGE"));
  m_pImageCtrl = (VImageControl *)Items().FindItem(VGUIManager::GetID("IMAGE"));
  VASSERT(m_pImageScrollH && m_pImageScrollV && m_pImageCtrl);

  hkvVec2 vTexSize = m_pImageCtrl->Image().m_States[VWindowBase::NORMAL].GetTextureSize();
  hkvVec2 vCtrlSize = m_pImageCtrl->GetSize();

  // determine the scroll ranges
  VASSERT(vTexSize.x>vCtrlSize.x && vTexSize.y>vCtrlSize.y);
  m_pImageScrollH->SetSliderRange(0.f, (vTexSize.x-vCtrlSize.x)/vTexSize.x, 0);
  m_pImageScrollV->SetSliderRange(0.f, (vTexSize.y-vCtrlSize.y)/vTexSize.y, 0);
  m_pImageScrollH->SetSliderRelSize(vCtrlSize.x/vTexSize.x);
  m_pImageScrollV->SetSliderRelSize(vCtrlSize.x/vTexSize.x);

  m_pSliderGamma = (VSliderControl *)Items().FindItem(VGUIManager::GetID("SLIDER_GAMMA"));
  VASSERT(m_pSliderGamma);
  Vision::Video.SaveGamma(Vision::Video.GetCurrentConfig()->m_iAdapter);

  bool bShowGammaSlider = false;

#if (defined(_WIN32) && defined(_VR_DX9)) || defined(_VISION_XENON)
  bShowGammaSlider  = true;
#endif

  if(!Vision::Video.GetCurrentConfig()->m_bFullScreen)
    bShowGammaSlider  = false;

  if(!bShowGammaSlider)
  {
    VDlgControlBase* pLabelGamma  = Items().FindItem(VGUIManager::GetID("LABEL_GAMMA"));
    VASSERT(pLabelGamma);

    pLabelGamma->SetVisible(false);
    m_pSliderGamma->SetVisible(false);
  }
}


void SliderDialog::OnValueChanged(VItemValueChangedEvent *pEvent)
{
  // scroll the image
  if (pEvent->m_pItem==m_pImageScrollH || pEvent->m_pItem==m_pImageScrollV)
  {
    // scroll the image by setting new texture coordinates
    hkvVec2 vTexOfs(m_pImageScrollH->GetValue(), m_pImageScrollV->GetValue());
    m_pImageCtrl->Image().SetTextureRange(VRectanglef(vTexOfs.x,vTexOfs.y,vTexOfs.x+1.f,vTexOfs.y+1.f));
    return;
  }

  // adjust the gamma
  if (pEvent->m_pItem==m_pSliderGamma)
  {
    float fExpVal = 1.5f - m_pSliderGamma->GetValue(); // use range [0.5 .. 1.5]
    Vision::Video.SetGamma(Vision::Video.GetCurrentConfig()->m_iAdapter, fExpVal,fExpVal,fExpVal);
  }
}

///////////////////////////////////////////////////////////////////////////////
// MaskedPictureItem : This custom control is a custom control class to render
//    the masked out map via a shader. See "Maps" dialog inside the sample
///////////////////////////////////////////////////////////////////////////////

class MaskedPictureItem : public VImageControl
{
public:
  V_DECLARE_SERIAL_DLLEXP( MaskedPictureItem, DECLSPEC_DLLEXPORT )
  virtual void Serialize( VArchive &ar ) HKV_OVERRIDE {}

  virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite) HKV_OVERRIDE
  {
    if (!VImageControl::Build(pNode,szPath,bWrite))
      return false;

    // Additionally get the "mask" texture filename from the control definition
    const char *szMaskTex = XMLHelper::Exchange_String(pNode,"mask",NULL,bWrite);
    if (szMaskTex)
      m_spMaskTex = VGUIManager::GlobalManager().LoadTexture(szMaskTex,szPath);

    m_vFadeColor = hkvVec4(1,1,1,1); // modulation color
    m_vTransform = hkvVec4(0.2f,0.2f,0.f,0.f); // current scaling and offset (0.2 means it magnifies the image under the mask by factor 5)

    return true;
  }

  // Do some shader initialisation here
  virtual void OnActivate() HKV_OVERRIDE
  {
    VImageControl::OnActivate();

    // Load shader library
    Vision::Shaders.LoadShaderLibrary("GUI\\Shaders\\GUIAlphaMask.Shaderlib");
    VCompiledTechnique *pTechnique = Vision::Shaders.CreateTechnique("GUIAlphaMask",NULL);
    VASSERT(pTechnique);
    m_spMaskShader = pTechnique->GetShader(0);

    // Apply a custom texture here
    int iMaskSampler = m_spMaskShader->GetSamplerIndexByName(VSS_PixelShader, "MaskTexture");
    VASSERT(iMaskSampler>=0);
    VStateGroupTexture *pStateGroupTexture = m_spMaskShader->GetStateGroupTexture(VSS_PixelShader, iMaskSampler);
    if (pStateGroupTexture != NULL)
      pStateGroupTexture->m_spCustomTex = m_spMaskTex;  

    // Cache the shader registers:
    m_RegTransform.Init(m_spMaskShader,"BaseTransform");
    m_RegFadeColor.Init(m_spMaskShader,"FadeColor");
    VASSERT(m_RegTransform.IsValid());
    VASSERT(m_RegFadeColor.IsValid());

    // Bouncing speed
    m_fSpeedX = 0.31f;
    m_fSpeedY = 0.22f;
  }

  // Reset some smart pointers here  
  virtual void OnDeactivate()  HKV_OVERRIDE
  {
    VImageControl::OnDeactivate();
    m_spMaskShader = NULL;
    m_spMaskTex = NULL;
  }  

  // Update function to update the positions
  virtual void OnTick(float fDeltaTime) HKV_OVERRIDE
  {
    VImageControl::OnTick(fDeltaTime);

    // Do the bouncing:
    m_vTransform.z += m_fSpeedX*fDeltaTime;
    m_vTransform.w += m_fSpeedY*fDeltaTime;

    if (m_vTransform.z<0.f)  
    {
      m_vTransform.z = 0.f;
      m_fSpeedX= hkvMath::Abs (m_fSpeedX);
    }
    else if (m_vTransform.z>1.f-m_vTransform.x)
    {
      m_vTransform.z = 1.f-m_vTransform.x;
      m_fSpeedX=-hkvMath::Abs (m_fSpeedX);
    }
    if (m_vTransform.w<0.f)  
    {
      m_vTransform.w = 0.f;
      m_fSpeedY=hkvMath::Abs (m_fSpeedY);
    }
    else if (m_vTransform.w>1.f-m_vTransform.y)
    {
      m_vTransform.w = 1.f-m_vTransform.y;
      m_fSpeedY=-hkvMath::Abs (m_fSpeedY);
    }
  }

  virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE
  {
    VASSERT(m_spMaskShader);
    VImageState &img = m_Image.m_States[GetCurrentState()];
    VColorRef iColor = parentState.iFadeColor;
    VRectanglef rect = GetBoundingBox();
    VRectanglef texCoord(0,0,1,1);
    VTextureObject *pTex = img.GetCurrentTexture();

    // Update shader registers in the const tables of the shader pass:
    m_RegTransform.SetRegisterValueF(m_spMaskShader, m_vTransform.data);
    m_RegFadeColor.SetRegisterValueF(m_spMaskShader, m_vFadeColor.data);

    // Render the quad with our mask shader
    Overlay2DVertex_t v[6];
    IVRender2DInterface::CreateQuadVertices(rect.m_vMin,rect.m_vMax,texCoord.m_vMin,texCoord.m_vMax,iColor,v);
    Graphics.Renderer.Draw2DBufferWithShader(6,v,pTex,*m_spMaskShader);

  }

  // New members of this control
  VCompiledShaderPassPtr m_spMaskShader;
  VTextureObjectPtr m_spMaskTex;
  hkvVec4 m_vTransform; // xy: scaling of the image under the mask, zw: offset; see shader implementation
  hkvVec4 m_vFadeColor; // rgba color modulation
  float m_fSpeedX, m_fSpeedY;
  VConstantBufferRegister m_RegTransform, m_RegFadeColor; // store shader registers
};

V_IMPLEMENT_SERIAL( MaskedPictureItem, VImageControl, 0, &GUIModule );

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
