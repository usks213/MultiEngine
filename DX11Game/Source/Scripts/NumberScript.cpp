/*****************************************************************//**
 * \file   NumberScript.h
 * \brief  数字スクリプト
 *
 * \author USAMI KOSHI
 * \date   2021/08/29
 *********************************************************************/


//====== インクルード部 ======

// 自身
#include "NumberScript.h"

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
#define ANIM_SPLIT_X (10)  // 画像の分割数(横)
#define ANIM_SPLIT_Y (1)  // 画像の分割数(縦)
#define MAX_DIGITS (6)

//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void NumberScript::Start()
{
	{
		// トランスフォーム
		transform().lock()->m_pos = Vector3{ 0, 0, 0 };
		transform().lock()->m_scale = Vector3(100, 100, 1);
	}

	// 数字データの生成
	NumberData data;
	for (int i = 0; i < MAX_DIGITS; i++)
	{
		Vector3 scale = transform().lock()->m_scale;
		Vector3 pos = transform().lock()->m_pos;
		pos.x -= (MAX_DIGITS / 2 - i) * (scale.x + m_nNumSpace);

		const auto& obj = Instantiate<GameObject>(pos);
		// トランスフォーム
		obj->transform().lock()->m_scale = scale;
		// レンダラー
		const auto& renderer = obj->AddComponent<SpriteRenderer>();
		renderer->SetDiffuseTexture("data/texture/DeltaNumber.png");
		renderer->SetLayer(renderer->eUI4);
		renderer->SetTexSize(Vector3{ 1.0f / (float)ANIM_SPLIT_X, 1.0f / (float)ANIM_SPLIT_Y , 0 });
		// 格納
		data.m_sprite = renderer;
		data.m_nAnimNo = 0;
		m_data.push_back(data);
	}
}

//========================================
//
//	更新時
//
//========================================
void NumberScript::Update()
{
}

//========================================
//
//	後更新時
//
//========================================
void NumberScript::LateUpdate()
{
	// 数宇の更新
	UpdateTexCoord();

	// 真ん中寄せ
	for (int i = 0; i < m_nDigits; i++)
	{
		Vector3 scale = transform().lock()->m_scale;
		Vector3 pos = transform().lock()->m_pos;
		pos.x += -(scale.x + m_nNumSpace) * ((m_nDigits - 1) - i) +
			(scale.x + m_nNumSpace) * ((m_nDigits - 1) / 2.0f);
		const auto& sprite = m_data[(MAX_DIGITS - m_nDigits) + i].m_sprite.lock();
		if (sprite)
		{
			sprite->transform().lock()->m_pos = pos;
			sprite->transform().lock()->m_scale = scale;
		}
	}
}

//========================================
//
//	終了時
//
//========================================
void NumberScript::End()
{
}

//========================================
//
//	UV更新
//
//========================================
void NumberScript::UpdateTexCoord()
{
	int tmp = m_nNumber;
	int nCnt = 0;
	// 桁数を数える
	while (tmp > 0)
	{
		tmp /= 10;
		nCnt++;
	}
	m_nDigits = nCnt;

	// UV更新
	tmp = m_nNumber;
	for (int i = m_data.size() - 1; 0 <= i; i--)
	{
		const auto& sprite = m_data[i].m_sprite.lock();
		if (!sprite) continue;

		m_data[i].m_nAnimNo = tmp % 10;

		sprite->SetTexPos(
			Vector3{ sprite->GetTexSize().x * (m_data[i].m_nAnimNo % ANIM_SPLIT_X) ,
					 sprite->GetTexSize().y * (m_data[i].m_nAnimNo / ANIM_SPLIT_X),
					 0 });

		if (m_data[i].m_nAnimNo == 0 && (m_data.size() - i) > nCnt)
		{
			sprite->SetAlpha(0);
		}
		else
		{
			sprite->SetAlpha(1);
		}

		tmp /= 10;
	}
}