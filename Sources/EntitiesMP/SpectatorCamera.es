2003
%{
  #include "StdH.h"
%}

class CSpectatorCamera : CMovableModelEntity {
name "Spectator Camera";
thumbnail "";
features "HasName", "IsTargetable", "CanBePredictable";

properties:
  1 CTString m_strName "Name" 'N' = "Spectator Camera",
  // 3 CTString m_strDescription = "",
  3 BOOL m_bActive = FALSE,
  4 CEntityPointer m_penOwner,
  
  5 BOOL m_bPlayerControl = FALSE,
  
  15 BOOL m_bButtonLeft = FALSE,
  16 BOOL m_bButtonRight = FALSE,
  17 BOOL m_bButtonUp = FALSE,
  18 BOOL m_bButtonDown = FALSE,
  
  19 BOOL m_bButtonMUp = FALSE,
  20 BOOL m_bButtonMDown = FALSE,
  21 BOOL m_bButtonFire = FALSE,
  
  30 ANGLE3D m_a3dRotationAngle = ANGLE3D(0, 0, 0),

components:
  1 model MODEL_MARKER "Models\\Editor\\Axis.mdl",
  2 texture TEXTURE_MARKER "Models\\Editor\\Vector.tex",

functions:
  // --------------------------------------------------------------------------------------
  // Render particles.
  // --------------------------------------------------------------------------------------
  void RenderParticles(void)
  {
    if (Particle_GetViewer()==this) {
      Particles_ViewerLocal(this);
    }
  }

  // --------------------------------------------------------------------------------------
  // Toggles what camera is active.
  // --------------------------------------------------------------------------------------
  void Toggle()
  {
    m_bActive = !m_bActive;
    
    if (m_bActive) {
      m_a3dRotationAngle = GetPlacement().pl_OrientationAngle;
    }
  }
  
  // --------------------------------------------------------------------------------------
  // Toggles player control.
  // --------------------------------------------------------------------------------------
  void TogglePlayerControl()
  {
    m_bPlayerControl = !m_bPlayerControl;
  }

  // --------------------------------------------------------------------------------------
  // Called before PreMoving() so we don't need to use PreMoving()
  // --------------------------------------------------------------------------------------
  void Rotation(ANGLE3D a3dRotation)
  {
    if (!m_bActive) {
      return;
    }

    a3dRotation *= _pTimer->TickQuantum;

    m_a3dRotationAngle = GetPlacement().pl_OrientationAngle;
    m_a3dRotationAngle(1) += a3dRotation(1);
    m_a3dRotationAngle(2) = Clamp(m_a3dRotationAngle(2) + a3dRotation(2), -90.0F, 90.0F);

    en_plLastPlacement = en_plPlacement; // Store last placement.
  }

  // --------------------------------------------------------------------------------------
  // Called every time after processing movement.
  // --------------------------------------------------------------------------------------
  void PostMoving()
  {
    if (m_penOwner == NULL) {
      return;
    }
    
    FLOAT fSpeed = 1;
    
    FLOAT3D vSpeed = FLOAT3D(0, 0, 0);
    
    if (m_bButtonFire) {
      fSpeed *= 3;
    }
    
    if (m_bButtonLeft) {
      vSpeed(1) = -fSpeed;
    } else if (m_bButtonRight) {
      vSpeed(1) = fSpeed;
    } else {
      vSpeed(1) = 0;
    }
    
    // up and down
    if (m_bButtonMUp) {
      vSpeed(2) = fSpeed * 0.5f;
    } else if (m_bButtonMDown) {
      vSpeed(2) = -fSpeed * 0.5f;
    } else {
      vSpeed(2) = 0;
    }
    
    if (m_bButtonUp) {
      vSpeed(3) = -fSpeed;
    } else if (m_bButtonDown) {
      vSpeed(3) = fSpeed;
    } else {
      vSpeed(3) = 0;
    }
    
    SetPlacement(CPlacement3D(GetPlacement().pl_PositionVector + vSpeed * GetRotationMatrix(), m_a3dRotationAngle));
  }

  // --------------------------------------------------------------------------------------
  // Reduces lagging.
  // --------------------------------------------------------------------------------------
  void AddDependentsToPrediction(void)
  {
    m_penOwner->AddToPrediction();
  }

  // --------------------------------------------------------------------------------------
  // Forces full stop and resets all buttons.
  // --------------------------------------------------------------------------------------
  void StopCamera(void)
  {
    // stop all buttons
    m_bButtonLeft = FALSE;
    m_bButtonRight = FALSE;
    m_bButtonUp = FALSE;
    m_bButtonDown = FALSE;
    
    m_bButtonMUp = FALSE;
    m_bButtonMDown = FALSE;
    m_bButtonFire = FALSE;
    // full stop
    SetDesiredRotation(ANGLE3D(0, 0, 0));
    SetDesiredTranslation(ANGLE3D(0, 0, 0));
    ForceFullStop();
  }

procedures:
  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main(EVoid)
  {
    if (m_penOwner == NULL) {
      Destroy();
    }
    
    m_a3dRotationAngle = GetPlacement().pl_OrientationAngle;

    SetPredictable(TRUE);
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MOVABLE);
    SetCollisionFlags(ECF_CAMERA);
    AddToMovers();
    
    // Set appearance.
    SetModel(MODEL_MARKER);
    SetModelMainTexture(TEXTURE_MARKER);
    
    autowait(_pTimer->TickQuantum);
    
    while (TRUE)
    {
      if (m_penOwner == NULL) {
        return;
      }

      autowait(_pTimer->TickQuantum);
    }
    
    Destroy();
    
    return;
  }
};
