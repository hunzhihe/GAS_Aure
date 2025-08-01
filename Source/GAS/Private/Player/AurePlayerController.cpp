// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AurePlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AureGameplayTags.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "Components/DecalComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GAS/GAS.h"
#include "Input/AureEnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "Interaction/HighlightInterface.h"
#include "UI/Widget/DamageTextComponent.h"

class UNavigationPath;

AAurePlayerController::AAurePlayerController()
{
	//是否将数据传送服务器
	bReplicates  = true;
    //样条线初始化
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
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

	// 将输入组件转换为自定义增强输入组件，以支持更复杂和可定制的输入处理
	UAureEnhancedInputComponent* EnhancedInputComponent =
		CastChecked<UAureEnhancedInputComponent>(InputComponent);
	
	// 绑定移动操作到增强输入组件，当移动操作被触发时，调用AAurePlayerController的Move方法
	EnhancedInputComponent->BindAction(
		MoveAction, ETriggerEvent::Triggered, this, &AAurePlayerController::Move);


	//绑定Shift按键事件
	EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &ThisClass::ShiftPressed);
	EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &ThisClass::ShiftReleased);
	
	
	
	// 绑定输入操作到增强输入组件，当输入操作被触发时，调用AAurePlayerController的AbilityInputTagPressed方法
	EnhancedInputComponent->BindAbilityActions(InputConfig, this,
		&AAurePlayerController::AbilityInputTagPressed,
		&AAurePlayerController::AbilityInputTagReleased,
		&AAurePlayerController::AbilityInputTagHeld);
}

void AAurePlayerController::Move(const FInputActionValue& InputActionValue)
{
	//通过标签阻止悬停事件的触发
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FAureGameplayTags::Get().Player_Block_InputHeld))
	{
		return;
	}
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
    //自动寻路
	AutoRun();
	//更新魔法阵位置
	UpdateMagicCircleLocation();
}

void AAurePlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	// 检查MagicCircle的有效性，如果无效，则在世界中生成一个新的MagicCircle实例
	if(!IsValid(MagicCircle)) MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
	
	// 如果DecalMaterial材质不为空，则将其应用到MagicCircle的MagicCircleDecal组件上
	if (DecalMaterial != nullptr)
	{
	    MagicCircle->MagicCircleDecal->SetMaterial(0,DecalMaterial);
	}
}

void AAurePlayerController::HideMagicCircle() const
{
	if(IsValid(MagicCircle)) MagicCircle->Destroy();
}

void AAurePlayerController::CursorTrace()
{
	//判断当前事件是否被阻挡，如果事件被阻挡，则清除相关内容
	if (GetASC()&&GetASC()->HasMatchingGameplayTag(FAureGameplayTags::Get().Player_Block_CursorTrace))
	{
		if (ThisActor) UnHighlightActor(ThisActor);
		if (LastActor) UnHighlightActor(LastActor);
		ThisActor = nullptr;
		LastActor = nullptr;
		return;
	}
	//如果当前鼠标处于魔法阵释放阶段，将忽略场景中的角色
	const ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_EXCLUDEPLAYERS : ECC_Visibility;
	
	 // 获取光标下的命中结果
	 GetHitResultUnderCursor( TraceChannel, false, CursorHit);
	
	// 如果光标命中结果没有阻挡命中，则返回
	if (!CursorHit.bBlockingHit) return;
	
	// 将最后一个演员设置为当前演员
	LastActor = ThisActor;
	// 如果命中结果对应的演员有效且实现了UHighlightInterface接口，则将其设置为当前演员
	if (IsValid(CursorHit.GetActor()) &&
		CursorHit.GetActor()->Implements<UHighlightInterface>())
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

void AAurePlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	// GEngine->AddOnScreenDebugMessage(
	// 	-1,
	// 	5.f,
	// 	FColor::Red,
	// 	FString::Printf(TEXT("AbilityInputTagPressed: %s"),
	// 		*InputTag.ToString()));

	//处理判断按下事件是否被阻挡
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FAureGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}
	if(InputTag.MatchesTagExact(FAureGameplayTags::Get().InputTag_LMB))
	{
		//ThisActor为鼠标悬停在敌人身上才会有值
		if(IsValid(ThisActor))
		{
			if(ThisActor->Implements<UEnemyInterface>())
			{
				//继承敌人接口，目标为敌人
				TargetingStatus = ETargetingStatus::TargetingEnemy;
			}
			else
			{
				//无敌人接口，基本为场景静态物体
				TargetingStatus = ETargetingStatus::TargetingNonEnemy;
			}
		}
		else
		{
			//目标不存在，设置为无目标状态
			TargetingStatus = ETargetingStatus::NotTargeting;
		}
		
		bAutoRunning = false;
	}
	//调用ASC内创建的键位按下事件
	if(GetASC()) GetASC()->AbilityInputTagPressed(InputTag);
}

void AAurePlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	// GEngine->AddOnScreenDebugMessage(
	// 	-1,
	// 	5.f,
	// 	FColor::Red,
	// 	FString::Printf(TEXT("AbilityInputTagReleased: %s"),
	// 		*InputTag.ToString()));
	// 检查输入标签是否与左鼠标按钮的标签匹配

	//处理判断抬起事件是否被阻挡
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FAureGameplayTags::Get().Player_Block_InputReleased))
	{
		return;
	}
	if(!InputTag.MatchesTagExact(FAureGameplayTags::Get().InputTag_LMB))
	{
	    // 如果不匹配且存在能力系统组件(ASC)，则调用其AbilityInputTagReleased方法
	    if(GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
	    return;
	}

	// 如果存在能力系统组件(ASC)，则调用其AbilityInputTagReleased方法
	if(GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
	
	
	// 如果当前处于目标锁定状态
	if(TargetingStatus != ETargetingStatus::TargetingEnemy && !bShiftKeyDown)
	{

		// 获取当前控制的Pawn
		const APawn* ControlledPawn = GetPawn();
	   
	    // 检查FollowTime是否小于短按阈值且ControlledPawn存在
	    if(FollowTime <= ShortPressThreshold && ControlledPawn)
	    {
		    // 检查当前Actor是否有效且实现了高亮接口，如果满足条件则调用接口方法设置移动目标位置
		    if (IsValid(ThisActor) && ThisActor->Implements<UHighlightInterface>())
		    {
			    IHighlightInterface::Execute_SetMoveToLocation(ThisActor, CachedDestination);
		    }
		    else if (GetASC() && !GetASC()->HasMatchingGameplayTag(FAureGameplayTags::Get().Player_Block_InputPressed))
		    {
		    	//触发点击时，播放点击特效
		    	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
		    }
	    	
	        // 使用UNavigationSystemV1找到从当前Pawn位置到缓存目标位置的路径
	        if(UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
	        	this, ControlledPawn->GetActorLocation(), CachedDestination))
	        {
	            // 清除样条内现有的点
	            Spline->ClearSplinePoints();
	            // 遍历导航路径上的所有点并将它们添加到样条曲线中
	            for(const FVector& PointLoc : NavPath->PathPoints)
	            {
	                // 将新的位置添加到样条曲线中
	                Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
	                 //DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Orange, false, 5.f); // 点击后debug调试
	            }
	            if (NavPath->PathPoints.Num() > 0)
	            {
	            	// 自动寻路将最终目的地设置为导航的终点，方便停止导航
	            	CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
	            	// 设置当前正常自动寻路状态，将在tick中更新位置
	            	bAutoRunning = true;
	            }
	        }
	    }
		//重置统计的时间
		FollowTime = 0.f;
		//将目标锁定状态设置为非目标
		TargetingStatus = ETargetingStatus::NotTargeting;
	}
}

void AAurePlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{

	//通过标签阻止悬停事件的触发
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FAureGameplayTags::Get().Player_Block_InputHeld))
	{
		return;
	}
	// 检查输入标签是否精确匹配左鼠标按钮的标签
	if(!InputTag.MatchesTagExact(FAureGameplayTags::Get().InputTag_LMB))
	{
	    // 如果不匹配且存在能力系统组件(ASC)，则调用其AbilityInputTagHeld方法
	    if(GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	    return;
	}

	// 获取当前控制的Pawn
	APawn* ControlledPawn = GetPawn();
	
	// 根据是否处于目标锁定状态执行不同的逻辑
	if(TargetingStatus == ETargetingStatus::TargetingEnemy||bShiftKeyDown)
	{
	    // 如果处于目标锁定状态且存在ASC，则调用其AbilityInputTagHeld方法
	    if(GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		Spline->ClearSplinePoints();
		CachedDestination = ControlledPawn->GetActorLocation();
	}
	else
	{
	    // 统计悬停时间来判断是否为点击
	    FollowTime += GetWorld()->GetDeltaSeconds();
	
	    // 获取鼠标拾取位置
	    if(CursorHit.bBlockingHit) CachedDestination = CursorHit.ImpactPoint;
		//DrawDebugSphere(GetWorld(), CachedDestination, 8.f, 8, FColor::Orange, false, 5.f); // 点击后debug调试
	    // 如果控制了Pawn，则计算世界方向并添加移动输入
	    if(ControlledPawn)
	    {
	        // 计算从Pawn位置到目标位置的世界方向
	        const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
	        // 向计算出的世界方向添加移动输入
	        ControlledPawn->AddMovementInput(WorldDirection);
	    }
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

void AAurePlayerController::AutoRun()
{
	//如果不在自动行驶模式下，则直接返回
	if(!bAutoRunning) return;
	
	//获取当前控制的Pawn对象
	if(APawn* ControlledPawn = GetPawn())
	{
	    //找到距离样条最近的位置
	    const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(
	        ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
	    //获取这个位置在样条上的方向
	    const FVector Direction = Spline->FindDirectionClosestToWorldLocation(
	        LocationOnSpline, ESplineCoordinateSpace::World);
	    //根据方向添加移动输入，使Pawn朝向样条方向移动
	    ControlledPawn->AddMovementInput(Direction);
	
	    //计算当前位置与目标位置的距离
	    const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
	    //如果距离小于等于自动行驶的接受半径，则停止自动行驶
	    if(DistanceToDestination <= AutoRunAcceptanceRadius)
	    {
	        bAutoRunning = false;
	    }
	}
}

void AAurePlayerController::UpdateMagicCircleLocation()
{
	// 检查魔法圈对象是否有效
	if(IsValid(MagicCircle))
	{
	    // 设置魔法圈的当前位置为光标击中点的位置
	    MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
	}
}

void AAurePlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter,bool IsBlockedHit, bool IsCriticalHit)
{
	// 检查目标角色是否有效，并且伤害文本组件类是否已指定
	if(IsValid(TargetCharacter) && DamageTextComponentClass)
	{
	    // 创建伤害文本组件实例
	    UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
	    // 注册组件，使其在世界中可见
	    DamageText->RegisterComponent(); 
	    // 将伤害文本附加到目标角色的根组件上，保持相对位置
	    DamageText->AttachToComponent(
	    	TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform); 
	    // 从角色组件上分离伤害文本组件，保持世界位置
	    DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); 
	    // 设置伤害文本内容
	    DamageText->SetDamageText(DamageAmount, IsBlockedHit, IsCriticalHit); 
	}
}
