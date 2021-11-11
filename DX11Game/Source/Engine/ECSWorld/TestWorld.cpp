//==================================================================
//												TestWorld.h
//	テストワールド
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/05/14	テストワールドクラス作成
//
//===================================================================


//===== インクルード部 =====
#include "TestWorld.h"

// システム
#include "../ECS/Entity/EntityManager.h"
#include "../ECSEntity/GameObject.h"
#include "../ECSCompoent/Camera.h"
#include "../System/Sound.h"

// コンポーネント
#include "../ECSCompoent/Transform.h"
#include "../ECSCompoent/MeshRenderer.h"

// スクリプト
#include "../../Scripts/PlayerScript.h"
#include "../../Scripts/MakeEnemyScript.h"
#include "../../Scripts/StartCrystalScript.h"
#include "../../Scripts/SkyDomeScript.h"
#include "../../Scripts/DeltaUIScript.h"
#include "../../Scripts/FieldScript.h"
#include "../../Scripts/ExplosionScript.h"
#include "../../Scripts/PlaneControlScript.h"
#include "../../Scripts/OBBTestScript.h"
#include "../../Scripts/CameraControlScript.h"
#include "../../Scripts/TestMoveScript.h"
#include "../../Scripts/AnimPlayerScript.h"


using namespace ECS;


//===== マクロ定義 =====


//===== プロトタイプ宣言 =====


//===== グローバル変数 =====


//===================================
//
//	コンストラクタ
//
//===================================
TestWorld::TestWorld(std::string name)
	:World(name)
{
}

//===================================
//
//	デストラクタ
//
//===================================
TestWorld::~TestWorld()
{
}

//===================================
//
//	ワールドの初期化
//
//===================================
void TestWorld::Start()
{
	//--- ゲームオブジェクトを追加

	// カメラ
	const auto& cameraObj = GetEntityManager()->CreateEntity<GameObject>();
	cameraObj->transform().lock()->m_pos = Vector3(0, 300, 500);
	const auto& camera = cameraObj->AddComponent<Camera>();
	camera->SetMainCamera(camera);
	//cameraObj->AddComponent<CameraControlScript>();

	// プレイヤー
	const auto& player = GetEntityManager()->CreateEntity<GameObject>();
	player->AddComponent<AnimPlayerScript>();
	player->AddComponent<OBBTestScript>();

	//const auto& obb = GetEntityManager()->CreateEntity<GameObject>();
	//obb->transform().lock()->m_pos = Vector3(-200, 300, 0);
	//obb->transform().lock()->m_scale = Vector3(200, 200, 200);
	//obb->AddComponent<OBBTestScript>();

	//const auto& obb2 = GetEntityManager()->CreateEntity<GameObject>();
	//obb2->transform().lock()->m_pos = Vector3(200, 300, 0);
	//obb2->transform().lock()->m_scale = Vector3(200, 200, 200);
	//obb2->AddComponent<OBBTestScript>();
	////obb2->AddComponent<TestMoveScript>();

		// スタートクリスタル
	const auto& crystal = GetEntityManager()->CreateEntity<GameObject>();
	Vector3 dir = camera->GetViewMatrix().Right();
	dir *= 1000;
	dir.y = 100;
	crystal->transform().lock()->m_pos = dir;
	crystal->AddComponent<StartCrystalScript>()->SetPlayer(player);

	//// デルタUI
	//const auto& deltaUi = GetEntityManager()->CreateEntity<GameObject>();
	//deltaUi->AddComponent<DeltaUIScript>()->SetPlayer(playerScript);

	// スカイドーム
	const auto& sky = GetEntityManager()->CreateEntity<GameObject>();
	const auto& skyRn = sky->AddComponent<MeshRenderer>();
	skyRn->MakeSkyDome("Sky", 100);
	skyRn->SetDiffuseTexture("data/texture/skydome.png");
	skyRn->SetLighting(false);
	skyRn->SetLayer(camera->GetFarZ());
	skyRn->SetUpdateLayer(false);
	//skyRn->SetRendererBlendState(BS_ALPHABLEND);
	//sky->transform().lock()->m_scale = Vector3{ VIEW_FAR_Z * 1.5f, VIEW_FAR_Z * 1.5f, VIEW_FAR_Z * 1.5f };
	sky->transform().lock()->m_scale = 
		Vector3{ camera->GetFogFarZ() * 2.5f, camera->GetFogFarZ() * 2.5f, camera->GetFogFarZ() * 2.5f };
	sky->AddComponent<SkyDomeScript>()->SetTarget(cameraObj);

	// 床
	const auto& plane = GetEntityManager()->CreateEntity<GameObject>();
	plane->AddComponent<FieldScript>()->SetTarget(cameraObj);
	
	//// カーソル
	//const auto& cursor = GetEntityManager()->CreateEntity<GameObject>();
	//const auto& cursorRn = cursor->AddComponent<SpriteRenderer>();
	//cursorRn->SetDiffuseTexture("data/texture/Cursor.png");
	//cursorRn->SetLayer(cursorRn->eUI1);
	//cursor->transform().lock()->m_pos.y = -64 * SCREEN_SCALE_Y * 0.5f;
	//cursor->transform().lock()->m_scale = Vector3{ 64 * SCREEN_SCALE_X,64 * SCREEN_SCALE_Y,32 };

	//// 爆発
	//const auto& ex = GetEntityManager()->CreateEntity<GameObject>();
	//const auto& exsc = ex->AddComponent<PlaneControlScript>();

	//// エネミー
	//const auto& makerobj = GetEntityManager()->CreateEntity<GameObject>();
	//const auto& maker = makerobj->AddComponent<MakeEnemyScript>();
	//maker->SetPlayer(player);

	//CSound::PlayBGM("GameBGM.mp3", 0.3f);
}

//===================================
//
//	ワールドの終了
//
//===================================
void TestWorld::End()
{
	// BGM
	CSound::StopBGM("GameBGM.mp3");

}
