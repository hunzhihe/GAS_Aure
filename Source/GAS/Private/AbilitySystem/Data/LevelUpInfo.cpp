// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP)
{
	int32 Level = 1;
	bool bLevelUp = true;
	while (bLevelUp)
	{
		//如果完遍历整个数组。说明达到最大等级,返回当前等级.数组的下标表示对应等级的数据
		if (LevelUpInformation.Num()-1<= Level) return Level;

		//判断当前XP是否大于等于对应等级的等级要求
		if (XP>=LevelUpInformation[Level].LevelUpRequirement)
		{
			Level++;
		}
		else
		{
			//否则，返回当前等级
			bLevelUp = false;
		}
	}
	return Level;
}
