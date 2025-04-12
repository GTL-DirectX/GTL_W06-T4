#pragma once
#include "HAL/PlatformType.h"

class FString;

enum EViewModeIndex : uint8
{
    Lit_Gouraud,
    Lit_Lambert,
    Lit_Phong,
    Unlit,
    Wireframe,
    SceneDepth,
    Max
};

FString GetViewModeIndexAsString(enum EViewModeIndex InViewMode);

enum ELevelViewportType : uint8
{
    LVT_Perspective = 0,

    /** Top */
    LVT_OrthoXY = 1,
    /** Bottom */
    LVT_OrthoNegativeXY,
    /** Left */
    LVT_OrthoYZ,
    /** Right */
    LVT_OrthoNegativeYZ,
    /** Front */
    LVT_OrthoXZ,
    /** Back */
    LVT_OrthoNegativeXZ,

    LVT_MAX,
    LVT_None = 255,
};
