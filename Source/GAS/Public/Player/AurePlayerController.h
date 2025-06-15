// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "AurePlayerController.generated.h"

class UAureAbilitySystemComponent;
struct FInputActionValue;
class UInputMappingContext;
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




	  UPROPERTY()
	  TObjectPtr<UAureAbilitySystemComponent> AureAbilitySystemComponent;
	 
	  UAureAbilitySystemComponent* GetASC();
};
