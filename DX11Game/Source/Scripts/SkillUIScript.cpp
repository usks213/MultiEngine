/*****************************************************************//**
 * \file   SkillUIScript.h
 * \brief  スキルUI
 *
 * \author USAMI KOSHI
 * \date   2021/08/29
 *********************************************************************/


//====== インクルード部 ======

// 自身
#include "SkillUIScript.h"

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
	const float skillSize = 64;
	const float fadeSize = skillSize;
	const float keySize = 32;
	const float numSize = 24;
}


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************

//========================================
//
//	開始時
//
//========================================
void SkillUIScript::Start()
{
	const int skillNum = m_aUIData.size();
	Vector3 pos(SCREEN_CENTER_X / 2, -SCREEN_CENTER_Y + SCREEN_CENTER_Y / 3, 0);
	const char* skillPath[4] = {
	"data/texture/icon/shot.png",
	"data/texture/icon/step.png",
	"data/texture/icon/burst.png",
	"data/texture/icon/bomb.png",
	};
	const char* keyPath[4] = {
		"data/texture/key/m1.png",
		"data/texture/key/m2.png",
		"data/texture/key/e.png",
		"data/texture/key/q.png",
	};

	// UIデータ
	for (int i = 0; i < m_aUIData.size(); ++i)
	{
		auto& data = m_aUIData[i];
		// スキル画像
		{
			const auto& sprite = Instantiate<GameObject>()->AddComponent<SpriteRenderer>();
			data.skillImage = sprite;
			sprite->transform().lock()->m_pos = pos;
			sprite->transform().lock()->m_scale = Vector3(skillSize, skillSize, 0);
			sprite->SetDiffuseTexture(skillPath[i]);
			sprite->SetLayer(SpriteRenderer::eUI1);
		}
		// スキルキー画像
		{
			const auto& sprite = Instantiate<GameObject>()->AddComponent<SpriteRenderer>();
			data.keyImage = sprite;
			sprite->transform().lock()->m_pos = pos - Vector3(0, skillSize / 2 + keySize / 3 * 2, 0);
			sprite->transform().lock()->m_scale = Vector3(keySize, keySize, 0);
			sprite->SetDiffuseTexture(keyPath[i]);
			sprite->SetLayer(SpriteRenderer::eUI1);
		}
		// スキルリキャストフェード
		{
			const auto& sprite = Instantiate<GameObject>()->AddComponent<SpriteRenderer>();
			data.recastFade = sprite;
			sprite->transform().lock()->m_pos = pos;
			sprite->transform().lock()->m_scale = Vector3(fadeSize, fadeSize, 0);
			sprite->SetDiffuseColor(0, 0, 0);
			sprite->SetAlpha(0.5f);
			sprite->SetLayer(SpriteRenderer::eUI2);
		}
		// スキルリキャストタイムの数字
		{
			const auto& number = Instantiate<GameObject>()->AddComponent<NumberScript>();
			data.recastTime = number;
			number->transform().lock()->m_pos = pos;
			number->transform().lock()->m_scale = Vector3(numSize, numSize, 0);
			number->SetNumber(99);
		}

		pos.x += skillSize + 6;
	}
}

//========================================
//
//	更新時
//
//========================================
void SkillUIScript::Update()
{
}

//========================================
//
//	後更新時
//
//========================================
void SkillUIScript::LateUpdate()
{
	for (int i = 0; i < static_cast<int>(PlayerSkill::Max); ++i)
	{
		// リキャストタイムの割合
		float skillPer = m_player.lock()->m_aSkillRecastCnt[i] / m_player.lock()->getSkiilRecast(i);
		// リキャストフェード
		m_aUIData[i].recastFade.lock()->transform().lock()->m_scale.y = skillPer * fadeSize;
		// リキャストタイム
		if(i == 0)
			m_aUIData[i].recastTime.lock()->SetNumber((m_player.lock()->m_aSkillRecastCnt[i]) / 60);
		else
			m_aUIData[i].recastTime.lock()->SetNumber((m_player.lock()->m_aSkillRecastCnt[i] + 55) / 60);
	}
}

//========================================
//
//	終了時
//
//========================================
void SkillUIScript::End()
{
}
