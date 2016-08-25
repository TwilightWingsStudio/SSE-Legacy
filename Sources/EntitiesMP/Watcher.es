700
%{
#include "StdH.h"

#include "EntitiesMP/EnemyBase.h"
%}

// input parameter for watcher
event EWatcherInit {
  CEntityPointer penOwner,        // who owns it
};

// entity is seen
event EWatch {
  CEntityPointer penSeen,
};

class export CWatcher : CRationalEntity {
name      "Watcher";
thumbnail "";
features  "CanBePredictable";


properties:

  1 CEntityPointer m_penOwner,  // entity which owns it
  2 FLOAT m_tmDelay = 2.0f,     // delay between checking moments - set depending on distance of closest player

 20 FLOAT m_fClosestPlayer = UpperLimit(0.0f),  // distance from closest player to owner of this watcher
 21 INDEX m_iPlayerToCheck = 0,   // sequence number for checking next player in each turn
 22 CEntityPointer m_penAllocatedEnemy,
 23 CEntityPointer m_penClosestFriend,
 24 FLOAT m_fClosestFriend = UpperLimit(0.0f),


components:


functions:
  // --------------------------------------------------------------------------------------
  // Get watcher's owner.
  // --------------------------------------------------------------------------------------
  class CEnemyBase* GetOwner(void)
  {
    ASSERT(m_penOwner != NULL);
    return (CEnemyBase*)&*m_penOwner;
  }
  
  // --------------------------------------------------------------------------------------
  // Find one player number by random.
  // --------------------------------------------------------------------------------------
  INDEX GetRandomPlayer(void)
  {
    //CPrintF("Getting random number... ");

    // get maximum number of players in game
    INDEX ctMaxPlayers = GetMaxPlayers();

    // find actual number of players
    INDEX ctActivePlayers = 0;

    {for(INDEX i = 0; i < ctMaxPlayers; i++) {
      // Skip invalid players.
      if (GetPlayerEntity(i) == NULL) {
        continue;
      }

      ctActivePlayers++;
    }}
    
    //CPrintF("active players %d, ", ctActivePlayers);

    // If no any active players.
    if (ctActivePlayers == 0) {
      // return first index anyway
      return 0;
    }

    // choose one player by random
    INDEX iChosenActivePlayer = IRnd()%ctActivePlayers;

    //CPrintF("chosen %d, ", iChosenActivePlayer);

    // find its physical index
    INDEX iActivePlayer = 0;

    {for(INDEX i = 0; i < ctMaxPlayers; i++) {
      // Skip invalid players.
      if (GetPlayerEntity(i) == NULL) {
        continue;
      }

      if (iActivePlayer == iChosenActivePlayer) {
          //CPrintF("actual index %d\n", iActivePlayer);
          return i;
      }

      iActivePlayer++;
    }}

    ASSERT(FALSE);

    return 0;
  }

  // --------------------------------------------------------------------------------------
  // Cast a ray to entity checking only for brushes.
  // --------------------------------------------------------------------------------------
  /*
  BOOL IsVisible(CEntity *penEntity) 
  {
    ASSERT(penEntity!=NULL);
    // get ray source and target
    FLOAT3D vSource, vTarget;
    GetPositionCastRay(this, penEntity, vSource, vTarget);

    // cast the ray
    CCastRay crRay(this, vSource, vTarget);
    crRay.cr_ttHitModels = CCastRay::TT_NONE;     // check for brushes only
    crRay.cr_bHitTranslucentPortals = FALSE;
    en_pwoWorld->CastRay(crRay);

    if (crRay.cr_penHit) {
      if (IsOfClass(crRay.cr_penHit,"Movable Model") || IsOfClass(crRay.cr_penHit, "Movable Brush")) {
        if ( crRay.cr_penHit == penEntity->GetParent() || penEntity == crRay.cr_penHit->GetParent()) {
          return true;
        }
      }
    }

    // if hit nothing (no brush) the entity can be seen
    return (crRay.cr_penHit == NULL);     
  };
  */

  // --------------------------------------------------------------------------------------
  // Find Closest CEnemyBase Entity.
  // --------------------------------------------------------------------------------------
  CEntity* FindClosestEB(void)
  {
    CEntity *penClosestPlayer = NULL;
    m_penClosestFriend = NULL;
    FLOAT fClosestPlayer = UpperLimit(0.0f);
    FLOAT fClosestFriend = UpperLimit(0.0f);
    //m_fClosestPlayer = UpperLimit(0.0f);
    m_fClosestFriend = UpperLimit(0.0f);

    CEnemyFactionHolder* penEFH = GetOwner()->GetFactionHolder(TRUE);

    {FOREACHINDYNAMICCONTAINER(GetWorld()->wo_cenEntities, CEntity, iten) {
        CEntity *pen = iten;

        // Skip invalid entities.
        if (pen == NULL) {
          continue;
        }

        // Skip non enemies.
        if (!IsDerivedFromClass(pen, "Enemy Base")) {
          continue;
        }

        // Skip dead enemies.
        if (!(pen->GetFlags()&ENF_ALIVE)) {
          continue;
        }

        // Cast to CEnemyBase.
        CEnemyBase &enEB = (CEnemyBase&)*pen;

        // Skip templates.
        if (enEB.m_bTemplate) {
          continue;
        }

        CEnemyFactionHolder* penEnemyEFH = enEB.GetFactionHolder(TRUE);

        // Skip enemies without faction holders.
        if (penEnemyEFH == NULL) {
          continue;
        }

        // Skip enemies with invalid faction index.
        if (!penEnemyEFH->IsIndexValid()) {
          continue;
        }

        // Skip enemies from the same faction as owner.
        if (penEFH->m_iFactionIndex == penEnemyEFH->m_iFactionIndex) {
          continue;
        }

        // Skip enmies from ally and neutral factions.
        EFRelationToPlayers efrtpRelation = penEFH->GetRelationToFaction(penEnemyEFH->m_iFactionIndex);
        if (efrtpRelation == FRT_ALLY || efrtpRelation == FRT_NEUTRAL) {
          continue;
        }

        // Calculate distance between onwer and entity.
        FLOAT fDistance = (pen->GetPlacement().pl_PositionVector-m_penOwner->GetPlacement().pl_PositionVector).Length();

        // If satisfies the distance.
        if (fDistance < fClosestPlayer) {
          if (GetOwner()->SeeEntity(pen, Cos(GetOwner()->m_fViewAngle/2.0f)) || GetOwner()->m_fSenseRange > fDistance) {
            fClosestPlayer = fDistance;
            //if (m_fClosestPlayer>fDistance) { m_fClosestPlayer = fDistance; }
            penClosestPlayer = pen;
            m_penAllocatedEnemy = pen;
          }
        }
    }}

    if (fClosestPlayer < m_fClosestPlayer) { m_fClosestPlayer = fClosestPlayer; }
    return penClosestPlayer;
  }

  // --------------------------------------------------------------------------------------
  // Find Closest CPlayer Entity.
  // --------------------------------------------------------------------------------------
  CEntity* FindClosestPlayer(void)
  {
    CEntity *penClosestPlayer = NULL;
    FLOAT fClosestPlayer = UpperLimit(0.0f);

    CEnemyFactionHolder* penEFH = GetOwner()->GetFactionHolder(TRUE);

    // If invalid index in faction holder we cant' use this faction holder!
    if (penEFH != NULL && !penEFH->IsIndexValid()) {
      penEFH = NULL;
    }

    // Cycle through all players
    for (INDEX iPlayer = 0; iPlayer < GetMaxPlayers(); iPlayer++) {
      CEntity *penPlayer = GetPlayerEntity(iPlayer);
    
      // Skip invalid players.
      if (penPlayer == NULL) {
        continue;
      }
    
      // Skip dead players.
      if (!(penPlayer->GetFlags()&ENF_ALIVE)) {
        continue;
      }
    
      // Skip invisible players.
      if (penPlayer->GetFlags()&ENF_INVISIBLE) {
        continue;
      }

      // If we have faction holder.
      if (penEFH != NULL) {
        if (penEFH->m_efrtRelationToPlayers != FRT_ENEMY) {
          continue;
        }
      }
    
      // calculate distance to player
      FLOAT fDistance =  (penPlayer->GetPlacement().pl_PositionVector - m_penOwner->GetPlacement().pl_PositionVector).Length();
    
      // update if closer
      if (fDistance < fClosestPlayer) {
        fClosestPlayer = fDistance;
        penClosestPlayer = penPlayer;
      }
    }

    // if no players found
    if (penClosestPlayer == NULL) {
      // behave as if very close - must check for new ones
      fClosestPlayer = 10.0f;
    }

    m_fClosestPlayer = fClosestPlayer;
    return penClosestPlayer;
  }

  // --------------------------------------------------------------------------------------
  // notify owner that a player has been seen
  // --------------------------------------------------------------------------------------
  void SendWatchEvent(CEntity *penPlayer)
  {
    EWatch eWatch;
    eWatch.penSeen = penPlayer;
//  if (GetOwner()->m_penFriend == NULL) { GetOwner()->m_penFriend = m_penClosestFriend; }
    m_penOwner->SendEvent(eWatch);
  }

  // --------------------------------------------------------------------------------------
  void CheckIfPlayerVisible(void)
  {
    // if the owner is blind
    if ( GetOwner()->m_bBlind) {
      // don't even bother checking
      return;
    }

    // get maximum number of players in game
    INDEX ctPlayers = GetMaxPlayers();

    // find first one after current sequence
    CEntity *penPlayer = NULL;
    m_iPlayerToCheck = (m_iPlayerToCheck+1)%ctPlayers;
    INDEX iFirstChecked = m_iPlayerToCheck;
    FOREVER {
      if (m_penAllocatedEnemy != NULL ) {
        penPlayer=(CEntity*)&(*m_penAllocatedEnemy);
        break;
      }
      penPlayer = GetPlayerEntity(m_iPlayerToCheck);
      if (penPlayer!=NULL) {
        break;
      }

      m_iPlayerToCheck++;
      m_iPlayerToCheck%=ctPlayers;

      if (m_iPlayerToCheck == iFirstChecked) {
        return; // we get here if there are no players at all
      }
    }

    // if this one is dead or invisible
    if (!(penPlayer->GetFlags()&ENF_ALIVE) || (penPlayer->GetFlags()&ENF_INVISIBLE)) {
      // do nothing
      return;
    }

    // if inside view angle and visible
    if (GetOwner()->m_fViewAngle >= 360.0F || (GetOwner()->SeeEntity(penPlayer, Cos(GetOwner()->m_fViewAngle/2.0f))) || IsDerivedFromClass(penPlayer, "Enemy Base")) {
      // send event to owner
      SendWatchEvent(penPlayer);
    }/* else if (m_penClosestFriend != NULL) { 
      if (GetOwner()->SeeEntity(m_penClosestFriend,Cos(GetOwner()->m_fViewAngle/2.0f))) {
        SendWatchEvent(NULL);
      }
    }*/
  };

  // --------------------------------------------------------------------------------------
  // Set new watch time.
  // --------------------------------------------------------------------------------------
  void SetWatchDelays(void)
  {
    const FLOAT tmMinDelay = 0.1f;   // delay at closest distance
    const FLOAT tmSeeDelay = 5.0f;   // delay at see distance
    const FLOAT tmTick = _pTimer->TickQuantum; // Tick length.

    FLOAT fSeeDistance  = GetOwner()->m_fIgnoreRange;
    FLOAT fNearDistance = Min(GetOwner()->m_fStopDistance, GetOwner()->m_fCloseDistance);

    // if closer than near distance OR after spotting a friend
    if (m_fClosestPlayer <= fNearDistance || (m_penAllocatedEnemy == NULL && m_penClosestFriend != NULL) ) {
      // always use minimum delay
      m_tmDelay = tmMinDelay;
    // if further than near distance
    } else {
      // interpolate between near and see
      m_tmDelay = tmMinDelay + (m_fClosestPlayer-fNearDistance)*(tmSeeDelay-tmMinDelay) / (fSeeDistance - fNearDistance);
      // round to nearest tick
      m_tmDelay = floor(m_tmDelay/tmTick)*tmTick;
    }
  };

  // --------------------------------------------------------------------------------------
  // Do watch.
  // --------------------------------------------------------------------------------------
  void Watch(void)
  {
    // remember original distance
    FLOAT fOrgDistance = m_fClosestPlayer;

    if (m_penAllocatedEnemy != NULL) {
      // If dead or invisible.
      if (!(m_penAllocatedEnemy->GetFlags()&ENF_ALIVE) || (m_penAllocatedEnemy->GetFlags()&ENF_INVISIBLE) /*|| !IsVisible(m_penAllocatedEnemy)*/) 
      {
        m_penAllocatedEnemy = NULL;
      }
    }

    // find closest player
    CEntity *penClosest = NULL;
    penClosest = FindClosestPlayer();
    CEntity *penClosestEB = NULL;

    CEnemyFactionHolder* penEFH = GetOwner()->GetFactionHolder(TRUE);

    // We have faction holder with valid faction index? Then we can EnemyBase as target!
    if (penEFH && penEFH->IsIndexValid()) {
      penClosestEB = FindClosestEB();
    } 

    if (penClosestEB != NULL) {
      penClosest = penClosestEB;
    }

    //GetOwner()->m_penFriend = m_penClosestFriend;

    FLOAT fSeeDistance  = GetOwner()->m_fIgnoreRange;
    FLOAT fStopDistance  = Max(fSeeDistance*1.5f, GetOwner()->m_fActivityRange);

    // if players exited enemy's scope
    if (fOrgDistance < fStopDistance && m_fClosestPlayer >= fStopDistance) {
      // stop owner
      m_penOwner->SendEvent(EStop());
    // if players entered enemy's scope
    } else if (fOrgDistance >= fStopDistance && m_fClosestPlayer < fStopDistance) {
      // start owner
      m_penOwner->SendEvent(EStart());
    }

    // if the closest player is close enough to be seen
    if (penClosest != NULL && m_fClosestPlayer < fSeeDistance/* || m_fClosestFriend < fSeeDistance*/) {
      // check for seeing any of the players
      if (m_penAllocatedEnemy == NULL) { m_penAllocatedEnemy = penClosest; }
      CheckIfPlayerVisible();
    }

    // if the closest player is inside sense range
    FLOAT fSenseRange = GetOwner()->m_fSenseRange;
    if (penClosest != NULL && fSenseRange > 0 && m_fClosestPlayer < fSenseRange) {
      // detect it immediately
      SendWatchEvent(penClosest);
    }

    // set new watch time
    //SetWatchDelays();
  };

  // --------------------------------------------------------------------------------------
  // This is called directly from CEnemyBase to check if another player has come too close.
  // --------------------------------------------------------------------------------------
  CEntity* CheckCloserPlayer(CEntity *penCurrentTarget, FLOAT fRange)
  {
    // if the owner is blind
    if ( GetOwner()->m_bBlind) {
      // don't even bother checking
      return NULL;
    }

    CEntity *penClosestPlayer = NULL;
    FLOAT fClosestPlayer;

    if (penCurrentTarget != NULL) { 
        fClosestPlayer = (penCurrentTarget->GetPlacement().pl_PositionVector-m_penOwner->GetPlacement().pl_PositionVector).Length(); 
    } else { 
        fClosestPlayer = UpperLimit(0.0f);
        fRange = fClosestPlayer - 0.1f;
    }

    fClosestPlayer = Min(fClosestPlayer, fRange);  // this is maximum considered range

    //CEntity *penAllocatedEnemy = FindClosestEB();
    //if (GetOwner()->GetTeam()==0 && penAllocatedEnemy != penCurrentTarget && penAllocatedEnemy != NULL) {
    //  return /*FindClosestEB();*/penAllocatedEnemy;
    //}

    // for all other players
    for (INDEX iPlayer = 0; iPlayer < GetMaxPlayers(); iPlayer++) {
      CEntity *penPlayer = GetPlayerEntity(iPlayer);

      if (penPlayer == NULL || penPlayer == penCurrentTarget) {
        continue;
      }

      // if player is alive and visible
      if ((penPlayer->GetFlags()&ENF_ALIVE) && !(penPlayer->GetFlags()&ENF_INVISIBLE)) {
        // calculate distance to player
        FLOAT fDistance = (penPlayer->GetPlacement().pl_PositionVector-m_penOwner->GetPlacement().pl_PositionVector).Length();
        // if closer than current and you can see him
        if (fDistance < fClosestPlayer && GetOwner()->SeeEntity(penPlayer, Cos(GetOwner()->m_fViewAngle/2.0f))) {
          // update
          fClosestPlayer = fDistance;
          penClosestPlayer = penPlayer;
        }
      }
    }

    return penClosestPlayer;
  }

  // --------------------------------------------------------------------------------------
  // This is called directly from enemybase to attack multiple players (for really big enemies).
  // --------------------------------------------------------------------------------------
  CEntity* CheckAnotherPlayer(CEntity *penCurrentTarget)
  {
    // if the owner is blind, or no current target
    if ( GetOwner()->m_bBlind || penCurrentTarget==NULL) {
      // don't even check
      return NULL;
    }

    // get allowed distance
    CEntity *penClosestPlayer = NULL;
    FLOAT fCurrentDistance;
    FLOAT fRange;
    if (penCurrentTarget != NULL) { 
        fCurrentDistance = (penCurrentTarget->GetPlacement().pl_PositionVector-m_penOwner->GetPlacement().pl_PositionVector).Length();
        fRange = fCurrentDistance*1.5f; 
    } else { 
        fRange = UpperLimit(0.0f);
        fCurrentDistance = fRange/1.5f;
    }

    // find a random offset to start searching
    INDEX iOffset = GetRandomPlayer();

    // for all other players
    INDEX ctPlayers = GetMaxPlayers();
    for (INDEX iPlayer=0; iPlayer<ctPlayers; iPlayer++) {
      CEntity *penPlayer = GetPlayerEntity((iPlayer+iOffset)%ctPlayers);
      if (penPlayer == NULL || penPlayer == penCurrentTarget) {
        continue;
      }

      // if player is alive and visible
      if ((penPlayer->GetFlags()&ENF_ALIVE) && !(penPlayer->GetFlags()&ENF_INVISIBLE)) {
        // calculate distance to player
        FLOAT fDistance = 
          (penPlayer->GetPlacement().pl_PositionVector-m_penOwner->GetPlacement().pl_PositionVector).Length();
        // if inside allowed range and visible
        if (fDistance<fRange && 
            GetOwner()->SeeEntity(penPlayer, Cos(GetOwner()->m_fViewAngle/2.0f))) {
          // attack that one
          return penPlayer;
        }
      }
    }

    return penCurrentTarget;
  }

  // --------------------------------------------------------------------------------------
  // Returns bytes of memory used by this object.
  // --------------------------------------------------------------------------------------
  SLONG GetUsedMemory(void)
  {
    return( sizeof(CWatcher) - sizeof(CRationalEntity) + CRationalEntity::GetUsedMemory());
  }

procedures:

  // watching
  Active() {
    // repeat
    while (TRUE) {
      // check all players
      Watch();
      // wait for given delay
      wait(m_tmDelay) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        // stop looking
        on (EStop) : { jump Inactive(); }
        // force re-checking if receiving start or teleport
        on (EStart) : { stop; }
        on (ETeleport) : { stop; }
      }
    }
  };

  // not watching
  Inactive(EVoid) {
    wait() {
      on (EBegin) : { resume; }
      on (EStart) : { jump Active(); }
    }
  };

  // dummy mode
  Dummy(EVoid)
  {
    // ignores all events forever
    wait() {
      on (EBegin) : { resume; }
      otherwise() : { resume; };
    };
  }

  Main(EWatcherInit eInit) {
    // remember the initial parameters
    ASSERT(eInit.penOwner!=NULL);
    m_penOwner = eInit.penOwner;

    // init as nothing
    InitAsVoid();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // if in flyover game mode
    if (GetSP()->sp_gmGameMode == CSessionProperties::GM_FLYOVER) {
      // go to dummy mode
      jump Dummy();
      // NOTE: must not destroy self, because owner has a pointer
    }

    // generate random number of player to check next 
    // (to provide even distribution of enemies among players)
    m_iPlayerToCheck = GetRandomPlayer()-1;

    // start in disabled state
    autocall Inactive() EEnd;

    // cease to exist
    Destroy();

    return;
  };
};