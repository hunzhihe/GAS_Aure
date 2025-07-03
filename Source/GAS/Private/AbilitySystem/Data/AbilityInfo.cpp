// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AbilityInfo.h"

#include "GAS/AureLogChannels.h"

FAureAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for(const FAureAbilityInfo& Info : AbilityInformation)
	{
		if(Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}

	if(bLogNotFound)
	{
		//获取不到数据时.打印错误信息
		UE_LOG(LogAure, Error, TEXT("无法通过技能标签[%s] 在技能数据[%s]中查找到相对应的技能数据."),
			*AbilityTag.ToString(), *GetNameSafe(this));
	}

	return FAureAbilityInfo();

}
