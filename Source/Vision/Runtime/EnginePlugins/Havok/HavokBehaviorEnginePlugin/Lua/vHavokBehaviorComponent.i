#ifndef VLUA_APIDOC

%nodefaultctor vHavokBehaviorComponent;
%nodefaultdtor vHavokBehaviorComponent;

class vHavokBehaviorComponent : public IVObjectComponent
{
public:
   %rename(Remove) DisposeObject();
   void DisposeObject();
	
   bool IsNodeActive(const char* nodeName);
   void SetFloatVar(const char* variableName, float value);
   void SetWordVar(const char* variableName, int value);
   void SetBoolVar(const char* variableName, bool value);
   void TriggerEvent(const char* variableName);
   void GetBoneTransform( const char* boneName, hkvVec3& outPos, hkvMat3& outRot );
    
   %extend{  
   void TempMeth( hkvVec3 center )
		{
		}
		
	VSWIG_CREATE_CAST(vHavokBehaviorComponent)
   }
};

#else

/// \brief Behavior component class (Havok): Object component wrapper class that provides a behavior functionality for an entity.
class vHavokBehaviorComponent : public IVObjectComponent
{
public:
	
  /// @}
  /// @name Variables management
  /// @{

  /// \brief Gets if the current node is active in the behavior graph
  /// \returns if the node is active
  bool IsNodeActive(const char* nodeName);

  /// \brief Sets the value of the selected behavior float variable
  /// \param variableName  Name of the behavior variable
  /// \param value  value we want to assign to it
  void SetFloatVar(const char* variableName, float value);

  /// \brief Sets the value of the selected behavior word variable
  /// \param variableName  Name of the behavior variable
  /// \param value  value we want to assign to it
  void SetWordVar(const char* variableName, int value);
  
  /// \brief Sets the value of the selected behavior boolean variable
  /// \param variableName  Name of the behavior variable
  /// \param value  value we want to assign to it
  void SetBoolVar(const char* variableName, bool value);
  
  /// \brief Triggers a behavior event
  /// \param eventName  Name of the behavior event
  void TriggerEvent(const char* eventName);

  /// \brief Returns world space transform of the selected bone
  /// \param boneName  Name of the selected bone
  /// \param outPos    Bone's position
  /// \param outDir    Bone's direction vector
  void GetBoneTransform(const char* boneName, hkvVec3& outPos, hkvMat3& outRot );

  /// @}

};

#endif
