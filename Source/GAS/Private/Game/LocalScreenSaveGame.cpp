// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LocalScreenSaveGame.h"

FSavedMap ULocalScreenSaveGame::GetSavedMapWithMapName(const FString& InMapName)
{
	for(const FSavedMap& Map : SavedMaps)
	{
		if(Map.MapAssetName == InMapName)
		{
			return Map;
		}
	}
	return FSavedMap();
}


bool ULocalScreenSaveGame::HasMap(const FString& InMapName)
{
	for(const FSavedMap& Map : SavedMaps)
	{
		if(Map.MapAssetName == InMapName)
		{
			return true;
		}
	}
	return false;
}
