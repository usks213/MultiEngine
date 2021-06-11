/*****************************************************************//**
 * \file   Camera.cpp
 * \brief  カメラコンポーネント
 *
 * \author USAMI KOSHI
 * \date   2021/06/10
 *********************************************************************/


//====== インクルード部 ======
#include "Camera.h"
#include "../main.h"
#include "../ECSSystem/CameraSystem.h"

#include "../ECS/Entity/EntityManager.h"
#include "../ECSCompoent/Transform.h"
#include "../ECSEntity/GameObject.h"

using namespace ECS;
using namespace DirectX;


//===== マクロ定義 =====
#define	VIEW_ANGLE			(80.0f)								// ビュー平面の視野角
#define	VIEW_ASPECT			((float)SCREEN_WIDTH/SCREEN_HEIGHT)	// ビュー平面のアスペクト比
#define	VIEW_NEAR_Z			(50.0f)								// ビュー平面のNearZ値
#define	VIEW_FAR_Z			(5000.0f * 1.5f)					// ビュー平面のFarZ値
#define FOG_NEAR_Z	(VIEW_FAR_Z * 0.3f)
#define FOG_FAR_Z	(VIEW_FAR_Z * 0.6f)


//===== グローバル変数 =====
std::weak_ptr<Camera> Camera::m_mainCamera;


//========================================
//
//	コンストラクタ
//
//========================================
Camera::Camera()
{
	m_fAspectRatio = VIEW_ASPECT;			// 縦横比
	m_fFovY = VIEW_ANGLE;					// 視野角(Degree)
	m_fNearZ = VIEW_NEAR_Z;					// 前方クリップ距離
	m_fFarZ = VIEW_FAR_Z;					// 後方クリップ距離
	m_fFogNearZ = FOG_NEAR_Z;
	m_fFogFarZ = FOG_FAR_Z;
	m_up = Vector3(0, 1, 0);
}

//========================================
//
//	デストラクタ
//
//========================================
Camera::~Camera()
{

}

//========================================
//
//	生成時
//
//========================================
void Camera::OnCreate()
{
	// システムに格納
	CameraSystem* sys = GetEntityManager()->GetWorld()->GetSystem<CameraSystem>();
	if (sys) sys->AddList(this);

	// トランスフォーム取得
	m_transform = m_Parent.lock()->GetComponent<Transform>();
	// ゲームオブジェクト取得
	m_gameObject = m_transform.lock()->gameObject();
	// 名前・タグ
	m_gameObject.lock()->SetName("Camera");
	m_gameObject.lock()->SetTag("Camera");

	// マトリックス更新
	UpdateCameraMatrix();
}

//========================================
//
//	破棄時
//
//========================================
void Camera::OnDestroy()
{
	// システムから除外
	CameraSystem* sys = GetEntityManager()->GetWorld()->GetSystem<CameraSystem>();
	if (sys) sys->RemoveList(this);
}

//========================================
//
//	メッセージ受信
//
//========================================
void Camera::SendComponentMessage(const std::string& message, void* value)
{
	// Updateメッセージでマトリックスを更新
	if ("UpdateCameraMatrix" == message)
	{
		UpdateCameraMatrix();
	}
}

//========================================
//
//	更新
//
//========================================
void Camera::UpdateCameraMatrix()
{
	const auto& trans = transform().lock();
	if (!trans) return;

	const Vector3& pos = trans->m_pos;
	const Vector3& sca = trans->m_scale;
	const Quaternion& rot = trans->m_rot;

	Vector3 target = pos + Matrix::CreateFromQuaternion(rot).Forward();

	Vector3 up = Mathf::Normalize(Matrix::CreateFromQuaternion(rot).Up());
	//Vector3 up = Mathf::Normalize(Mathf::Cross(
	//	Matrix::CreateFromQuaternion(rot).Right(),
	//	Matrix::CreateFromQuaternion(rot).Forward()));
	//Vector3 up = Vector3(0, 1, 0);

	XMStoreFloat4x4(&m_mtxView, XMMatrixLookAtLH(
		XMLoadFloat3(&pos), XMLoadFloat3(&target), XMLoadFloat3(&up)));

	XMStoreFloat4x4(&m_mtxProj, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(m_fFovY), m_fAspectRatio, m_fNearZ, m_fFarZ));
}