/*****************************************************************//**
 * \file   NumberScript.h
 * \brief  �����X�N���v�g
 *
 * \author USAMI KOSHI
 * \date   2021/08/29
 *********************************************************************/


//====== �C���N���[�h�� ======

// ���g
#include "NumberScript.h"

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
#define ANIM_SPLIT_X (10)  // �摜�̕�����(��)
#define ANIM_SPLIT_Y (1)  // �摜�̕�����(�c)
#define MAX_DIGITS (6)

//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void NumberScript::Start()
{
	{
		// �g�����X�t�H�[��
		transform().lock()->m_pos = Vector3{ 0, 0, 0 };
		transform().lock()->m_scale = Vector3(100, 100, 1);
	}

	// �����f�[�^�̐���
	NumberData data;
	for (int i = 0; i < MAX_DIGITS; i++)
	{
		Vector3 scale = transform().lock()->m_scale;
		Vector3 pos = transform().lock()->m_pos;
		pos.x -= (MAX_DIGITS / 2 - i) * (scale.x + m_nNumSpace);

		const auto& obj = Instantiate<GameObject>(pos);
		// �g�����X�t�H�[��
		obj->transform().lock()->m_scale = scale;
		// �����_���[
		const auto& renderer = obj->AddComponent<SpriteRenderer>();
		renderer->SetDiffuseTexture("data/texture/DeltaNumber.png");
		renderer->SetLayer(renderer->eUI4);
		renderer->SetTexSize(Vector3{ 1.0f / (float)ANIM_SPLIT_X, 1.0f / (float)ANIM_SPLIT_Y , 0 });
		// �i�[
		data.m_sprite = renderer;
		data.m_nAnimNo = 0;
		m_data.push_back(data);
	}
}

//========================================
//
//	�X�V��
//
//========================================
void NumberScript::Update()
{
}

//========================================
//
//	��X�V��
//
//========================================
void NumberScript::LateUpdate()
{
	// ���F�̍X�V
	UpdateTexCoord();

	// �^�񒆊�
	for (int i = 0; i < m_nDigits; i++)
	{
		Vector3 scale = transform().lock()->m_scale;
		Vector3 pos = transform().lock()->m_pos;
		pos.x += -(scale.x + m_nNumSpace) * ((m_nDigits - 1) - i) +
			(scale.x + m_nNumSpace) * ((m_nDigits - 1) / 2.0f);
		const auto& sprite = m_data[(MAX_DIGITS - m_nDigits) + i].m_sprite.lock();
		if (sprite)
		{
			sprite->transform().lock()->m_pos = pos;
			sprite->transform().lock()->m_scale = scale;
		}
	}
}

//========================================
//
//	�I����
//
//========================================
void NumberScript::End()
{
}

//========================================
//
//	UV�X�V
//
//========================================
void NumberScript::UpdateTexCoord()
{
	int tmp = m_nNumber;
	int nCnt = 0;
	// �����𐔂���
	while (tmp > 0)
	{
		tmp /= 10;
		nCnt++;
	}
	m_nDigits = nCnt;

	// UV�X�V
	tmp = m_nNumber;
	for (int i = m_data.size() - 1; 0 <= i; i--)
	{
		const auto& sprite = m_data[i].m_sprite.lock();
		if (!sprite) continue;

		m_data[i].m_nAnimNo = tmp % 10;

		sprite->SetTexPos(
			Vector3{ sprite->GetTexSize().x * (m_data[i].m_nAnimNo % ANIM_SPLIT_X) ,
					 sprite->GetTexSize().y * (m_data[i].m_nAnimNo / ANIM_SPLIT_X),
					 0 });

		if (m_data[i].m_nAnimNo == 0 && (m_data.size() - i) > nCnt)
		{
			sprite->SetAlpha(0);
		}
		else
		{
			sprite->SetAlpha(1);
		}

		tmp /= 10;
	}
}