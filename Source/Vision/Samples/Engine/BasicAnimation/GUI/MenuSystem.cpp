/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/BasicAnimation/BasicAnimationPCH.h>
#include <Vision/Samples/Engine/BasicAnimation/GUI/MenuSystem.hpp>



VModule &GUIModule = VGUIManager::GUIModule();
V_IMPLEMENT_SERIAL( AnimationMainMenu, VDialog, 0, &GUIModule );
V_IMPLEMENT_SERIAL( VDialogSubCtrl, VDlgControlBase, 0, &GUIModule );



#define ADD_MODE(val,name,desc) \
    pItem = m_pSkeletalModeList->AddItem(name,-1,SimpleSkeletalAnimatedObject_cl::val);\
      pItem->SetTooltipText(desc);

void AnimationMainMenu::OnInitDialog()
{
  // position the GUI to the right side of the screen
  SetPosition(Vision::Video.GetXRes() - GetSize().x,0);
  SetSize(GetSize().x, (float)Vision::Video.GetYRes());

  // find the entities in the scene
  m_pSkeletalCharacter = (SimpleSkeletalAnimatedObject_cl *)Vision::Game.SearchEntity("SkeletalAnimEntity");
  m_pVertexAnimCharacter = (SimpleVertexAnimatedObject_cl *)Vision::Game.SearchEntity("VertexAnimEntity");
  VASSERT(m_pSkeletalCharacter && m_pVertexAnimCharacter);

  // sub menues
  m_pSubMenu = (VDialogSubCtrl *)Items().FindItem(VGUIManager::GetID("SUBMENU"));
  m_spSubMenu_Skeletal = GetMenuManager()->CreateDialogInstance("BasicAnimation\\SubMenu_Skeletal.xml", GetContext(), this);
  m_spSubMenu_Vertex   = GetMenuManager()->CreateDialogInstance("BasicAnimation\\SubMenu_Vertex.xml", GetContext(), this);
  VASSERT(m_pSubMenu && m_spSubMenu_Skeletal && m_spSubMenu_Vertex);

  m_pCheckBoxSkeletalAnim = (VCheckBox *)Items().FindItem(VGUIManager::GetID("SKELETALANIM"));
  m_pCheckBoxVertexAnim = (VCheckBox *)Items().FindItem(VGUIManager::GetID("VERTEXANIM"));
  VASSERT(m_pCheckBoxSkeletalAnim);
  VASSERT(m_pCheckBoxVertexAnim);

  m_pCheckBoxSkeletalAnim->SetChecked(true);

  // vertex anim
  m_pCheckBoxPlayVertexAnim = (VCheckBox *)m_spSubMenu_Vertex->Items().FindItem(VGUIManager::GetID("PLAY_VERTEXANIM"));
  VASSERT(m_pCheckBoxPlayVertexAnim);

  // skeletal anim
  m_pSkeletalModeList = (VListControl *)m_spSubMenu_Skeletal->Items().FindItem(VGUIManager::GetID("SKELETAL_MODE"));
  VASSERT(m_pSkeletalModeList);
  VListControlItem *pItem;
    // fill the list control (put the enum in the data member):
    ADD_MODE(MODE_SINGLEANIM,       "Single animation",     "Plays a single animation");
    ADD_MODE(MODE_BLENDTWOANIMS,    "Blend two animations", "Blends two animations");
    ADD_MODE(MODE_LISTENTOEVENTS,   "Listen to events",     "Demonstrates event callbacks");
    ADD_MODE(MODE_SETANIMATIONTIME, "Set animation time",   "Shows manual animation time setting");
    ADD_MODE(MODE_LAYERTWOANIMATIONS,"Layer two animations","Layers one animation on top of the other");
    ADD_MODE(MODE_FORWARDKINEMATICS,"Forward kinematics",   "Inserts a bone modifier node\ninto the animation tree");
    ADD_MODE(MODE_LOOKAT,           "Look-at",              "Uses forward kinematics for simple lookat");

  // skeletal mode specific:
  m_pCheckBoxSkelPlay   = (VCheckBox *)m_spSubMenu_Skeletal->Items().FindItem(VGUIManager::GetID("SKEL_PLAYANIM"));
  m_pCheckBoxSkelBlend  = (VCheckBox *)m_spSubMenu_Skeletal->Items().FindItem(VGUIManager::GetID("SKEL_BLEND_TOGGLE"));
  m_pSliderSetAnimTime  = (VSliderControl *)m_spSubMenu_Skeletal->Items().FindItem(VGUIManager::GetID("SKEL_ANIM_TIME"));
  m_pSetAnimTimeLabel   = (VTextLabel *)m_spSubMenu_Skeletal->Items().FindItem(VGUIManager::GetID("SKEL_ANIM_TIME_LABEL"));
  m_pSliderFKinYaw      = (VSliderControl *)m_spSubMenu_Skeletal->Items().FindItem(VGUIManager::GetID("SKEL_FKIN_YAW"));
  m_pFKinLabel          = (VTextLabel *)m_spSubMenu_Skeletal->Items().FindItem(VGUIManager::GetID("SKEL_FKIN_LABEL"));
  m_pSliderLayered      = (VSliderControl *)m_spSubMenu_Skeletal->Items().FindItem(VGUIManager::GetID("SKEL_LAYERANIMS"));
  m_pLayeredLabel       = (VTextLabel *)m_spSubMenu_Skeletal->Items().FindItem(VGUIManager::GetID("SKEL_LAYERANIMS_LABEL"));

  VASSERT(m_pCheckBoxSkelPlay);
  VASSERT(m_pCheckBoxSkelBlend);
  VASSERT(m_pSliderSetAnimTime);
  VASSERT(m_pSetAnimTimeLabel);
  VASSERT(m_pSliderFKinYaw);
  VASSERT(m_pFKinLabel);
  VASSERT(m_pSliderLayered);
  VASSERT(m_pLayeredLabel);

  // set the initial mode
  SetSkeletalMode(SimpleSkeletalAnimatedObject_cl::MODE_SINGLEANIM);

}



void AnimationMainMenu::OnValueChanged(VItemValueChangedEvent *pEvent)
{
  VDialog::OnValueChanged(pEvent);
  
  // toggle animation type radio buttons
  if (pEvent->m_pItem==m_pCheckBoxSkeletalAnim && pEvent->AsBool()==true)
  {
    SetAnimationType(SKELETAL);
    return;
  }
  if (pEvent->m_pItem==m_pCheckBoxVertexAnim && pEvent->AsBool()==true)
  {
    SetAnimationType(VERTEX);
    return;
  }


  // start vertex animation
  if (pEvent->m_pItem==m_pCheckBoxPlayVertexAnim)
  {
    VisVertexAnimControl_cl *pAnimCtrl = m_pVertexAnimCharacter->GetVertexAnimControl();

#if defined(_VISION_PSP2)
    // PSP2 uses hardware deformation by default, which does not support vertex animation.
    VisAnimConfig_cl *pCfg = m_pVertexAnimCharacter->GetAnimConfig();
    pCfg->SetSkinningMode(VIS_SKINNINGMODE_SOFTWARE);
#endif

    if (pEvent->AsBool())
    {
      pAnimCtrl->SetCurrentSequencePosition(0.0f);
      pAnimCtrl->Play();
    } else
    {
      pAnimCtrl->Pause();
    }
    return;
  }

  // start single skeletal anim
  if (pEvent->m_pItem==m_pCheckBoxSkelPlay)
  {
    VisSkeletalAnimControl_cl *pAnimCtrl = m_pSkeletalCharacter->GetSingleAnimControl();

    if (pEvent->AsBool())
    {
      pAnimCtrl->SetCurrentSequencePosition(0.0f);
      pAnimCtrl->Play();
    } else
    {
      pAnimCtrl->Pause();
    }
    return;
  }

  
  // select animtion type for skeletal animation
  if (pEvent->m_pItem==m_pSkeletalModeList)
  {
    SetSkeletalMode((SimpleSkeletalAnimatedObject_cl::SampleMode_e)pEvent->AsListItem()->GetData());
    return;
  }

  // blend animation
  if (pEvent->m_pItem==m_pCheckBoxSkelBlend)
  {
    m_pSkeletalCharacter->SetBlendWalkToRun(pEvent->AsBool());
    return;
  }

  // set animation timing via slider
  if (pEvent->m_pItem==m_pSliderSetAnimTime)
  {
    SetSkeletalAnimPhase(pEvent->AsFloat()); // in interval [0..1]
    return;
  }

  // set neck yaw via slider (forward kinematic)
  if (pEvent->m_pItem==m_pSliderFKinYaw)
  {
    SetSkeletalNeckYaw(pEvent->AsFloat()); // in interval [-70..70]
    return;
  }
  
  if (pEvent->m_pItem==m_pSliderLayered)
  {
    SetSkeletalLayeredAnimWeight(pEvent->AsFloat());
    return;
  }
}


void AnimationMainMenu::OnItemClicked(VMenuEventDataObject *pEvent)
{
  VDialog::OnItemClicked(pEvent);
}


void AnimationMainMenu::OnTick(float dtime)
{
  VDialog::OnTick(dtime);

  // update some items per-frame 
  if (m_pCheckBoxPlayVertexAnim->IsVisible())
    m_pCheckBoxPlayVertexAnim->SetChecked(m_pVertexAnimCharacter->GetVertexAnimControl()->IsPlaying());

  if (m_pCheckBoxSkelPlay->IsVisible())
    m_pCheckBoxSkelPlay->SetChecked(m_pSkeletalCharacter->GetSingleAnimControl()->IsPlaying());
}


// switch between skeletal and vertex animation model
void AnimationMainMenu::SetAnimationType(AnimationType_e eType)
{
  m_eCurrentAnimType = eType;
  m_pSkeletalCharacter->SetActivate(eType==SKELETAL);
  m_pVertexAnimCharacter->SetActivate(eType==VERTEX);

  if (eType==SKELETAL)
  {
    m_pCheckBoxVertexAnim->SetChecked(false);
    m_pSubMenu->SetDialog(m_spSubMenu_Skeletal);
  }

  if (eType==VERTEX)
  {
    m_pCheckBoxSkeletalAnim->SetChecked(false);
    m_pSubMenu->SetDialog(m_spSubMenu_Vertex);
  }
}


// switch the skeletal playing mode
void AnimationMainMenu::SetSkeletalMode(SimpleSkeletalAnimatedObject_cl::SampleMode_e eMode)
{
  // select the list item in the control
  m_pSkeletalModeList->SetSelection(m_pSkeletalModeList->Items().FindItemByDataValue(eMode));

  // update the entity
  m_pSkeletalCharacter->SetMode(eMode);

  // show/hide mode specific controls:
  m_pCheckBoxSkelPlay->SetVisible(eMode==SimpleSkeletalAnimatedObject_cl::MODE_SINGLEANIM);
  m_pCheckBoxSkelBlend->SetVisible(eMode==SimpleSkeletalAnimatedObject_cl::MODE_BLENDTWOANIMS);
  m_pSliderSetAnimTime->SetVisible(eMode==SimpleSkeletalAnimatedObject_cl::MODE_SETANIMATIONTIME);
  m_pSetAnimTimeLabel->SetVisible(eMode==SimpleSkeletalAnimatedObject_cl::MODE_SETANIMATIONTIME);
  m_pSliderFKinYaw->SetVisible(eMode==SimpleSkeletalAnimatedObject_cl::MODE_FORWARDKINEMATICS);
  m_pFKinLabel->SetVisible(eMode==SimpleSkeletalAnimatedObject_cl::MODE_FORWARDKINEMATICS);
  m_pSliderLayered->SetVisible(eMode==SimpleSkeletalAnimatedObject_cl::MODE_LAYERTWOANIMATIONS);
  m_pLayeredLabel->SetVisible(eMode==SimpleSkeletalAnimatedObject_cl::MODE_LAYERTWOANIMATIONS);

  // some more initialisation
  if (eMode==SimpleSkeletalAnimatedObject_cl::MODE_SETANIMATIONTIME)
    SetSkeletalAnimPhase(m_pSliderSetAnimTime->GetValue());
  if (eMode==SimpleSkeletalAnimatedObject_cl::MODE_BLENDTWOANIMS)
    m_pCheckBoxSkelBlend->SetChecked(true);
  if (eMode==SimpleSkeletalAnimatedObject_cl::MODE_LAYERTWOANIMATIONS)
    SetSkeletalLayeredAnimWeight(m_pSliderLayered->GetValue());
  if (eMode==SimpleSkeletalAnimatedObject_cl::MODE_FORWARDKINEMATICS)
    SetSkeletalNeckYaw(m_pSliderFKinYaw->GetValue());
}



void AnimationMainMenu::SetSkeletalAnimPhase(float fPhase)
{
  // update slider
  m_pSliderSetAnimTime->SetValue(fPhase);

  // set phase
  fPhase *= m_pSkeletalCharacter->GetSingleAnimControl()->GetAnimSequence()->GetLength();
  m_pSkeletalCharacter->GetSingleAnimControl()->SetCurrentSequenceTime(fPhase);
}


void AnimationMainMenu::SetSkeletalNeckYaw(float fAngle)
{
  // set slider position
  m_pSliderFKinYaw->SetValue(fAngle); 

  // set the bone rotation
  ASSERT(m_pSkeletalCharacter->GetBoneModifiedNode());
  int iNeckBoneIndex = m_pSkeletalCharacter->GetNeckBoneIndex();

  // convert Euler to quaternion
  hkvQuat customBoneRotation (hkvNoInitialization);
  customBoneRotation.setFromEulerAngles (0, fAngle, 0);
  m_pSkeletalCharacter->GetBoneModifiedNode()->SetCustomBoneRotation(iNeckBoneIndex, customBoneRotation, VIS_MODIFY_BONE);
}



void AnimationMainMenu::SetSkeletalLayeredAnimWeight(float fWeight)
{
  // update slider
  m_pSliderLayered->SetValue(fWeight);

  // set weighting on animation
  m_pSkeletalCharacter->GetLayerMixerNode()->SetInputWeight(1, fWeight);
}


/////////////////////////////////////////////////////////////////////////////////


VWindowBase* VDialogSubCtrl::TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse)
{
  if (!VDlgControlBase::TestMouseOver(user, vAbsMouse))
    return NULL;
  if (!m_spDialog)
    return NULL;

  VWindowBase* pDlgItem = m_spDialog->TestMouseOver(user, vAbsMouse);
  if (pDlgItem==m_spDialog)
    return this; // do not return the dialog, because it would drag around
  return pDlgItem;
}

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
