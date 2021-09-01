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

	transform().lock()->m_pos = Vector3(0, 1000, 0);
	transform().lock()->m_scale = Vector3(300, 600, 300);
	transform().lock()->m_rot = Quaternion();

	// �R���|�[�l���g�̒ǉ�

	// ���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	m_rb = rb;
	rb->SetMass(2);
	//rb->SetUseGravity(false);

	//// �����_���[
	//const auto& renderer = gameObject().lock()->AddComponent<MeshRenderer>();
	//renderer->MakeSphere("Player", 100, 50);
	//renderer->SetDiffuseColor({ 0,1,0,1 });

	// �R���C�_�[
	//const auto& collider = gameObject().lock()->AddComponent<SphereCollider>();
	//collider->SetRadius(50);
	const auto& collider = gameObject().lock()->AddComponent<DeltaCollider>();

	// UI
	const auto& ui = gameObject().lock()->AddComponent<SkillUIScript>();
	ui->SetPlayer(gameObject().lock()->GetComponent<PlayerScript>());
	const auto& hp = gameObject().lock()->AddComponent<PlayerHPUIScript>();
	hp->SetPlayer(gameObject().lock()->GetComponent<PlayerScript>());

	// �}�E�X
	SetCursorPos(SCREEN_CENTER_X, SCREEN_CENTER_Y);

	// �A�N�e�B�u
	m_bActive = true;
	// �W�����v
	m_nJump = 0;
	// �V���b�g
	m_bShot = false;
	// �X�e�[�^�X
	m_HP = 100;

	Update();
	LateUpdate();
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

	const float speed = getMoveSpeed();
	const float jump = getJumpForce();
	const float rotSpeed = 3.141592f / 60.0f;

	// ����
	const auto& trans = transform().lock();
	Matrix matR = Matrix::CreateFromQuaternion(trans->m_rot);
	Vector3 vPos = matR.Translation();
	Vector3 right = matR.Right();
	Vector3 up = Vector3(0, 1, 0);
	Vector3 forward = matR.Forward();
	Vector3 vLook = vPos + forward;
	float focus = 0.0f;

	// �ړ�
	if (GetKeyPress(VK_W))
	{
		//transform().lock()->m_pos.z += 1.0f;
		m_rb.lock()->AddForce(forward * Vector3(1,0,1));
	}
	if (GetKeyPress(VK_S))
	{
		//transform().lock()->m_pos.z -= 1.0f;
		m_rb.lock()->AddForce(-forward * Vector3(1, 0, 1));
	}
	if (GetKeyPress(VK_D))
	{
		//transform().lock()->m_pos.x += 1.0f;
		m_rb.lock()->AddForce(-right);
	}
	if (GetKeyPress(VK_A))
	{
		//transform().lock()->m_pos.x -= 1.0f;
		m_rb.lock()->AddForce(right);
	}

	// ���_�ړ�
	static POINT m_oldMousePos;
	POINT* mousePos = GetMousePosition();
	POINT mouseDist = {
		mousePos->x - m_oldMousePos.x,
		mousePos->y - m_oldMousePos.y,
	};
	m_oldMousePos = *mousePos;

	// ���{�^��(�J������荞��
	static bool isRot = true;
	if (GetKeyTrigger(VK_MBUTTON))
	{
		isRot ^= 1;
		SetShowCursor(!isRot);
	}
	if(isRot)
	{
		// �}�E�X�Œ�
		SetCursorPos(SCREEN_CENTER_X, SCREEN_CENTER_Y);
		m_oldMousePos = POINT{ SCREEN_CENTER_X, SCREEN_CENTER_Y };
		SetShowCursor(false);

		// ��]��
		float angleX = 360.f * mouseDist.x / SCREEN_WIDTH * 0.5f;	// ��ʈ����360�x��](��ʃT�C�Y�̔�����180�x��])
		float angleY = 180.f * mouseDist.y / SCREEN_HEIGHT * 0.5f;	// ��ʈ����180�x��](��ʃT�C�Y�̔�����90�x��])

		//Vector3::Transform(right, Matrix::CreateFromAxisAngle(up, XMConvertToRadians(angleX)));
		trans->m_rot *= Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(-angleY));
		trans->m_rot *= Quaternion::CreateFromAxisAngle(up, XMConvertToRadians(angleX));
	}
	else
	{
		SetShowCursor(true);
	}


	// �W�����v
	if (GetKeyTrigger(VK_SPACE) && 
		(transform().lock()->m_pos.y <= transform().lock()->m_scale.y / 2 || m_nJump > 0))
	{
		m_rb.lock()->SetForceY(jump + m_nJump);
		// �T�E���h
		CSound::PlaySE("Jump.wav", 0.7f);
		// ��ʗh��
		Camera::main()->SetShakeOffset(Vector2(0, 20));
		Camera::main()->SetShake(8);
		m_bGround = false;
	}
	m_nJump--;
	if (m_nJump < 0) m_nJump = 0;

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
		m_rb.lock()->SetForce(force * 50);

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

	// �V���b�g
	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)]--;
	if (GetKeyPress(VK_LBUTTON) && m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)] < 0)
	{
		//const auto& test = GetEntityManager()->CreateEntity<GameObject>();
		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BulletScript>()->m_damage = getDamage();
		const auto& rb = test->GetComponent<Rigidbody>();

		const auto& camera = Camera::main();
		Vector3 dir =
			Mathf::Normalize(Matrix::CreateFromQuaternion(camera->transform().lock()->m_rot).Forward());

		test->transform().lock()->m_pos = transform().lock()->m_pos + dir * 300;
		rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));

		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)] = 
			getSkiilRecast(static_cast<int>(PlayerSkill::Shot));

		// �T�E���h
		CSound::PlaySE("Shot.wav", 0.2f);
	}

	// �o�[�X�g
	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)]--;
	if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] < 0)
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] = 0;
	if (GetKeyTrigger(VK_E) && m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] <= 0)
	{
		// �X�e�b�v���L���X�g
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] =
			getSkiilRecast(static_cast<int>(PlayerSkill::Burst));
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
		Vector3 dir =
			Mathf::Normalize(Matrix::CreateFromQuaternion(camera->transform().lock()->m_rot).Forward());
		test->transform().lock()->m_pos = transform().lock()->m_pos + dir * 300;
		rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));
		// �T�E���h
		CSound::PlaySE("Shot.wav", 0.2f);
	}



	// �{��
	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Bom)]--;
	if (GetKeyPress(VK_Q) && m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Bom)] < 0)
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

	/*Matrix mrot = Matrix::CreateFromQuaternion(transform().lock()->m_rot);
	mrot.Up(Vector3(0, 1, 0));
	transform().lock()->m_rot = Quaternion::CreateFromRotationMatrix(mrot);*/

	// �J������ǔ�
	Quaternion rot = transform().lock()->m_rot;
	const auto& camera = Camera::main();

	//--- ��l��
	camera->transform().lock()->m_rot = rot;
	camera->transform().lock()->m_pos =
		transform().lock()->m_pos;
	//--- �O�l��
	//camera->transform().lock()->m_rot = rot;
	//camera->transform().lock()->m_pos =
	//	transform().lock()->m_pos + Vector3::Transform(Vector3(0, 500, 1000), rot) * 0.75f;

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

