// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorLanguageSwitchCommands.h"

#define LOCTEXT_NAMESPACE "FEditorLanguageSwitchModule"

void FEditorLanguageSwitchCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "EditorLanguageSwitch", "Execute EditorLanguageSwitch action", EUserInterfaceActionType::Button, FInputChord(EKeys::L));
}

#undef LOCTEXT_NAMESPACE
