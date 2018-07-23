// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SHealthComponent.h"
#include "SWeapon.h"
#include "CoopGame.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	//	without it crouching won't work
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	//	fixes issue with showing effects when bullet hitting in CapsuleComponent extended by our own "weapon" part 
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	ZoomedFOV = 30.f;
	ZoomInterpSpeed = 20;

	WeaponAttachSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;

	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	if (Role == ROLE_Authority)
	{
		//	Spawn a default weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
	}

}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	//	AddControllerPitchInput and YawInput exist in APawn
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::CrouchBegin);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::CrouchEnd);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::ZoomBegin);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::ZoomEnd);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::FireStart);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::FireStop);

	//	CHALLENGE  code
	//	ACharacter already has function Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::CrouchBegin()
{
	Crouch();
}

void ASCharacter::CrouchEnd()
{
	UnCrouch();
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void ASCharacter::ZoomBegin()
{
	bWantsToZoom = true;
}

void ASCharacter::ZoomEnd()
{
	bWantsToZoom = false;
}

void ASCharacter::FireStart()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->FireStart();
	}
}

void ASCharacter::FireStop()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->FireStop();
	}
}

void ASCharacter::OnHealthChanged(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.f && !bDied)
	{
		//	Die!
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//	Detach the Pawn from Controller
		DetachFromControllerPendingDestroy();

		//	After 10 sec the Pawn will be destroyed
		SetLifeSpan(10.f);
	}
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
}
