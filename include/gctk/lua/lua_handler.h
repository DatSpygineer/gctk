#ifndef GCTK_LUA_HANDLER_H
#define GCTK_LUA_HANDLER_H

#include "gctk/common.h"
#include "gctk/input.h"
#include "gctk/math.h"

#include <lua.h>

GCTK_API void       GctkSetupLua();
GCTK_API void       GctkCloseLua();
GCTK_API lua_State* GctkGetLuaState();
GCTK_API lua_State* GctkCreateCoroutine();

GCTK_API int        GctkLuaLoadString(const char* cstr);
GCTK_API int        GctkLuaRunString(const char* cstr);

inline void GctkLuaPushNil           (lua_State* L) { lua_pushnil(L); }
inline void GctkLuaPushBool          (lua_State* L,bool value) { lua_pushboolean(L, value); }
inline void GctkLuaPushInt           (lua_State* L,lua_Integer value) { lua_pushinteger(L, value); }
inline void GctkLuaPushNumber        (lua_State* L,lua_Number value) { lua_pushnumber(L, value); }
inline void GctkLuaPushString        (lua_State* L,const char* value) { lua_pushstring(L, value); }
inline void GctkLuaPushCallback      (lua_State* L,lua_CFunction value) { lua_pushcfunction(L, value); }

GCTK_API void GctkLuaPushVec2        (lua_State* L, Vec2 value);
GCTK_API void GctkLuaPushVec3        (lua_State* L, Vec3 value);
GCTK_API void GctkLuaPushVec4        (lua_State* L, Vec4 value);
GCTK_API void GctkLuaPushQuat        (lua_State* L, Quat value);
GCTK_API void GctkLuaPushColor       (lua_State* L, Color value);
GCTK_API void GctkLuaPushMat4        (lua_State* L, Mat4 value);
GCTK_API void GctkLuaPushAxisAngle   (lua_State* L, AxisAngle value);
GCTK_API void GctkLuaPushTransform2D (lua_State* L, Transform2D value);
GCTK_API void GctkLuaPushTransform3D (lua_State* L, Transform3D value);

inline int         GctkLuaGetType    (lua_State* L, int idx) { return lua_type(L, idx); }
inline const char* GctkLuaGetTypeName(lua_State* L, int idx) { return lua_typename(L, idx); }

GCTK_API bool GctkLuaGetBool         (lua_State* L, int idx, bool* value);
GCTK_API bool GctkLuaGetInt          (lua_State* L, int idx, lua_Integer* value);
GCTK_API bool GctkLuaGetNumber       (lua_State* L, int idx, lua_Number* value);
GCTK_API bool GctkLuaGetString       (lua_State* L, int idx, const char** value);
GCTK_API bool GctkLuaGetTable        (lua_State* L, int idx);
GCTK_API bool GctkLuaGetVec2         (lua_State* L, int idx, Vec2* value);
GCTK_API bool GctkLuaGetVec3         (lua_State* L, int idx, Vec3* value);
GCTK_API bool GctkLuaGetVec4         (lua_State* L, int idx, Vec4* value);
inline   bool GctkLuaGetQuat         (lua_State* L, int idx, Quat* value) { return GctkLuaGetVec4(L, idx, (Quat*)value); }
GCTK_API bool GctkLuaGetColor        (lua_State* L, int idx, Color* value);
GCTK_API bool GctkLuaGetMat4         (lua_State* L, int idx, Mat4* value);
GCTK_API bool GctkLuaGetAxisAngle    (lua_State* L, int idx, AxisAngle* value);
GCTK_API bool GctkLuaGetTransform2D  (lua_State* L, int idx, Transform2D* value);
GCTK_API bool GctkLuaGetTransform3D  (lua_State* L, int idx, Transform3D* value);

inline void GctkLuaPop               (lua_State* L, int count) { lua_pop(L, count); }
inline void GctkSetGlobal            (lua_State* L, const char* name) { lua_setglobal(L, name); }
inline void GctkGetGlobal            (lua_State* L, const char* name) { lua_getglobal(L, name); }
inline void GctkSetField             (lua_State* L, const char* name, int obj_idx) { lua_getfield(L, obj_idx, name); }
inline void GctkGetField             (lua_State* L, const char* name, int obj_idx) { lua_getfield(L, obj_idx, name); }

#endif