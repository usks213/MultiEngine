//==================================================================
//								Renderer.h
//	レンダラー
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/23	ECS用に再設計
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../System/Math.h"
#include "../ECS/Component/IComponent.h"
#include "../main.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class Transform;
	class GameObject;

	class Renderer : public IComponent
	{
	public:
		Renderer();
		virtual ~Renderer();

		// オブジェクト生成時
		virtual void OnCreate() override;
		// オブジェクト破棄時
		virtual void OnDestroy() override;

		// 各レンダラーの描画処理
		virtual void EarlyDraw(ID3D11DeviceContext* pDC) = 0;
		virtual void LateDraw(ID3D11DeviceContext* pDC) = 0;

		// カメラからの距離を格納
		virtual void LayerUpdate();

		// ソート用
		static bool swapR(Renderer* lhs, Renderer* rhs) {
			return lhs->m_fLayer > rhs->m_fLayer; // 降順にしてみる
		}

		// テクスチャマトリックスの更新
		void UpdateTexMatrix();

		// UV
		Vector3 GetTexPos() { return m_texPos; }
		Vector3 GetTexSize() { return m_texSize; }
		void SetTexPos(Vector3 pos) { m_texPos = pos; }
		void SetTexSize(Vector3 size) { m_texSize = size; }

		// ブレンドステート
		void SetRendererBlendState(EBlendState eBlendState) { m_eState = eBlendState; }
		EBlendState GetRendererBlendState() { return m_eState; }

		// レイヤーの取得
		float GetLayer() { return m_fLayer; }
		void SetLayer(float fLayer) { m_fLayer = fLayer; }
		void SetUpdateLayer(bool bUpdate) { m_bUpdateLayer = bUpdate; }

		// ゲームオブジェクト取得
		const std::weak_ptr<GameObject>& gameObject() { return m_gameObject; }
		// トランスフォーム取得
		const std::weak_ptr<Transform>& transform() { return m_transform; }
	protected:
		// トランスフォーム
		std::weak_ptr<Transform> m_transform;
		// ゲームオブジェクト
		std::weak_ptr<GameObject> m_gameObject;

		XMFLOAT4X4 m_mtxTexture;				// テクスチャ マトリックス

		Vector3 m_texPos;
		Vector3 m_texSize;

		// ブレンドステート
		EBlendState m_eState;

		float m_fLayer;
		bool m_bUpdateLayer = true;
	};
}
