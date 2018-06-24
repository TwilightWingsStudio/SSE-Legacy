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

#ifndef SE_INCL_QUERY_PROTOCOL_MANAGER_H
#define SE_INCL_QUERY_PROTOCOL_MANAGER_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

extern CTString ms_strServer;
extern CTString ms_strLegacyMS;
extern CTString ms_strDarkPlacesMS;
extern CTString ms_strGameName;
extern BOOL ms_bDarkPlacesMS;
extern BOOL ms_bDarkPlacesDebug;
extern BOOL ms_bLegacyMS;
extern BOOL ms_bLegacyDebug;

class CQueryProtocolMgr
{
  public:
    // Server-list enumeration.
    static void EnumTrigger(BOOL bInternet);
    static void EnumUpdate();
    static void EnumCancel();
    
  public:
    static void SendHeartbeat(INDEX iChallenge = 0);

  public:
    // Event handlers.
    static void OnServerStart();
    static void OnServerEnd();
    static void OnServerUpdate();
    static void OnServerStateChanged();
};

/// Server request structure. Primarily used for getting server pings.
class CServerRequest
{
  public:
    ULONG sr_ulAddress;
    USHORT sr_iPort;
    long long sr_tmRequestTime;

  public:
    CServerRequest(void);
    ~CServerRequest(void);

    /* Destroy all objects, and reset the array to initial (empty) state. */
    void Clear(void);
};

#endif // include once check
