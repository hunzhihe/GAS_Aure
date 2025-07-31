// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "GAS/GAS.h"
#include "Interaction/HighlightInterface.h"
#include "Interaction/SaveInterface.h"
#include "CheckPoint.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class GAS_API ACheckPoint : public APlayerStart, public ISaveInterface, public IHighlightInterface
{
	GENERATED_BODY()
public:

	//构造函数
	ACheckPoint(const FObjectInitializer& ObjectInitializer);

	/*   Save Interface   */

	//是否需要修改变换，检查点不需要
	virtual bool ShouldLoadTransform_Implementation() override { return false; }
	//通过存档二进制修改Actor数据后，更新Actor
	virtual void LoadActor_Implementation() override; 


	//当前检查点是否已经被激活，设置SaveGame表示该值将会被存储到存档文件中
	UPROPERTY(BlueprintReadWrite, SaveGame)
	bool bReached = false;
	
	/*   End Save Interface   */


	//是否绑定叠加事件回调
	UPROPERTY(EditAnywhere)
	bool bBindOverlapCallback = true;
	
protected:

	virtual void BeginPlay() override;
	
	/**
	 * 球碰撞体和物体发生碰撞后的回调
	 * @param OverlappedComponent 发生重叠事件的自身的碰撞体对象
	 * @param OtherActor 目标的actor对象
	 * @param OtherComp 目标的碰撞体组件
	 * @param OtherBodyIndex 目标身体的索引
	 * @param bFromSweep 是否为瞬移检测到的碰撞
	 * @param SweepResult 如果位置发生过瞬移（直接设置到某处），两个位置中间的内容会记录到此对象内
	 */
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/* Highlight Interface */
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	/* Highlight Interface */


	//移动到检查点的位置
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MoveToComponent;

	//自定义深度值
	UPROPERTY(EditDefaultsOnly)
	int32 CustomDepthStencilOverride = CUSTOM_DEPTH_TAN;
	
	/**
	 * 检查点激活后的处理，需要在蓝图中对其实现
	 * @param DynamicMaterialInstance 传入检查点模型的材质实例
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(UMaterialInstanceDynamic* DynamicMaterialInstance);

	
	//当玩家角色和检测点产生碰撞后，检查点被激活触发此函数
	UFUNCTION(BlueprintCallable)
	void HandleGlowEffects();
	
	//检查点显示的模型
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;

	//检查点模型使用的碰撞体
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere; 

	//检查点模型使用的材质
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> CheckpointMaterial;
	
};
