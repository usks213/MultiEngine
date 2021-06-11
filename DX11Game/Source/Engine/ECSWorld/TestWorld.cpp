//==================================================================
//												TestWorld.h
//	�e�X�g���[���h
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/05/14	�e�X�g���[���h�N���X�쐬
//
//===================================================================


//===== �C���N���[�h�� =====
#include "TestWorld.h"

// �V�X�e��
#include "../ECS/Entity/EntityManager.h"
#include "../ECSEntity/GameObject.h"
#include "../ECSCompoent/Camera.h"
#include "../System/Sound.h"

// �R���|�[�l���g
#include "../ECSCompoent/Transform.h"
#include "../ECSCompoent/MeshRenderer.h"

// �X�N���v�g
#include "../../Scripts/PlayerScript.h"
#include "../../Scripts/MakeEnemyScript.h"
#include "../../Scripts/StartCrystalScript.h"
#include "../../Scripts/SkyDomeScript.h"
#include "../../Scripts/DeltaUIScript.h"
#include "../../Scripts/FieldScript.h"
#include "../../Scripts/ExplosionScript.h"
#include "../../Scripts/PlaneControlScript.h"


using namespace ECS;


//===== �}�N����` =====


//===== �v���g�^�C�v�錾 =====


//===== �O���[�o���ϐ� =====


//===================================
//
//	�R���X�g���N�^
//
//===================================
TestWorld::TestWorld(std::string name)
	:World(name)
{
}

//===================================
//
//	�f�X�g���N�^
//
//===================================
TestWorld::~TestWorld()
{
}

//===================================
//
//	���[���h�̏�����
//
//===================================
void TestWorld::Start()
{
	//--- �Q�[���I�u�W�F�N�g��ǉ�

	// �J����
	const auto& cameraObj = GetEntityManager()->CreateEntity<GameObject>();
	cameraObj->transform().lock()->m_pos = Vector3(0, 500, 300);
	const auto& camera = cameraObj->AddComponent<Camera>();
	camera->SetMainCamera(camera);

	// �v���C���[
	const auto& player = GetEntityManager()->CreateEntity<GameObject>();
	//const auto & playerScript = player->AddComponent<PlayerScript>();
	player->AddComponent<PlaneControlScript>();

	// �X�J�C�h�[��
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

	// ��
	const auto& plane = GetEntityManager()->CreateEntity<GameObject>();
	plane->AddComponent<FieldScript>()->SetTarget(cameraObj);
	
	//// �J�[�\��
	//const auto& cursor = GetEntityManager()->CreateEntity<GameObject>();
	//const auto& cursorRn = cursor->AddComponent<SpriteRenderer>();
	//cursorRn->SetDiffuseTexture("data/texture/Cursor.png");
	//cursorRn->SetLayer(cursorRn->eUI1);
	//cursor->transform().lock()->m_scale = Vector3{ 64 * SCREEN_SCALE_X,64 * SCREEN_SCALE_Y,32 };

	//// ����
	//const auto& ex = GetEntityManager()->CreateEntity<GameObject>();
	//const auto& exsc = ex->AddComponent<PlaneControlScript>();

	// �G�l�~�[
	const auto& makerobj = GetEntityManager()->CreateEntity<GameObject>();
	const auto& maker = makerobj->AddComponent<MakeEnemyScript>();
	maker->SetPlayer(player);

}

//===================================
//
//	���[���h�̏I��
//
//===================================
void TestWorld::End()
{
	// BGM
	CSound::StopBGM("GameBGM.mp3");

}
