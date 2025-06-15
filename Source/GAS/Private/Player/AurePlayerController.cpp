// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AurePlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "Interaction/HighlightInterface.h"

AAurePlayerController::AAurePlayerController()
{
	//是否将数据传送服务器
	bReplicates  = true;
}


void AAurePlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(InputMappingContext);


	// 获取当前玩家的Enhanced Input子系统
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
	    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	// 检查子系统是否存在，确保后续操作的安全性
	if(Subsystem)
	{
		// 向子系统添加输入映射上下文，参数0表示优先级
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
	
	// 设置鼠标光标显示和默认样式
	bShowMouseCursor  = true;
	DefaultMouseCursor  = EMouseCursor::Default;
	
	// 初始化游戏和UI的输入模式，并配置鼠标行为
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	// 应用配置的输入模式
	SetInputMode(InputMode);
	
}

void AAurePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 将输入组件转换为增强输入组件，以支持更复杂和可定制的输入处理
	UEnhancedInputComponent* EnhancedInputComponent =
		CastChecked<UEnhancedInputComponent>(InputComponent);
	
	// 绑定移动操作到增强输入组件，当移动操作被触发时，调用AAurePlayerController的Move方法
	EnhancedInputComponent->BindAction(
		MoveAction, ETriggerEvent::Triggered, this, &AAurePlayerController::Move);
}

void AAurePlayerController::Move(const FInputActionValue& InputActionValue)
{
	// 获取输入动作值并转换为二维向量
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	
	// 获取当前的控制旋转
	const FRotator Rotation = GetControlRotation();
	
	// 仅保留旋转的偏航分量，用于计算前进和右侧方向
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	// 计算基于偏航旋转的前进方向
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	// 计算基于偏航旋转的右侧方向
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	// 获取当前控制的Pawn
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
	
	    // 根据输入轴向量的Y轴分量和计算的前进方向，应用向前/后的移动输入
	    ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
	
	    // 根据输入轴向量的X轴分量和计算的右侧方向，应用向左/右的移动输入
	    ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAurePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	//鼠标追踪
	CursorTrace();
}

void AAurePlayerController::CursorTrace()
{
	 // 获取光标下的命中结果
	 GetHitResultUnderCursor( ECC_Visibility, false, CursorHit);
	
	// 如果光标命中结果没有阻挡命中，则返回
	if (!CursorHit.bBlockingHit) return;
	
	// 将最后一个演员设置为当前演员
	LastActor = ThisActor;
	// 如果命中结果对应的演员有效且实现了UHighlightInterface接口，则将其设置为当前演员
	if (IsValid(CursorHit.GetActor()) && CursorHit.GetActor()->Implements<UHighlightInterface>())
	{
		ThisActor = CursorHit.GetActor();
	}
	else
	{
		// 否则，将当前演员设置为nullptr
		ThisActor = nullptr;
	}
	
	// 如果最后一个演员和当前演员不同，则执行以下操作
	if (LastActor != ThisActor)
	{
		// 取消高亮最后一个演员
		UnHighlightActor(LastActor);
		// 高亮当前演员
		HighlightActor(ThisActor);
	}
}

void AAurePlayerController::HighlightActor(AActor* InActor)
{
	if (IsValid(InActor) && InActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_HighlightActor(InActor);
	}
}

void AAurePlayerController::UnHighlightActor(AActor* InActor)
{
	if (IsValid(InActor) && InActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_UnHighlightActor(InActor);
	}
}

 UAureAbilitySystemComponent* AAurePlayerController::GetASC()
 {
 	if (AureAbilitySystemComponent == nullptr)
 	{
 		AureAbilitySystemComponent = Cast<UAureAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
 	}
 	return AureAbilitySystemComponent;
}
