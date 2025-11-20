#pragma once
#include "sdk/class_helper.h"
#include <codecvt>
#include <wincrypt.h>
#include <iostream>
#include <string>
#include <WinUser.h>
#include <wingdi.h>
#include <ddeml.h>
#include <winsvc.h>
#include <ddeml.h>
#include <winnt.h>
#include <Windows.h>
//#include "ZeroGUI.h"
//#include "ZeroInput.h"
void aim_at(AActor* aim_target, APlayerController* player_controller, UPlayer* uplayer);


__forceinline float calc_distance(FVector2D from, FVector2D to)
{
	return from.DistTo(to);
}


wchar_t* s2wc(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

__forceinline void draw_text(std::string text, FVector2D location, FLinearColor color, UCanvas* canvas)
{
	/* simple wrapper so we don't need to type as much, as we usually don't need most
	of the options we have available in k2drawtext */

	std::wstring wide_string = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(text);
	return canvas->k2_draw_text(get_font(), FString(wide_string.c_str()), location, color, 1, FLinearColor(0, 0, 0, 1), FVector2D(0, 0), true, true, true, FLinearColor(0, 0, 0, 1));
}


void game_tick(settings_manager settings, UCanvas* canvas, uintptr_t uworld_addr)
{
	draw_text("Priority BTW", FVector2D(114, 20), purple_col, canvas);

	draw_text("Priority Version: LOL", FVector2D(97, 40), purple_col, canvas);



			/* This block of checks ensures two things
			//*
			//* a. that we are in a game, we have a gworld & a level to use
			//*	 ( aka , we aren't for example loading into a game or in a menu )
			//*
			//* b. that our local player controller exists and is valid. we shouldn't attempt to do much of anything if we
			//*	 don't have our local player controller or an associated pawn.
			//*    ( aka , the player obj that we control)
			//*
			//* It's important this is done sequentially, as any one failed check
			//* causes the next to result in a segfault. */

	UWorld* gworld = *reinterpret_cast<decltype(UWorld::GWorld)*>(uworld_addr);
	if (!gworld
		|| !gworld->GameState
		|| !gworld->OwningGameInstance
		|| !gworld->OwningGameInstance->LocalPlayers[0]
		|| !gworld->OwningGameInstance->LocalPlayers[0]->PlayerController
		|| !gworld->OwningGameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn)

		return;

	ULevel* persistent_level = gworld->PersistentLevel;
	TTransArray<AActor*> actors_array = persistent_level->Actors;
	UGameInstance* game_instance = gworld->OwningGameInstance;
	UPlayer* local_uplayer = game_instance->LocalPlayers[0];
	APlayerController* local_controller = game_instance->LocalPlayers[0]->PlayerController;
	AActor* local_player = game_instance->LocalPlayers[0]->PlayerController->AcknowledgedPawn;



	RECT window_rect = { 0 };
	GetClientRect(FindWindowA("UnrealWindow", "ARK: Survival Evolved"), &window_rect);

	/* If we're not in a game world don't bother checking anything else, actors count is fine for this... */
	if (actors_array.Count < 50)
		return;

	FVector2D screen_center = { (float)(window_rect.right - window_rect.left), (float)(window_rect.bottom - window_rect.top) };
	bool      wants_to_aim = true;

	float calcDistance = true;
	float DistTo = true;

	int       target_dist = 8000;
	AActor* aim_target = 0;





	for (int i = 0; i < actors_array.Count; i++)
	{
		AActor* actor = actors_array[i];

		/* sanity check */
		if (!actor)
			continue;

		FLinearColor actor_color;
		FVector2D	 actor_screen_location;
		FVector		 actor_world_location = actor->k2_get_actor_location();

		/* If we fail to get this actor's world location or screen location, there's no point doing anything... */
		if (!actor_world_location.X || !actor_world_location.Y || !actor_world_location.Z)
			continue;
		if (!w2s(actor_world_location, actor_screen_location, local_controller, window_rect))
			continue;

		if (GetAsyncKeyState(VK_XBUTTON2))
		{
			wants_to_aim = true;
			float distToActor = local_player->GetDistanceTo(actor);
			float abs_dist = calc_distance(actor_screen_location, screen_center);
			if (abs_dist < target_dist)
			{
				target_dist = abs_dist;
				aim_target = actor;
			}
		}


		if (actor->is_player())
		{
			/* we don't need to draw ourself...*/
			if (actor->is_local_player())
				continue;

			if (actor->is_primal_char_friendly((APrimalCharacter*)local_player))
				actor_color = green_col;


			if (!actor->is_conscious())
				actor_color = purple_col;

			if (actor->is_dead())
				actor_color = dead_col;

			FString   player_platform = *(FString*)((uintptr_t)actor + 0x790);			/* AShooterCharacter::PlatFormprofileName 0x1578; */
			FString   player_tribe_name = *(FString*)((uintptr_t)actor + 0x790);
			FString   player_name = *(FString*)((uintptr_t)actor + 0x14b0);			/* AShooterCharacter::PlayerName */
			float	  player_weight = *(float*)((uintptr_t)actor + 0x1bcc);				/* AShooterCharacter::ReplicatedWeight */
			float	  player_cur_health = *(float*)((uintptr_t)actor + 0x92c);				/* APrimalCharacter::ReplicatedCurrentHealth */
			float	  player_max_health = *(float*)((uintptr_t)actor + 0x930);				/* APrimalCharacter::ReplicatedMaxHealth */
			float	  orbitcam = *(float*)((uintptr_t)actor + 0xe00) = 8000.0f;				/* APrimalCharacter::OrbitCamMaxZoom*/


			float	    player_cur_torpor = *(float*)((uintptr_t)actor + 0x934);
			float	    player_max_torpor = *(float*)((uintptr_t)actor + 0x938);

			uintptr_t status_component = *(uintptr_t*)((uintptr_t)actor + 0xcd0);			/* APrimalCharacter::MyCharacterStatusComponent */
			int32_t   tmp_base_lvl = *(int32_t*)((uintptr_t)status_component + 0x6cc); /* UPrimalCharacterStatusComponent::BaseCharacterLevel */
			int32_t   tmp_extra_lvl = *(int32_t*)((uintptr_t)status_component + 0x6d0); /* UPrimalCharacterStatusComponent::ExtraCharacterLevel */
			int32_t   player_level = tmp_base_lvl + tmp_extra_lvl;
			float	  EquipTime = *(float*)((uintptr_t)actor + 468);
			draw_text("LVL: " + std::to_string(player_level) + " " + player_name.ToString(), actor_screen_location, white_col, canvas);
			draw_text("HP: " + std::to_string((int)player_cur_health) + "/" + std::to_string((int)player_max_health), FVector2D(actor_screen_location.X, actor_screen_location.Y + 15), white_col, canvas);
			draw_text("Torpor: " + std::to_string((int)player_cur_torpor) + "/" + std::to_string((int)player_max_torpor), FVector2D(actor_screen_location.X, actor_screen_location.Y + 45), white_col, canvas);
			draw_text("Weight: " + std::to_string((int)player_weight), FVector2D(actor_screen_location.X, actor_screen_location.Y + 30), white_col, canvas);
			draw_text("" + player_tribe_name.ToString(), FVector2D(actor_screen_location.X, actor_screen_location.Y + 60), white_col, canvas);
			continue;
		}

		if (actor->is_dino())
		{
			if (actor->is_tamed())
			{
				if (actor->is_primal_char_friendly((APrimalCharacter*)local_player))
					actor_color = green_col; /* tribe dino */
				else
					actor_color = yellow_col;  /* enemy dino */
			}
			else
				actor_color = cyan_col;    /* wild dino */

			if (actor->is_dead())
				actor_color = dead_col;

			std::string dino_name = (*(FName*)((uintptr_t)actor + 0x1978)).get_name(); /* APrimalDinoCharacter::DinoNameTag */
			float	    dino_cur_health = *(float*)((uintptr_t)actor + 0x92c);				 /* APrimalCharacter::ReplicatedCurrentHealth */
			float	    dino_max_health = *(float*)((uintptr_t)actor + 0x930);				 /* APrimalCharacter::ReplicatedMaxHealth */
			float	    dino_cur_torpor = *(float*)((uintptr_t)actor + 0x934);
			float	    dino_max_torpor = *(float*)((uintptr_t)actor + 0x938);


			FString   tamer_name = *(FString*)((uintptr_t)actor + 0x1270);


			uintptr_t   status_component = *(uintptr_t*)((uintptr_t)actor + 0xcd0);			 /* APrimalCharacter::MyCharacterStatusComponent */
			int32_t     tmp_base_lvl = *(int32_t*)((uintptr_t)status_component + 0x6cc); /* UPrimalCharacterStatusComponent::BaseCharacterLevel */
			int32_t     tmp_extra_lvl = *(int32_t*)((uintptr_t)status_component + 0x6d0); /* UPrimalCharacterStatusComponent::ExtraCharacterLevel */

			int32_t     dino_level = tmp_base_lvl + tmp_extra_lvl;

			draw_text("LVL: " + std::to_string(dino_level) + " " + dino_name, actor_screen_location, actor_color, canvas);
			draw_text("HP: " + std::to_string((int)dino_cur_health) + "/" + std::to_string((int)dino_max_health), FVector2D(actor_screen_location.X, actor_screen_location.Y + 15), actor_color, canvas);
			draw_text("Torpor: " + std::to_string((int)dino_cur_torpor) + "/" + std::to_string((int)dino_max_torpor), FVector2D(actor_screen_location.X, actor_screen_location.Y + 30), actor_color, canvas);


			continue;
		}

		if (actor->instance_of(turret_class()))
		{
			FString turret_name = *(FString*)((uintptr_t)actor + 0x4e8); /* APrimalTargetableActor::DescriptiveName */
			int32_t ammo_count = *(int32_t*)((uintptr_t)actor + 0xeb0); /* APrimalStructureTurret::NumBullets */
			int32_t bullets_per_shot = *(int32_t*)((uintptr_t)actor + 0xeb4); /* APrimalStructureTurret::NumBulletsPerShot */

			if (!turret_name.IsValid())
				continue;

			draw_text(turret_name.ToString() + " [" + std::to_string(ammo_count) + "]", actor_screen_location, red_col, canvas);
		}



		if (actor->instance_of(supply_crate_class()))
		{
			int32_t supply_items_min = *(int32_t*)((uintptr_t)actor + 0xc04); // APrimalStructureItemContainer	CurrentItemCount
			int32_t supply_items_max = *(int32_t*)((uintptr_t)actor + 0xc08); // APrimalStructureItemContainer	MaxItemCount //
			draw_text("Supply Crate", actor_screen_location, blue_col, canvas);
			draw_text("" + std::to_string((int)supply_items_min) + "/" + std::to_string((int)supply_items_max), FVector2D(actor_screen_location.X, actor_screen_location.Y + 15), blue_col, canvas);
			continue;
		}

		if (actor->instance_of(placed_c4_class()))
		{
			draw_text("C4 Charge", actor_screen_location, red_col, canvas);
			continue;
		}

		if (actor->instance_of(bed_class()))
		{
			draw_text("Bed", actor_screen_location, white_col, canvas);
			continue;
		}

		if (actor->instance_of(sleeping_bag_class()))
		{
			draw_text("Sleeping Bag", actor_screen_location, white_col, canvas);
			continue;
		}

		if (actor->instance_of(item_cache_class()))
		{
			int32_t supply_items_min = *(int32_t*)((uintptr_t)actor + 0xc04); // APrimalStructureItemContainer	CurrentItemCount
			int32_t supply_items_max = *(int32_t*)((uintptr_t)actor + 0xc08); // APrimalStructureItemContainer	MaxItemCount //
			draw_text("Item Cache", actor_screen_location, white_col, canvas);
			draw_text("" + std::to_string((int)supply_items_min) + "/" + std::to_string((int)supply_items_max), FVector2D(actor_screen_location.X, actor_screen_location.Y + 15), white_col, canvas);
			continue;
		}

		if (actor->instance_of(dropped_item_class()))
		{
			draw_text("Dropped Item", actor_screen_location, black_col, canvas);
			continue;
		}



	}




	APrimalDinoCharacter* riding_dino = local_player->get_based_or_seating_on_dino();
	if (riding_dino)
	{
		/* instant turn */
		*(float*)((uintptr_t)riding_dino + 0x1698) = 8000.0f; /* APrimalDinoCharacter::RiderRotationRateModifier */
		*(float*)((uintptr_t)riding_dino + 0x1ed4) = 8000; /* APrimalDinoCharacter::WalkingRotationRateModifier */
		*(float*)((uintptr_t)riding_dino + 0x169c) = 8000; /* APrimalDinoCharacter::SwimmingRotationRateModifier */
		*(float*)((uintptr_t)riding_dino + 0x14ec) = 800.0f; /* APrimalDinoCharacter::FlyingForceRotationRateModifier */
	}


	AShooterWeapon* current_weapon = ((AShooterCharacter*)local_player)->get_weapon();
	if (current_weapon)
	{

		/* no recoil, no sway, no shake, no spread */
		*(float*)((uintptr_t)current_weapon + 0xb94) = 0; /* AShooterWeapon::GlobalFireCameraShakeScale */
		*(float*)((uintptr_t)current_weapon + 0xbac) = 0; /* AShooterWeapon::ReloadCameraShakeSpeedScale */
		*(float*)((uintptr_t)current_weapon + 0xba4) = 0; /* AShooterWeapon::GlobalFireCameraShakeScaleTargeting */

		*(float*)((uintptr_t)current_weapon + 0xb70) = 0; /* AShooterWeapon::AimDriftPitchFrequency */
		*(float*)((uintptr_t)current_weapon + 0xb6c) = 0; /* AShooterWeapon::AimDriftYawFrequency */

		*(float*)((uintptr_t)current_weapon + 0xcf8) = 0; /* AShooterWeapon::CurrentFiringSpread */
	}

	if (wants_to_aim)
		if (aim_target)
			aim_at(aim_target, local_controller, local_uplayer);

}

//
void aim_at(AActor* aim_target, APlayerController* player_controller, UPlayer* uplayer)
{
	FVector  camera_location = player_controller->PlayerCameraManager->get_camera_location();
	FVector  aim_location = aim_target->k2_get_actor_location();
	//FRotator aim_rotation = uplayer->FindLookAtRotation(camera_location, aim_location);
	//player_controller->SetControlRotation(aim_rotation);
}


