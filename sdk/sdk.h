#include "../includes.h"

#include "ue4_extra.h"
#include "ue4/UE4.h"

/* class forward declarations */
class UFont;
class AActor;
class ULocalPlayer;
class APlayerState;
class APlayerController;
class AController;
class UPlayer;
class APawn;
class USceneComponent;
class APlayerCameraManager;
class APrimalCharacter;
class AShooterWeapon;
class APrimalDinoCharacter;

class APlayerCameraManager
{
public:
	PAD(0x470);
	USceneComponent* TransformComponent;
	PAD(0x8);
	float DefaultFOV; // 0x0480(0x0004)
	PAD(0x4C);

	FRotator get_camera_rotation()
	{
		static auto fn = UObject::find_object<UFunction>("Function Engine.PlayerCameraManager.GetCameraRotation");
		struct {
			FRotator ReturnValue;
		} Params;
		process_event(this, fn, &Params);
		return Params.ReturnValue;
	}
	FVector get_camera_location()
	{
		static auto fn = UObject::find_object<UFunction>("Function Engine.PlayerCameraManager.GetCameraLocation");
		struct {
			FVector ReturnValue;
		} Params;
		process_event(this, fn, &Params);
		return Params.ReturnValue;
	}
};

class USkeletalMeshComponent
{
	static UClass* static_class()
	{
		static auto ptr = UObject::find_class("Class Engine.SkeletalMeshComponent");
		return ptr;
	}
	FName get_bone_name(int BoneIndex)
	{
		static UFunction* fn = nullptr;
		if (!fn)
			fn = UObject::find_object<UFunction>("Function Engine.SkinnedMeshComponent.GetBoneName");
		struct {
			int BoneIndex;
			FName ReturnValue;
		} Params;
		Params.BoneIndex = BoneIndex;
		process_event(this, fn, &Params);
		return Params.ReturnValue;
	}
};

class AActor : public UObject {
public:
	FVector k2_get_actor_location()
	{
		static auto fn = UObject::find_object<UFunction>("Function Engine.Actor.K2_GetActorLocation");
		struct
		{
			FVector ReturnVector;
		} Params;
		process_event(this, fn, &Params);
		return Params.ReturnVector;
	}
	float GetDistanceTo(AActor* toTarget)
	{
		static auto fn = UObject::find_object<UFunction>("Function Engine.Actor.GetDistanceTo");
		struct
		{
			AActor* ActorTo;
			float ReturnVector;
		} Params;
		Params.ActorTo = toTarget;
		process_event(this, fn, &Params);
		return Params.ReturnVector;
	}
	inline bool is_dino()
	{
		static UClass* obj;
		if (!obj)
			obj = UObject::find_class("Class ShooterGame.PrimalDinoCharacter");
		return instance_of(obj);
	}
	inline bool is_player()
	{
		static UClass* obj;
		if (!obj)
			obj = UObject::find_class("Class ShooterGame.ShooterCharacter");
		return instance_of(obj);
	}
	bool is_tamed()
	{
		static auto fn = UObject::find_object<UFunction>("Function ShooterGame.PrimalDinoCharacter.BPIsTamed");
		struct {
			bool ReturnValue;
		} Params;
		process_event(this, fn, &Params);
		return Params.ReturnValue;
	}
	bool is_local_player()
	{
		static auto fn = UObject::find_object<UFunction>("Function ShooterGame.PrimalCharacter.IsOwningClient");
		struct {
			bool ReturnValue;
		} Params;
		process_event(this, fn, &Params);
		return Params.ReturnValue;
	}

	bool Is_Broken()
	{
		static auto fn = UObject::find_object<UFunction>("Function ShooterGame.PrimalItem.IsBroken");

		struct {
			bool                                               ReturnValue;
		}Params;

		//auto flags = fn->FunctionFlags;
		//fn->FunctionFlags |= 0x00000400;

		process_event(this, fn, &Params);
		//fn->FunctionFlags = flags;


		return Params.ReturnValue;
	}
	bool is_conscious()
	{
		static auto fn = UObject::find_object<UFunction>("Function ShooterGame.PrimalCharacter.BPIsConscious");
		struct {
			bool ReturnValue;
		} Params;
		process_event(this, fn, &Params);
		return Params.ReturnValue;
	}
	bool is_primal_char_friendly(APrimalCharacter* primalChar)
	{
		static auto fn = UObject::find_object<UFunction>("Function ShooterGame.PrimalCharacter.IsPrimalCharFriendly");

		struct {
			class APrimalCharacter* primalChar;
			bool ReturnValue;
		}params;
		params.primalChar = primalChar;

		process_event(this, fn, &params);
		return params.ReturnValue;
	}
	bool is_dead()
	{
		static auto fn = UObject::find_object<UFunction>("Function Engine.Actor.IsDead");
		struct {
			bool ReturnValue;
		} Params;
		process_event(this, fn, &Params);
		return Params.ReturnValue;
	}
	bool is_structure()
	{
		static auto fn = UObject::find_object<UFunction>("Function Engine.Actor.IsPrimalStructure");

		struct {
			bool                           ReturnValue;
		}params;

		process_event(this, fn, &params);

		return params.ReturnValue;
	}
	bool is_item_container()
	{
		static auto fn = UObject::find_object<UFunction>("Function Engine.Actor.IsPrimalStructureItemContainer");
		struct {
			bool ReturnValue;
		} Params;
		process_event(this, fn, &Params);
		return Params.ReturnValue;
	}
	struct APrimalDinoCharacter* get_based_or_seating_on_dino()
	{
		static UFunction* fn = nullptr;
		if (!fn)
			fn = UObject::find_object<UFunction>("Function ShooterGame.PrimalCharacter.GetBasedOrSeatingOnDino");

		struct {
			struct APrimalDinoCharacter* ReturnValue;
		} Params;

		process_event(this, fn, &Params);
		return Params.ReturnValue;
	}
};

class APawn : public AActor {
public:
	PAD(0x18);
	UClass* AIControllerClass;                                         // 0x0488
	class APlayerState* PlayerState;                                               // 0x0490
	static UClass* static_class()
	{
		static auto ptr = UObject::find_class("Class Engine.Pawn");
		return ptr;
	}
};

class APlayerController
{
public:
	PAD(0x4D0);
	APawn* AcknowledgedPawn;
	PAD(0x18);
	class APlayerCameraManager* PlayerCameraManager;
	PAD(0x534);
	float	MaxUseDistance;
};

class UPlayer {
public:
	PAD(0x30);
	APlayerController* PlayerController;											// 0x0030
};

class ULocalPlayer : public UPlayer {

};

class ULevelBase
{
public:
	PAD(0x88);
	TTransArray<AActor*> Actors;				// 0x0088
};

class ULevel : public ULevelBase
{

};

class UGameInstance
{
public:
	PAD(0x38);
	TArray<ULocalPlayer*>                        LocalPlayers;						// 0x0038
};

class UWorld : public UObject
{
public:
	static inline UWorld* GWorld = nullptr;
	PAD(0xD0);
	ULevel* PersistentLevel;          // 0x00F8
	PAD(0x28);
	class AGameState* GameState;				  // 0x0128
	PAD(0x160);
	UGameInstance* OwningGameInstance;       // 0x0290
	PAD(0x554);
	int                                                NumPlayerConnected;
};

struct AShooterWeapon_Rapid
{

	PAD(0x1DC);
	long double                                              LastFireTime;                                              // 0x0AB0(0x0008)
	PAD(0x38);
	long double                                           LastNotifyShotTime;                                          // 0x0AF0(0x0008)
};

struct AGameState
{
	PAD(0x4c0);
	TArray<APlayerState*> PlayerStates;
	PAD(0x554);
	int                                                NumPlayerConnected;
};

UFont* get_font()
{
	static UFont* font;
	if(!font)
		font = UObject::find_object<UFont>("Font OpenSansRegular12.OpenSansRegular12");

	return font;
}

class UCanvas : public UObject
{
public:
	void k2_draw_text(UFont* RenderFont, const FString& RenderText, const FVector2D& ScreenPosition, const FLinearColor& RenderColor, float Kerning, const FLinearColor& ShadowColor, const FVector2D& ShadowOffset, bool bCentreX, bool bCentreY, bool bOutlined, const FLinearColor& OutlineColor)
	{
		static UFunction* fn = nullptr;
		if (!fn)
			fn = UObject::find_object<UFunction>("Function Engine.Canvas.K2_DrawText");

		struct
		{
			UFont* RenderFont;
			FString RenderText;
			FVector2D ScreenPosition;
			FLinearColor RenderColor;
			float Kerning;
			FLinearColor ShadowColor;
			FVector2D ShadowOffset;
			bool bCentreX;
			bool bCentreY;
			bool bOutlined;
			FLinearColor OutlineColor;
		} params;

		params.RenderFont = RenderFont;
		params.RenderText = RenderText;
		params.ScreenPosition = ScreenPosition;
		params.RenderColor = RenderColor;
		params.Kerning = Kerning;
		params.ShadowColor = ShadowColor;
		params.ShadowOffset = ShadowOffset;
		params.bCentreX = bCentreX;
		params.bCentreY = bCentreY;
		params.bOutlined = bOutlined;
		params.OutlineColor = OutlineColor;

		process_event(this, fn, &params);
	}
	void k2_draw_line(const FVector2D& ScreenPositionA, const FVector2D& ScreenPositionB, float Thickness, const FLinearColor& RenderColor)
	{
		static UFunction* fn = nullptr;
		if (!fn)
			fn = UObject::find_object<UFunction>("Function Engine.Canvas.K2_DrawLine");

		struct
		{
			FVector2D ScreenPositionA;
			FVector2D ScreenPositionB;
			float Thickness;
			FLinearColor RenderColor;
		} params;

		params.ScreenPositionA = ScreenPositionA;
		params.ScreenPositionB = ScreenPositionB;
		params.Thickness = Thickness;
		params.RenderColor = RenderColor;

		process_event(this, fn, &params);
	}
};

class AShooterCharacter
{
public:

	static UClass* static_class()
	{
		static auto ptr = UObject::find_class("Class ShooterGame.ShooterCharacter");
		return ptr;
	}

	AShooterWeapon* CurrentWeapon;                                             // 0x1708(0x0008)

	AShooterWeapon* get_weapon()
	{
		static auto fn = UObject::find_object<UFunction>("Function ShooterGame.ShooterCharacter.GetWeapon");
		struct {
			AShooterWeapon* ReturnValue;
		} Params;
		process_event(this, fn, &Params);
		return Params.ReturnValue;
	}
	APrimalDinoCharacter* get_riding_dino()
	{
		static UFunction* fn = nullptr;
		if (!fn)
			fn = UObject::find_object<UFunction>("Function ShooterGame.ShooterCharacter.GetRidingDino");

		struct AShooterCharacter_GetRidingDino_Params {
			class APrimalDinoCharacter* ReturnValue;
		} params{};

		process_event(this, fn, &params);

		return params.ReturnValue;
	}
};