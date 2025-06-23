// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorLanguageSwitch.h"
#include "EditorLanguageSwitchStyle.h"
#include "EditorLanguageSwitchCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Internationalization/Culture.h"
#include "Widgets/Notifications/SNotificationList.h"

static const FName EditorLanguageSwitchTabName("EditorLanguageSwitch");

#define LOCTEXT_NAMESPACE "FEditorLanguageSwitchModule"

void FEditorLanguageSwitchModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FEditorLanguageSwitchStyle::Initialize();
	FEditorLanguageSwitchStyle::ReloadTextures();

	FEditorLanguageSwitchCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FEditorLanguageSwitchCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FEditorLanguageSwitchModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FEditorLanguageSwitchModule::RegisterMenus));
}

void FEditorLanguageSwitchModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FEditorLanguageSwitchStyle::Shutdown();

	FEditorLanguageSwitchCommands::Unregister();
}

void FEditorLanguageSwitchModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	//获取当前编辑器语言
	FString CurrentLanguage = FInternationalization::Get().GetCurrentLanguage()->GetName();
	//切换语言
	FString NewLanguage = (CurrentLanguage == "en")?"zh-Hans" : "en";
	//动态修改编辑器语言
	if (FInternationalization::Get().SetCurrentLanguage(NewLanguage))
	{
		//刷新编辑器界面
		GConfig->SetString(TEXT("Internationalization"), TEXT("Culture"), *NewLanguage, GEditorSettingsIni);
		GConfig->Flush(false, GEditorSettingsIni);

		GEditor->RedrawAllViewports(true);
		// 提示用户语言已切换
		FNotificationInfo Info(FText::Format(
			LOCTEXT("LanguageSwitched", "Editor language switched to {0}."),
			FText::FromString(NewLanguage)
		));
		Info.ExpireDuration = 5.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}
	else
	{
		// 提示语言切换失败
		FNotificationInfo Info(LOCTEXT("LanguageSwitchFailed", "Failed to switch editor language."));
		Info.ExpireDuration = 5.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}
}

void FEditorLanguageSwitchModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	// {
	// 	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
	// 	{
	// 		FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
	// 		Section.AddMenuEntryWithCommandList(FEditorLanguageSwitchCommands::Get().PluginAction, PluginCommands);
	// 	}
	// }

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(
					FToolMenuEntry::InitToolBarButton(
						FEditorLanguageSwitchCommands::Get().PluginAction,
						FText::FromString(""),
						FText::FromString(TEXT("点击按钮，切换编辑器的显示语言")),
						FSlateIcon(FEditorLanguageSwitchStyle::GetStyleSetName(), "EditorLanguageSwitch.ToolbarIcon")
						));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("AssetEditor.DefaultToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(
					FToolMenuEntry::InitToolBarButton(
						FEditorLanguageSwitchCommands::Get().PluginAction,
						FText::FromString(""),
						FText::FromString(TEXT("点击按钮，切换编辑器的显示语言")),
						FSlateIcon(FEditorLanguageSwitchStyle::GetStyleSetName(), "EditorLanguageSwitch.ToolbarIcon")
						));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEditorLanguageSwitchModule, EditorLanguageSwitch)