/*****************************************************************//**
 * \file   DamageNumberScript.h
 * \brief  ダメージ表記
 * 
 * \author USAMI KOSHI
 * \date   2021/08/29
 *********************************************************************/

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"
#include "../Engine/ECSCompoent/BillboardRenderer.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class DamageNumberScript : public Script
	{
	private:
		// ----- メンバ -----

		// リジッドボディ
		std::weak_ptr<Rigidbody> m_rb;
		// 生存時間
		int m_nExistTime = 0;

		// 数字
		struct NumberData
		{
			std::weak_ptr<BillboardRenderer> m_sprite;
			int m_nAnimNo;
		};
		std::vector<NumberData> m_data;

		// 数
		int m_nNumber = 0;
		// 桁数
		int m_nDigits = 0;
		// 数字間隔
		int m_nNumSpace = 0;

	public:
		// 数字指定
		void SetNumber(int num) { m_nNumber = num; }
		// 数字間隔指定
		void SetNumScape(int space) { m_nNumSpace = space; }

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
