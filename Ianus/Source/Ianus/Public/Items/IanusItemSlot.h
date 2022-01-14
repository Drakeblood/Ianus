// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/PrimaryAssetId.h"
#include "IanusItemSlot.generated.h"

class UIanusItemBase;
class UIanusSaveGame;

/** Struct representing a slot for an item, shown in the UI */
USTRUCT(BlueprintType)
struct IANUS_API FIanusItemSlot
{
	GENERATED_BODY()

	/** The type of items that can go in this slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FPrimaryAssetType ItemType;

	/** The number of this slot, 0 indexed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 SlotNumber;

	FIanusItemSlot()
	: SlotNumber(-1)
	{}

	FIanusItemSlot(const FPrimaryAssetType& InItemType, int32 InSlotNumber)
		: ItemType(InItemType)
		, SlotNumber(InSlotNumber)
	{}

	/** Equality operators */
	bool operator==(const FIanusItemSlot& Other) const
	{
		return ItemType == Other.ItemType && SlotNumber == Other.SlotNumber;
	}
	bool operator!=(const FIanusItemSlot& Other) const
	{
		return !(*this == Other);
	}

	/** Implemented so it can be used in Maps/Sets */
	friend inline uint32 GetTypeHash(const FIanusItemSlot& Key)
	{
		uint32 Hash = 0;

		Hash = HashCombine(Hash, GetTypeHash(Key.ItemType));
		Hash = HashCombine(Hash, (uint32)Key.SlotNumber);
		return Hash;
	}

	/** Returns true if slot is valid */
	bool IsValid() const
	{
		return ItemType.IsValid() && SlotNumber >= 0;
	}
};

/** Delegate called when the contents of an inventory slot change */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlottedItemChanged, FIanusItemSlot, ItemSlot, UIanusItemBase*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSlottedItemChangedNative, FIanusItemSlot, UIanusItemBase*);