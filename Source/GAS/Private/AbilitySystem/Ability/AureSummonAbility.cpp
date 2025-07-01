// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/AureSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAureSummonAbility::GetSpawnLocation()
{
	// 获取角色的前进方向向量
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	// 获取角色的位置
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	// 计算每个生物体之间的分散角度
	const float DeltaSpread = SpawnSpread / NumMinions;
	
	// 计算最左侧的分散方向
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	// 初始化用于存储所有生物体生成位置的数组
	TArray<FVector> SpawnLocations;
	// 遍历每个生物体，计算其生成位置
	for (int32 i = 0; i < NumMinions; i++)
	{
	    // 根据当前生物体的索引和分散角度，计算其方向
	    const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
	    // 在角色位置和计算出的方向上，随机选择一个生成位置
	    FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
	
	    // 执行线迹检测，以确定生成位置是否合适
	    FHitResult Hit;
	    GetWorld()->LineTraceSingleByChannel(
	        Hit, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f),
	        ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECC_Visibility);
	    // 如果线迹检测命中，使用命中点作为生成位置
	    if (Hit.bBlockingHit)
	    {
	        ChosenSpawnLocation = Hit.ImpactPoint;
	    }

		// 绘制生成位置
		DrawDebugSphere(GetWorld(), ChosenSpawnLocation, 15.f,12,FColor::Green,false, 3.f);

		// 将计算出的生成位置添加到数组中
	    SpawnLocations.Add(ChosenSpawnLocation);
	}

    //----打印测试------//
	const FVector RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector); //获取到最右侧的角度
	
	UKismetSystemLibrary::DrawDebugArrow(
		GetAvatarActorFromActorInfo(), Location, Location + LeftOfSpread * MaxSpawnDistance,
		4.f, FLinearColor::Red, 3.f);
	UKismetSystemLibrary::DrawDebugArrow(
		GetAvatarActorFromActorInfo(), Location, Location + RightOfSpread * MaxSpawnDistance,
		4.f, FLinearColor::Black, 3.f);
	
	DrawDebugSphere(GetWorld(), Location + LeftOfSpread * MinSpawnDistance, 15.f,12,FColor::Blue,false, 3.f);
	DrawDebugSphere(GetWorld(), Location + LeftOfSpread * MaxSpawnDistance, 15.f,12,FColor::Blue,false, 3.f);
	DrawDebugSphere(GetWorld(), Location + RightOfSpread * MinSpawnDistance, 15.f,12,FColor::Blue,false, 3.f);
	DrawDebugSphere(GetWorld(), Location + RightOfSpread * MaxSpawnDistance, 15.f,12,FColor::Blue,false, 3.f);



	
	// 返回所有生物体的生成位置数组
	return SpawnLocations;
}

TSubclassOf<APawn> UAureSummonAbility::GetRandomMinionClass()
{
	const int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses[Selection];
}
