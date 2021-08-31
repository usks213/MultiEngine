//==================================================================
//								EnemyBaseScript.h
//	エネミーベーススクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/09	エネミーベースクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"
#include "MakeEnemyScript.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class MakeEnemyScript;

	class EnemyBaseScript : public Script
	{
		friend MakeEnemyScript;
	protected:
		// ----- メンバ -----
		// HPUI
		std::weak_ptr<GameObject> m_HPUI;
		// リジッドボディ
		std::weak_ptr<Rigidbody> m_rb;

		// アクティブ
		bool m_bActive = true;

		// スケール
		float m_fScale;
		float m_fMaxScale;

		// イベントリスト
		std::list<std::function<void(void)>> m_eventList;
		// イベントフラグ
		bool m_isNext = false;

		// 徘徊カウント
		int m_nMoveCnt = 240;
		Vector3 m_dir;
	public:
		// タイプ
		enum class Type
		{
			Attack,		// 攻撃
			Speed,		// スピード
			Skiil,	// 防御
			Physical,	// 体力
			Balance,	// バランス
			Max,
		};
		Type m_type;

		// ステータス
		struct Status
		{
			int   Level;
			float HP;
			float MaxHP;
			float Damage;
			float AttackSpeed;
			float MoveSpeed;
		};
		Status m_status;

		// メイカー
		std::weak_ptr<MakeEnemyScript> m_maker;
		// ターゲット
		std::weak_ptr<GameObject> m_player;
		// ロックオン
		bool m_isLockon = false;
		// 攻撃カウント
		int m_nAttackCount = 0;

	public:
		// リジッドボディ取得
		std::weak_ptr<Rigidbody> GetRb() { return m_rb; }
		// プレイヤーのセット
		void SetPlayer(std::weak_ptr<GameObject> player) { m_player = player; }

		// イベント格納更新
		void EventPushBack(const std::function<void(void)>& event) { m_eventList.push_back(event); }
		void EventPushFront(const std::function<void(void)>& event) { m_eventList.push_front(event); }
		// レベルからステータスを取得(エネミータイプで違いがある)
		void SetUpStatus(Type type, float level);
		// サイズ
		void SetMaxScale(float scale) { m_fMaxScale = scale; }
	protected:
		// ----- メソッド -----

		// 開始時に呼ばれます
		void Start() override final;
		// 毎フレーム呼ばれます
		void Update() override final;
		// 毎フレーム更新後に呼ばれます
		void LateUpdate() override final;
		// 終了時に呼ばれます
		void End() override final;

		// ----- コールバック関数 -----

		// Delta
		// 当たった時
		virtual void OnDeltaCollisionEnter(DeltaCollider* collider) override final;
		// 当たっている間
		virtual void OnDeltaCollisionStay(DeltaCollider* collider) override final;
		// 離れた時
		virtual void OnDeltaCollisionExit(DeltaCollider* collider) override final;

		
		// ----- エネミー -----

		// 生成時更新
		void CreateUpdate();
		// 削除時更新
		void DestroyUpdate();
	};
}
