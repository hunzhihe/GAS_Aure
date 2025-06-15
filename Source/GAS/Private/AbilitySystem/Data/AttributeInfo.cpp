// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AttributeInfo.h"

FAureAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	// 遍历属性信息数组，寻找匹配的属性标签
	for (const FAureAttributeInfo& Info : AttributeInformation)
	{
	    // 如果当前属性信息的标签与指定的属性标签完全匹配，则返回该属性信息
	    if (Info.AttributeTag.MatchesTagExact(AttributeTag))
	    {
	        return Info;
	    }
	}
	
	// 如果遍历结束后仍未找到匹配的属性信息，且设置了日志记录标志
	if (bLogNotFound)
	{
	    // 输出错误日志，指示无法在指定的数据列表中找到对应的属性标签
	    UE_LOG( LogTemp, Error, TEXT("从数据列表 [%s] 中无法找到对应的 AttributeTag [%s]."),*GetNameSafe(this) , *AttributeTag.ToString());
	}
	
	// 如果未找到匹配的属性信息，返回一个默认构造的属性信息对象
	return FAureAttributeInfo();
}
