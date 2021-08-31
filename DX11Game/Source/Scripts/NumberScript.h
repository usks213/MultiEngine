/*****************************************************************//**
 * \file   NumberScript.h
 * \brief  数字スクリプト
 * 
 * \author USAMI KOSHI
 * \date   2021/08/29
 *********************************************************************/

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class NumberScript : public Script
	{
	private:
		// ----- メンバ -----

		// 数字
		struct NumberData
		{
			std::weak_ptr<SpriteRenderer> m_sprite;
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
