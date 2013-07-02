
#ifndef VLUA_APIDOC

%nodefaultctor VisParticleEffect_cl;
%nodefaultdtor VisParticleEffect_cl;
class VisParticleEffect_cl : public VisObject3D_cl
{
public:
 
  %rename(Remove) Dispose();
  inline void Dispose();
 
  %extend{
    VSWIG_BITMASK_SETTER(SetVisibleBitmask);
    
    const char * GetName()
    {
      return self->GetSourceEffect()->GetFilename();
    }
    
    void IncPosition(float x, float y, float z, bool bMoveParticles = false)
    {
      self->IncPosition(hkvVec3(x,y,z), bMoveParticles);
    }
    
    void ContinueSimulationWhenInvisible(bool bContinue)
    {
        self->SetHandleWhenVisible(!bContinue); //this is a very confusing method name
    }
    
    hkvAlignedBBox GetLocalBoundingBox()
    {
      hkvAlignedBBox box;
      self->GetLocalBoundingBox(box);
      return box;
    }
  }
  
  inline void SetVisible(bool bStatus);
  bool IsVisible() const;

  void IncPosition(const hkvVec3& vDelta, bool bMoveParticles = false);

  void SetScaling(float fScale);

  //void SetAmbientColor(VColorRef iColor);

  %rename(SetPaused) SetPause(bool bStatus);
  void SetPause(bool bStatus);
  bool IsPaused() const;

  void SetHalted(bool bStatus);
  bool IsHalted() const;
 
  float GetRemainingLifeTime();
  bool IsLifeTimeOver();
  bool IsDead();
  
  void SetFinished();

  void SetApplyTimeOfDayLight(bool bApply);
  bool GetApplyTimeOfDayLight(void) const;

  void Restart();
  void SetRemoveWhenFinished(bool bStatus);
  void SetIntensity(float fIntensity);

  void RemoveAllConstraints();
  void SetWindSpeed(const hkvVec3& vWind, bool bApplyInLocalSpace=false);

  void SetMeshEmitterEntity(VisBaseEntity_cl *pEntity);
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VisParticleEffect_cl, 128, "[%s/%s/%s : %1.2f,%1.2f,%1.2f]", self->IsVisible()?"V":"-", self->IsPaused()?"P":"-", self->IsHalted()?"H":"-", self->GetPosition().x, self->GetPosition().y, self->GetPosition().z)
VSWIG_CREATE_TOSTRING_EX(VisParticleEffect_cl, hkvVec3 vPos = self->GetPosition();, "VisParticleEffect_cl '%s' [%s/%s/%s : %1.2f,%1.2f,%1.2f]", self->GetObjectKey()==NULL?self->GetSourceEffect()->GetFilename():self->GetObjectKey(), self->IsVisible()?"V":"-", self->IsPaused()?"P":"-", self->IsHalted()?"H":"-", vPos.x, vPos.y, vPos.z)

#else

/// \brief Base class for particle effects.
/// \see VisGame_cl::GetEffect
/// \see VisGame_cl::CreateEffect
/// \par Example
///   \code
///     function OnAfterSceneLoaded(self)
///       local pos = Vision.hkvVec3(100,0,100)
///       local effect = Game:CreateEffect(pos, "Particles/Fire.xml", "MyFire")
///       effect:SetPaused(false)
///     end
///   \endcode
class VisParticleEffect_cl : public VisObject3D_cl {
public:

  /// @name Common Particle Effect Functions
  /// @{
  
  /// \brief Removes this particle effect from the scene and destroys it.
  void Remove();

  /// \brief Get the particle effects file name.
  /// \return The file name of the xml configuration.
  string GetName();
  
  /// \brief Increment the particle emitters position.
  /// \param delta The incremental vector.
  /// \param moveParticles true = move already spawned partivles as well; false = move the emitter only.
  /// \par Example
  ///   \code
  ///     function OnThink(self) -- attached to a particle effect
  ///       self:IncPosition(Vision.hkvVec3(10*Timer:GetTimeDiff(),0,0), true)
  ///     end
  ///   \endcode
  void IncPosition(hkvVec3 delta, boolean moveParticles = false);
  
  /// \brief Increment the particle emitters position.
  /// \param x x axis increment.
  /// \param y y axis increment.
  /// \param z z axis increment.
  /// \param moveParticles true = move already spawned partivles as well; false = move the emitter only.
  void IncPosition(number x, number y, number z, boolean moveParticles = false);

  /// \brief Unifrom scake the particle effect.
  /// \param scale The scale amount.
  void SetScaling(number scale);

  /// \brief Set an additional color that is multiplied on each particle.
  /// \param color The additional color.
  void SetColor(VColorRef color);
  
  /// @}
  /// @name Visiblity / Lighting
  /// @{
  
  /// \brief Makes the entity visible or invisible in all contexts.
  /// \param visible The new visible status.
  /// \note This is a convenience function for SetVisibleBitmask.
  /// \see SetVisibleBitmask
  void SetVisible(boolean visible);

  /// \brief Checks whether the entity is visible in all contexts.
  /// \return true if visible; other false.
  boolean IsVisible();
  
  /// \brief Makes the entity visible in certain rendering contexts only.
  /// \details
  ///    This entity will only be rendered in render contexts, if the logical AND result of this bitmask and the context's bitmask is not zero.
  /// \param mask The new visible bitmask.
  void SetVisibleBitmask(VBitmask mask);
  
  /// \brief Set the effect of the TimeOfDay light to the particles.
  /// \param apply If set to true, the light of the TimeOfDay component will be added to the particles color. 
  /// \par Example
  ///   \code
  ///     function OnCreate(self) -- attached to a particle effect
  ///       self:SetApplyTimeOfDayLight(true)
  ///     end
  ///   \endcode
  void SetApplyTimeOfDayLight(boolean apply);

  /// \brief Returns whether TimeOfDay light is supposed to be applied on this effect.
  boolean GetApplyTimeOfDayLight();
  
  /// \brief Changes simulation state for all layers: Continue particle simulation even when invisble.
  /// \param continue If set to true, the simulation will continue even if the effect is invisible, otherwise not.
  void ContinueSimulationWhenInvisible(boolean continue);

  /// @}
  /// @name Bounding Box
  /// @{
  
  /// \brief Get the local bounding box (merged from all children).
  /// \return The local bounding box.
  hkvAlignedBBox GetLocalBoundingBox();
  
  /// @}
  /// @name Particle Effect Control
  /// @{
  
  /// \brief Increment the particle emitters position.
  /// \param paused true = no more particles will be spawned; false = particle emitter will continue.
  /// \par Example
  ///   \code
  ///     function OnThink(self) -- attached to a particle effect
  ///       if Timer:GetTime() > 20 then 
  ///         self:SetPaused(true) -- pause the effect after 20 sec
  ///       end
  ///     end
  ///   \endcode
  void SetPaused(boolean paused = false);
  
  /// \brief Checks whether the particle emitter has been paused or not.
  /// \return true if paused; other false.
  boolean IsPaused();
  
  /// \brief Restarts the particle effect.
  void Restart();
  
  /// \brief When this flag is enabled, then this instance is deleted as soon as it is dead (all layers finished). 
  /// \note In the Lua binding this is not default since there is a
  ///   potential to access a "not any more" existing particle effect (when the effect is already finished),
  ///   causing an unpredictable behavior.
  /// \param status Set to true in order to delete the instance automatically when finished.  
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       local pos = Vision.hkvVec3(0,0,200)
  ///       local effect = Game:CreateEffect(pos, "Particles/Explosion.xml")
  ///       effect:SetRemoveWhenFinished(true)
  ///       effect:SetPaused(false)
  ///     end
  ///   \endcode
  void SetRemoveWhenFinished(boolean status);

  /// \brief Returns the remaining lifetime of this effect in seconds.
  /// \return -1 for infinite lifetime otherwise a value >= 0.
  number GetRemainingLifeTime();
 
  /// \brief Checks whether the the lifetime of all layers of the particle effect is over.
  /// \return true if the liftime for all layer is over; other false.
  boolean IsLifeTimeOver();
  
  /// \brief Indicates whether this instance is ready for removal. 
  /// \return true if ready for removal; other false.
  boolean IsDead();

  /// \brief Marks the particle instance as finished, which means that it won't emit any particles anymore. 
  /// \details The status of the finished particle group can be retrieved with IsLifeTimeOver and IsDead. 
  void SetFinished();   

  /// \brief Sets the emitter intensity on all layers. 
  /// \param intensity This factor modulates the spawn frequency of the emitters. 
  void SetIntensity(float intensity);
  
  /// \brief Detach all constraints from all layers. 
  void RemoveAllConstraints();
  
  /// \brief Sets a wind speed vector for all layers. Simple wind can be applied with no performance impact.
  /// \param wind The wind vector. 
  /// \param applyInLocalSpace true = apply wind vector in the effects local space: false = apply wind in world space. 
  void SetWindSpeed(hkvVec3 wind, boolean applyInLocalSpace);
  
  /// \brief Associate all layers with emitter type 'mesh' with the passed entity instance. 
  /// \param entity The entity to be used as emitter.
  void SetMeshEmitterEntity(VisBaseEntity_cl entity);

  /// @}
  
};

#endif
