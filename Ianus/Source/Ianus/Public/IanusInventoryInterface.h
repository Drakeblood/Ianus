// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Ianus.h"
#include "UObject/Interface.h"
#include "Items/IanusItemBase.h"
#include "IanusInventoryInterface.generated.h"

/** Delegate called when an inventory item changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChanged, bool, bAdded, UIanusItemBase*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChangedNative, bool, UIanusItemBase*);

/** Delegate called when the entire inventory has been loaded, all items may have been replaced */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryLoaded);
DECLARE_MULTICAST_DELEGATE(FOnInventoryLoadedNative);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UIanusInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IANUS_API IIanusInventoryInterface
{
	GENERATED_BODY()

public:
	/** Returns the map of items to data */
	virtual const TMap<UIanusItemBase*, FIanusItemData>& GetInventoryDataMap() const = 0;

	/** Returns the map of slots to items */
	virtual const TMap<FIanusItemSlot, UIanusItemBase*>& GetSlottedItemMap() const = 0;

	/** Gets the delegate for inventory item changes */
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() = 0;

	/** Gets the delegate for inventory slot changes */
	virtual FOnSlottedItemChangedNative& GetSlottedItemChangedDelegate() = 0;

	/** Gets the delegate for when the inventory loads */
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() = 0;
};
