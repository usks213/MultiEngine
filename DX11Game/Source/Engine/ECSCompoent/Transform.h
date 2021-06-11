//==================================================================
//								Transform.h
//	トランスフォーム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/07/22	トランスフォームクラスの作成
//	2020/09/23	コンポーネント化
//	2020/12/23	ECS用に再設計
//	2021/05/25	回転をクォータニオン化
//
//===================================================================

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

	class Transform final : public IComponent
	{
	public:
		// コンストラクタ
		Transform();
		// デストラクタ
		~Transform();

		// オブジェクト生成時
		void OnCreate() override;
		// オブジェクト破棄時
		void OnDestroy() override;

		// メッセージ受信
		void SendComponentMessage(const std::string& message, void* value) override;

		// マトリックスの更新
		void UpdateMatrix();

		// ゲット関数
		Matrix* GetWorldMatrix() { return &m_mtxWorld; }
		// セット関数
		void SetWorldMatrix(const Matrix& world) { m_mtxWorld = world; }

		// ステータス
		Vector3 m_pos;				// 中心座標
		Vector3 m_scale;			// 縦横サイズ
		//Vector3 m_rot;				// オブジェクトの回転角度
		Quaternion m_rot;

		void right(Vector3 right) {
			Matrix mtx = Matrix::CreateFromQuaternion(m_rot);
			mtx.Right(right);
			m_rot = Quaternion::CreateFromRotationMatrix(mtx);
		}
		Vector3 right() {
			return Matrix::CreateFromQuaternion(m_rot).Right();
		}

		void up(Vector3 up) {
			Matrix mtx = Matrix::CreateFromQuaternion(m_rot);
			mtx.Up(up);
			m_rot = Quaternion::CreateFromRotationMatrix(mtx);
		}
		Vector3 up() {
			return Matrix::CreateFromQuaternion(m_rot).Up();
		}

		void forward(Vector3 forward) {
			Matrix mtx = Matrix::CreateFromQuaternion(m_rot);
			mtx.Forward(forward);
			m_rot = Quaternion::CreateFromRotationMatrix(mtx);
		}
		Vector3 forward() {
			return Matrix::CreateFromQuaternion(m_rot).Forward();
		}

		// ゲームオブジェクトセット
		void SetGameObject(std::weak_ptr<GameObject>& gameObject) { m_gameObject = gameObject; }
		// ゲームオブジェクトの取得
		const std::weak_ptr<GameObject>& gameObject()			  { return m_gameObject; }

	private:
		// マトリックス
		Matrix m_mtxWorld;

		// 親のゲームオブジェクト
		std::weak_ptr<GameObject> m_gameObject;
	};
}
