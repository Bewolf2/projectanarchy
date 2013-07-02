/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/GUIManager_VisionGUI.h>

#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/GUIMinimap_VisionGUI.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerUI.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerUIDialog.h>
#include <Vision/Samples/Engine/RPGPlugin/LevelInfo.h>

#if defined(_VISION_MOBILE)
namespace
{
  // Map a (multi)touch area over the client rect for uiButton
  void MapTouchArea(VPushButton const *uiButton, RPG_PlayerInputs_e input)
  {
    // Behind scene viewer (-900), in front of primary interaction (-950)
    float const depth = -925.0f;
    VRectanglef const clientRect = uiButton->GetClientRect();
    // TODO Hack: button image is 128x128, button onscreen area is 80x80; subtract to cover the difference
    VRectanglef const rect(clientRect.m_vMin, clientRect.m_vMax - hkvVec2(48.0f, 48.0f));

    VTouchArea *touchArea = new VTouchArea(VInputManager::GetTouchScreen(), rect, depth);
    RPG_PlayerUI::s_instance.GetDialog()->GetInputMap()->MapTrigger(input, touchArea, CT_TOUCH_ANY);
  }
}
#endif

RPG_GuiManager_VisionGUI::RPG_GuiManager_VisionGUI()
  : RPG_GuiManager()
  //, m_hudDialogFilename("GUI/Dialogs/Gui_Hud.xml")
  //, m_hudDialog()

  , m_indicatorBarTrackTextureFilename("")
  , m_indicatorBarTrackTexture(NULL)
  , m_indicatorBarTrackSize()
  , m_indicatorBarSize()
  , m_indicatorBarGlowSize()
  , m_indicatorBarGlowDefaultDuration(0.05f)
  , m_indicatorBarFlareTextureFilename("")
  , m_indicatorBarFlareTexture(NULL)
  , m_indicatorBarFlareSize()
  , m_indicatorBarFlareOrigin()

  , m_healthBarTextureFilename("")
  , m_healthBarTexture(NULL)
  , m_healthBarGlowTextureFilename("")
  , m_healthBarGlowTexture(NULL)
  , m_healthBarPosition()
  , m_healthBarTrack()
  , m_healthBar()
  , m_healthBarGlow()
  , m_healthBarFlare()
  , m_lastHealth(0)
  , m_healthGlowStartTime(0.f)
  , m_healthGlowDuration(0.f)

  , m_manaBarTextureFilename("")
  , m_manaBarTexture(NULL)
  , m_manaBarGlowTextureFilename("")
  , m_manaBarGlowTexture(NULL)
  , m_manaBarPosition()
  , m_manaBarTrack()
  , m_manaBar()
  , m_manaBarGlow()
  , m_manaBarFlare()
  , m_lastMana(0)
  , m_manaGlowStartTime(0.f)
  , m_manaGlowDuration(0.f)
  , m_manaSpendWarningStartTime(0.f)
  , m_manaSpendWarningDefaultDuration(0.8f)
  , m_manaSpendWarningFlashesPerSecond(3)
  , m_manaSpendWarningActive(false)

  , m_minimapTextureFilename("")
  , m_minimapFrameTextureFilename("")
  , m_minimapMaskTextureFilename("")
  , m_minimapPlayerTextureFilename("")
  , m_minimapTexture(NULL)
  , m_minimapFrameTexture(NULL)
  , m_minimapMaskTexture(NULL)
  , m_minimapPlayerTexture(NULL)
  , m_minimapScaleFactor(1.0f)
  , m_minimapZoomFactor(0.2f)
  , m_minimap()
  , m_minimapFrame()
  , m_minimapPlayer()
{
  for (int i = 0; i < BTN_Count; ++i)
  {
    m_skillButtonDefinitions[i] = RPG_SkillButtonDefinition();
    m_skillButtons[i] = NULL;
  }
}

void RPG_GuiManager_VisionGUI::OnActivate(PlayerUIDialog* parentDialog)
{
  RPG_GuiManager::OnActivate(parentDialog);
  VASSERT(m_parentDialog);  // should have been set by the parent's OnActivate()

  VString msg = "RPG_GuiManager_VisionGUI::OnActivate()";
  Vision::Error.SystemMessage(msg.AsChar());
  Vision::Message.Add(1, msg.AsChar());

  AddSkillButtonsToParentDialog();
  AddHealthAndManaBarsToParentDialog();
  AddMinimapToParentDialog();

  //DisplayHud(); // test - alternate approach
}

void RPG_GuiManager_VisionGUI::OnDeactivate()
{
  //m_hudDialog = NULL;

  for (int i = 0; i < BTN_Count; ++i)
  {
    m_skillButtons[i] = NULL;
  }

  m_healthBar = NULL;
  m_healthBarTrack = NULL;
  m_healthBarGlow = NULL;
  m_healthBarFlare = NULL;
  m_manaBar = NULL;
  m_manaBarTrack = NULL;
  m_manaBarGlow = NULL;
  m_manaBarFlare = NULL;
  m_minimap = NULL;
  m_minimapFrame = NULL;
  m_minimapPlayer = NULL;

  RPG_GuiManager::OnDeactivate();
}

void RPG_GuiManager_VisionGUI::OnTick(float const deltaTime)
{
  RPG_Character* characterEntity = static_cast<RPG_Character *>(RPG_PlayerUI::s_instance.GetController()->GetOwner());
  VASSERT(characterEntity);
  UpdateHUD(characterEntity, deltaTime);
}

void RPG_GuiManager_VisionGUI::InitializeButtonDefinitions()
{
  RPG_SkillButtons_e skillbutton;
  int textureSizeX, textureSizeY, textureDepth;

  skillbutton = BTN_RangedAttack;
  {
    m_skillButtonDefinitions[skillbutton].m_imageFileName = "GUI/Textures/btn_rangedAttack.tga";
    m_skillButtonDefinitions[skillbutton].m_positionCorner = CORNER_LowerLeft;
    m_skillButtonDefinitions[skillbutton].m_positionOffsetFromCorner = hkvVec2(48.f, 372.f);
    m_skillButtonDefinitions[skillbutton].m_initiallyVisible = true;
  }

  skillbutton = BTN_PowerAttack;
  {
    m_skillButtonDefinitions[skillbutton].m_imageFileName = "GUI/Textures/btn_powerAttack.tga";
    m_skillButtonDefinitions[skillbutton].m_positionCorner = CORNER_LowerLeft;
    m_skillButtonDefinitions[skillbutton].m_positionOffsetFromCorner = hkvVec2(48.f, 284.f);
    m_skillButtonDefinitions[skillbutton].m_initiallyVisible = true;
  }

  skillbutton = BTN_AoeAttack;
  {
    m_skillButtonDefinitions[skillbutton].m_imageFileName = "GUI/Textures/btn_aoeAttack.tga";
    m_skillButtonDefinitions[skillbutton].m_positionCorner = CORNER_LowerLeft;
    m_skillButtonDefinitions[skillbutton].m_positionOffsetFromCorner = hkvVec2(48.f, 196.f);
    m_skillButtonDefinitions[skillbutton].m_initiallyVisible = true;
  }

  skillbutton = BTN_RangedAttackAlt;
  {
    m_skillButtonDefinitions[skillbutton].m_imageFileName = "GUI/Textures/btn_rangedAttack.tga";
    m_skillButtonDefinitions[skillbutton].m_positionCorner = CORNER_LowerRight;
    m_skillButtonDefinitions[skillbutton].m_positionOffsetFromCorner = hkvVec2(128.f, 372.f);
    m_skillButtonDefinitions[skillbutton].m_initiallyVisible = true;
  }

  skillbutton = BTN_PowerAttackAlt;
  {
    m_skillButtonDefinitions[skillbutton].m_imageFileName = "GUI/Textures/btn_powerAttack.tga";
    m_skillButtonDefinitions[skillbutton].m_positionCorner = CORNER_LowerRight;
    m_skillButtonDefinitions[skillbutton].m_positionOffsetFromCorner = hkvVec2(128.f, 284.f);
    m_skillButtonDefinitions[skillbutton].m_initiallyVisible = true;
  }

  skillbutton = BTN_AoeAttackAlt;
  {
    m_skillButtonDefinitions[skillbutton].m_imageFileName = "GUI/Textures/btn_aoeAttack.tga";
    m_skillButtonDefinitions[skillbutton].m_positionCorner = CORNER_LowerRight;
    m_skillButtonDefinitions[skillbutton].m_positionOffsetFromCorner = hkvVec2(128.f, 196.f);
    m_skillButtonDefinitions[skillbutton].m_initiallyVisible = true;
  }

  // load the textures and set sizes
  for (int i = 0; i < BTN_Count; ++i)
  {
    if (!m_skillButtonDefinitions[i].m_imageFileName.IsEmpty())
    {
      m_skillButtonDefinitions[i].m_textureObject = Vision::TextureManager.Load2DTexture(m_skillButtonDefinitions[i].m_imageFileName.AsChar());
      VASSERT(m_skillButtonDefinitions[i].m_textureObject);
      VASSERT(m_skillButtonDefinitions[i].m_textureObject->IsTexturePowerOfTwo());
      m_skillButtonDefinitions[i].m_textureObject->GetTextureDimensions(textureSizeX, textureSizeY, textureDepth);
      m_skillButtonDefinitions[i].m_size = hkvVec2(static_cast<float>(textureSizeX), static_cast<float>(textureSizeY));
    }
  }
}

void RPG_GuiManager_VisionGUI::InitializeHealthAndManaBarDefinitions()
{
  int textureSizeX, textureSizeY, textureDepth;

  m_indicatorBarTrackTextureFilename  = "GUI/Textures/ind_track.tga";
  m_indicatorBarFlareTextureFilename  = "GUI/Textures/track_flare.tga";
  m_healthBarTextureFilename          = "GUI/Textures/ind_health.tga";
  m_healthBarGlowTextureFilename      = "GUI/Textures/ind_health_glow.tga";
  m_manaBarTextureFilename            = "GUI/Textures/ind_mana.tga";
  m_manaBarGlowTextureFilename        = "GUI/Textures/ind_mana_glow.tga";
  m_indicatorBarFlareOrigin = hkvVec2(72.f, 16.f);
  m_healthBarPosition = hkvVec2(32.f, 32.f);
  m_manaBarPosition = hkvVec2(32.f, 64.f);

  m_indicatorBarTrackTexture = Vision::TextureManager.Load2DTexture(m_indicatorBarTrackTextureFilename.AsChar());
  VASSERT_MSG(m_indicatorBarTrackTexture, "m_indicatorBarTrackTexture failed to load.");
  VASSERT_MSG(m_indicatorBarTrackTexture->IsTexturePowerOfTwo(), "m_indicatorBarTrackTexture's dimensions must be a power of two.");
  m_indicatorBarTrackTexture->GetTextureDimensions(textureSizeX, textureSizeY, textureDepth);
  m_indicatorBarTrackSize = hkvVec2(static_cast<float>(textureSizeX), static_cast<float>(textureSizeY));

  m_indicatorBarFlareTexture = Vision::TextureManager.Load2DTexture(m_indicatorBarFlareTextureFilename.AsChar());
  VASSERT_MSG(m_indicatorBarFlareTexture, "m_indicatorBarFlareTexture failed to load.");
  VASSERT_MSG(m_indicatorBarFlareTexture->IsTexturePowerOfTwo(), "m_indicatorBarFlareTexture's dimensions must be a power of two.");
  m_indicatorBarFlareTexture->GetTextureDimensions(textureSizeX, textureSizeY, textureDepth);
  m_indicatorBarFlareSize = hkvVec2(static_cast<float>(textureSizeX), static_cast<float>(textureSizeY));

  m_healthBarTexture = Vision::TextureManager.Load2DTexture(m_healthBarTextureFilename.AsChar());
  VASSERT_MSG(m_healthBarTexture, "m_healthBarGlowTexture failed to load.");
  VASSERT_MSG(m_healthBarTexture->IsTexturePowerOfTwo(), "m_healthBarGlowTexture's dimensions must be a power of two.");
  m_healthBarTexture->GetTextureDimensions(textureSizeX, textureSizeY, textureDepth);
  m_indicatorBarSize = hkvVec2(static_cast<float>(textureSizeX), static_cast<float>(textureSizeY));

  m_healthBarGlowTexture = Vision::TextureManager.Load2DTexture(m_healthBarGlowTextureFilename.AsChar());
  VASSERT_MSG(m_healthBarGlowTexture, "m_healthBarGlowTexture failed to load.");
  VASSERT_MSG(m_healthBarGlowTexture->IsTexturePowerOfTwo(), "m_healthBarGlowTexture's dimensions must be a power of two.");
  m_healthBarGlowTexture->GetTextureDimensions(textureSizeX, textureSizeY, textureDepth);
  m_indicatorBarGlowSize = hkvVec2(static_cast<float>(textureSizeX), static_cast<float>(textureSizeY));

  m_manaBarTexture = Vision::TextureManager.Load2DTexture(m_manaBarTextureFilename.AsChar());
  VASSERT_MSG(m_manaBarTexture, "m_manaBarTexture failed to load.");
  VASSERT_MSG(m_manaBarTexture->IsTexturePowerOfTwo(), "m_manaBarTexture's dimensions must be a power of two.");

  m_manaBarGlowTexture = Vision::TextureManager.Load2DTexture(m_manaBarGlowTextureFilename.AsChar());
  VASSERT_MSG(m_manaBarGlowTexture, "m_manaBarGlowTexture failed to load.");
  VASSERT_MSG(m_manaBarGlowTexture->IsTexturePowerOfTwo(), "m_manaBarGlowTexture's dimensions must be a power of two.");
}

void RPG_GuiManager_VisionGUI::InitializeMinimapDefinitions()
{
  const RPG_LevelInfo* levelInfo = RPG_GameManager::s_instance.GetLevelInfo();
  if (levelInfo)
  {
    // Set the minimap's image path to the path specified in the levelInfo object
    VString m_minimapTextureFilename = levelInfo->GetMinimapImagePath();
    m_minimapTexture = Vision::TextureManager.Load2DTexture(m_minimapTextureFilename.AsChar());
    VASSERT_MSG(m_minimapTexture, "m_minimapTexture failed to load.");
    VASSERT_MSG(m_minimapTexture->IsTexturePowerOfTwo(), "m_minimapTexture's dimensions must be a power of two.");

    VString m_minimapMaskTextureFilename = "GUI/Textures/minimap_mask.tga";
    m_minimapMaskTexture = Vision::TextureManager.Load2DTexture(m_minimapMaskTextureFilename.AsChar());
    VASSERT_MSG(m_minimapMaskTexture, "m_minimapMaskTexture failed to load.");
    VASSERT_MSG(m_minimapMaskTexture->IsTexturePowerOfTwo(), "m_minimapMaskTexture's dimensions must be a power of two.");

    VString m_minimapFrameTextureFilename = "GUI/Textures/minimap_frame.tga";
    m_minimapFrameTexture = Vision::TextureManager.Load2DTexture(m_minimapFrameTextureFilename.AsChar());
    VASSERT_MSG(m_minimapFrameTexture, "m_minimapFrameTexture failed to load.");
    VASSERT_MSG(m_minimapFrameTexture->IsTexturePowerOfTwo(), "m_minimapFrameTexture's dimensions must be a power of two.");

    VString m_minimapPlayerTextureFilename = "GUI/Textures/minimap_player.tga";
    m_minimapPlayerTexture = Vision::TextureManager.Load2DTexture(m_minimapPlayerTextureFilename.AsChar());
    VASSERT_MSG(m_minimapPlayerTexture, "m_minimapPlayerTexture failed to load.");
    VASSERT_MSG(m_minimapPlayerTexture->IsTexturePowerOfTwo(), "m_minimapPlayerTexture's dimensions must be a power of two.");

    // Minimap position and size    
    m_minimapSize = hkvVec2(128.f, 128.f);
    m_minimapPosition = hkvVec2(Vision::Video.GetXRes() - m_minimapSize.x - 32.f, 32.f);
    
    // Minimap image relative origin (we need this value in the range 0..1 in order to add the character position)
    hkvVec2 minimapImageDimensions = levelInfo->GetMinimapImageDimensions();
    hkvVec2 minimapImageOrigin = levelInfo->GetMinimapImageOriginPosition();
    m_minimapRelativeOrigin.x = (minimapImageOrigin.x / minimapImageDimensions.x) - (m_minimapZoomFactor * 0.5f);
    m_minimapRelativeOrigin.y = (minimapImageOrigin.y / minimapImageDimensions.y) - (m_minimapZoomFactor * 0.5f);

    // Minimap scale factor (depending on the maximum extents either horizontally or vertically)
    hkvAlignedBBox scenebox;
    Vision::GetSceneManager()->GetSceneExtents(scenebox);
    if (scenebox.getSizeX() > scenebox.getSizeY())
      m_minimapScaleFactor = (1.f / scenebox.getSizeX());
    else
      m_minimapScaleFactor = (1.f / scenebox.getSizeY());
  }
}

void RPG_GuiManager_VisionGUI::AddSkillButtonsToParentDialog()
{
  InitializeButtonDefinitions();

  // add skill buttons
  for (int i = 0; i < BTN_Count; ++i)
  {
    VASSERT(!m_skillButtons[i]);
    m_skillButtons[i] = new VPushButton();
    VASSERT(m_skillButtons[i]);

    // button image(s)
    VTextureObject *buttonTexture = Vision::TextureManager.Load2DTexture(m_skillButtonDefinitions[i].m_imageFileName.AsChar());
    VASSERT_MSG(buttonTexture, "Button texture failed to load.");
    VASSERT_MSG(buttonTexture->IsTexturePowerOfTwo(), "Button texture's dimensions must be a power of two.");
    m_skillButtons[i]->Image().SetTexture(buttonTexture); // set all 4 textures for now
    m_skillButtons[i]->Image().SetTransparency(VIS_TRANSP_ALPHA);
    m_skillButtons[i]->Image().m_States[VWindowBase::MOUSEOVER].SetColor(VColorRef(128,128,255,255));
    m_skillButtons[i]->Image().m_States[VWindowBase::SELECTED].SetColor(VColorRef(208,24,24,255));
    m_skillButtons[i]->Image().m_States[VWindowBase::DISABLED].SetColor(VColorRef(128,128,128,255));

    // position, size and visibility
    hkvVec2 const position = FindButtonPosition(m_skillButtonDefinitions[i].m_positionOffsetFromCorner, m_skillButtonDefinitions[i].m_positionCorner);
    m_skillButtons[i]->SetPosition(position.x, position.y);
    m_skillButtons[i]->SetSize(m_skillButtonDefinitions[i].m_size.x, m_skillButtonDefinitions[i].m_size.y);
    m_skillButtons[i]->SetVisible(m_skillButtonDefinitions[i].m_initiallyVisible);

    VASSERT(m_parentDialog);
    m_parentDialog->AddControl(m_skillButtons[i]);
  }

#if defined(_VISION_MOBILE)
  MapTouchArea(m_skillButtons[BTN_RangedAttack], PI_ShiftModifier);
  MapTouchArea(m_skillButtons[BTN_PowerAttack], PI_SecondaryShiftModifier);
  MapTouchArea(m_skillButtons[BTN_AoeAttack], PI_SpecialAction01);
  MapTouchArea(m_skillButtons[BTN_RangedAttackAlt], PI_ShiftModifier);
  MapTouchArea(m_skillButtons[BTN_PowerAttackAlt], PI_SecondaryShiftModifier);
  MapTouchArea(m_skillButtons[BTN_AoeAttackAlt], PI_SpecialAction01);
#endif
}

hkvVec2 RPG_GuiManager_VisionGUI::FindButtonPosition(hkvVec2 const& positionOffset, RPG_InterfaceCorners_e const& corner) const
{
  VASSERT(m_parentDialog);
  hkvVec2 position = positionOffset;

  switch(corner)
  {
  case CORNER_UpperLeft:
    // no modification required - position offset is already an offset from 0,0.
    break;
  case CORNER_UpperRight:
    position.x = m_parentDialog->GetSize().x - positionOffset.x;
    break;
  case CORNER_LowerRight:
    position.x = m_parentDialog->GetSize().x - positionOffset.x;
    position.y = m_parentDialog->GetSize().y - positionOffset.y;
    break;
  case CORNER_LowerLeft:
    position.y = m_parentDialog->GetSize().y - positionOffset.y;
    break;
  default:
    VASSERT(false);
  }

  return position;
}

bool RPG_GuiManager_VisionGUI::IsButtonSelected(RPG_SkillButtons_e const& skillButton) const
{
  return(m_skillButtons[skillButton] && m_skillButtons[skillButton]->IsSelected());
}

void RPG_GuiManager_VisionGUI::AddHealthAndManaBarsToParentDialog()
{
  InitializeHealthAndManaBarDefinitions();

  // add tracks
  VASSERT(m_indicatorBarTrackTexture);
  {
    hkvVec2 const trackOffset = hkvVec2((m_indicatorBarTrackSize.x - m_indicatorBarSize.x) / 2, (m_indicatorBarTrackSize.y - m_indicatorBarSize.y) / 2);

    // add health bar track
    VASSERT(!m_healthBarTrack);
    m_healthBarTrack = new VImageControl();
    VASSERT(m_healthBarTrack);
    m_healthBarTrack->Image().SetTexture(m_indicatorBarTrackTexture);
    m_healthBarTrack->Image().SetTransparency(VIS_TRANSP_ALPHA);
    m_healthBarTrack->SetPosition(m_healthBarPosition.x - trackOffset.x, m_healthBarPosition.y - trackOffset.y);
    m_healthBarTrack->SetSize(m_indicatorBarTrackSize.x, m_indicatorBarTrackSize.y);
    m_healthBarTrack->SetVisible(true);

    m_parentDialog->AddControl(m_healthBarTrack);

    // add mana bar track
    VASSERT(!m_manaBarTrack);
    m_manaBarTrack = new VImageControl();
    VASSERT(m_manaBarTrack);
    m_manaBarTrack->Image().SetTexture(m_indicatorBarTrackTexture);
    m_manaBarTrack->Image().SetTransparency(VIS_TRANSP_ALPHA);
    m_manaBarTrack->SetPosition(m_manaBarPosition.x - trackOffset.x, m_manaBarPosition.y - trackOffset.y);
    m_manaBarTrack->SetSize(m_indicatorBarTrackSize.x, m_indicatorBarTrackSize.y);
    m_manaBarTrack->SetVisible(true);

    m_parentDialog->AddControl(m_manaBarTrack);
  }

  // add health meter
  VASSERT(!m_healthBar);
  m_healthBar = new VImageControl();
  VASSERT(m_healthBar);

  VASSERT_MSG(m_healthBarTexture, "Health bar texture failed to load.");
  m_healthBar->Image().SetTexture(m_healthBarTexture);
  m_healthBar->Image().SetTransparency(VIS_TRANSP_ALPHA);
  m_healthBar->Image().SetStretchMode(VImageState::TEXTURE_SIZE);
  m_healthBar->SetPosition(m_healthBarPosition.x, m_healthBarPosition.y);
  m_healthBar->SetSize(m_indicatorBarSize.x, m_indicatorBarSize.y);
  m_healthBar->SetVisible(true);

  m_parentDialog->AddControl(m_healthBar);

  // add mana meter
  VASSERT(!m_manaBar);
  m_manaBar = new VImageControl();
  VASSERT(m_manaBar);

  VASSERT_MSG(m_manaBarTexture, "Mana bar texture failed to load.");
  m_manaBar->Image().SetTexture(m_manaBarTexture);
  m_manaBar->Image().SetTransparency(VIS_TRANSP_ALPHA);
  m_manaBar->Image().SetStretchMode(VImageState::TEXTURE_SIZE);
  m_manaBar->SetPosition(m_manaBarPosition.x, m_manaBarPosition.y);
  m_manaBar->SetSize(m_indicatorBarSize.x, m_indicatorBarSize.y);
  m_manaBar->SetVisible(true);

  m_parentDialog->AddControl(m_manaBar);

  // add health meter glow
  hkvVec2 const glowOffset = hkvVec2((m_indicatorBarGlowSize.x - m_indicatorBarSize.x) / 2, (m_indicatorBarGlowSize.y - m_indicatorBarSize.y) / 2);

  VASSERT(!m_healthBarGlow);
  m_healthBarGlow = new VImageControl();
  VASSERT(m_healthBarGlow);

  VASSERT_MSG(m_healthBarGlowTexture, "Health bar glow texture failed to load.");
  m_healthBarGlow->Image().SetTexture(m_healthBarGlowTexture);
  m_healthBarGlow->Image().SetTransparency(VIS_TRANSP_ADDITIVE);
  m_healthBarGlow->Image().SetStretchMode(VImageState::TEXTURE_SIZE);
  m_healthBarGlow->SetPosition(m_healthBarPosition.x - glowOffset.x, m_healthBarPosition.y - glowOffset.y);
  m_healthBarGlow->SetSize(m_indicatorBarGlowSize.x, m_indicatorBarGlowSize.y);
  m_healthBarGlow->SetVisible(false);

  m_parentDialog->AddControl(m_healthBarGlow);

  // add mana meter glow
  VASSERT(!m_manaBarGlow);
  m_manaBarGlow = new VImageControl();
  VASSERT(m_manaBarGlow);

  VASSERT_MSG(m_manaBarGlowTexture, "Mana bar glow texture failed to load.");
  m_manaBarGlow->Image().SetTexture(m_manaBarGlowTexture);
  m_manaBarGlow->Image().SetTransparency(VIS_TRANSP_ADDITIVE);
  m_manaBarGlow->Image().SetStretchMode(VImageState::TEXTURE_SIZE);
  m_manaBarGlow->SetPosition(m_manaBarPosition.x - glowOffset.x, m_manaBarPosition.y - glowOffset.y);
  m_manaBarGlow->SetSize(m_indicatorBarGlowSize.x, m_indicatorBarGlowSize.y);
  m_manaBarGlow->SetVisible(false);

  m_parentDialog->AddControl(m_manaBarGlow);

  VASSERT(m_indicatorBarFlareTexture);
  {
    // add health bar flare
    VASSERT(!m_healthBarFlare);
    m_healthBarFlare = new VImageControl();
    VASSERT(m_healthBarFlare);

    m_healthBarFlare->Image().SetTexture(m_indicatorBarFlareTexture);
    m_healthBarFlare->Image().SetTransparency(VIS_TRANSP_ALPHA);
    m_healthBarFlare->Image().SetStretchMode(VImageState::TEXTURE_SIZE);
    m_healthBarFlare->SetPosition(m_healthBarPosition.x - m_indicatorBarFlareOrigin.x, (m_healthBarPosition.y - m_indicatorBarFlareOrigin.y) + m_indicatorBarSize.y / 2);
    m_healthBarFlare->SetSize(m_indicatorBarFlareSize.x, m_indicatorBarFlareSize.y);
    m_healthBarFlare->SetVisible(true);

    m_parentDialog->AddControl(m_healthBarFlare);

    // add mana bar flare
    VASSERT(!m_manaBarFlare);
    m_manaBarFlare = new VImageControl();
    VASSERT(m_manaBarFlare);

    m_manaBarFlare->Image().SetTexture(m_indicatorBarFlareTexture);
    m_manaBarFlare->Image().SetTransparency(VIS_TRANSP_ALPHA);
    m_manaBarFlare->Image().SetStretchMode(VImageState::TEXTURE_SIZE);
    m_manaBarFlare->SetPosition(m_manaBarPosition.x - m_indicatorBarFlareOrigin.x, (m_manaBarPosition.y - m_indicatorBarFlareOrigin.y) + m_indicatorBarSize.y / 2);
    m_manaBarFlare->SetSize(m_indicatorBarFlareSize.x, m_indicatorBarFlareSize.y);
    m_manaBarFlare->SetVisible(true);

    m_parentDialog->AddControl(m_manaBarFlare);
  }
}

void RPG_GuiManager_VisionGUI::UpdateHUD(const RPG_Character* characterEntity, const float deltaTime)
{
  UpdateIndicators(characterEntity, deltaTime);
  UpdateMinimap(characterEntity, deltaTime);
}

void RPG_GuiManager_VisionGUI::UpdateIndicators(const RPG_Character* characterEntity, const float deltaTime)
{
  VASSERT(characterEntity);

  // health indicator
  const int charHealth = characterEntity->GetCharacterStats().GetHealth();
  const int charHealthMax = characterEntity->GetCharacterStats().GetHealthMax();

  if (charHealth != m_lastHealth)
  {
    // set bar scale
    const float barVal = static_cast<float>(charHealth) / static_cast<float>(charHealthMax > 0 ? charHealthMax : 1);
    VASSERT(0.f <= barVal && barVal <= 1.f);
    VASSERT(m_healthBar);
    m_healthBar->SetSize(m_indicatorBarSize.x * barVal, m_indicatorBarSize.y);
    VASSERT(m_healthBarFlare);
    m_healthBarFlare->SetPosition((m_healthBarPosition.x - m_indicatorBarFlareOrigin.x) + m_indicatorBarSize.x * barVal, m_healthBarFlare->GetPosition().y);

    int const healthChange = charHealth - m_lastHealth;
    if (healthChange < 0.f)  // only flash on decrease
    {
      StartHealthBarGlow(m_indicatorBarGlowDefaultDuration);
    }

    m_lastHealth = charHealth;
  }
  if (m_healthGlowDuration > 0.f)
  {
    UpdateHealthBarGlow(deltaTime);
  }

  // mana indicator
  const int charMana = characterEntity->GetCharacterStats().GetMana();
  const int charManaMax = characterEntity->GetCharacterStats().GetManaMax();

  if (charMana != m_lastMana)
  {
    // set bar scale
    const float barVal = static_cast<float>(charMana) / static_cast<float>(charManaMax > 0 ? charManaMax : 1);
    VASSERT(0.f <= barVal && barVal <= 1.f);
    VASSERT(m_manaBar);
    m_manaBar->SetSize(m_indicatorBarSize.x * barVal, m_indicatorBarSize.y);
    VASSERT(m_manaBarFlare);
    m_manaBarFlare->SetPosition((m_manaBarPosition.x - m_indicatorBarFlareOrigin.x) + m_indicatorBarSize.x * barVal, m_manaBarFlare->GetPosition().y);

    int const manaChange = charMana - m_lastMana;
    if (manaChange < 0.f)  // only flash on decrease
    {
      StartManaBarGlow(m_indicatorBarGlowDefaultDuration);
    }

    m_lastMana = charMana;
  }

  // have we recently tried to spend mana we didn't have?
  float const lastManaSpendFailTime = characterEntity->GetCharacterStats().GetLastManaSpendFailTime();
  float const manaSpendFailWarningEndTime = lastManaSpendFailTime + m_manaSpendWarningDefaultDuration;
  if (lastManaSpendFailTime > 0.f && Vision::GetTimer()->GetTime() <= manaSpendFailWarningEndTime)
  {
    UpdateManaDepletedWarning(lastManaSpendFailTime, deltaTime);
  }
  else if (m_manaSpendWarningActive)
  {
    StopManaDepletedWarning();
  }

  if (m_manaGlowDuration > 0.f)
  {
    UpdateManaBarGlow(deltaTime);
  }
}

void RPG_GuiManager_VisionGUI::StartHealthBarGlow(float const duration)
{
  m_healthBarGlow->SetVisible(true);
  m_healthGlowDuration = duration;
  m_healthGlowStartTime = Vision::GetTimer()->GetTime();
}

void RPG_GuiManager_VisionGUI::StartManaBarGlow(float const duration)
{
  m_manaBarGlow->SetVisible(true);
  m_manaGlowDuration = duration;
  m_manaGlowStartTime = Vision::GetTimer()->GetTime();
}

void RPG_GuiManager_VisionGUI::UpdateHealthBarGlow(float const deltaTime)
{
  float const endTime = m_healthGlowStartTime + m_healthGlowDuration;
  float const remainingTime = endTime - Vision::GetTimer()->GetTime();
  if (remainingTime > 0.f)
  {
    UBYTE const alphaInterp = static_cast<UBYTE>(remainingTime * 255.f / m_healthGlowDuration);
    m_healthBarGlow->Image().m_States->SetColor(VColorRef(255, 255, 255, alphaInterp));
  }
  else
  {
    m_healthGlowDuration = 0.f;
    m_healthBarGlow->SetVisible(false);
  }
}

void RPG_GuiManager_VisionGUI::UpdateManaBarGlow(float const deltaTime)
{
  float const endTime = m_manaGlowStartTime + m_manaGlowDuration;
  float const remainingTime = endTime - Vision::GetTimer()->GetTime();
  if (remainingTime > 0.f)
  {
    UBYTE const alphaInterp = static_cast<UBYTE>(remainingTime * 255.f / m_manaGlowDuration);
    m_manaBarGlow->Image().m_States->SetColor(VColorRef(255, 255, 255, alphaInterp));
  }
  else
  {
    m_manaGlowDuration = 0.f;
    m_manaBarGlow->SetVisible(false);
  }
}

void RPG_GuiManager_VisionGUI::UpdateManaDepletedWarning(float const startTime, float const deltaTime)
{
  float const endTime = startTime + m_manaSpendWarningDefaultDuration;
  float const remainingTime = endTime - Vision::GetTimer()->GetTime();
  if (remainingTime > 0.f)
  {
    if (static_cast<int>(Vision::GetTimer()->GetTime() * 2 * m_manaSpendWarningFlashesPerSecond) % 2)
    {
      m_manaBarGlow->SetVisible(true);
    }
    else
    {
      m_manaBarGlow->SetVisible(false);
    }
  }
  m_manaSpendWarningActive = true;
}

void RPG_GuiManager_VisionGUI::StopManaDepletedWarning()
{
  m_manaBarGlow->SetVisible(false);
  m_manaSpendWarningActive = false;
}

void RPG_GuiManager_VisionGUI::AddMinimapToParentDialog()
{
  InitializeMinimapDefinitions();

  const RPG_LevelInfo* levelInfo = RPG_GameManager::s_instance.GetLevelInfo();
  if (levelInfo)
  {
    // add minimap
    VASSERT(m_minimapTexture && m_minimapFrameTexture && m_minimapMaskTexture);
    {
      // Add minimap
      VASSERT(!m_minimap);
      m_minimap = new RPG_GuiMinimap_VisionGUI();
      VASSERT(m_minimap);

      m_minimap->SetMaskTexture(m_minimapMaskTexture);
      m_minimap->SetFadeColor(hkvVec4(1.f, 1.f, 1.f, 1.f));
      m_minimap->SetTransform(hkvVec4(m_minimapZoomFactor, m_minimapZoomFactor, 0.f, 0.f));
      m_minimap->Image().SetTexture(m_minimapTexture);
      m_minimap->Image().SetTransparency(VIS_TRANSP_ALPHA);
      m_minimap->SetPosition(m_minimapPosition.x, m_minimapPosition.y);
      m_minimap->SetSize(m_minimapSize.x, m_minimapSize.y);
      m_minimap->SetVisible(true);     
      m_parentDialog->AddControl(m_minimap);
      m_minimap->OnActivate();

      // Add minimap frame
      VASSERT(!m_minimapFrame);
      m_minimapFrame = new VImageControl();
      VASSERT(m_minimapFrame);

      m_minimapFrame->Image().SetTexture(m_minimapFrameTexture);
      m_minimapFrame->Image().SetTransparency(VIS_TRANSP_ALPHA);
      m_minimapFrame->SetPosition(m_minimapPosition.x, m_minimapPosition.y);
      m_minimapFrame->SetSize(m_minimapSize.x, m_minimapSize.y);
      m_minimapFrame->SetVisible(true);
      m_parentDialog->AddControl(m_minimapFrame);

      // Add minimap player icon
      VASSERT(!m_minimapPlayer);
      m_minimapPlayer = new VImageControl();
      VASSERT(m_minimapPlayer);

      m_minimapPlayer->Image().SetTexture(m_minimapPlayerTexture);
      m_minimapPlayer->Image().SetTransparency(VIS_TRANSP_ALPHA);
      m_minimapPlayer->Image().SetTextureRange(VRectanglef(-0.5f,-0.5f, 0.5f, 0.5f)); 
      m_minimapPlayer->SetPosition(m_minimapPosition.x + m_minimapSize.x / 2.f - 16.f, m_minimapPosition.y + m_minimapSize.y / 2.f - 16.f);
      m_minimapPlayer->SetSize(32.f, 32.f);
      m_minimapPlayer->SetVisible(true);
      m_parentDialog->AddControl(m_minimapPlayer);
    }
  }
}

void RPG_GuiManager_VisionGUI::UpdateMinimap(const RPG_Character* characterEntity, const float deltaTime)
{
  if (m_minimap)
  {
    hkvVec2 pos = hkvVec2(characterEntity->GetPosition().x, characterEntity->GetPosition().y);
    float relX = m_minimapRelativeOrigin.x + (pos.x * m_minimapScaleFactor);
    float relY = m_minimapRelativeOrigin.y - (pos.y * m_minimapScaleFactor);
    m_minimap->SetTransform(hkvVec4(m_minimapZoomFactor, m_minimapZoomFactor, relX, relY));

    m_minimapPlayer->Image().m_States->CreateTransformation(m_minimapPlayerRotation, hkvVec2(0.5f, 0.5f), characterEntity->GetOrientation().x - 90.f);
    m_minimapPlayer->Image().SetTransformation(m_minimapPlayerRotation.getPointer(), NULL); 
  }
}

//void RPG_GuiManager_VisionGUI::DisplayHud()
//{
//  VASSERT(!m_hudDialog);  // ensure that nothing's creating this twice
//  VASSERT(m_parentDialog->GetContext());
//  m_hudDialog = m_parentDialog->GetContext()->ShowDialog(m_hudDialogFilename.AsChar());
//  VASSERT_MSG(m_hudDialog, "Hud dialog failed to load.");
//}


RPG_SkillButtonDefinition::RPG_SkillButtonDefinition()
  : m_imageFileName("")
  , m_textureObject(NULL)
  , m_size()
  , m_positionOffsetFromCorner()
  , m_positionCorner(CORNER_UpperLeft)
  , m_initiallyVisible(true)
{
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
