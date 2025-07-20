// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define CUSTOM_DEPTH_RED 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252


//对投掷物响应通道
#define ECC_PROJECTILE ECollisionChannel::ECC_GameTraceChannel1
//技能对攻击目标拾取的通道，只包含场景中的角色
#define ECC_TARGET ECollisionChannel::ECC_GameTraceChannel2
//技能范围选择时的通道，忽略场景中可移动的角色
#define ECC_EXCLUDEPLAYERS  ECollisionChannel::ECC_GameTraceChannel3