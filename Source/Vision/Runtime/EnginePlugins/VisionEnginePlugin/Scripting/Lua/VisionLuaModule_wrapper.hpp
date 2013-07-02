#ifndef __VISION_SWIG_WRAPPER_HPP
#define __VISION_SWIG_WRAPPER_HPP
#ifndef _VISION_DOC


typedef void *(*swig_converter_func)(void *, int *);
typedef struct swig_type_info *(*swig_dycast_func)(void **);

/* Structure to store information on one type */
typedef struct swig_type_info {
  const char             *name;			/* mangled name of this type */
  const char             *str;			/* human readable name of this type */
  swig_dycast_func        dcast;		/* dynamic cast function down a hierarchy */
  struct swig_cast_info  *cast;			/* linked list of types that can cast into this type */
  void                   *clientdata;		/* language specific type data */
  int                    owndata;		/* flag if the structure owns the clientdata */
} swig_type_info;

/* Structure to store a type and conversion function used for casting */
typedef struct swig_cast_info {
  swig_type_info         *type;			/* pointer to type that is equivalent to this type */
  swig_converter_func     converter;		/* function to cast the void pointers */
  struct swig_cast_info  *next;			/* pointer to next cast in linked list */
  struct swig_cast_info  *prev;			/* pointer to the previous cast */
} swig_cast_info;

/* Structure used to store module information
 * Each module generates one structure like this, and the runtime collects
 * all of these structures and stores them in a circularly linked list.*/
typedef struct swig_module_info {
  swig_type_info         **types;		/* Array of pointers to swig_type_info structures that are in this module */
  size_t                 size;		        /* Number of types in this module */
  struct swig_module_info *next;		/* Pointer to next element in circularly linked list */
  swig_type_info         **type_initial;	/* Array of initially generated type structures */
  swig_cast_info         **cast_initial;	/* Array of initially generated casting structures */
  void                    *clientdata;		/* Language specific module data */
} swig_module_info;


/* -----------------------------------------------------------------------------
 * global swig types
 * ----------------------------------------------------------------------------- */
/* Constant table */
#define SWIG_LUA_INT     1
#define SWIG_LUA_FLOAT   2
#define SWIG_LUA_STRING  3
#define SWIG_LUA_POINTER 4
#define SWIG_LUA_BINARY  5
#define SWIG_LUA_CHAR    6

/* Structure for variable linking table */
typedef struct {
  const char *name;
  lua_CFunction get;
  lua_CFunction set;
} swig_lua_var_info;

/* Constant information structure */
typedef struct {
    int type;
    char *name;
    long lvalue;
    double dvalue;
    void   *pvalue;
    swig_type_info **ptype;
} swig_lua_const_info;

typedef struct {
  const char     *name;
  lua_CFunction   method;
} swig_lua_method;

typedef struct {
  const char     *name;
  lua_CFunction   getmethod;
  lua_CFunction   setmethod;
} swig_lua_attribute;

typedef struct swig_lua_class {
  const char    *name;
  swig_type_info   **type;
  lua_CFunction  constructor;
  void    (*destructor)(void *);
  swig_lua_method   *methods;
  swig_lua_attribute     *attributes;
  struct swig_lua_class **bases;
  const char **base_names;
} swig_lua_class;

/* this is the struct for wrappering all pointers in SwigLua
*/
typedef struct {
  swig_type_info   *type;
  int     own;  /* 1 if owned & must be destroyed */
  void        *ptr;
} swig_lua_userdata;

/* this is the struct for wrapping arbitary packed binary data
(currently it is only used for member function pointers)
the data ordering is similar to swig_lua_userdata, but it is currently not possible
to tell the two structures apart within SWIG, other than by looking at the type
*/
typedef struct {
  swig_type_info   *type;
  int     own;  /* 1 if owned & must be destroyed */
  char data[1];       /* arbitary amount of data */    
} swig_lua_rawdata;

/* -------- TYPES TABLE (BEGIN) -------- */

#define SWIGTYPE_p_CubeMapHandle_cl swig_types[0]
#define SWIGTYPE_p_IVConsoleManager swig_types[1]
#define SWIGTYPE_p_IVObjectComponent swig_types[2]
#define SWIGTYPE_p_IVRendererNode swig_types[3]
#define SWIGTYPE_p_IVTimeOfDay swig_types[4]
#define SWIGTYPE_p_IVTimer swig_types[5]
#define SWIGTYPE_p_IVisTriggerBaseComponent_cl swig_types[6]
#define SWIGTYPE_p_PathCameraEntity swig_types[7]
#define SWIGTYPE_p_TriggerBoxEntity_cl swig_types[8]
#define SWIGTYPE_p_VAnimationComponent swig_types[9]
#define SWIGTYPE_p_VBitmask swig_types[10]
#define SWIGTYPE_p_VColorRef swig_types[11]
#define SWIGTYPE_p_VDialog swig_types[12]
#define SWIGTYPE_p_VDynamicMesh swig_types[13]
#define SWIGTYPE_p_VGUIManager swig_types[14]
#define SWIGTYPE_p_VPostProcessingBaseComponent swig_types[15]
#define SWIGTYPE_p_VScriptApp_wrapper swig_types[16]
#define SWIGTYPE_p_VScriptDebug_wrapper swig_types[17]
#define SWIGTYPE_p_VScriptDraw_wrapper swig_types[18]
#define SWIGTYPE_p_VScriptFileSystem_wrapper swig_types[19]
#define SWIGTYPE_p_VScriptInput_wrapper swig_types[20]
#define SWIGTYPE_p_VScriptRenderer_wrapper swig_types[21]
#define SWIGTYPE_p_VScriptScreen_wrapper swig_types[22]
#define SWIGTYPE_p_VScriptUtil_wrapper swig_types[23]
#define SWIGTYPE_p_VStringInputMap swig_types[24]
#define SWIGTYPE_p_VTextureObject swig_types[25]
#define SWIGTYPE_p_VTransitionStateMachine swig_types[26]
#define SWIGTYPE_p_VTypedObject swig_types[27]
#define SWIGTYPE_p_VWindowBase swig_types[28]
#define SWIGTYPE_p_VisBaseEntity_cl swig_types[29]
#define SWIGTYPE_p_VisContextCamera_cl swig_types[30]
#define SWIGTYPE_p_VisGame_cl swig_types[31]
#define SWIGTYPE_p_VisLightSource_cl swig_types[32]
#define SWIGTYPE_p_VisObject3D_cl swig_types[33]
#define SWIGTYPE_p_VisObjectKey_cl swig_types[34]
#define SWIGTYPE_p_VisParticleEffect_cl swig_types[35]
#define SWIGTYPE_p_VisPathNode_cl swig_types[36]
#define SWIGTYPE_p_VisPath_cl swig_types[37]
#define SWIGTYPE_p_VisScreenMask_cl swig_types[38]
#define SWIGTYPE_p_VisStaticMeshInstance_cl swig_types[39]
#define SWIGTYPE_p_VisSurface_cl swig_types[40]
#define SWIGTYPE_p_VisTriggerSourceComponent_cl swig_types[41]
#define SWIGTYPE_p_VisTriggerTargetComponent_cl swig_types[42]
#define SWIGTYPE_p_VisTypedEngineObject_cl swig_types[43]
#define SWIGTYPE_p___int64 swig_types[44]
#define SWIGTYPE_p_char swig_types[45]
#define SWIGTYPE_p_float swig_types[46]
#define SWIGTYPE_p_hkvAlignedBBox swig_types[47]
#define SWIGTYPE_p_hkvMat3 swig_types[48]
#define SWIGTYPE_p_hkvVec3 swig_types[49]
#define SWIGTYPE_p_int swig_types[50]
#define SWIGTYPE_p_long swig_types[51]
#define SWIGTYPE_p_p_VDynamicMesh swig_types[52]
#define SWIGTYPE_p_p_char swig_types[53]
#define SWIGTYPE_p_p_unsigned_long swig_types[54]
#define SWIGTYPE_p_short swig_types[55]
#define SWIGTYPE_p_signed___int64 swig_types[56]
#define SWIGTYPE_p_signed_char swig_types[57]
#define SWIGTYPE_p_unsigned___int64 swig_types[58]
#define SWIGTYPE_p_unsigned_char swig_types[59]
#define SWIGTYPE_p_unsigned_int swig_types[60]
#define SWIGTYPE_p_unsigned_long swig_types[61]
#define SWIGTYPE_p_unsigned_short swig_types[62]
#define SWIG_TypeQuery(name) SWIG_TypeQueryModule(&swig_module, &swig_module, name)
#define SWIG_MangledTypeQuery(name) SWIG_MangledTypeQueryModule(&swig_module, &swig_module, name)

extern swig_type_info *swig_types[];
extern swig_module_info swig_module;

/* -------- TYPES TABLE (END) -------- */

extern "C" void VSWIG_Lua_get_class_registry(lua_State* L);
extern "C" int  VSWIG_Lua_ConvertPtr(lua_State* L,int index,void** ptr,swig_type_info *type,int flags);
extern "C" void VSWIG_Lua_NewPointerObj(lua_State* L,void* ptr,swig_type_info *type, int own);
extern "C" void *VSWIG_TypeCast(swig_cast_info *ty, void *ptr, int *newmemory);
extern "C" swig_cast_info *VSWIG_TypeCheckStruct(swig_type_info *from, swig_type_info *ty);
extern "C" void VSWIG_VisionLuaClassSet(lua_State* L);
extern "C" void VSWIG_VisionLuaClassGet(lua_State* L);
extern "C" swig_cast_info *VSWIG_TypeCheck(const char *fromName, swig_type_info *ty);
extern "C" const char *VSWIG_Lua_typename(lua_State *L, int tp);

#endif // _VISION_DOC
#endif // __VISION_SWIG_WRAPPER_HPP
