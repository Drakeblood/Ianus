// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Ianus.h"
#include "Engine/AssetManager.h"
#include "IanusAssetManager.generated.h"

class UIanusItemBase;

/**
 * 
 */
UCLASS()
class IANUS_API UIanusAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	/** Static types for items */
	static const FPrimaryAssetType	PotionItemType;
	static const FPrimaryAssetType	SkillItemType;
	static const FPrimaryAssetType	WeaponItemType;

public:
	UIanusAssetManager();

	virtual void StartInitialLoading() override;

	/** Returns the current AssetManager object */
	static UIanusAssetManager& Get();

	/**
	 * Synchronously loads an RPGItem subclass, this can hitch but is useful when you cannot wait for an async load
	 * This does not maintain a reference to the item so it will garbage collect if not loaded some other way
	 *
	 * @param PrimaryAssetId The asset identifier to load
	 * @param bDisplayWarning If true, this will log a warning if the item failed to load
	 */
	UIanusItemBase* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);
	
};
