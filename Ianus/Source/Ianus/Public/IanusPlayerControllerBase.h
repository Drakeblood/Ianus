// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Ianus.h"
#include "GameFramework/PlayerController.h"
#include "IanusInventoryInterface.h"
#include "IanusPlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class IANUS_API AIanusPlayerControllerBase : public APlayerController, public IIanusInventoryInterface
{
	GENERATED_BODY()

public:
	/** Map of all items owned by this player, from definition to data */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<UIanusItemBase*, FIanusItemData> InventoryData;

	/** Map of slot, from type/num to item, initialized from ItemSlotsPerType on IanusGameInstanceBase */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<FIanusItemSlot, UIanusItemBase*> SlottedItems;

	/** Delegate called when an inventory item has been added or removed */
	UPROPERTY(BlueprintAssignable, Category = Inventory)
		FOnInventoryItemChanged OnInventoryItemChanged;

	/** Native version above, called before BP delegate */
	FOnInventoryItemChangedNative OnInventoryItemChangedNative;

	/** Delegate called when an inventory slot has changed */
	UPROPERTY(BlueprintAssignable, Category = Inventory)
		FOnSlottedItemChanged OnSlottedItemChanged;

	/** Native version above, called before BP delegate */
	FOnSlottedItemChangedNative OnSlottedItemChangedNative;

	/** Delegate called when the inventory has been loaded/reloaded */
	UPROPERTY(BlueprintAssignable, Category = Inventory)
		FOnInventoryLoaded OnInventoryLoaded;

	/** Native version above, called before BP delegate */
	FOnInventoryLoadedNative OnInventoryLoadedNative;

public:
	AIanusPlayerControllerBase();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	/** Called after the inventory was changed and we notified all delegates */
	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
		void InventoryItemChanged(bool bAdded, UIanusItemBase* Item);

	/** Called after an item was equipped and we notified all delegates */
	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
		void SlottedItemChanged(FIanusItemSlot ItemSlot, UIanusItemBase* Item);

	/** Adds a new inventory item, will add it to an empty slot if possible. If the item supports count you can add more than one count. It will also update the level when adding if required */
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool AddInventoryItem(UIanusItemBase* NewItem, int32 ItemCount = 1, int32 ItemLevel = 1, bool bAutoSlot = true);

	/** Remove an inventory item, will also remove from slots. A remove count of <= 0 means to remove all copies */
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool RemoveInventoryItem(UIanusItemBase* RemovedItem, int32 RemoveCount = 1);

	/** Returns all inventory items of a given type. If none is passed as type it will return all */
	UFUNCTION(BlueprintCallable, Category = Inventory)
		void GetInventoryItems(TArray<UIanusItemBase*>& Items, FPrimaryAssetType ItemType);

	/** Returns number of instances of this item found in the inventory. This uses count from GetItemData */
	UFUNCTION(BlueprintPure, Category = Inventory)
		int32 GetInventoryItemCount(UIanusItemBase* Item) const;

	/** Returns the item data associated with an item. Returns false if none found */
	UFUNCTION(BlueprintPure, Category = Inventory)
		bool GetInventoryItemData(UIanusItemBase* Item, FIanusItemData& ItemData) const;

	/** Sets slot to item, will remove from other slots if necessary. If passing null this will empty the slot */
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool SetSlottedItem(FIanusItemSlot ItemSlot, UIanusItemBase* Item);

	/** Returns item in slot, or null if empty */
	UFUNCTION(BlueprintPure, Category = Inventory)
		UIanusItemBase* GetSlottedItem(FIanusItemSlot ItemSlot) const;

	/** Returns all slotted items of a given type. If none is passed as type it will return all */
	UFUNCTION(BlueprintCallable, Category = Inventory)
		void GetSlottedItems(TArray<UIanusItemBase*>& Items, FPrimaryAssetType ItemType, bool bOutputEmptyIndexes);

	/** Fills in any empty slots with items in inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
		void FillEmptySlots();

	/** Manually save the inventory, this is called from add/remove functions automatically */
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool SaveInventory();

	/** Loads inventory from save game on game instance, this will replace arrays */
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool LoadInventory();

	// Implement IIanusInventoryInterface
	virtual const TMap<UIanusItemBase*, FIanusItemData>& GetInventoryDataMap() const override
	{
		return InventoryData;
	}
	virtual const TMap<FIanusItemSlot, UIanusItemBase*>& GetSlottedItemMap() const override
	{
		return SlottedItems;
	}
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() override
	{
		return OnInventoryItemChangedNative;
	}
	virtual FOnSlottedItemChangedNative& GetSlottedItemChangedDelegate() override
	{
		return OnSlottedItemChangedNative;
	}
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() override
	{
		return OnInventoryLoadedNative;
	}

protected:
	/** Auto slots a specific item, returns true if anything changed */
	bool FillEmptySlotWithItem(UIanusItemBase* NewItem);

	/** Calls the inventory update callbacks */
	void NotifyInventoryItemChanged(bool bAdded, UIanusItemBase* Item);
	void NotifySlottedItemChanged(FIanusItemSlot ItemSlot, UIanusItemBase* Item);
	void NotifyInventoryLoaded();

	/** Called when a global save game as been loaded */
	void HandleSaveGameLoaded(UIanusSaveGame* NewSaveGame);
};
