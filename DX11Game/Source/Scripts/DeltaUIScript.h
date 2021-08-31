//==================================================================
//								DeltaUIScript.h
//	Δ数表示UI
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/15	Δ表示UIクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"
#include "MakeEnemyScript.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class DeltaUIScript;

	class DeltaUIData :public Script
	{
		friend DeltaUIScript;
	private:
		// ----- メンバ -----
		std::weak_ptr<SpriteRenderer> m_sprite;
		int m_nAnimNo;

	public:
		// ----- メソッド -----

		// 開始時に呼ばれます
		void Start() override;

	};

	class DeltaUIScript : public Script
	{
	private:
		// ----- メンバ -----

		// プレイヤー
		std::weak_ptr<MakeEnemyScript> m_maker;

		// 数字
		std::vector<std::weak_ptr<DeltaUIData>> m_data;

	public:
		void SetMaker(std::weak_ptr<MakeEnemyScript> maker) { m_maker = maker; }

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

		// UV更新
		void UpdateTexCoord();
	};
}
