// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/IanusAbilitySystemComponent.h"

#include "IanusCharacterBase.h"
#include "Abilities/IanusGameplayAbility.h"
#include "AbilitySystemGlobals.h"

UIanusAbilitySystemComponent::UIanusAbilitySystemComponent()
{
    
}

void UIanusAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<UIanusGameplayAbility*>& ActiveAbilities)
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActivate, false);

	// Iterate the list of all ability specs
	for (FGameplayAbilitySpec* Spec : AbilitiesToActivate)
	{
		// Iterate all instances on this ability spec
		TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances();

		for (UGameplayAbility* ActiveAbility : AbilityInstances)
		{
			ActiveAbilities.Add(Cast<UIanusGameplayAbility>(ActiveAbility));
		}
	}
}

int32 UIanusAbilitySystemComponent::GetDefaultAbilityLevel() const
{
	AIanusCharacterBase* OwningCharacter = Cast<AIanusCharacterBase>(GetOwnerActor());

	if (OwningCharacter)
	{
		return OwningCharacter->GetCharacterLevel();
	}
	return 1;
}

UIanusAbilitySystemComponent* UIanusAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent)
{
	return Cast<UIanusAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}
