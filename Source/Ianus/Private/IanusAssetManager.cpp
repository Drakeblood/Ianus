// Fill out your copyright notice in the Description page of Project Settings.


#include "IanusAssetManager.h"
#include "Items/IanusItemBase.h"
#include "AbilitySystemGlobals.h"

const FPrimaryAssetType	UIanusAssetManager::PotionItemType = TEXT("Potion");
const FPrimaryAssetType	UIanusAssetManager::SkillItemType = TEXT("Skill");
const FPrimaryAssetType	UIanusAssetManager::WeaponItemType = TEXT("Weapon");

UIanusAssetManager::UIanusAssetManager()
{

}

UIanusAssetManager& UIanusAssetManager::Get()
{
	UIanusAssetManager* This = Cast<UIanusAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogIanus, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be IanusAssetManager!"));
		return *NewObject<UIanusAssetManager>(); // never calls this
	}
}

void UIanusAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}


UIanusItemBase* UIanusAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UIanusItemBase* LoadedItem = Cast<UIanusItemBase>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogIanus, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}
