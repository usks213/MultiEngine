//==================================================================
//												RendererSystem.h
//	レンダラー管理システム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/23	レンダラーシステム作成
//
//===================================================================


//===== インクルード部 =====
#include "RendererSystem.h"
#include "../ECS/Entity/EntityManager.h"
#include "../ECSCompoent/Renderer.h"
#include <algorithm>
#include <execution>

#include "../main.h"


using namespace ECS;


//===== マクロ定義 =====


//===== プロトタイプ宣言 =====


//===== グローバル変数 =====



//===================================
//
//	コンストラクタ
//
//===================================
RendererSystem::RendererSystem(World* pWorld)
	: System<Renderer>(pWorld)
{
	// 更新順
	SetUpdateOrder(UpdateOrder::eRenderer);
}


//===================================
//
//	デストラクタ
//
//===================================
RendererSystem::~RendererSystem()
{
}


//===================================
//
//	生成時コールバック
//
//===================================
void RendererSystem::OnCreate()
{
}

//===================================
//
//	描画時コールバック
//
//===================================
void RendererSystem::OnDraw()
{
	// デバイスコンテキスト
	ID3D11DeviceContext* pDC = GetDeviceContext();
	std::vector<Renderer*> opequeList;
	std::vector<Renderer*> transparentList;
	opequeList.reserve(m_ComponentList.size());
	transparentList.reserve(m_ComponentList.size() / 2);

	// レイヤー更新
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[&opequeList, &transparentList](Renderer* trans)
		{
			trans->LayerUpdate();
			if (trans->GetTransparent())
			{
				transparentList.push_back(trans);
			}
			else
			{
				opequeList.push_back(trans);
			}
		});

	// ソート
	std::sort(transparentList.begin(), transparentList.end(), Renderer::swapR);
	std::sort(opequeList.begin(), opequeList.end(), Renderer::swapL);
	//m_ComponentList.sort(Renderer::swapR);

	// 前描画
	//std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
	//	[&pDC](Renderer* trans)
	//	{
	//		trans->EarlyDraw(pDC);
	//	});

	// 不透明
	std::for_each(opequeList.begin(), opequeList.end(),
		[&pDC](Renderer* trans)
		{
			trans->LateDraw(pDC);
		});
	// 半透明
	std::for_each(transparentList.begin(), transparentList.end(),
		[&pDC](Renderer* trans)
		{
			trans->LateDraw(pDC);
		});
}

//===================================
//
//	削除時コールバック
//
//===================================
void RendererSystem::OnDestroy()
{
}

//===================================
//
//	ソート用
//
//===================================
bool RendererSystem::swapR(Renderer* lhs, Renderer* rhs) {
	return lhs->GetLayer() > rhs->GetLayer(); // 降順にしてみる
}