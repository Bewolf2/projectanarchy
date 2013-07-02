/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_GUI_MANAGER_VISIONGUI_H__
#define RPG_PLUGIN_GUI_MANAGER_VISIONGUI_H__

#include <Vision/Samples/Engine/RPGPlugin/GUIManager.h>

class RPG_Character;

typedef VSmartPtr<VImageControl> VImageControlPtr;

class RPG_GuiMinimap_VisionGUI;
typedef VSmartPtr<RPG_GuiMinimap_VisionGUI> RPG_GuiMinimap_VisionGUIPtr;

enum RPG_SkillButtons_e
{
  BTN_None=-1,
  BTN_RangedAttack=0,
  BTN_PowerAttack,
  BTN_AoeAttack,
  BTN_RangedAttackAlt,
  BTN_PowerAttackAlt,
  BTN_AoeAttackAlt,
  BTN_Count
};

enum RPG_InterfaceCorners_e
{
  CORNER_UpperLeft=0,
  CORNER_UpperRight,
  CORNER_LowerRight,
  CORNER_LowerLeft,
  CORNER_Count
};

struct RPG_SkillButtonDefinition
{
  RPG_SkillButtonDefinition();

  VString m_imageFileName;
  VTextureObject* m_textureObject;
  hkvVec2 m_size;
  hkvVec2 m_positionOffsetFromCorner;
  RPG_InterfaceCorners_e m_positionCorner;
  bool m_initiallyVisible;
};

class RPG_GuiManager_VisionGUI : public RPG_GuiManager
{
public:
  RPG_GuiManager_VisionGUI();

  virtual void OnActivate(PlayerUIDialog* parentDialog) HKV_OVERRIDE;
  virtual void OnDeactivate() HKV_OVERRIDE;
  virtual void OnTick(float const deltaTime) HKV_OVERRIDE;

  bool IsButtonSelected(RPG_SkillButtons_e const& skillButton) const;

protected:
  void UpdateHUD(const RPG_Character* characterEntity, const float deltaTime);
  void UpdateIndicators(const RPG_Character* characterEntity, const float deltaTime);

  //@{
  // Test - adding HUD as a child dialog of the parent UI dialog
  //void DisplayHud();
  //VString m_hudDialogFilename;
  //VDialogPtr m_hudDialog;
  //@}

  //@{
  // skill buttons
  void InitializeButtonDefinitions();
  void AddSkillButtonsToParentDialog();
  hkvVec2 FindButtonPosition(hkvVec2 const& positionOffset, RPG_InterfaceCorners_e const& corner) const;
  RPG_SkillButtonDefinition m_skillButtonDefinitions[BTN_Count];
  VPushButton* m_skillButtons[BTN_Count];
  //@}

  //@{
  // health and mana bars
  void InitializeHealthAndManaBarDefinitions();
  void AddHealthAndManaBarsToParentDialog();
  void StartHealthBarGlow(float const duration);
  void StartManaBarGlow(float const duration);
  void UpdateHealthBarGlow(float const deltaTime);
  void UpdateManaBarGlow(float const deltaTime);
  void UpdateManaDepletedWarning(float const startTime, float const deltaTime);
  void StopManaDepletedWarning();

  VString m_indicatorBarTrackTextureFilename;
  VTextureObject* m_indicatorBarTrackTexture;
  hkvVec2 m_indicatorBarTrackSize;
  hkvVec2 m_indicatorBarSize; ///< both bars use the same size track
  hkvVec2 m_indicatorBarGlowSize;
  float m_indicatorBarGlowDefaultDuration;
  VString m_indicatorBarFlareTextureFilename;
  VTextureObject* m_indicatorBarFlareTexture;
  hkvVec2 m_indicatorBarFlareSize;
  hkvVec2 m_indicatorBarFlareOrigin;

  VString m_healthBarTextureFilename;
  VTextureObject* m_healthBarTexture;
  VString m_healthBarGlowTextureFilename;
  VTextureObject* m_healthBarGlowTexture;
  hkvVec2 m_healthBarPosition;
  VImageControlPtr m_healthBarTrack;
  VImageControlPtr m_healthBar;
  VImageControlPtr m_healthBarGlow;
  VImageControlPtr m_healthBarFlare;
  int m_lastHealth;
  float m_healthGlowStartTime;
  float m_healthGlowDuration;

  VString m_manaBarTextureFilename;
  VTextureObject* m_manaBarTexture;
  VString m_manaBarGlowTextureFilename;
  VTextureObject* m_manaBarGlowTexture;
  hkvVec2 m_manaBarPosition;
  VImageControlPtr m_manaBarTrack;
  VImageControlPtr m_manaBar;
  VImageControlPtr m_manaBarGlow;
  VImageControlPtr m_manaBarFlare;
  int m_lastMana;
  float m_manaGlowStartTime;
  float m_manaGlowDuration;
  float m_manaSpendWarningStartTime;
  float m_manaSpendWarningDefaultDuration;
  int m_manaSpendWarningFlashesPerSecond;
  bool m_manaSpendWarningActive;
  //@}#

  //@{
  // minimap
  void InitializeMinimapDefinitions();
  void AddMinimapToParentDialog();
  void UpdateMinimap(const RPG_Character* characterEntity, const float deltaTime);

  VString m_minimapTextureFilename;
  VString m_minimapFrameTextureFilename;
  VString m_minimapMaskTextureFilename;
  VString m_minimapPlayerTextureFilename;
  VTextureObject* m_minimapTexture;
  VTextureObject* m_minimapFrameTexture;
  VTextureObject* m_minimapMaskTexture;
  VTextureObject* m_minimapPlayerTexture;
  hkvVec2 m_minimapPosition;
  hkvVec2 m_minimapSize;
  hkvVec2 m_minimapRelativeOrigin;
  hkvMat3 m_minimapPlayerRotation;
  float m_minimapScaleFactor;
  float m_minimapZoomFactor;
  RPG_GuiMinimap_VisionGUIPtr m_minimap;
  VImageControlPtr m_minimapFrame;
  VImageControlPtr m_minimapPlayer;
  //@}
};

#endif  // RPG_PLUGIN_GUI_MANAGER_VISIONGUI_H__

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
