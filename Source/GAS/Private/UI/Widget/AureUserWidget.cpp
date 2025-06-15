// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AureUserWidget.h"

void UAureUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
