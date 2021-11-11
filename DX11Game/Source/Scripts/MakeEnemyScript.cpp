//==================================================================
//								MakeEnemyScript.h
//	エネミー生成スクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/08	エネミー生成スクリプトクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "MakeEnemyScript.h"

// システム
#include "../Engine/System/input.h"
#include "../Engine/System/Sound.h"
#include "../Engine/System/debugproc.h"

// マネージャー
#include "../Engine/ECS/Entity/EntityManager.h"

// エンティティ
#include "../Engine/ECSEntity/GameObject.h"

// コンポーネント
#include "../Engine/ECSCompoent/Transform.h"
#include "../Engine/ECSCompoent/MeshRenderer.h"
#include "../Engine/ECSCompoent/BillboardRenderer.h"
#include "../Engine/ECSCompoent/AssimpRenderer.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"
#include "../Engine/ECSCompoent/InstancingMeshRenderer.h"
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"

// ECSコンポーネント
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/DeltaCollider.h"

// ECSシステム
#include "../Engine/ECSSystem/RigidbodySystem.h"
#include "../Engine/ECSSystem/DeltaCollisionSystem.h"

// スクリプト
#include "BalanceEnemyScript.h"
#include "AttackEnemyScript.h"
#include "SpeedEnemyScript.h"
#include "SkiilEnemyScript.h"
#include "PhysicalEnemyScript.h"
#include "PlayerScript.h"
#include "DeltaUIScript.h"

// ネームスペース
using namespace ECS;


//===== マクロ定義 =====
#define MAX_SPAWN_NUM (50)

#define MAX_ENETETY_NUM (MAX_SPAWN_NUM * 120)


//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void MakeEnemyScript::Start()
{
	const auto& deltaUi = GetEntityManager()->CreateEntity<GameObject>();
	deltaUi->AddComponent<DeltaUIScript>()->
		SetMaker(gameObject().lock()->GetComponent<MakeEnemyScript>());
}

//========================================
//
//	更新時
//
//========================================
void MakeEnemyScript::Update()
{
	//----- プレイヤーと同期 -----

	// プレイヤーを取得
	const auto& player = m_player.lock();
	if (!player) return;

	// プレイヤー座標
	const auto& playerTrans = player->transform().lock();
	Vector3 playerPos = player->transform().lock()->m_pos;
	// 座標の同期
	transform().lock()->m_pos = playerPos;

	//----- エネミーの生成 -----

	// 経過時間
	m_nTime++;
	if (m_nTime % (60 * 60) != 0 && m_nEnemyCount > 0) return;
	m_nWave++;

	// 生成数を計算
	int nSpawnNum = m_nWave  * 1 + 3;
	if (nSpawnNum > MAX_SPAWN_NUM) nSpawnNum = MAX_SPAWN_NUM;
	m_nEnemyCount = nSpawnNum;

	// 生成するエネミーの種類
	for (int i = 0; i < nSpawnNum; ++i)
	{
		//--- エネミーを生成する座標
		float height = (rand() % 5 - 2) * 100;
		if (playerPos.y < 600) playerPos.y = 100;
		Vector3 spawnPos = { 2000, height, 0 };
		Vector3 temp = playerPos;
		temp.y = rand() % 500 + 100;
		spawnPos = Mathf::RotationY(spawnPos, rand() % 360) + temp;

		// エンティティの数
		if (GetEntityManager()->GetEntityCount() > MAX_ENETETY_NUM) continue;

		// エネミー生成
		const auto& obj = Instantiate<GameObject>(spawnPos);
		// コンポーネントの追加
		const auto& enemy = obj->AddComponent<EnemyBaseScript>();
		enemy->SetPlayer(player);
		enemy->m_maker = gameObject().lock()->GetComponent<MakeEnemyScript>();

		//--- タイプで分ける
		int type = rand() % (int)EnemyBaseScript::Type::Max;
		switch (type)
		{
		case (int)ECS::EnemyBaseScript::Type::Attack:
			obj->AddComponent<AttackEnemyScript>();
			enemy->SetUpStatus(EnemyBaseScript::Type::Attack, m_nWave);
			break;
		case (int)ECS::EnemyBaseScript::Type::Physical:
			obj->AddComponent<PhysicalEnemyScript>();
			enemy->SetUpStatus(EnemyBaseScript::Type::Physical, m_nWave);
			break;
		case (int)ECS::EnemyBaseScript::Type::Skiil:
			obj->AddComponent<SkiilEnemyScript>();
			enemy->SetUpStatus(EnemyBaseScript::Type::Skiil, m_nWave);
			break;
		case (int)ECS::EnemyBaseScript::Type::Speed:
			obj->AddComponent<SpeedEnemyScript>();
			enemy->SetUpStatus(EnemyBaseScript::Type::Speed, m_nWave);
			break;
		case (int)ECS::EnemyBaseScript::Type::Balance:
			obj->AddComponent<BalanceEnemyScript>();
			enemy->SetUpStatus(EnemyBaseScript::Type::Balance, m_nWave);
			break;
		}
	}
}

//========================================
//
//	後更新時
//
//========================================
void MakeEnemyScript::LateUpdate()
{
	// デバック表示
	PrintDebugProc("Wave:%d\n\n", m_nWave);
	PrintDebugProc("EnemyCount:%d\n", m_nEnemyCount);
	PrintDebugProc("EntityNum:%d\n", GetEntityManager()->GetEntityCount());
}

//========================================
//
//	終了時
//
//========================================
void MakeEnemyScript::End()
{
	// スコアのロード
	FILE* fp;
	int nScore = 0;

	// 前回のスコアを読み込む
	fopen_s(&fp, "data/score.bin", "rb");

	if (fp)
	{
		fread(&nScore, sizeof(int), 1, fp);

		fclose(fp);
	}

	if (m_nWave > nScore)
	{
		// スコアの書き出し
		FILE* fp;
		nScore = m_nWave;

		// 前回のスコアを読み込む
		fopen_s(&fp, "data/score.bin", "wb");

		if (fp)
		{
			fwrite(&nScore, sizeof(int), 1, fp);

			fclose(fp);
		}
	}
}
