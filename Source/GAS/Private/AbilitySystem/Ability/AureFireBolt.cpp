// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/AureFireBolt.h"

#include "AureGameplayTags.h"

FString UAureFireBolt::GetDescription(int32 Level)
{
	return GetDescriptionAtLevel(Level, L"火球术");
}

FString UAureFireBolt::GetNextLevelDescription(int32 Level)
{
	return GetDescriptionAtLevel(Level, L"下一等级");
}

FString UAureFireBolt::GetDescriptionAtLevel(int32 Level, const FString& Title)
{
	const int32 Damage = GetDamageByDamageType(Level, FAureGameplayTags::Get().Damage_Fire);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(TEXT(
		// 标题
		"<Title>%s</>\n"

		// 细节
		"<Small>等级：</> <Level>%i</>\n"
		"<Small>技能冷却：</> <Cooldown>%.1f</>\n"
		"<Small>蓝量消耗：</> <ManaCost>%.1f</>\n\n"//%.1f会四舍五入到小数点后一位

		// 技能描述
		"<Default>发射 %i 颗火球，在发生撞击时产生爆炸，并造成</> <Damage>%i</> <Default>点火焰伤害，并有一定几率燃烧。</>"),

		// 动态修改值
		*Title,
		Level,
		Cooldown,
		ManaCost,
		FMath::Min(Level, 5),
		Damage);
	
}
