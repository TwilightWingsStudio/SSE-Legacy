/* Copyright (c) 2002-2012 Croteam Ltd. 
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

// Players are divided in teams.
#define GMF_TEAMPLAY      (1L << 0)
#define GMF_BASEONLYSPAWN (1L << 1)

#define MUTF_INSTAGIB    (1L << 0)
#define MUTF_VAMPIRE     (1L << 1)
#define MUTF_ROCKETJUMP  (1L << 2)

/*
 * Class responsible for describing game session
 */
class CSessionProperties
{
public:
  enum GameMode {
    GM_FLYOVER = -1,
    GM_COOPERATIVE = 0,
    GM_SCOREMATCH,
    GM_FRAGMATCH,

    GM_TEAMDEATHMATCH,   // [SSE] GameModes - Team DeathMatch
    GM_LASTMANSTANDING,  // [SSE] GameModes - LMS

    GM_LASTGAMEMODE,     // Not actually gamemode. Just border.
    
    GM_CAPTURETHEFLAG,   // [SSE] GameModes - CTF
    GM_LASTTEAMSTANDING, // [SSE] GameModes - LTS
    GM_PAYLOAD,          // [SSE] GameModes - PD
    GM_PAYLOADRACE,      // [SSE] GameModes - PFR
    GM_HOLDTHEOBJECT,    // [SSE] GameModes - HTO
    GM_KINGOFTHEHILL,    // [SSE] GameModes - KotH
    GM_DOMINATION,       // [SSE] GameModes - Domination
    
    // Cooperative
    GM_SURVIVAL,         // [SSE] GameModes - Survival
    GM_DEFEND,           // [SSE] GameModes - Defend
  };

  enum GameDifficulty {
    GD_TOURIST = -1,
    GD_EASY = 0,
    GD_NORMAL,
    GD_HARD,
    GD_EXTREME,
  };

  INDEX sp_ctMaxPlayers;    // maximum number of players in game
  BOOL sp_bWaitAllPlayers;  // wait for all players to connect
  BOOL sp_bQuickTest;       // set when game is tested from wed
  BOOL sp_bCooperative;     // players are not intended to kill each other
  BOOL sp_bSinglePlayer;    // single player mode has some special rules
  BOOL sp_bUseFrags;        // set if frags matter instead of score

  enum GameMode sp_gmGameMode;    // general game rules

  enum GameDifficulty sp_gdGameDifficulty;
  ULONG sp_ulSpawnFlags;
  BOOL sp_bMental;            // set if mental mode engaged

  INDEX sp_iScoreLimit;       // stop game after a player/team reaches given score
  INDEX sp_iFragLimit;        // stop game after a player/team reaches given score
  INDEX sp_iTimeLimit;        // stop game after given number of minutes elapses

  ULONG sp_ulGameModeFlags;

  BOOL sp_bFriendlyFire;      // can harm player of same team
  BOOL sp_bWeaponsStay;       // weapon items do not dissapear when picked-up
  BOOL sp_bAmmoStays;         // ammo items do not dissapear when picked-up
  BOOL sp_bHealthArmorStays;  // health/armor items do exist
  BOOL sp_bPlayEntireGame;    // don't finish after one level in coop
  BOOL sp_bAllowHealth;       // health items do exist
  BOOL sp_bAllowArmor;        // armor items do exist
  BOOL sp_bAllowPowerUps;     // [SSE] PowerUps Allow
  BOOL sp_bAllowWeapons;      // [SSE] Weapons Allow
  BOOL sp_bInfiniteAmmo;      // ammo is not consumed when firing
  BOOL sp_bRespawnInPlace;    // players respawn on the place where they were killed, not on markers (coop only)

  FLOAT sp_fEnemyMovementSpeed; // enemy speed multiplier
  FLOAT sp_fEnemyAttackSpeed;   // enemy speed multiplier
  FLOAT sp_fDamageStrength;     // multiplier when damaged
  FLOAT sp_fAmmoQuantity;       // multiplier when picking up ammo
  FLOAT sp_fManaTransferFactor; // multiplier for the killed player mana that is to be added to killer's mana
  INDEX sp_iInitialMana;        // life price (mana that each player'll have upon respawning)
  FLOAT sp_fExtraEnemyStrength;            // fixed adder for extra enemy power 
  FLOAT sp_fExtraEnemyStrengthPerPlayer;   // adder for extra enemy power per each player playing
  // TODO: Extra Boss Strength
  // TODO: Extra boss Strength PerPlayer

  INDEX sp_ctCredits;               // number of credits for this game
  INDEX sp_ctCreditsLeft;           // number of credits left on this level
  FLOAT sp_tmSpawnInvulnerability;  // how many seconds players are invunerable after respawning
  FLOAT sp_tmRespawnDelay;          // [SSE] Respawn Delay - How many seconds should pass after player death to be able respawn.

  INDEX sp_iBlood;         // blood/gibs type (0=none, 1=green, 2=red, 3=hippie)
  BOOL  sp_bGibs;          // enable/disable gibbing

  BOOL  sp_bEndOfGame;     // marked when dm game is finished (any of the limits reached)

  ULONG sp_ulLevelsMask;    // mask of visited levels so far

  BOOL  sp_bUseExtraEnemies;  // spawn extra multiplayer enemies
  
  // [SSE] Extra Lives System
  BOOL sp_bSharedLives;
  BOOL sp_bRaisingLiveCost;
  BOOL sp_bTransferLivesWhenPlayerLeft;
  INDEX sp_iScoreForExtraLive;
  INDEX sp_iScoreForExtraLiveAccum; // Internal.
  FLOAT sp_fLiveCostMultiplier;     // Internal.
  //
  
  // [SSE] Gameplay - Better Keys
  BOOL sp_bSharedKeys;
  BOOL sp_bSaveKeysWhenServerEmpty;
  ULONG sp_ulPickedKeys;
  //
  
  // [SSE] Gameplay - Sniper No Scope
  BOOL sp_bSniperFullDamageInNoScope;
  //
  
  // [SSE] Gameplay - Safe Flamethrower
  BOOL sp_bSafeFlamethrower;
  //
  
  BOOL sp_bKeepSeriousDamageOnProjectiles; // [SSE] Better Serious Damage
  BOOL sp_bArmorInertiaDamping;            // [SSE] Armor Inertia Damping Toggle
  
  BOOL sp_bTeamPlayPrefferBases;
  INDEX sp_iTeamKillPenalty; // [SSE] Gameplay - TeamKill Penalty
  
  BOOL sp_bPickUpWeaponsOnce; // [SSE] Pick up weapons once.
  
  BOOL sp_bDropPowerUps; // [SSE] PowerUps Drop
  BOOL sp_bDropWeapons;  // [SSE] Weapons Drop
  
  // [SSE] GameModes - Team DeathMatch
  INDEX sp_ctTeams;
  
  INDEX sp_iTeamScore1;
  INDEX sp_iTeamScore2;
  INDEX sp_iTeamScore3;
  INDEX sp_iTeamScore4;
  //

  ULONG sp_ulMutatorFlags;
};

// NOTE: never instantiate CSessionProperties, as its size is not fixed to the size defined in engine
// use CUniversalSessionProperties for instantiating an object
class CUniversalSessionProperties {
public:
  union {
    CSessionProperties usp_sp;
    UBYTE usp_aubDummy[NET_MAXSESSIONPROPERTIES];
  };

  // must have exact the size as allocated block in engine
  CUniversalSessionProperties() { 
    ASSERT(sizeof(CSessionProperties)<=NET_MAXSESSIONPROPERTIES); 
    ASSERT(sizeof(CUniversalSessionProperties)==NET_MAXSESSIONPROPERTIES); 
  }
  operator CSessionProperties&(void) { return usp_sp; }
};

