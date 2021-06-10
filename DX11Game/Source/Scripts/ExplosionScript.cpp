/*****************************************************************//**
 * \file   ExplosionScript.cpp
 * \brief  �ۑ蔚���G�t�F�N�g
 * 
 * \author USAMI KOSHI
 * \date   2021/05/14�@�ۑ蔚���G�t�F�N�g�N���X�쐬
 *********************************************************************/


//====== �C���N���[�h�� ======

// ���g
#include "ExplosionScript.h"

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


// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====



//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void ExplosionScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("Explosin");
	gameObject().lock()->SetTag("Explosin");

	transform().lock()->m_pos = Vector3(0, 300, 1000);
	transform().lock()->m_scale = Vector3(1, 1, 1);

	// �R���|�[�l���g�̒ǉ�

	// �����_���[
	const auto& renderer = gameObject().lock()->AddComponent<AssimpRenderer>();
	renderer->ModelLoad("data/model/JAS39.fbx");
	m_renderer = renderer;

	// ����
	m_destroyTime = 60 * 3;
	m_time = 0;
}

//========================================
//
//	�X�V��
//
//========================================
void ExplosionScript::Update()
{
	if (GetKeyTrigger(VK_F))
	{
		if (!m_renderer.lock()) return;

		// ���_�擾
		auto model = m_renderer.lock()->GetModel();
		UINT vertexCount, indexCount;
		model->GetVertexCount(&vertexCount, &indexCount);
		TAssimpVertex* aVertex = new TAssimpVertex[vertexCount];
		UINT* aIndex = new UINT[indexCount];
		model->GetVertex(aVertex, aIndex);

		// ���_�f�[�^�쐬
		std::vector<Vector3> vertexData;
		//int* indexData = new int[indexCount];
		std::vector<int> indexData;
		for (UINT i = 0; i < vertexCount; ++i) vertexData.push_back(aVertex[i].vPos);
		for (UINT i = 0; i < indexCount; ++i) indexData.push_back((int)aIndex[i]);
		delete[] aVertex; delete[] aIndex;

		// �j�Ђ̐���
		for (UINT i = 0; i < indexCount; i += 3)
		{
			if (indexData[i] >= vertexCount ||
				indexData[i + 1] >= vertexCount ||
				indexData[i + 2] >= vertexCount) continue;

			// 3���_�̕���
			Vector3 scale = transform().lock()->m_scale;
			Vector3 pos = vertexData[indexData[i]];
			pos += vertexData[indexData[i + 1]];
			pos += vertexData[indexData[i + 2]];
			pos /= 3;

			// 3���_�̃f�[�^�쐬
			Vector3 ver[3] = { vertexData[indexData[i]] - pos,
								vertexData[indexData[i + 1]] - pos,
								vertexData[indexData[i + 2]] - pos };
			int index[3] = { 0,1,2 };

			// �G���e�B�e�B�쐬
			auto obj = Instantiate<GameObject>(transform().lock()->m_pos + pos * scale,
				Vector3(), scale);
			m_aExplosionEffect.push_back(obj);

			// �����_���[�̒ǉ�
			auto renderer = obj->AddComponent<MeshRenderer>();
			char szTag[256];
			sprintf_s(szTag, "explosion%d", i);
;			renderer->MakeAnyVertex(szTag, ver, index, 3, 3);

			// ���W�b�h�{�f�B
			auto rb = obj->AddComponent<Rigidbody>();
			rb->AddForce(Mathf::Normalize(pos) * 10);
			rb->SetTorqueDrag(0);
			rb->SetUseGravity(true);
			// ��]��
			Vector3 axis = Mathf::Cross(ver[1] - ver[0], ver[2] - ver[0]);
			Quaternion q = Quaternion::CreateFromAxisAngle(axis, 0.3f);
			rb->AddTorque(q);
		}

		// �����_���[�폜
		gameObject().lock()->RemoveComponent<AssimpRenderer>();
		m_time = 0;

	}


	// ����
	m_time++;
	if (m_time == m_destroyTime)
	{
		int n = m_aExplosionEffect.size();
		for (int i = 0; i < n; ++i)
		{
			Destroy(m_aExplosionEffect[i].lock());
		}
		m_aExplosionEffect.clear();

		// �����_���[
		const auto& renderer = gameObject().lock()->AddComponent<AssimpRenderer>();
		renderer->ModelLoad("data/model/cub obj.obj");
	}

}

//========================================
//
//	��X�V��
//
//========================================
void ExplosionScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void ExplosionScript::End()
{
}


//******************** �R�[���o�b�N�֐� ********************


//========================================
//
// ����������
//
//========================================
void ExplosionScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// �������Ă����
//
//========================================
void ExplosionScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// ���ꂽ��
//
//========================================
void ExplosionScript::OnCollisionExit(Collider* collider)
{

}

