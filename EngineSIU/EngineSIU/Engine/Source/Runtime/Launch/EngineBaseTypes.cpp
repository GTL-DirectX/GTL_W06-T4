#include "EngineBaseTypes.h"

#include "Container/String.h"

FString GetViewModeIndexAsString(EViewModeIndex InViewMode)
{
    switch (InViewMode)
    {
    case EViewModeIndex::Lit_Gouraud:
        return FString("Lit_Gouraud");
    case EViewModeIndex::Lit_Lambert:
        return FString("Lit_Lambert");
    case EViewModeIndex::Lit_Phong:
        return FString("Lit_Phong");
    case EViewModeIndex::Unlit:
        return FString("Unlit");
    case EViewModeIndex::Wireframe:
        return FString("Wireframe");
    case EViewModeIndex::SceneDepth:
        return FString("SceneDepth");
    default:
        return FString("Unknown");
    }
}
