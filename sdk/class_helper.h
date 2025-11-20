#pragma once
#include "sdk/sdk_extra.h"

UClass* turret_class()
{
    static UClass* ptr = nullptr;
    if (!ptr)
        ptr = UObject::find_class("Class ShooterGame.PrimalStructureTurret");
    return ptr;
}


UClass* dropped_item_class()
{
    static UClass* ptr = nullptr;
    if (!ptr)
        ptr = UObject::find_class("BlueprintGeneratedClass DroppedItemGeneric.DroppedItemGeneric_C");
    return ptr;
}

UClass* item_cache_class()
{
    static UClass* ptr = nullptr;
    if (!ptr)
        ptr = UObject::find_class("BlueprintGeneratedClass DeathItemCache.DeathItemCache_C");
    return ptr;
}

UClass* supply_crate_class()
{
    static UClass* ptr = nullptr;
    if (!ptr)
        ptr = UObject::find_class("Class ShooterGame.PrimalStructureItemContainer_SupplyCrate");
    return ptr;
}

UClass* sleeping_bag_class()
{
    static UClass* ptr = nullptr;
    if (!ptr)
        ptr = UObject::find_class("BlueprintGeneratedClass PrimalItemStructure_SleepingBag_Base.PrimalItemStructure_SleepingBag_Base_C");
    return ptr;
}

UClass* bed_class()
{
    static UClass* ptr = nullptr;
    if (!ptr)
        ptr = UObject::find_class("Class ShooterGame.PrimalStructureBed");
    return ptr;
}

UClass* placed_c4_class()
{
    static UClass* ptr = nullptr;
    if (!ptr)
        ptr = UObject::find_class("BlueprintGeneratedClass C4Charge.C4Charge_C");
    return ptr;
}

