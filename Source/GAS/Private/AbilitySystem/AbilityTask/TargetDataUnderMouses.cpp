// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTask/TargetDataUnderMouses.h"

#include "AbilitySystemComponent.h"
#include "GAS/GAS.h"

/**
 * 创建一个UTargetDataUnderMouses对象的函数
 * 
 * 该函数用于生成一个UTargetDataUnderMouses类的实例，该实例与鼠标下的目标数据相关联
 * 主要用于在游戏中根据鼠标位置获取目标数据，为游戏能力系统的一部分
 * 
 * @param OwningAbility 拥有该能力的UGameplayAbility指针，用于创建新的能力任务
 * @return 返回新创建的UTargetDataUnderMouses对象指针，如果创建失败则返回nullptr
 */
UTargetDataUnderMouses* UTargetDataUnderMouses::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
    // 使用NewAbilityTask函数创建一个新的UTargetDataUnderMouses对象
    // NewAbilityTask是一个用于创建特定任务对象的函数模板，这里用于创建UTargetDataUnderMouses类型的对象
    UTargetDataUnderMouses* MyObj = NewAbilityTask<UTargetDataUnderMouses>(OwningAbility);
    
    // 返回新创建的对象指针
    return MyObj;
}


// 当能力激活时调用此函数来处理目标数据
void UTargetDataUnderMouses::Activate()
{
    // 检查当前控制的演员是否为本地控制
    const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
    if (bIsLocallyControlled)
    {
        // 如果是本地控制，直接发送鼠标光标数据
        SendMouseCursorData();
    }
    else
    {
        // 如果不是本地控制，准备接收复制的目标数据
        const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
        const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
        // 注册一个委托，当目标数据可用时调用
        AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(
        	SpecHandle, ActivationPredictionKey).AddUObject(this, &UTargetDataUnderMouses::OnTargetDataReplicatedCallback);
        // 尝试调用复制的目标数据委托，如果已设置
        const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(
        	SpecHandle, ActivationPredictionKey);
        if (!bCalledDelegate)
        {
            // 如果没有调用委托，设置等待远程玩家数据状态
            SetWaitingOnRemotePlayerData();
        }
    }
}


// UTargetDataUnderMouses 类中的 SendMouseCursorData 方法
// 该方法用于发送鼠标光标下的目标数据
void UTargetDataUnderMouses::SendMouseCursorData()
{
    // 创建一个作用域预测窗口，用于处理能力系统的预测
    FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
    
    // 获取当前玩家控制器
    APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
    
    // 声明一个命中结果变量，用于存储鼠标光标下的碰撞信息
    FHitResult CursorHit;
    
    // 使用玩家控制器获取鼠标光标下的碰撞信息
    PC->GetHitResultUnderCursor(ECC_Target, false, CursorHit);

    // 创建一个游戏能力目标数据句柄
    FGameplayAbilityTargetDataHandle DataHandle;
    
    // 创建一个单目标命中数据对象
    FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
    
    // 将命中结果存储到数据对象中
    Data->HitResult = CursorHit;
    
    // 将数据对象添加到数据句柄中
    DataHandle.Add(Data);
    
    // 调用服务器方法，设置复制的目标数据
    AbilitySystemComponent->ServerSetReplicatedTargetData(
        GetAbilitySpecHandle(),
        GetActivationPredictionKey(),
        DataHandle,
        FGameplayTag(),
        AbilitySystemComponent->ScopedPredictionKey);

    // 如果应该广播能力任务委托，则广播有效数据
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        ValidData.Broadcast(DataHandle);
    }
}


// 当目标数据在客户端复制时调用此函数
// 该函数处理接收到的复制数据，并根据情况广播有效数据
void UTargetDataUnderMouses::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
    // 消费客户端复制的目标数据，确保数据同步和一致性
    AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

    // 检查是否应该广播能力任务委托
    // 如果是，广播接收到的数据句柄，通知其他系统或组件
    if (ShouldBroadcastAbilityTaskDelegates())
    {
        ValidData.Broadcast(DataHandle);
    }
}
