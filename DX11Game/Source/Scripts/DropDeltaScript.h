//==================================================================
//								DropDeltaScript.h
//	敵からドロップするアイテム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/09	ドロップデルタスクリプトクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class DropDeltaScript : public Script
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
	public:
		// プレイヤーのセット
		void SetPlayer(std::weak_ptr<GameObject> player) { m_player = player; }

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
