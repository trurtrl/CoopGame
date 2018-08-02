// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class USHealthComponent;

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


	FVector GetNextPathPoint();


	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	float RequiredDistanceToTraget;

	UPROPERTY(EditDefaultsOnly, Category = "Trackerbot")
	bool bUseVelocityChange;


	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
