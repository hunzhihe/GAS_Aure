// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "Input/InputConfig.h"
#include "AurePlayerController.generated.h"

class AMagicCircle;
class UDamageTextComponent;
class USplineComponent;
class UAureAbilitySystemComponent;
struct FInputActionValue;
class UInputMappingContext;




// 定义一个枚举类，用于表示目标锁定状态
enum class ETargetingStatus : uint8
{
	// 正在锁定敌人
	TargetingEnemy,
	// 正在锁定非敌人
	TargetingNonEnemy,
	// 未锁定任何目标
	NotTargeting
};

/**
 * 
 */
UCLASS()
class GAS_API AAurePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAurePlayerController();
	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool IsBlockedHit, bool IsCriticalHit);

	//显示魔法阵，设置光圈贴花材质
	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial);

	//隐藏魔法阵，并销毁
	UFUNCTION(BlueprintCallable)
	void HideMagicCircle() const; 


protected:
	virtual void BeginPlay() override;
	//生成输入组件时触发
	virtual void SetupInputComponent() override;

private:
	//输入映射
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	//输入操作-移动
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
    //输入操作-SHIFT
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPressed() { bShiftKeyDown = true; };
	void ShiftReleased() { bShiftKeyDown = false; };
	bool bShiftKeyDown = false;

	void Move(const FInputActionValue& InputActionValue);


    //通过光标追踪检测鼠标是否悬停在场景中的某个Actor上
    void CursorTrace();
    // 指向最后一个被高亮显示的Actor的智能指针
    TObjectPtr<AActor> LastActor;
    // 指向当前正在考虑要高亮显示的Actor的智能指针
    TObjectPtr<AActor> ThisActor;
    // 存储光标追踪的命中结果，用于确定光标指向的是什么物体
    FHitResult CursorHit;
    // 高亮指定的Actor，例如通过改变其外观等方式实现
    static void HighlightActor(AActor* InActor);
    // 移除指定Actor的高亮效果，将其外观恢复到原始状态
    static void UnHighlightActor(AActor* InActor);

	// 当某个能力输入标签被按下时调用此函数
	// 参数 InputTag: 被按下的输入标签
	void AbilityInputTagPressed(FGameplayTag InputTag);
	
	// 当某个能力输入标签被释放时调用此函数
	// 参数 InputTag: 被释放的输入标签
	void AbilityInputTagReleased(FGameplayTag InputTag);
	
	// 当某个能力输入标签被持续按住时调用此函数
	// 参数 InputTag: 被持续按住的输入标签
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	// InputConfig 属性用于编辑默认设置，属于输入配置类别
	// 该属性用于关联输入配置，以定义能力输入标签的行为
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputConfig> InputConfig;


	  UPROPERTY()
	  TObjectPtr<UAureAbilitySystemComponent> AureAbilitySystemComponent;
	 
	  UAureAbilitySystemComponent* GetASC();

	// 缓存的目标位置，初始化为零向量，用于存储和快速访问目的地
	FVector CachedDestination = FVector::ZeroVector;
	// 跟踪时间，初始化为0，用于累计鼠标按住时间
	float FollowTime = 0.f;
	// 短按阈值，设定为0.5秒，用于区分短按和长按操作
	UPROPERTY(EditDefaultsOnly)
	float ShortPressThreshold = 0.5f;
	// 自动运行标志，初始化为false，表示是否处于自动运行状态
	bool bAutoRunning = false;
    //当前目标是否为敌人
	ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting;


	//当小于该距离时，关闭自动寻路
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	// 存储自动寻路路径的曲线
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	void AutoRun();

    /*伤害数字显示*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
	
	/*伤害数字显示*/

    //创建魔法阵使用的类
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMagicCircle> MagicCircleClass;
    //存储魔法阵属性的智能指针
	UPROPERTY()
	TObjectPtr<AMagicCircle> MagicCircle;
    //每帧更新魔法阵位置
	void UpdateMagicCircleLocation();
};
