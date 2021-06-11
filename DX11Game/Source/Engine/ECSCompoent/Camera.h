/*****************************************************************//**
 * \file   Camera.h
 * \brief  カメラ
 * 
 * \author USAMI KOSHI
 * \date   2021/06/10
 *********************************************************************/

#pragma once

//====== インクルード部 ======
#include "../System/Math.h"
#include "../ECS/Component/IComponent.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class GameObject;
	class Transform;

	class Camera final : public IComponent
	{
	public:
		// コンストラクタ
		Camera();
		// デストラクタ
		~Camera();

		// オブジェクト生成時
		void OnCreate() override;
		// オブジェクト破棄時
		void OnDestroy() override;

		// メッセージ受信
		void SendComponentMessage(const std::string& message, void* value) override;

		// マトリックスの更新
		inline void UpdateCameraMatrix();

		// ゲームオブジェクトセット
		void SetGameObject(std::weak_ptr<GameObject>& gameObject) { m_gameObject = gameObject; }
		// ゲームオブジェクトの取得
		const std::weak_ptr<GameObject>& gameObject()			  { return m_gameObject; }

		// トランスフォーム取得
		const std::weak_ptr<Transform>& transform() { return m_transform; }

		// ゲット
		Matrix& GetViewMatrix() { return m_mtxView; }
		Matrix& GetProjMatrix() { return m_mtxProj; }
		float GetApsect() { return m_fAspectRatio; }
		float GetFOV() { return m_fFovY; }
		float GetNearZ() { return m_fNearZ; }
		float GetFarZ() { return m_fFarZ; }
		float GetFogNearZ() { return m_fFogNearZ; }
		float GetFogFarZ() { return m_fFogFarZ; }

	public:
		static void SetMainCamera(const std::shared_ptr<Camera>& camera) 
		{ m_mainCamera = camera; }
		static Camera* GetMainCamera() { 
			if (m_mainCamera.lock())
			{
				return m_mainCamera.lock().get();
			}
			return nullptr;
		}
		static std::shared_ptr<Camera> main() { return m_mainCamera.lock(); }

	private:
		// 親のゲームオブジェクト
		std::weak_ptr<GameObject> m_gameObject;
		// トランスフォーム
		std::weak_ptr<Transform> m_transform;

		// ビューマトリックス
		Matrix m_mtxView;
		// プロジェクションマトリックス
		Matrix m_mtxProj;

		Vector3 m_up;					// UPベクトル
		float m_fAspectRatio;			// 縦横比
		float m_fFovY;					// 視野角(Degree)
		float m_fNearZ;					// 前方クリップ距離
		float m_fFarZ;					// 後方クリップ距離
		float m_fFogNearZ;				// フォグのかかり始める距離
		float m_fFogFarZ;				// フォグのかかり終わる距離


		static std::weak_ptr<Camera> m_mainCamera;
	};
}
