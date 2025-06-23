// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "EditorLanguageSwitchStyle.h"

class FEditorLanguageSwitchCommands : public TCommands<FEditorLanguageSwitchCommands>
{
public:

	FEditorLanguageSwitchCommands()
		: TCommands<FEditorLanguageSwitchCommands>(
			TEXT("EditorLanguageSwitch"),
			NSLOCTEXT("Contexts",
				"EditorLanguageSwitch",
				"EditorLanguageSwitch Plugin"), 
				NAME_None,
				FEditorLanguageSwitchStyle::GetStyleSetName()
				)
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
