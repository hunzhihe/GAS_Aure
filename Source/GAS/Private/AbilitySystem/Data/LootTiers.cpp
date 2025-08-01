// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LootTiers.h"

TArray<FLootItem> ULootTiers::GetLootItems()
{
	/**
	 * 根据掉落物品配置生成实际掉落物品列表
	 * 
	 * 该函数遍历所有可能的掉落物品，根据每个物品的掉落概率和最大生成数量来决定
	 * 最终生成哪些物品。每个物品会进行多次随机判定，如果随机数小于掉落概率，
	 * 则将该物品添加到返回列表中。
	 *
	 * @return TArray<FLootItem> 实际生成的掉落物品列表
	 */
	TArray<FLootItem> ReturnItems;
	
	// 遍历所有可能的掉落物品配置
	for(const FLootItem Item : LootItems)
	{
		// 对每个物品进行最多MaxNumberToSpawn次的生成尝试
		for(int32 i=0; i<Item.MaxNumberToSpawn; ++i)
		{
			// 根据物品的掉落概率决定是否生成该物品
			if(FMath::RandRange(1.f, 100.f) < Item.ChanceToSpawn)
			{
				// 创建新的掉落物品实例并添加到返回列表
				FLootItem NewItem;
				NewItem.LootClass = Item.LootClass;
				NewItem.bLootLevelOverride = Item.bLootLevelOverride;
				ReturnItems.Add(NewItem);
			}
		}
	}
	
	return ReturnItems;
	
}
