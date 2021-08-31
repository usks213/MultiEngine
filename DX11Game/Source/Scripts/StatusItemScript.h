/*****************************************************************//**
 * \file   StatusItemScript.h
 * \brief  ステータスアイテム
 * 
 * \author USAMI KOSHI
 * \date   2021/08/28
 *********************************************************************/

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	enum class ItemType
	{
		Physical,
		Attack,
		Speed,
		Skiil,
		Max,
	};

	class StatusItemScript : public Script
	{
	private:
		// ----- メンバ -----
		int m_nExitTime;	// 生存時間
		std::weak_ptr<Rigidbody> m_rb;
		// プレイヤー
		std::weak_ptr<GameObject> m_player;
		// 追尾フラグ
		bool m_IsTracking;
		// スケール
		float m_fScale;
		// タイプ
		ItemType m_eType;

	public:
		// プレイヤーのセット
		void SetPlayer(std::weak_ptr<GameObject> player) { m_player = player; }
		// タイプの指定
		void SetItemType(ItemType type);
		// アイテムタイプ
		ItemType getType() { return m_eType; }
	protected:
		// ----- メソッド -----

		// 開始時に呼ばれます
		void Start() override;
		// 毎フレーム呼ばれます
		void Update() override;
		// 毎フレーム更新後に呼ばれます
		void LateUpdate() override;
		// 終了時に呼ばれます
		void End() override;

		// ----- コールバック関数 -----

		// 当たった時
		void OnDeltaCollisionEnter(DeltaCollider* collider) override;
		// 当たっている間
		void OnDeltaCollisionStay(DeltaCollider* collider) override;
		// 離れた時
		void OnDeltaCollisionExit(DeltaCollider* collider) override;
	};
}
