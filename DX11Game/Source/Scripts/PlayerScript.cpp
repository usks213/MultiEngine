//==================================================================
//								PlayerScript.cpp
//	�v���C���[�X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�v���C���[�X�N���v�g�N���X�쐬(Test)
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "PlayerScript.h"

// �V�X�e��
#include "../Engine/System/input.h"
#include "../Engine/System/Sound.h"
#include "../Engine/System/debugproc.h"

// �}�l�[�W���[
#include "../Engine/ECS/Entity/EntityManager.h"

// �G���e�B�e�B
#include "../Engine/ECSEntity/GameObject.h"

// �R���|�[�l���g
#include "../Engine/ECSCompoent/Transform.h"
#include "../Engine/ECSCompoent/MeshRenderer.h"
#include "../Engine/ECSCompoent/AssimpRenderer.h"
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"
#include "../Engine/ECSCompoent/DeltaCollider.h"
#include "../Engine/ECSCompoent/InstancingMeshRenderer.h"

// �����_���[
#include "../Engine/ECSCompoent/Camera.h"
#include "../Engine/Renderer/Light.h"
#include "../Engine/Renderer/PostProcessing.h"

#include "../Engine/ECSSystem/DeltaCollisionSystem.h"

// ���[���h
#include "../Engine/ECS/World/WorldManager.h"
#include "../Engine/ECSWorld/TitleWorld.h"

// �X�N���v�g
#include "BulletScript.h"
#include "EnemyBaseScript.h"
#include "GameOverScript.h"
#include "BombCrystalScript.h"
#include "BombEffectScript.h"
#include "SkillUIScript.h"
#include "PlayerHPUIScript.h"


#include <iostream>


// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====
#define ANIM_IDLE			(0)
#define ANIM_JUMP			(1)
#define ANIM_KICK			(2)
#define ANIM_RUN			(3)
#define ANIM_SLASH			(4)
#define ANIM_WALK			(5)


//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void PlayerScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("Player");
	gameObject().lock()->SetTag("Player");

	transform().lock()->m_pos = Vector3(0, 0, 0);
	transform().lock()->m_scale = Vector3(2, 2, 2);
	transform().lock()->m_rot = Quaternion();

	// �R���|�[�l���g�̒ǉ�

	// ���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	m_rb = rb;
	rb->SetMass(2);
	//rb->SetUseGravity(false);

	// �����_���[
	const auto& renderer = gameObject().lock()->AddComponent<AssimpRenderer>();
	renderer->ModelLoad("data/model/Sword And Shield.fbx");
	renderer->SetAnimIndex(ANIM_IDLE);
	renderer->SetAnimSpeed(1.0f);
	m_assimp = renderer;

	// �R���C�_�[
	//const auto& collider = gameObject().lock()->AddComponent<SphereCollider>();
	//collider->SetRadius(50);
	const auto& collider = gameObject().lock()->AddComponent<DeltaCollider>();
	collider->SetRadius(50);

	// UI
	const auto& ui = gameObject().lock()->AddComponent<SkillUIScript>();
	ui->SetPlayer(gameObject().lock()->GetComponent<PlayerScript>());
	const auto& hp = gameObject().lock()->AddComponent<PlayerHPUIScript>();
	hp->SetPlayer(gameObject().lock()->GetComponent<PlayerScript>());

	// �A�N�e�B�u
	m_bActive = true;
	// �W�����v
	m_nJump = 0;
	// �V���b�g
	m_bShot = false;
	// �X�e�[�^�X
	m_HP = getMaxHP();

	m_rotSpeed = 0.2f;
}

//========================================
//
//	�X�V��
//
//========================================
void PlayerScript::Update()
{
	// �A�N�e�B�u
	if (!m_bActive) return;

	const auto& rb = m_rb.lock();
	Quaternion rot = transform().lock()->m_rot;
	Vector3 forward = Matrix::CreateFromQuaternion(rot).Forward();
	Vector3 right = Matrix::CreateFromQuaternion(rot).Right();
	Vector3 up = Vector3(0, 1, 0);

	const auto& camera = Camera::main();
	auto& cameraTrans = camera->transform();
	Vector3 inputDir;
	if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)] <= 0 && 
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] <= 0 && 
		m_jumpCount < 0)
	{
		// �O��
		if (GetKeyPress(VK_UP) || GetKeyPress(VK_W))
		{
			inputDir.z -= 1.0f;
		}
		if (GetKeyPress(VK_DOWN) || GetKeyPress(VK_S))
		{
			inputDir.z += 1.0f;
		}
		// ���E
		if (GetKeyPress(VK_RIGHT) || GetKeyPress(VK_D))
		{
			inputDir.x += 1.0f;
		}
		if (GetKeyPress(VK_LEFT) || GetKeyPress(VK_A))
		{
			inputDir.x -= 1.0f;
		}

		// ��]�𔽉f
		if (inputDir.x != 0.0f || inputDir.z != 0.0f)
		{
			inputDir.Normalize();
			Vector3 moveForce = Vector3::Transform(inputDir, Matrix::CreateFromQuaternion(cameraTrans.lock()->m_rot));
			moveForce.y = 0;
			moveForce.Normalize();
			moveForce.x *= -1;
			Quaternion moveRot = Quaternion::CreateFromRotationMatrix(Matrix::CreateLookAt(Vector3(), moveForce, up));
			rot = Quaternion::Slerp(rot, moveRot, m_rotSpeed);
			rot.Normalize();
			transform().lock()->m_rot = rot;
			// �͂𔽉f
			moveForce.x *= -1;
			moveForce *= getMoveSpeed();
			rb->AddForce(moveForce);

		}

		// ���x�����ɃA�j���[�V����
		Vector3 velo = rb->GetVelocity();
		velo.y = 0;
		float len = velo.Length() * 0.1f;
		if (len > 0.3f)
		{
			// ���s�A�j���[�V����
			if (m_assimp.lock()->GetCurrentAnimIndex() != ANIM_RUN)
			{
				m_assimp.lock()->SetAnimIndex(ANIM_RUN);
			}
			len = len > 1.0f ? 1.0f : len;
			m_assimp.lock()->SetAnimSpeed(len);
		}
		else
		{
			// �ҋ@�A�j���[�V����
			if (m_assimp.lock()->GetCurrentAnimIndex() != ANIM_IDLE)
			{
				m_assimp.lock()->SetAnimIndex(ANIM_IDLE);
				m_assimp.lock()->SetAnimSpeed(1.0f);
			}
		}
	}

	// �W�����v
	m_jumpCount--;
	if (GetKeyTrigger(VK_SPACE) &&
		m_jumpCount < 0 &&
		m_bGround == true &&
		m_assimp.lock()->GetCurrentAnimIndex() != ANIM_JUMP)
	{
		m_jumpCount = 50;
		m_assimp.lock()->SetAnimIndex(ANIM_JUMP);

		// �W�����v�͂���t�Z���āA�A�j���[�V�����̃X�s�[�h��
		m_assimp.lock()->SetAnimSpeed(1.0f);
		rb->AddForceY(getJumpForce());

		// �T�E���h
		CSound::PlaySE("Jump.wav", 0.7f);
		// ��ʗh��
		Camera::main()->SetShakeOffset(Vector2(0, 20));
		Camera::main()->SetShake(8);
		m_bGround = false;
	}

	// �U��
	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)]--;
	if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)] < 0)
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)] = 0;
	if (GetKeyTrigger(MK_LBUTTON) &&
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)] <= 0 &&
		m_assimp.lock()->GetCurrentAnimIndex() != ANIM_SLASH)
	{
		m_assimp.lock()->SetAnimIndex(ANIM_SLASH);
		m_assimp.lock()->SetAnimSpeed(90 / getSkiilRecast(static_cast<int>(PlayerSkill::Shot)));

		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)] = 
			getSkiilRecast(static_cast<int>(PlayerSkill::Shot));
	}
	if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)] ==
		(int)(getSkiilRecast(static_cast<int>(PlayerSkill::Shot)) / 2))
	{
		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BulletScript>()->m_damage = getDamage();
		const auto& rb = test->GetComponent<Rigidbody>();

		Vector3 dir =
			Mathf::Normalize(transform().lock()->forward());

		test->transform().lock()->m_pos = transform().lock()->m_pos + dir;
		test->transform().lock()->m_scale = Vector3(300, 300, 300);
		rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));

		// �T�E���h
		CSound::PlaySE("Shot.wav", 0.2f);
		// ��ʗh��
		Camera::main()->SetShakeOffset(Vector2(40, 20));
		Camera::main()->SetShake(4);
	}

	//// �L�b�N
	//m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)]--;
	//if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] < 0)
	//	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] = 0;
	//if (GetKeyTrigger(VK_E) &&
	//	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] <= 0 &&
	//	m_assimp.lock()->GetCurrentAnimIndex() != ANIM_KICK)
	//{
	//	m_assimp.lock()->SetAnimIndex(ANIM_KICK);
	//	m_assimp.lock()->SetAnimSpeed(75 / getSkiilRecast(static_cast<int>(PlayerSkill::Burst)));

	//	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] =
	//		getSkiilRecast(static_cast<int>(PlayerSkill::Burst));
	//}
	//if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] == 
	//	(int)(getSkiilRecast(static_cast<int>(PlayerSkill::Burst)) / 2))
	//{
	//	const auto& test = Instantiate<GameObject>();
	//	test->AddComponent<BulletScript>()->m_damage = getDamage();
	//	const auto& rb = test->GetComponent<Rigidbody>();

	//	Vector3 dir =
	//		Mathf::Normalize(transform().lock()->forward());

	//	test->transform().lock()->m_pos = transform().lock()->m_pos + dir;
	//	test->transform().lock()->m_scale = Vector3(500, 500, 500);
	//	rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
	//	rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));
	//}

	// �L�b�N
	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)]--;
	if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] < 0)
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] = 0;
	if (GetKeyTrigger(VK_E) &&
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] <= 0 &&
		m_assimp.lock()->GetCurrentAnimIndex() != ANIM_KICK)
	{
		m_assimp.lock()->SetAnimIndex(ANIM_KICK);
		m_assimp.lock()->SetAnimSpeed(75 / getSkiilRecast(static_cast<int>(PlayerSkill::Burst)));
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] =
					getSkiilRecast(static_cast<int>(PlayerSkill::Burst));
	}
	if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] == 
		(int)(getSkiilRecast(static_cast<int>(PlayerSkill::Burst)) / 3 * 2))
	{
		// �o�[�X�g
		m_nBurstCount = 20;
		// ��ʗh��
		Camera::main()->SetShakeOffset(Vector2(10, 40));
		Camera::main()->SetShake(m_nBurstCount);
	}
	// �o�[�X�g�J�E���^
	m_nBurstCount--;
	if (m_nBurstCount < 0) m_nBurstCount = 0;
	// �o�[�X�g�e����
	if (m_nBurstCount > 0 && m_nBurstCount % 4 == 0)
	{
		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BulletScript>()->m_damage = getDamage();
		const auto& rb = test->GetComponent<Rigidbody>();
		const auto& camera = Camera::main();
		Vector3 dir = Mathf::Normalize(transform().lock()->forward());
		test->transform().lock()->m_pos = transform().lock()->m_pos + dir * 300;
		test->transform().lock()->m_scale = Vector3(300, 300, 300);
		rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));
		// �T�E���h
		CSound::PlaySE("Shot.wav", 0.2f);
	}

	// �X�e�b�v
	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Step)]--;
	if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Step)] < 0)
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Step)] = 0;
	Vector3 force = m_rb.lock()->GetForce();
	if (GetKeyTrigger(VK_RBUTTON) && m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Step)] <= 0 &&
		force.Length() > 1.0f)
	{
		// �X�e�b�v���L���X�g
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Step)] =
			getSkiilRecast(static_cast<int>(PlayerSkill::Step));
		// ���G����
		m_nStepInvCount = getStepTime();
		// ��ʗh��
		Camera::main()->SetShakeOffset(Vector2(7, 5));
		Camera::main()->SetShake(m_nStepInvCount);
	}

	// ���G����
	m_nStepInvCount--;
	if (m_nStepInvCount < 0) m_nStepInvCount = 0;
	// �X�e�b�v�G�t�F�N�g����
	if (m_nStepInvCount > 0 && m_nStepInvCount % 4 == 0)
	{
		force.y = 0;
		force.Normalize();
		m_rb.lock()->SetForce(force * 100);

		const int n = 10;
		//const float f = rand() % 314 / 100.0f;
		for (int i = 0; i < n; ++i)
		{
			float phi = 360.0f / n * i * (XM_PI / 180.0f);

			for (int j = 0; j < n; ++j)
			{
				float theta = 360.0f / n * j * (XM_PI / 180.0f);
				// ���W
				Vector3 pos;
				pos.x = cosf(phi) * cosf(theta);
				pos.y = cosf(phi) * sinf(theta);
				pos.z = sinf(phi);
				pos *= 50;

				// �G�t�F�N�g����
				const auto& obj = Instantiate<GameObject>(pos + transform().lock()->m_pos);
				// �R���|�[�l���g�̒ǉ�
				const auto& effect = obj->AddComponent<BombEffectScript>();
				// ���W�b�h�{�f�B
				const auto& rb = obj->GetComponent<Rigidbody>();
				rb->AddForce(Mathf::Normalize(pos) * 5);
				// �����_��
				const auto& render = obj->GetComponent<InstancingMeshRenderer>();
				render->SetDiffuseColor(Vector4(0, 1, 1, 1));
			}
		}
	}

	// �{��
	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Bom)]--;
	if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Bom)] < 0)
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Bom)] = 0;
	if (GetKeyPress(VK_Q) && m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Bom)] <= 0)
	{
		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BombCrystalScript>()->SetDamage(getDamage());
		const auto& rb = test->AddComponent<Rigidbody>();
		rb->SetUseGravity(false);
		rb->SetDrag(Vector3(0, 0, 0));
		rb->SetTorqueDrag(0);
		const auto& col = test->GetComponent<DeltaCollider>();
		col->SetMain(true);

		Vector3 dir = Mathf::Normalize(forward);
		test->transform().lock()->m_pos = transform().lock()->m_pos + dir * 500;
		rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));

		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Bom)] =
			getSkiilRecast(static_cast<int>(PlayerSkill::Bom));

		// �T�E���h
		CSound::PlaySE("Shot.wav", 0.2f);
	}
}

//========================================
//
//	��X�V��
//
//========================================
void PlayerScript::LateUpdate()
{
	// �A�N�e�B�u
	if (!m_bActive) return;

	const auto& camera = Camera::main();
	auto cameraTrans = camera->transform().lock();

	cameraTrans->m_pos = transform().lock()->m_pos + Vector3(0, 10, -5) * 75 * 2;
	Vector3 dir = transform().lock()->m_pos - cameraTrans->m_pos;
	dir.Normalize();
	cameraTrans->forward(dir);

	if (!m_bGround && transform().lock()->m_pos.y <= transform().lock()->m_scale.y / 2)
	{
		m_bGround = true;
		// ��ʗh��
		Camera::main()->SetShakeOffset(Vector2(0, 20));
		Camera::main()->SetShake(6);
		// �T�E���h
		CSound::PlaySE("PlayerGround.wav", 1.0f);
	}


	// --- �X�e�[�^�X

	// ���G����
	m_nDamageInvCount--;
	if (m_nDamageInvCount < 0) m_nDamageInvCount = 0;

	// ��
	m_nHeelCount++;
	if (m_nHeelCount > getHeelTime())
	{
		m_HP += getMaxHP() * 0.1f;
		m_nHeelCount = 0;
	}
	// �ő�l
	if (m_HP > getMaxHP()) m_HP = getMaxHP();


	// HP����ʂɔ��f
	if (m_HP < getMaxHP() / 2)
	{
		auto post = PostProcessing::GetMain();
		post->GetColor()->y = m_HP / (getMaxHP() / 2);
		post->GetColor()->z = m_HP / (getMaxHP() / 2);
	}
}

//========================================
//
//	�I����
//
//========================================
void PlayerScript::End()
{

}


//******************** �R�[���o�b�N�֐� ********************

//========================================
//
// ����������
//
//========================================
void PlayerScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	// �A�N�e�B�u
	if (!m_bActive) return;

	if (collider->gameObject().lock()->tag() == "StatusItem")
	{
		// �J�E���^�[���Z
		const auto& item = collider->gameObject().lock()->GetComponent<StatusItemScript>();
		m_aItemCount[static_cast<int>(item->getType())]++;
		if(item->getType() == ItemType::Physical)
			m_HP += getMaxHP() * 0.1f;
	}
	else if (collider->gameObject().lock()->tag() == "BombCrystal")
	{
		m_nJump = 15;
		// ��ʗh��
		//Camera::GetMainCamera()->SetShakeFrame(6);
		// ��
	}
	else if (collider->gameObject().lock()->tag() == "StartCrystal")
	{
		m_bShot = true;
		// ��ʗh��
		//Camera::GetMainCamera()->SetShakeFrame(6);
		// BGM
		CSound::PlayBGM("GameBGM.mp3", 0.3f);

	}
	else if (collider->gameObject().lock()->tag() == "Enemy" || 
		collider->gameObject().lock()->tag() == "EnemyBullet")
	{
		// ���G����
		if (m_nStepInvCount > 0) return;
		if (m_nDamageInvCount > 0) return;
		m_nDamageInvCount = 60;

		if (collider->gameObject().lock()->tag() == "Enemy")
		{
			// �擾
			const auto& enemy = collider->gameObject().lock()->GetComponent<EnemyBaseScript>();
			// �_���[�W
			float damage = enemy->m_status.Damage;
			// HP
			m_HP -= damage;
		}
		else if (collider->gameObject().lock()->tag() == "EnemyBullet")
		{
			// �擾
			const auto& enemy = collider->gameObject().lock()->GetComponent<BulletScript>();
			// �_���[�W
			float damage = enemy->m_damage;
			// HP
			m_HP -= damage;
		}

		// �m�b�N�o�b�N
		Vector3 dir = transform().lock()->m_pos - collider->transform().lock()->m_pos;
		dir.Normalize();
		m_rb.lock()->SetForce(dir * 20);

		// ��ʗh��
		Camera::main()->SetShakeOffset(Vector2(20, 20));
		Camera::main()->SetShake(16);

		// �T�E���h
		CSound::PlaySE("PlayerDamage.wav", 1.0f);

		if (m_HP > 0) return;
		// �Q�[���I�[�o�[
		const auto& gameover = GetEntityManager()->CreateEntity<GameObject>();
		gameover->AddComponent<GameOverScript>();

		// �A�N�e�B�u
		m_bActive = false;

		// �폜
		Destroy(gameObject().lock());
	}
}

//========================================
//
// �������Ă����
//
//========================================
void PlayerScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	// ��������
	OnDeltaCollisionEnter(collider);
}

//========================================
//
// ���ꂽ��
//
//========================================
void PlayerScript::OnDeltaCollisionExit(DeltaCollider* collider)
{

}

