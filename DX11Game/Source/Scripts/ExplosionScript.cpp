/*****************************************************************//**
 * \file   ExplosionScript.cpp
 * \brief  課題爆発エフェクト
 * 
 * \author USAMI KOSHI
 * \date   2021/05/14　課題爆発エフェクトクラス作成
 *********************************************************************/


//====== インクルード部 ======

// 自身
#include "ExplosionScript.h"

// システム
#include "../Engine/System/input.h"
#include "../Engine/System/Sound.h"

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
#include "../Engine/ECSCompoent/DeltaCollider.h"

// ECSシステム
#include "../Engine/ECSSystem/DeltaCollisionSystem.h"


// ネームスペース
using namespace ECS;


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void ExplosionScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("Explosin");
	gameObject().lock()->SetTag("Explosin");

	transform().lock()->m_pos = Vector3(0, 300, 1000);
	transform().lock()->m_scale = Vector3(1, 1, 1);

	// コンポーネントの追加

	// レンダラー
	const auto& renderer = gameObject().lock()->AddComponent<AssimpRenderer>();
	renderer->ModelLoad("data/model/JAS39.fbx");
	m_renderer = renderer;

	// 時間
	m_destroyTime = 60 * 3;
	m_time = 0;
}

//========================================
//
//	更新時
//
//========================================
void ExplosionScript::Update()
{
	if (GetKeyTrigger(VK_F))
	{
		if (!m_renderer.lock()) return;

		// 頂点取得
		auto model = m_renderer.lock()->GetModel();
		UINT vertexCount, indexCount;
		model->GetVertexCount(&vertexCount, &indexCount);
		TAssimpVertex* aVertex = new TAssimpVertex[vertexCount];
		UINT* aIndex = new UINT[indexCount];
		model->GetVertex(aVertex, aIndex);

		// 頂点データ作成
		std::vector<Vector3> vertexData;
		//int* indexData = new int[indexCount];
		std::vector<int> indexData;
		for (UINT i = 0; i < vertexCount; ++i) vertexData.push_back(aVertex[i].vPos);
		for (UINT i = 0; i < indexCount; ++i) indexData.push_back((int)aIndex[i]);
		delete[] aVertex; delete[] aIndex;

		// 破片の生成
		for (UINT i = 0; i < indexCount; i += 3)
		{
			if (indexData[i] >= vertexCount ||
				indexData[i + 1] >= vertexCount ||
				indexData[i + 2] >= vertexCount) continue;

			// 3頂点の平均
			Vector3 scale = transform().lock()->m_scale;
			Vector3 pos = vertexData[indexData[i]];
			pos += vertexData[indexData[i + 1]];
			pos += vertexData[indexData[i + 2]];
			pos /= 3;

			// 3頂点のデータ作成
			Vector3 ver[3] = { vertexData[indexData[i]] - pos,
								vertexData[indexData[i + 1]] - pos,
								vertexData[indexData[i + 2]] - pos };
			int index[3] = { 0,1,2 };

			// エンティティ作成
			auto obj = Instantiate<GameObject>(transform().lock()->m_pos + pos * scale,
				Vector3(), scale);
			m_aExplosionEffect.push_back(obj);

			// レンダラーの追加
			auto renderer = obj->AddComponent<MeshRenderer>();
			char szTag[256];
			sprintf_s(szTag, "explosion%d", i);
;			renderer->MakeAnyVertex(szTag, ver, index, 3, 3);

			// リジッドボディ
			auto rb = obj->AddComponent<Rigidbody>();
			rb->AddForce(Mathf::Normalize(pos) * 10);
			rb->SetTorqueDrag(0);
			rb->SetUseGravity(true);
			// 回転軸
			Vector3 axis = Mathf::Cross(ver[1] - ver[0], ver[2] - ver[0]);
			Quaternion q = Quaternion::CreateFromAxisAngle(axis, 0.3f);
			rb->AddTorque(q);
		}

		// レンダラー削除
		gameObject().lock()->RemoveComponent<AssimpRenderer>();
		m_time = 0;

	}


	// 時間
	m_time++;
	if (m_time == m_destroyTime)
	{
		int n = m_aExplosionEffect.size();
		for (int i = 0; i < n; ++i)
		{
			Destroy(m_aExplosionEffect[i].lock());
		}
		m_aExplosionEffect.clear();

		// レンダラー
		const auto& renderer = gameObject().lock()->AddComponent<AssimpRenderer>();
		renderer->ModelLoad("data/model/cub obj.obj");
	}

}

//========================================
//
//	後更新時
//
//========================================
void ExplosionScript::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void ExplosionScript::End()
{
}


//******************** コールバック関数 ********************


//========================================
//
// 当たった時
//
//========================================
void ExplosionScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// 当たっている間
//
//========================================
void ExplosionScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// 離れた時
//
//========================================
void ExplosionScript::OnCollisionExit(Collider* collider)
{

}

