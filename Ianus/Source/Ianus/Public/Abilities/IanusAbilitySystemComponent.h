// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "IanusAbilitySystemComponent.generated.h"

class UIanusGameplayAbility;

/**
 * 
 */
UCLASS()
class IANUS_API UIanusAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UIanusAbilitySystemComponent();

	/** Returns a list of currently active ability instances that match the tags */
	void GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<UIanusGameplayAbility*>& ActiveAbilities);

	/** Returns the default level used for ability activations, derived from the character */
	int32 GetDefaultAbilityLevel() const;

	/** Version of function in AbilitySystemGlobals that returns correct type */
	static UIanusAbilitySystemComponent* GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent = false);
	
};
