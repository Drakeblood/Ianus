// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/IanusItemBase.h"
#include "IanusWeaponItem.generated.h"

/**
 * 
 */
UCLASS()
class IANUS_API UIanusWeaponItem : public UIanusItemBase
{
	GENERATED_BODY()

public:
	/** Weapon actor to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
		TSubclassOf<AActor> WeaponActor;

public:
	UIanusWeaponItem();
	
};
