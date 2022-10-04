// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/IanusItemBase.h"

UIanusItemBase::UIanusItemBase()
	: MaxCount(1)
	, AbilityLevel(1)
{

}

bool UIanusItemBase::IsConsumable() const
{
	if (MaxCount <= 0)
	{
		return true;
	}
	return false;
}

FString UIanusItemBase::GetIdentifierString() const
{
	return GetPrimaryAssetId().ToString();
}

FPrimaryAssetId UIanusItemBase::GetPrimaryAssetId() const
{
	// This is a DataAsset and not a blueprint so we can just use the raw FName
	// For blueprints you need to handle stripping the _C suffix
	return FPrimaryAssetId(ItemType, GetFName());
}