/*
 * lua_audio.h
 *
 * Lua audio library. Uses OpenAL software implementation
 * */
#ifndef lua_audio_h
#define lua_audio_h

#include <lua.h>
#include <lauxlib.h>

void lua_audio_load(lua_State *L);

#endif // lua_audio_h
