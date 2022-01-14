// Fill out your copyright notice in the Description page of Project Settings.


#include "IanusPlayerControllerBase.h"
#include "IanusCharacterBase.h"
#include "IanusGameInstanceBase.h"
#include "IanusSaveGame.h"
#include "Items/IanusItemBase.h"

AIanusPlayerControllerBase::AIanusPlayerControllerBase()
{
	//DisableInput(this);
}

void AIanusPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	LoadInventory();
}

void AIanusPlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

}

bool AIanusPlayerControllerBase::AddInventoryItem(UIanusItemBase* NewItem, int32 ItemCount, int32 ItemLevel, bool bAutoSlot)
{
	bool bChanged = false;
	if (!NewItem)
	{
		UE_LOG(LogIanus, Warning, TEXT("AddInventoryItem: Failed trying to add null item!"));
		return false;
	}

	if (ItemCount <= 0 || ItemLevel <= 0)
	{
		UE_LOG(LogIanus, Warning, TEXT("AddInventoryItem: Failed trying to add item %s with negative count or level!"), *NewItem->GetName());
		return false;
	}

	// Find current item data, which may be empty
	FIanusItemData OldData;
	GetInventoryItemData(NewItem, OldData);

	// Find modified data
	FIanusItemData NewData = OldData;
	NewData.UpdateItemData(FIanusItemData(ItemCount, ItemLevel), NewItem->MaxCount, NewItem->MaxLevel);

	if (OldData != NewData)
	{
		// If data changed, need to update storage and call callback
		InventoryData.Add(NewItem, NewData);
		NotifyInventoryItemChanged(true, NewItem);
		bChanged = true;
	}

	if (bAutoSlot)
	{
		// Slot item if required
		bChanged |= FillEmptySlotWithItem(NewItem);
	}

	if (bChanged)
	{
		// If anything changed, write to save game
		SaveInventory();
		return true;
	}
	return false;
}

bool AIanusPlayerControllerBase::RemoveInventoryItem(UIanusItemBase* RemovedItem, int32 RemoveCount)
{
	if (!RemovedItem)
	{
		UE_LOG(LogIanus, Warning, TEXT("RemoveInventoryItem: Failed trying to remove null item!"));
		return false;
	}

	// Find current item data, which may be empty
	FIanusItemData NewData;
	GetInventoryItemData(RemovedItem, NewData);

	if (!NewData.IsValid())
	{
		// Wasn't found
		return false;
	}

	// If RemoveCount <= 0, delete all
	if (RemoveCount <= 0)
	{
		NewData.ItemCount = 0;
	}
	else
	{
		NewData.ItemCount -= RemoveCount;
	}

	if (NewData.ItemCount > 0)
	{
		// Update data with new count
		InventoryData.Add(RemovedItem, NewData);
	}
	else
	{
		// Remove item entirely, make sure it is unslotted
		InventoryData.Remove(RemovedItem);

		for (TPair<FIanusItemSlot, UIanusItemBase*>& Pair : SlottedItems)
		{
			if (Pair.Value == RemovedItem)
			{
				Pair.Value = nullptr;
				NotifySlottedItemChanged(Pair.Key, Pair.Value);
			}
		}
	}

	// If we got this far, there is a change so notify and save
	NotifyInventoryItemChanged(false, RemovedItem);

	SaveInventory();
	return true;
}

void AIanusPlayerControllerBase::GetInventoryItems(TArray<UIanusItemBase*>& Items, FPrimaryAssetType ItemType)
{
	for (const TPair<UIanusItemBase*, FIanusItemData>& Pair : InventoryData)
	{
		if (Pair.Key)
		{
			FPrimaryAssetId AssetId = Pair.Key->GetPrimaryAssetId();

			// Filters based on item type
			if (AssetId.PrimaryAssetType == ItemType || !ItemType.IsValid())
			{
				Items.Add(Pair.Key);
			}
		}
	}
}

bool AIanusPlayerControllerBase::SetSlottedItem(FIanusItemSlot ItemSlot, UIanusItemBase* Item)
{
	// Iterate entire inventory because we need to remove from old slot
	bool bFound = false;
	for (TPair<FIanusItemSlot, UIanusItemBase*>& Pair : SlottedItems)
	{
		if (Pair.Key == ItemSlot)
		{
			// Add to new slot
			bFound = true;
			Pair.Value = Item;
			NotifySlottedItemChanged(Pair.Key, Pair.Value);
		}
		else if (Item != nullptr && Pair.Value == Item)
		{
			// If this item was found in another slot, remove it
			Pair.Value = nullptr;
			NotifySlottedItemChanged(Pair.Key, Pair.Value);
		}
	}

	if (bFound)
	{
		SaveInventory();
		return true;
	}

	return false;
}

int32 AIanusPlayerControllerBase::GetInventoryItemCount(UIanusItemBase* Item) const
{
	const FIanusItemData* FoundItem = InventoryData.Find(Item);

	if (FoundItem)
	{
		return FoundItem->ItemCount;
	}
	return 0;
}

bool AIanusPlayerControllerBase::GetInventoryItemData(UIanusItemBase* Item, FIanusItemData& ItemData) const
{
	const FIanusItemData* FoundItem = InventoryData.Find(Item);

	if (FoundItem)
	{
		ItemData = *FoundItem;
		return true;
	}
	ItemData = FIanusItemData(0, 0);
	return false;
}

UIanusItemBase* AIanusPlayerControllerBase::GetSlottedItem(FIanusItemSlot ItemSlot) const
{
	UIanusItemBase* const* FoundItem = SlottedItems.Find(ItemSlot);

	if (FoundItem)
	{
		return *FoundItem;
	}
	return nullptr;
}

void AIanusPlayerControllerBase::GetSlottedItems(TArray<UIanusItemBase*>& Items, FPrimaryAssetType ItemType, bool bOutputEmptyIndexes)
{
	for (TPair<FIanusItemSlot, UIanusItemBase*>& Pair : SlottedItems)
	{
		if (Pair.Key.ItemType == ItemType || !ItemType.IsValid())
		{
			Items.Add(Pair.Value);
		}
	}
}

void AIanusPlayerControllerBase::FillEmptySlots()
{
	bool bShouldSave = false;
	for (const TPair<UIanusItemBase*, FIanusItemData>& Pair : InventoryData)
	{
		bShouldSave |= FillEmptySlotWithItem(Pair.Key);
	}

	if (bShouldSave)
	{
		SaveInventory();
	}
}

bool AIanusPlayerControllerBase::SaveInventory()
{
	UWorld* World = GetWorld();
	UIanusGameInstanceBase* GameInstance = World ? World->GetGameInstance<UIanusGameInstanceBase>() : nullptr;

	if (!GameInstance)
	{
		return false;
	}

	UIanusSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	if (CurrentSaveGame)
	{
		// Reset cached data in save game before writing to it
		CurrentSaveGame->InventoryData.Reset();
		CurrentSaveGame->SlottedItems.Reset();

		for (const TPair<UIanusItemBase*, FIanusItemData>& ItemPair : InventoryData)
		{
			FPrimaryAssetId AssetId;

			if (ItemPair.Key)
			{
				AssetId = ItemPair.Key->GetPrimaryAssetId();
				CurrentSaveGame->InventoryData.Add(AssetId, ItemPair.Value);
			}
		}

		for (const TPair<FIanusItemSlot, UIanusItemBase*>& SlotPair : SlottedItems)
		{
			FPrimaryAssetId AssetId;

			if (SlotPair.Value)
			{
				AssetId = SlotPair.Value->GetPrimaryAssetId();
			}
			CurrentSaveGame->SlottedItems.Add(SlotPair.Key, AssetId);
		}

		// Now that cache is updated, write to disk
		GameInstance->WriteSaveGame();
		return true;
	}
	return false;
}

bool AIanusPlayerControllerBase::LoadInventory()
{
	InventoryData.Reset();
	SlottedItems.Reset();

	// Fill in slots from game instance
	UWorld* World = GetWorld();
	UIanusGameInstanceBase* GameInstance = World ? World->GetGameInstance<UIanusGameInstanceBase>() : nullptr;

	if (!GameInstance)
	{
		return false;
	}

	// Bind to loaded callback if not already bound
	if (!GameInstance->OnSaveGameLoadedNative.IsBoundToObject(this))
	{
		GameInstance->OnSaveGameLoadedNative.AddUObject(this, &AIanusPlayerControllerBase::HandleSaveGameLoaded);
	}

	for (const TPair<FPrimaryAssetType, int32>& Pair : GameInstance->ItemSlotsPerType)
	{
		for (int32 SlotNumber = 0; SlotNumber < Pair.Value; SlotNumber++)
		{
			SlottedItems.Add(FIanusItemSlot(Pair.Key, SlotNumber), nullptr);
		}
	}

	UIanusSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	UIanusAssetManager& AssetManager = UIanusAssetManager::Get();
	if (CurrentSaveGame)
	{
		// Copy from save game into controller data
		bool bFoundAnySlots = false;
		for (const TPair<FPrimaryAssetId, FIanusItemData>& ItemPair : CurrentSaveGame->InventoryData)
		{
			UIanusItemBase* LoadedItem = AssetManager.ForceLoadItem(ItemPair.Key);

			if (LoadedItem != nullptr)
			{
				InventoryData.Add(LoadedItem, ItemPair.Value);
			}
		}

		for (const TPair<FIanusItemSlot, FPrimaryAssetId>& SlotPair : CurrentSaveGame->SlottedItems)
		{
			if (SlotPair.Value.IsValid())
			{
				UIanusItemBase* LoadedItem = AssetManager.ForceLoadItem(SlotPair.Value);
				if (GameInstance->IsValidItemSlot(SlotPair.Key) && LoadedItem)
				{
					SlottedItems.Add(SlotPair.Key, LoadedItem);
					bFoundAnySlots = true;
				}
			}
		}

		if (!bFoundAnySlots)
		{
			// Auto slot items as no slots were saved
			FillEmptySlots();
		}

		NotifyInventoryLoaded();

		return true;
	}

	// Load failed but we reset inventory, so need to notify UI
	NotifyInventoryLoaded();

	return false;
}

bool AIanusPlayerControllerBase::FillEmptySlotWithItem(UIanusItemBase* NewItem)
{
	// Look for an empty item slot to fill with this item
	FPrimaryAssetType NewItemType = NewItem->GetPrimaryAssetId().PrimaryAssetType;
	FIanusItemSlot EmptySlot;
	for (TPair<FIanusItemSlot, UIanusItemBase*>& Pair : SlottedItems)
	{
		if (Pair.Key.ItemType == NewItemType)
		{
			if (Pair.Value == NewItem)
			{
				// Item is already slotted
				return false;
			}
			else if (Pair.Value == nullptr && (!EmptySlot.IsValid() || EmptySlot.SlotNumber > Pair.Key.SlotNumber))
			{
				// We found an empty slot worth filling
				EmptySlot = Pair.Key;
			}
		}
	}

	if (EmptySlot.IsValid())
	{
		SlottedItems[EmptySlot] = NewItem;
		NotifySlottedItemChanged(EmptySlot, NewItem);
		return true;
	}

	return false;
}

void AIanusPlayerControllerBase::NotifyInventoryItemChanged(bool bAdded, UIanusItemBase* Item)
{
	// Notify native before blueprint
	OnInventoryItemChangedNative.Broadcast(bAdded, Item);
	OnInventoryItemChanged.Broadcast(bAdded, Item);

	// Call BP update event
	InventoryItemChanged(bAdded, Item);
}

void AIanusPlayerControllerBase::NotifySlottedItemChanged(FIanusItemSlot ItemSlot, UIanusItemBase* Item)
{
	// Notify native before blueprint
	OnSlottedItemChangedNative.Broadcast(ItemSlot, Item);
	OnSlottedItemChanged.Broadcast(ItemSlot, Item);

	// Call BP update event
	SlottedItemChanged(ItemSlot, Item);
}

void AIanusPlayerControllerBase::NotifyInventoryLoaded()
{
	// Notify native before blueprint
	OnInventoryLoadedNative.Broadcast();
	OnInventoryLoaded.Broadcast();
}

void AIanusPlayerControllerBase::HandleSaveGameLoaded(UIanusSaveGame* NewSaveGame)
{
	LoadInventory();
}