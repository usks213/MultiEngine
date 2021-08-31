/*****************************************************************//**
 * \file   PlayerHPUIScript.h
 * \brief  プレイヤーHPUI
 *
 * \author USAMI KOSHI
 * \date   2021/08/30
 *********************************************************************/


//====== インクルード部 ======

// 自身
#include "PlayerHPUIScript.h"

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

namespace {
	const float bgSizeX = 360;
	const float bgSizeY = 40;
	const float gaugeSizeX = bgSizeX - 4;
	const float gaugeSizeY = bgSizeY - 4;
	const float numSize = 24;
	const Vector3 initPos(-SCREEN_CENTER_X / 3 * 2, -SCREEN_CENTER_Y + SCREEN_CENTER_Y / 11 * 3, 0);

}


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************

//========================================
//
//	開始時
//
//========================================
void PlayerHPUIScript::Start()
{
	Vector3 pos(initPos);

	// UIデータ

	// 背景画像
	{
		const auto& sprite = Instantiate<GameObject>()->AddComponent<SpriteRenderer>();
		m_bgImage = sprite;
		sprite->transform().lock()->m_pos = pos;
		sprite->transform().lock()->m_scale = Vector3(bgSizeX, bgSizeY, 0);
		sprite->SetDiffuseColor(0, 0, 0);
		sprite->SetAlpha(0.5f);
		sprite->SetLayer(SpriteRenderer::eUI1);
	}
	// ゲージ
	{
		const auto& sprite = Instantiate<GameObject>()->AddComponent<SpriteRenderer>();
		m_gaugeImage = sprite;
		sprite->transform().lock()->m_pos = pos;
		sprite->transform().lock()->m_scale = Vector3(gaugeSizeX, gaugeSizeY, 0);
		sprite->SetDiffuseColor(0, 1, 0);
		sprite->SetLayer(SpriteRenderer::eUI2);
	}
	// スラッシュ
	{
		const auto& sprite = Instantiate<GameObject>()->AddComponent<SpriteRenderer>();
		m_slashImage = sprite;
		sprite->transform().lock()->m_pos = pos;
		sprite->transform().lock()->m_scale = Vector3(numSize, numSize, 0);
		sprite->SetDiffuseTexture("data/texture/slash.png");
		sprite->SetLayer(SpriteRenderer::eUI3);
	}
	// HP表記
	{
		const auto& number = Instantiate<GameObject>()->AddComponent<NumberScript>();
		m_hpNum = number;
		number->transform().lock()->m_pos = pos - Vector3(numSize * 2.5f, -2, 0);
		number->transform().lock()->m_scale = Vector3(numSize, numSize, 0);
		number->SetNumber(99);
		number->SetNumScape(-4);
	}
	// 最大HP表記
	{
		const auto& number = Instantiate<GameObject>()->AddComponent<NumberScript>();
		m_maxHpNum = number;
		number->transform().lock()->m_pos = pos + Vector3(numSize * 2.5f, 2, 0);
		number->transform().lock()->m_scale = Vector3(numSize, numSize, 0);
		number->SetNumber(99);
		number->SetNumScape(-4);
	}
}
//========================================
//
//	更新時
//
//========================================
void PlayerHPUIScript::Update()
{
}

//========================================
//
//	後更新時
//
//========================================
void PlayerHPUIScript::LateUpdate()
{
	// HP割合
	float HpPer = m_player.lock()->m_HP / m_player.lock()->getMaxHP();
	// ゲージ表記
	m_gaugeImage.lock()->transform().lock()->m_pos.x = initPos.x - (1.0f - HpPer) * gaugeSizeX / 2;
	m_gaugeImage.lock()->transform().lock()->m_scale.x = HpPer * gaugeSizeX;
	// HP数字
	m_hpNum.lock()->SetNumber(m_player.lock()->m_HP);
	m_maxHpNum.lock()->SetNumber(m_player.lock()->getMaxHP());
}

//========================================
//
//	終了時
//
//========================================
void PlayerHPUIScript::End()
{
}
