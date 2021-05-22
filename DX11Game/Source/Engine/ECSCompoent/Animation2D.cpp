//==================================================================
//									Animation2D.cpp
//	アニメーションクリップ
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/02	アニメーションクラス作成
//
//===================================================================

//====== インクルード部 ======
#include "Animation2D.h"

using namespace ECS;


//===== マクロ定義 =====


//===== クラス定義 =====



//===== プロトタイプ宣言 =====




//========================================
//
//	コンストラクタ
//
//========================================
Animation2D::Animation2D()
{
	m_nArraySize = 0;
	m_pAnimNoArray = nullptr;
	m_name = "\0";
}


//========================================
//
//	デストラクタ
//
//========================================
Animation2D::~Animation2D()
{
	if (m_pAnimNoArray != nullptr)
	{
		delete[] this->m_pAnimNoArray;
		m_pAnimNoArray = nullptr;
	}
}
