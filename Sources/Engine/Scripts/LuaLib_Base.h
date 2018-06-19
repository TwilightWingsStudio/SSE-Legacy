/* Copyright (c) 2018 by ZCaliptium.

This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */


#ifndef SE_INCL_LUA_LIB_BASE_H
#define SE_INCL_LUA_LIB_BASE_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/API/IScriptAPI.h>
#include <luajit/src/lua.hpp>

class CLuaLibBase : public IScriptAPI
{
  public:
    static const luaL_Reg methods[];
  
  private:
    static int l_print(lua_State* L);
    static int l_debug(lua_State* L);
};

#endif  /* include-once check. */