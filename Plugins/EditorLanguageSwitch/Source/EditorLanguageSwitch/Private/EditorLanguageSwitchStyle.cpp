// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorLanguageSwitchStyle.h"
#include "EditorLanguageSwitch.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FEditorLanguageSwitchStyle::StyleInstance = nullptr;

void FEditorLanguageSwitchStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FEditorLanguageSwitchStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FEditorLanguageSwitchStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("EditorLanguageSwitchStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FEditorLanguageSwitchStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("EditorLanguageSwitchStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("EditorLanguageSwitch")->GetBaseDir() / TEXT("Resources"));

	Style->Set("EditorLanguageSwitch.PluginAction", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));

	Style->Set("EditorLanguageSwitch.ToolbarIcon", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));

	
	return Style;
}

void FEditorLanguageSwitchStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FEditorLanguageSwitchStyle::Get()
{
	return *StyleInstance;
}
