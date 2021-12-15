// Fill out your copyright notice in the Description page of Project Settings.


#include "IanusSaveGame.h"
#include "IanusGameInstanceBase.h"

void UIanusSaveGame::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if (Ar.IsLoading() && SavedDataVersion != EIanusSaveGameVersion::LatestVersion)
	{
		if (SavedDataVersion < EIanusSaveGameVersion::AddedItemData)
		{
			// Convert from list to item data map
			for (const FPrimaryAssetId& ItemId : InventoryItems_DEPRECATED)
			{
				InventoryData.Add(ItemId, FIanusItemData(1, 1));
			}

			InventoryItems_DEPRECATED.Empty();
		}

		SavedDataVersion = EIanusSaveGameVersion::LatestVersion;
	}
}
