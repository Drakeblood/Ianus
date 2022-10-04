// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Ianus.h"
#include "Engine/GameInstance.h"
#include "IanusGameInstanceBase.generated.h"

class UIanusItemBase;
class UIanusSaveGame;

/** Delegate called when the save game has been loaded/reset */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameLoaded, UIanusSaveGame*, SaveGame);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSaveGameLoadedNative, UIanusSaveGame*);

/**
 * 
 */
UCLASS()
class IANUS_API UIanusGameInstanceBase : public UGameInstance
{
	GENERATED_BODY()

public:

	/** List of inventory items to add to new players */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<FPrimaryAssetId, FIanusItemData> DefaultInventory;

	/** Number of slots for each type of item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<FPrimaryAssetType, int32> ItemSlotsPerType;

	/** The slot name used for saving */
	UPROPERTY(BlueprintReadWrite, Category = Save)
		FString SaveSlot;

	/** The platform-specific user index */
	UPROPERTY(BlueprintReadWrite, Category = Save)
		int32 SaveUserIndex;

	/** Delegate called when the save game has been loaded/reset */
	UPROPERTY(BlueprintAssignable, Category = Inventory)
		FOnSaveGameLoaded OnSaveGameLoaded;

	/** Native delegate for save game load/reset */
	FOnSaveGameLoadedNative OnSaveGameLoadedNative;

protected:
	/** The current save game object */
	UPROPERTY()
		UIanusSaveGame* CurrentSaveGame;

	/** Rather it will attempt to actually save to disk */
	UPROPERTY()
		bool bSavingEnabled;

	/** True if we are in the middle of doing a save */
	UPROPERTY()
		bool bCurrentlySaving;

	/** True if another save was requested during a save */
	UPROPERTY()
		bool bPendingSaveRequested;

public:
	UIanusGameInstanceBase();

	/**
 * Adds the default inventory to the inventory array
 * @param InventoryArray Inventory to modify
 * @param RemoveExtra If true, remove anything other than default inventory
 */
	UFUNCTION(BlueprintCallable, Category = Inventory)
		void AddDefaultInventory(UIanusSaveGame* SaveGame, bool bRemoveExtra = false);

	/** Returns true if this is a valid inventory slot */
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool IsValidItemSlot(FIanusItemSlot ItemSlot) const;

	/** Returns the current save game, so it can be used to initialize state. Changes are not written until WriteSaveGame is called */
	UFUNCTION(BlueprintCallable, Category = Save)
		UIanusSaveGame* GetCurrentSaveGame();

	/** Sets rather save/load is enabled. If disabled it will always count as a new character */
	UFUNCTION(BlueprintCallable, Category = Save)
		void SetSavingEnabled(bool bEnabled);

	/** Synchronously loads a save game. If it fails, it will create a new one for you. Returns true if it loaded, false if it created one */
	UFUNCTION(BlueprintCallable, Category = Save)
		bool LoadOrCreateSaveGame();

	/** Handle the final setup required after loading a USaveGame object using AsyncLoadGameFromSlot. Returns true if it loaded, false if it created one */
	UFUNCTION(BlueprintCallable, Category = Save)
		bool HandleSaveGameLoaded(USaveGame* SaveGameObject);

	/** Gets the save game slot and user index used for inventory saving, ready to pass to GameplayStatics save functions */
	UFUNCTION(BlueprintCallable, Category = Save)
		void GetSaveSlotInfo(FString& SlotName, int32& UserIndex) const;

	/** Writes the current save game object to disk. The save to disk happens in a background thread*/
	UFUNCTION(BlueprintCallable, Category = Save)
		bool WriteSaveGame();

	/** Resets the current save game to it's default. This will erase player data! This won't save to disk until the next WriteSaveGame */
	UFUNCTION(BlueprintCallable, Category = Save)
		void ResetSaveGame();

protected:
	/** Called when the async save happens */
	virtual void HandleAsyncSave(const FString& SlotName, const int32 UserIndex, bool bSuccess);

};
