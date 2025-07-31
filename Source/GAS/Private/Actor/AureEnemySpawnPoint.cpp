// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AureEnemySpawnPoint.h"

#include "Character/Enemy.h"

void AAureEnemySpawnPoint::SpawnEnemy()
{
	//延迟生成Actor,并设置其尝试调整位置但固定生成
	AAureEnemy* Enemy = GetWorld()->SpawnActorDeferred<AAureEnemy>(
		EnemyClass,
		GetTransform(),
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
		);

	Enemy->SetLevel(EnemyLevel);
	Enemy->SetCharacterClass(CharacterClass);
	Enemy->FinishSpawning(GetTransform());
	Enemy->SpawnDefaultController();
}
