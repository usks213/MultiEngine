//==================================================================
//								EnemyBaseScript.h
//	�G�l�~�[�x�[�X�X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/09	�G�l�~�[�x�[�X�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "EnemyBaseScript.h"

// �V�X�e��
#include "../Engine/System/input.h"
#include "../Engine/System/Sound.h"

// �}�l�[�W���[
#include "../Engine/ECS/Entity/EntityManager.h"

// �G���e�B�e�B
#include "../Engine/ECSEntity/GameObject.h"

// �R���|�[�l���g
#include "../Engine/ECSCompoent/Transform.h"
#include "../Engine/ECSCompoent/MeshRenderer.h"
#include "../Engine/ECSCompoent/BillboardRenderer.h"
#include "../Engine/ECSCompoent/AssimpRenderer.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"
#include "../Engine/ECSCompoent/InstancingMeshRenderer.h"
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"

// ECS�R���|�[�l���g
#include "../Engine/ECSCompoent/DeltaCollider.h"

// ECS�V�X�e��
#include "../Engine/ECSSystem/DeltaCollisionSystem.h"

// �X�N���v�g
#include "StatusItemScript.h"
#include "EnemyEffectScript.h"
#include "DamageNumberScript.h"
#include "BulletScript.h"
#include "BombCollisionScript.h"

// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====
#define ADD_SCALE (200.0f / 10.0f)


//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void EnemyBaseScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("Enemy");
	gameObject().lock()->SetTag("Enemy");

	// �傫��
	m_fMaxScale = 300;
	m_fScale = 1.0f;
	transform().lock()->m_scale = Vector3(m_fScale, m_fScale, m_fScale);


	//--- �R���|�[�l���̒ǉ�

	// ECS���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	//rb->SetDrag({ 0,0,0 });
	rb->SetStaticFriction(0);
	rb->SetDynamicFriction(0);
	rb->SetMass(1);
	rb->SetTorqueDrag(0);
	// ��]
	Vector3 v = { rand() % 100 / 100.0f, rand() % 100 / 100.0f, rand() % 100 / 100.0f };
	Quaternion q = Quaternion::CreateFromAxisAngle(Mathf::Normalize(v),
		XMConvertToRadians(v.Length() * 5));
	rb->AddTorque(q);
	// ���W�b�h�{�f�B�ۑ�
	m_rb = rb;

	// ECS�R���C�_�[
	gameObject().lock()->AddComponent<DeltaCollider>()->SetMain(false);

	// HPUI
	const auto& hpUI = Instantiate<GameObject>(Vector3(), Vector3(), Vector3(m_fMaxScale, m_fMaxScale * 0.2f, 1));
	const auto& renderer = hpUI->AddComponent<InstancingMeshRenderer>();
	renderer->MakeSprite("EnemyHP");
	renderer->SetBillboard(true);
	renderer->SetDiffuseColor(Vector4(0, 1, 0.2f, 1));
	m_HPUI = hpUI;


	// �X�e�[�^�X
	SetUpStatus(Type::Balance, 1);

	// �����C�x���g�ǉ�
	m_eventList.push_front(std::bind(&EnemyBaseScript::CreateUpdate, this));
}

//========================================
//
//	�X�V��
//
//========================================
void EnemyBaseScript::Update()
{
	// �C�x���g�X�V
	if (m_eventList.size() > 0)
	{
		m_eventList.front()();

		// ���̃C�x���g��
		if (m_isNext)
		{
			m_eventList.pop_front();
			m_isNext = false;
		}
	}

	const auto& player = m_player.lock();
	if (!player) return;
	Vector3 vec = player->transform().lock()->m_pos - transform().lock()->m_pos;

	// �v���C���[�����b�N�I��
	if (m_isLockon)
	{
		// �U���J�E���g
		m_nAttackCount++;

		Vector3 dir;
		vec.Normalize(dir);
		Matrix rotMat;
		rotMat.Up(Vector3(0,1,0));
		rotMat.Forward(dir);
		//transform().lock()->m_rot = Quaternion::CreateFromRotationMatrix(rotMat);
 	}
	else
	{
		// �����p�j
		m_nMoveCnt++;
		if (m_nMoveCnt > 300)
		{
			m_nMoveCnt = 0;
			m_dir = Vector3(rand() % 100, 0.0f, rand() % 100);
			m_dir.Normalize();
		}
		m_rb.lock()->SetForce(m_dir * 3);
	}

	// ��苗���߂Â����烍�b�N�I��
	if (vec.Length() < 2000.0f)
	{
		m_isLockon = true;
	}

}

//========================================
//
//	��X�V��
//
//========================================
void EnemyBaseScript::LateUpdate()
{
	// HPUI�̓���
	m_HPUI.lock()->transform().lock()->m_pos = 
		transform().lock()->m_pos + Vector3(0, m_fMaxScale, 0);
	m_HPUI.lock()->transform().lock()->m_scale.x = m_fMaxScale * (m_status.HP / m_status.MaxHP);
}

//========================================
//
//	�I����
//
//========================================
void EnemyBaseScript::End()
{
	if (m_HPUI.lock())
	{
		Destroy(m_HPUI.lock());
	}
	if (m_maker.lock())
	{
		m_maker.lock()->m_nEnemyCount--;
	}
}


//******************** �R�[���o�b�N�֐� ********************

//========================================
//
// ����������
//
//========================================
void EnemyBaseScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Bullet" || 
		collider->gameObject().lock()->tag() == "Bomb")
	{
		if (!m_bActive) return;

		// �_���[�W���󂯂��烍�b�N�I��
		m_isLockon = true;

		// �G�t�F�N�g�̐���
		const auto& effect = Instantiate<GameObject>(collider->transform().lock()->m_pos);
		effect->AddComponent<EnemyEffectScript>();

		// ��ʗh��
		const auto& camera = Camera::main();
		camera->SetShakeOffset(Vector2(5,5));
		camera->SetShake(4);
		Vector3 cameraBack = Matrix::CreateFromQuaternion(camera->transform().lock()->m_rot).Backward();

		// �m�b�N�o�b�N
		Vector3 forcedir = transform().lock()->m_pos - collider->transform().lock()->m_pos;
		forcedir.Normalize();
		m_rb.lock()->AddForce(forcedir * 2);

		// �_���[�W
		float damage = 0.0f;
		if(collider->gameObject().lock()->tag() == "Bullet")
			damage = collider->gameObject().lock()->GetComponent<BulletScript>()->m_damage;
		else if(collider->gameObject().lock()->tag() == "Bomb")
			damage = collider->gameObject().lock()->GetComponent<BombCollisionScript>()->GetDamage();

		// �_���[�W�\�L
		const auto& damageUI = Instantiate<GameObject>(collider->transform().lock()->m_pos + cameraBack * m_fMaxScale / 2);
		const auto& number = damageUI->AddComponent<DamageNumberScript>();
		number->SetNumber(damage);
		const auto& rb = damageUI->GetComponent<Rigidbody>();
		Vector3 dir((rand() % 200 - 100) * 0.01f, 1.0f, (rand() % 200 - 100) * 0.01f);
		dir.Normalize();
		rb->AddForce(dir * 10);

		// HP
		m_status.HP -= damage;
		if (m_status.HP >= 0) return;

		// �h���b�v�A�C�e���̐���
		for (int i = 0; i < m_status.Level / 2 + 1; ++i)
		{
			const auto& obj = Instantiate<GameObject>(transform().lock()->m_pos);
			const auto& item = obj->AddComponent<StatusItemScript>();
			item->SetPlayer(m_player.lock());
			switch (m_type)
			{
			case ECS::EnemyBaseScript::Type::Attack:
				item->SetItemType(ItemType::Attack);
				break;
			case ECS::EnemyBaseScript::Type::Speed:
				item->SetItemType(ItemType::Speed);
				break;
			case ECS::EnemyBaseScript::Type::Skiil:
				item->SetItemType(ItemType::Skiil);
				break;
			case ECS::EnemyBaseScript::Type::Physical:
				item->SetItemType(ItemType::Physical);
				break;
			case ECS::EnemyBaseScript::Type::Balance:
				item->SetItemType((ItemType)(rand() % static_cast<int>(ItemType::Max)));
				break;
			}
		}

		// �폜�C�x���g�ǉ�
		m_eventList.push_front(std::bind(&EnemyBaseScript::DestroyUpdate, this));
	}
}

//========================================
//
// �������Ă����
//
//========================================
void EnemyBaseScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	OnDeltaCollisionEnter(collider);
}

//========================================
//
// ���ꂽ��
//
//========================================
void EnemyBaseScript::OnDeltaCollisionExit(DeltaCollider* collider)
{
}

// ���x������X�e�[�^�X���擾(�G�l�~�[�^�C�v�ňႢ������)
void EnemyBaseScript::SetUpStatus(Type type, float level)
{
	const int baseParam = 10;
	const int randParam = 5;
	m_type = type;
	m_status.Level = level;

	switch (type)
	{
	case ECS::EnemyBaseScript::Type::Attack:
		m_status.HP = m_status.MaxHP	= (rand() % randParam + baseParam - 5) * level;
		m_status.Damage					= (rand() % randParam + baseParam + 10) * level;
		m_status.AttackSpeed			= (rand() % randParam + baseParam + 5) * level;
		m_status.MoveSpeed				= (rand() % randParam + baseParam - 5) * level;
		break;
	case ECS::EnemyBaseScript::Type::Speed:
		m_status.HP = m_status.MaxHP	= (rand() % randParam + baseParam - 5) * level;
		m_status.Damage					= (rand() % randParam + baseParam - 5) * level;
		m_status.AttackSpeed			= (rand() % randParam + baseParam + 5) * level;
		m_status.MoveSpeed				= (rand() % randParam + baseParam + 10) * level;
		break;
	case ECS::EnemyBaseScript::Type::Skiil:
		m_status.HP = m_status.MaxHP	= (rand() % randParam + baseParam + 0) * level;
		m_status.Damage					= (rand() % randParam + baseParam - 5) * level;
		m_status.AttackSpeed			= (rand() % randParam + baseParam + 15) * level;
		m_status.MoveSpeed				= (rand() % randParam + baseParam - 5) * level;
		break;
	case ECS::EnemyBaseScript::Type::Physical:
		m_status.HP = m_status.MaxHP	= (rand() % randParam + baseParam + 15) * level;
		m_status.Damage					= (rand() % randParam + baseParam + 0) * level;
		m_status.AttackSpeed			= (rand() % randParam + baseParam - 5) * level;
		m_status.MoveSpeed				= (rand() % randParam + baseParam - 5) * level;
		break;
	case ECS::EnemyBaseScript::Type::Balance:
		m_status.HP = m_status.MaxHP	= (rand() % randParam + baseParam + 3) * level;
		m_status.Damage					= (rand() % randParam + baseParam + 3) * level;
		m_status.AttackSpeed			= (rand() % randParam + baseParam + 3) * level;
		m_status.MoveSpeed				= (rand() % randParam + baseParam + 3) * level;
		break;
	case ECS::EnemyBaseScript::Type::Max:
		break;
	default:
		break;
	}
}

// �������X�V
void EnemyBaseScript::CreateUpdate()
{
	// �傫�������X�ɑ傫������
	if (m_fScale < m_fMaxScale)
	{
		m_fScale += ADD_SCALE;
		if (m_fScale > m_fMaxScale)
		{
			m_fScale = m_fMaxScale;
			m_isNext = true;
		}

		// �傫���𔽉f
		transform().lock()->m_scale = Vector3(m_fScale, m_fScale, m_fScale);
	}
}
// �폜���X�V
void EnemyBaseScript::DestroyUpdate()
{
	m_bActive = false;

	// �傫�������X�ɏ���������
	if (m_fScale > 0)
	{
		m_fScale -= ADD_SCALE;
		if (m_fScale <= 0)
		{
			m_fScale = 0;
			// �폜
			Destroy(gameObject().lock());
			Destroy(m_HPUI.lock());
			m_isNext = true;
		}

		// �傫���𔽉f
		transform().lock()->m_scale = Vector3(m_fScale, m_fScale, m_fScale);
	}
}
