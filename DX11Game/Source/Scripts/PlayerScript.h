//==================================================================
//								PlayerScript.h
//	プレイヤースクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/27	プレイヤースクリプトクラス作成(Test)
//	2021/01/09	デルタカウンターの追加
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"
#include "StatusItemScript.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// プレイヤースキル
	enum class PlayerSkill {
		Shot,	// 通常
		Step,	// 回避
		Burst,	// 連射
		Bom,	// 爆発
		Max,
	};

	class PlayerScript : public Script
	{
	private:
		// ----- メンバ -----
		std::weak_ptr<Rigidbody> m_rb;
		std::weak_ptr<AssimpRenderer> m_assimp;
		float m_speed;
		float m_rotSpeed;
		int m_jumpCount = 0;
		int m_attackCount = 0;
		int m_kickCount = 0;

		// アクティブ
		bool m_bActive = true;
		// ジャンプフラグ
		int m_nJump = 0;
		// ショット
		bool m_bShot = false;
		// 地面
		bool m_bGround = false;

		// カウンタ
		int m_nStepInvCount = 0;
		int m_nDamageInvCount = 0;
		int m_nBurstCount = 0;
		int m_nHeelCount = 0;

	public:
		// HP
		float m_HP = 100;
		// ステータスアイテム
		int m_aItemCount[static_cast<std::size_t>(ItemType::Max)];
		// スキル
		int m_aSkillRecastCnt[static_cast<std::size_t>(PlayerSkill::Max)];

		//--- ステータス ---

		float getMaxHP() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Physical)];
			return 200 + cnt * 10;
		}
		float getHeelTime() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Physical)];
			float heel = 300 - cnt * 0.1f;
			if (heel < 0) heel = 0;
			return heel + 60;
		}

		float getDamage() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Attack)];
			return 10 + cnt * 5.0f;
		}
		float getAttackSpeed() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Attack)];
			float speed = 1.2f + cnt * 0.1f;
			if (speed < 0) speed = 0;
			return speed;
		}

		float getMoveSpeed() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Speed)];
			return 1.2f + cnt * 0.2f;
		}
		float getJumpForce() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Speed)];
			return 15 + cnt * 0.1f;
		}
		float getStepTime() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Speed)];
			return 10 + cnt * 0.05f;
		}

		float getSkiilRecast(int skillIndex) {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Skiil)];
			float recast = 180 - cnt;
			if (recast < 0) recast = 0;

			int skillTime = 0;
			switch (skillIndex)
			{
			case (int)ECS::PlayerSkill::Shot:
				return getAttackSpeed();
				break;
			case (int)ECS::PlayerSkill::Step:
				skillTime = 120;
				break;
			case (int)ECS::PlayerSkill::Burst:
				skillTime = 60;
				break;
			case (int)ECS::PlayerSkill::Bom:
				skillTime = 240;
				break;
			}
			return recast + skillTime;
		}

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
