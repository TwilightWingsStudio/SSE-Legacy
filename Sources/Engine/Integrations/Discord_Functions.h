/* Copyright (c) 2018 ZCaliptium. 
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

// DLLFUNCTION(dll, output, name, inputs, params, required)

// Functions from the discord_rpc.h
DLLFUNCTION( DRP, void, Discord_Initialize, (const char* applicationId, DiscordEventHandlers* handlers, int autoRegister, const char* optionalSteamId), 16,0);
DLLFUNCTION( DRP, void, Discord_Shutdown, (void), 0,0);
DLLFUNCTION( DRP, void, Discord_RunCallbacks, (void), 0,0);
//DLLFUNCTION( DRP, void, Discord_UpdateConnection, (void), 0,0);
DLLFUNCTION( DRP, void, Discord_UpdatePresence, (const DiscordRichPresence* presence), 8,0);
DLLFUNCTION( DRP, void, Discord_ClearPresence, (void), 0,0);
DLLFUNCTION( DRP, void, Discord_Respond, (const char* userid, /* DISCORD_REPLY_ */ int reply), 8,0);

// Functions from the discord_register.h
DLLFUNCTION( DRP, void, Discord_Register, (const char* applicationId, const char* command), 8,0);
DLLFUNCTION( DRP, void, Discord_RegisterSteamGame, (const char* applicationId, const char* steamId), 8,0);