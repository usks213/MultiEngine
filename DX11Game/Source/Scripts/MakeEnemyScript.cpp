//==================================================================
//								MakeEnemyScript.h
//	�G�l�~�[�����X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/08	�G�l�~�[�����X�N���v�g�N���X�쐬
//
//===================================================================


//====== �C���N���[�h�� ======

// ���g
#include "MakeEnemyScript.h"

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
#include "../Engine/ECSCompoent/BillboardRenderer.h"
#include "../Engine/ECSCompoent/AssimpRenderer.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"
#include "../Engine/ECSCompoent/InstancingMeshRenderer.h"
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"

// ECS�R���|�[�l���g
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/DeltaCollider.h"

// ECS�V�X�e��
#include "../Engine/ECSSystem/RigidbodySystem.h"
#include "../Engine/ECSSystem/DeltaCollisionSystem.h"

// �X�N���v�g
#include "BalanceEnemyScript.h"
#include "AttackEnemyScript.h"
#include "SpeedEnemyScript.h"
#include "SkiilEnemyScript.h"
#include "PhysicalEnemyScript.h"
#include "PlayerScript.h"
#include "DeltaUIScript.h"

// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====
#define MAX_SPAWN_NUM (50)

#define MAX_ENETETY_NUM (MAX_SPAWN_NUM * 120)


//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void MakeEnemyScript::Start()
{
	const auto& deltaUi = GetEntityManager()->CreateEntity<GameObject>();
	deltaUi->AddComponent<DeltaUIScript>()->
		SetMaker(gameObject().lock()->GetComponent<MakeEnemyScript>());
}

//========================================
//
//	�X�V��
//
//========================================
void MakeEnemyScript::Update()
{
	//----- �v���C���[�Ɠ��� -----

	// �v���C���[���擾
	const auto& player = m_player.lock();
	if (!player) return;

	// �v���C���[���W
	const auto& playerTrans = player->transform().lock();
	Vector3 playerPos = player->transform().lock()->m_pos;
	// ���W�̓���
	transform().lock()->m_pos = playerPos;

	//----- �G�l�~�[�̐��� -----

	// �o�ߎ���
	m_nTime++;
	if (m_nTime % (60 * 60) != 0 && m_nEnemyCount > 0) return;
	m_nWave++;

	// ���������v�Z
	int nSpawnNum = m_nWave  * 1 + 3;
	if (nSpawnNum > MAX_SPAWN_NUM) nSpawnNum = MAX_SPAWN_NUM;
	m_nEnemyCount = nSpawnNum;

	// ��������G�l�~�[�̎��
	for (int i = 0; i < nSpawnNum; ++i)
	{
		//--- �G�l�~�[�𐶐�������W
		float height = (rand() % 5 - 2) * 100;
		if (playerPos.y < 600) playerPos.y = 100;
		Vector3 spawnPos = { 2000, height, 0 };
		Vector3 temp = playerPos;
		temp.y = rand() % 500 + 100;
		spawnPos = Mathf::RotationY(spawnPos, rand() % 360) + temp;

		// �G���e�B�e�B�̐�
		if (GetEntityManager()->GetEntityCount() > MAX_ENETETY_NUM) continue;

		// �G�l�~�[����
		const auto& obj = Instantiate<GameObject>(spawnPos);
		// �R���|�[�l���g�̒ǉ�
		const auto& enemy = obj->AddComponent<EnemyBaseScript>();
		enemy->SetPlayer(player);
		enemy->m_maker = gameObject().lock()->GetComponent<MakeEnemyScript>();

		//--- �^�C�v�ŕ�����
		int type = rand() % (int)EnemyBaseScript::Type::Max;
		switch (type)
		{
		case (int)ECS::EnemyBaseScript::Type::Attack:
			obj->AddComponent<AttackEnemyScript>();
			enemy->SetUpStatus(EnemyBaseScript::Type::Attack, m_nWave);
			break;
		case (int)ECS::EnemyBaseScript::Type::Physical:
			obj->AddComponent<PhysicalEnemyScript>();
			enemy->SetUpStatus(EnemyBaseScript::Type::Physical, m_nWave);
			break;
		case (int)ECS::EnemyBaseScript::Type::Skiil:
			obj->AddComponent<SkiilEnemyScript>();
			enemy->SetUpStatus(EnemyBaseScript::Type::Skiil, m_nWave);
			break;
		case (int)ECS::EnemyBaseScript::Type::Speed:
			obj->AddComponent<SpeedEnemyScript>();
			enemy->SetUpStatus(EnemyBaseScript::Type::Speed, m_nWave);
			break;
		case (int)ECS::EnemyBaseScript::Type::Balance:
			obj->AddComponent<BalanceEnemyScript>();
			enemy->SetUpStatus(EnemyBaseScript::Type::Balance, m_nWave);
			break;
		}
	}
}

//========================================
//
//	��X�V��
//
//========================================
void MakeEnemyScript::LateUpdate()
{
	// �f�o�b�N�\��
	PrintDebugProc("Wave:%d\n\n", m_nWave);
	PrintDebugProc("EnemyCount:%d\n", m_nEnemyCount);
	PrintDebugProc("EntityNum:%d\n", GetEntityManager()->GetEntityCount());
}

//========================================
//
//	�I����
//
//========================================
void MakeEnemyScript::End()
{
	// �X�R�A�̃��[�h
	FILE* fp;
	int nScore = 0;

	// �O��̃X�R�A��ǂݍ���
	fopen_s(&fp, "data/score.bin", "rb");

	if (fp)
	{
		fread(&nScore, sizeof(int), 1, fp);

		fclose(fp);
	}

	if (m_nWave > nScore)
	{
		// �X�R�A�̏����o��
		FILE* fp;
		nScore = m_nWave;

		// �O��̃X�R�A��ǂݍ���
		fopen_s(&fp, "data/score.bin", "wb");

		if (fp)
		{
			fwrite(&nScore, sizeof(int), 1, fp);

			fclose(fp);
		}
	}
}
