#pragma once
#include "Core/HAL/PlatformType.h"

enum ETextureSlot : int {
    Diffuse = 0,
    Ambient = 1,
    Specular = 2,
    Bump = 3,
    Alpha = 4,
};

namespace MaterialTextureFlags {
    constexpr uint8 Diffuse  = 1 << static_cast<int>(ETextureSlot::Diffuse); 
    constexpr uint8 Ambient  = 1 << static_cast<int>(ETextureSlot::Ambient); 
    constexpr uint8 Specular = 1 << static_cast<int>(ETextureSlot::Specular);
    constexpr uint8 Bump     = 1 << static_cast<int>(ETextureSlot::Bump);    
    constexpr uint8 Alpha    = 1 << static_cast<int>(ETextureSlot::Alpha);   
}
