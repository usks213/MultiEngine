/*****************************************************************//**
 * \file   OBBTestScript.h
 * \brief  OBBテストプログラム
 *
 * \author USAMI KOSHI
 * \date   2021/09/09
 *********************************************************************/


//====== インクルード部 ======

// 自身
#include "OBBTestScript.h"

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
void OBBTestScript::Start()
{
	// OBBコライダー
	gameObject().lock()->AddComponent<BoxCollider>();

	// レンダラー
	const auto& renderer = gameObject().lock()->AddComponent<MeshRenderer>();
	renderer->MakeCube("OBB");
	renderer->SetDiffuseColor(Vector4(1, 0, 0, 0.5f));
	renderer->SetEmissiveColor(Vector4(1, 0, 0, 0.5f));
	renderer->SetRendererBlendState(BS_ALPHABLEND);
	renderer->SetTransparent(true);

}

//========================================
//
//	更新時
//
//========================================
void OBBTestScript::Update()
{
}

//========================================
//
//	後更新時
//
//========================================
void OBBTestScript::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void OBBTestScript::End()
{
}


//******************** コールバック関数 ********************


//========================================
//
// 当たった時
//
//========================================
void OBBTestScript::OnCollisionEnter(Collider* collider)
{
	const auto& renderer = gameObject().lock()->GetComponent<MeshRenderer>();
	renderer->SetDiffuseColor(Vector4(0, 1, 0, 0.5f));
	renderer->SetEmissiveColor(Vector4(0, 1, 0, 0.5f));
}

//========================================
//
// 当たっている間
//
//========================================
void OBBTestScript::OnCollisionStay(Collider* collider)
{
	OnCollisionEnter(collider);
}

//========================================
//
// 離れた時
//
//========================================
void OBBTestScript::OnCollisionExit(Collider* collider)
{
	const auto& renderer = gameObject().lock()->GetComponent<MeshRenderer>();
	renderer->SetDiffuseColor(Vector4(1, 0, 0, 0.5f));
	renderer->SetEmissiveColor(Vector4(1, 0, 0, 0.5f));
}

