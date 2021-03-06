// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class USHealthComponent;
class USphereComponent;
class USoundCue;

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//	Next point in navigation path
	FVector NextPathPoint;

	//	Dynamic material to pulse on damage
	UMaterialInstanceDynamic* MaterialInstance;

	bool bExploded;

	FTimerHandle TimerHandle_SelfDamage;

	bool bStartedSelfDestruction;


	void DamageSelf();

	void SelfDestruct();

	FVector GetNextPathPoint();


	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	float RequiredDistanceToTraget;

	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	float SelfDamageInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	USoundCue* SelfDestructSound;

	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	USoundCue* ExplodeSound;


	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:

	// CHALLENGE CODE	

	// Find nearby enemies and grow in 'power level' based on the amount.
	void OnCheckNearbyBots();

	// the power boost of the bot, affects damaged caused to enemies and color of the bot (range: 1 to 4)
	int32 PowerLevel;

	FTimerHandle TimerHandle_RefreshPath;

	void RefreshPath();
	
};
