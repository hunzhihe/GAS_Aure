// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AureGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AureAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"

class UCharacterClassInfo;

//伤害计算所需的属性捕获定义
struct AureDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);

	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);

	AureDamageStatics()
	{
		/*参数 属性集 属性名称 目标还是自身 是否设置快照（true为创建时获取，false为应用时获取）   */
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAureAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAureAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAureAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAureAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAureAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAureAttributeSet, CriticalHitResistance, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAureAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAureAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAureAttributeSet, PhysicalResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAureAttributeSet, ArcaneResistance, Target, false);
	}
};
/**
 * 返回AureDamageStatics的静态实例引用
 * 
 * 此函数用于在整个程序中提供一个全局访问点，以获得AureDamageStatics类的唯一实例
 * 它利用了静态局部变量的特性，确保在第一次调用时实例化，并在程序结束时自动销毁
 * 
 * @return AureDamageStatics& 返回AureDamageStatics类的静态实例引用
 */
static const AureDamageStatics& DamageStatics()
{
	// 静态局部变量，确保在程序生命周期内只被实例化一次
	static AureDamageStatics DStatics;
	// 返回静态实例的引用
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	/*将属性添加到捕获列表*/
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);

	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
}

void UExecCalc_Damage::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FGameplayEffectSpec& Spec, FAggregatorEvaluateParameters EvaluationParameters,
	const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& TagsToCaptrueDefs) const
{
	const FAureGameplayTags& GameplayTags = FAureGameplayTags::Get();

	//遍历所有的负面效果伤害类型，根据伤害类型是否赋值来判断是否需要应用负面效果
	for(const TTuple<FGameplayTag, FGameplayTag>& Pair : GameplayTags.DeBuffsToResistance)
	{
		FGameplayTag DeBuffDamageType = Pair.Key; //获取到负面效果伤害类型
		const FGameplayTag ResistanceType = Pair.Value; //获取到负面效果抵抗类型
		const float TypeDamage = Spec.GetSetByCallerMagnitude(DeBuffDamageType, false, -1.f);

		//如果负面效果设置了伤害，即使为0，也需要应用负面效果
		if(TypeDamage > -.5f)
		{
			//获取负面效果命中率
			const float SourceDeBuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance, false, -1.f);

			//----------------获取负面效果抵抗------------
			float TargetDeBuffResistance = 0.f; //计算目标对收到的负面效果类型的抵抗
			//检查对应的属性快照是否设置，防止报错
			checkf(TagsToCaptrueDefs.Contains(ResistanceType), TEXT("在ExecCalc_Damage中，无法获取到Tag[%s]对应的属性快照"), *ResistanceType.ToString());
			//通过抗性标签获取到属性快照的值
			const FGameplayEffectAttributeCaptureDefinition CaptureDef = TagsToCaptrueDefs[ResistanceType];
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, TargetDeBuffResistance);
			TargetDeBuffResistance = FMath::Clamp(TargetDeBuffResistance, 0.f, 100.f); //将抗住限制在0到100

			//----------------计算负面效果是否应用------------
			const float EffectiveDeBuffChance = SourceDeBuffChance * (100 - TargetDeBuffResistance) / 100.f; //计算出负面效果的实际命中率
			const bool bDeBuff = FMath::RandRange(1, 100) < EffectiveDeBuffChance; //判断此次效果是否实现命中
			if(bDeBuff)
			{
				//获取GE上下文设置负面效果相关配置
				FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

				//设置当前应用负面效果成功
				UAuraAbilitySystemLibrary::SetIsSuccessfulDeBuff(ContextHandle, true);

				const float SourceDeBuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration, false, 0.f);
				const float SourceDeBuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Frequency, false, 0.f);
				//设置负面效果 伤害类型 伤害 持续时间 触发频率
				UAuraAbilitySystemLibrary::SetDeBuff(ContextHandle, DeBuffDamageType, TypeDamage, SourceDeBuffDuration, SourceDeBuffFrequency);
			}
		}
	}
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	
    /****通用内容****/

	// 获取技能系统组件
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	// 获取技能系统组件关联的Avatar
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	//获取等级
	int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}
	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}
	

	//获取挂载此类的GE实例
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	//获取GE上下文句柄
	FGameplayEffectContextHandle EffectContext = Spec.GetContext();
	
	

	//设置评估参数
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	/***通用内容****/


	const FAureGameplayTags GameplayTags = FAureGameplayTags::Get();
	//存储抗性标签和属性快照对应的Map
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	//添加标签和属性快照
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Fire, DamageStatics().FireResistanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Lightning, DamageStatics().LightningResistanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Arcane, DamageStatics().ArcaneResistanceDef);
	TagsToCaptureDefs.Add(GameplayTags.Attributes_Resistance_Physical, DamageStatics().PhysicalResistanceDef);


	// Debuff
	DetermineDebuff(ExecutionParams, Spec, EvaluationParameters, TagsToCaptureDefs);

	
	//伤害
	float Damage = 0;


	//---------伤害类型和抗性-------//
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair :GameplayTags.DamageTypesToResistances )
	{
		const FGameplayTag& DamageType = Pair.Key;
		const FGameplayTag& ResistanceTag = Pair.Value;
		checkf(TagsToCaptureDefs.Contains(ResistanceTag), TEXT("在ExecCalc_Damage中，无法获取到Tag[%s]对应的属性快照"), *ResistanceTag.ToString());

		//获取抗性标签对应的属性快照
		const FGameplayEffectAttributeCaptureDefinition& CaptureDef = TagsToCaptureDefs[ResistanceTag];

		//获取属性值
		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);
		//通过Tag获取对应伤害值
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageType);
		//减去抗性
		DamageTypeValue *= (100.f - Resistance) / 100.f;

		if (DamageTypeValue<=0.f)
		{
			//如果伤害值小于等于零，没必要处理范围伤害
			continue;;
		}

		if(UAuraAbilitySystemLibrary::IsRadialDamage(EffectContext))
		{
           DamageTypeValue = UAuraAbilitySystemLibrary::ApplyRadialDamageWithFalloff(
           	TargetAvatar,
           	DamageTypeValue,
           	0.f,
           	UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectContext),
            UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContext),
            UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContext),
            1.0,
            SourceAvatar,
            nullptr
           	);
			// 1. 覆写 TakeDamage 函数，通过函数获取范围技能能够造成的最终伤害
			// 2. 创建一个委托 OnDamageDelegate， 在TakeDamage里向外广播最终伤害数值
			// 3. 在战斗接口声明一个函数用于返回委托，并在角色基类实现，在计算伤害时通过战斗接口获取到委托，并绑定匿名函数
			// 4. 调用 UGameplayStatics::ApplyRadialDamageWithFalloff 函数应用伤害，函数内会调用角色身上的TakeDamage来广播委托。
			// 5. 在匿名函数中，修改实际造成的伤害。
			
			// if(ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetAvatar))
			// {
			// 	CombatInterface->GetOnDamageDelegate().AddLambda([&](float DamageAmount)
			// 	{
			// 		DamageTypeValue = DamageAmount;
			// 	});
			// }
			//
			// UGameplayStatics::ApplyRadialDamageWithFalloff(
			// 	TargetAvatar,
			// 	DamageTypeValue,
			// 	0.f,
			// 	UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectContext),
			// 	UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContext),
			// 	UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContext),
			// 	1.f,
			// 	UDamageType::StaticClass(),
			// 	TArray<AActor*>(),
			// 	SourceAvatar,
			// 	nullptr);
		}
		//将每种属性伤害值合并进行后续计算
		Damage += DamageTypeValue;
	}

	

	//------- 格挡 --------//
	//获取格挡率，如果格挡触发，则受伤害减半
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Clamp(TargetBlockChance, 0, 90);

	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;

	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContext, bBlocked);
	
	//格挡触发
	if (bBlocked) Damage /= 2.f;
	//------- 格挡 --------//
	
	
	//------护甲和护甲穿透-------//
	// 初始化护甲值为0，用于后续的计算
	float Armor = 0.f;
	// 尝试计算捕获的属性幅度，这里专注于“护甲”属性。该函数调用旨在根据当前的评估参数，
	// 计算出作用对象上“护甲”属性的实际影响值，并将结果存储在Armor变量中。
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, Armor);
	// 确保护甲值在合理的范围内，即护甲值最小为0，最大为其原始计算值。
	// 这一步防止护甲值出现负数或超过其应有的最大值，维护数据的有效性和合理性。
	Armor = FMath::Clamp(Armor, 0.f, Armor);

	// 获取源护甲穿透
	float SoureArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SoureArmorPenetration);
	SoureArmorPenetration = FMath::Max(0.f, SoureArmorPenetration);

	//获取源角色的CharacterClassInfo角色配置
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);
	
	//计算护甲穿透影响的护甲值
	const float EffectiveArmor = Armor * (100.f - SoureArmorPenetration * ArmorPenetrationCoefficient)/100.f;

	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);
	//现护甲值影响的伤害
	Damage *= (100.f - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
	//------护甲和护甲穿透-------//


	//--------暴击和暴击伤害-------//
    //获取暴击率
	float CriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().CriticalHitChanceDef, EvaluationParameters, CriticalHitChance);
	CriticalHitChance = FMath::Max<float>(CriticalHitChance, 0.f);
	//目标暴击抵抗
	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
    TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);

	//获取暴击伤害
	float CriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().CriticalHitDamageDef, EvaluationParameters, CriticalHitDamage);
	CriticalHitDamage = FMath::Max<float>(CriticalHitDamage, 0.f);

	//计算暴击抵抗系数
	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(
		FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);

	//影响后的暴击几率
	const float EffectiveCriticalHitChance = CriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;

	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContext, bCriticalHit);
	
	//触发暴击 伤害乘以暴击伤害率
	if(bCriticalHit) Damage = Damage * 2.f + CriticalHitDamage;
	//---------暴击和暴击伤害-------//
	


	
	
	
	
	

	//输出计算结果
	const FGameplayModifierEvaluatedData EvaluatedData(UAureAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);

	Damage = FMath::Max(0.f, Damage);//防止伤害小于0
	// 打印日志
	UE_LOG(LogTemp, Warning,
			TEXT("%s s受到伤害，当前生命值减少：%f"),
			*SourceAvatar->GetName(), Damage);
	
	
}


