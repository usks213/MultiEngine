/*****************************************************************//**
 * \file   SkillUIScript.h
 * \brief  �X�L��UI
 *
 * \author USAMI KOSHI
 * \date   2021/08/29
 *********************************************************************/


//====== �C���N���[�h�� ======

// ���g
#include "SkillUIScript.h"

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

namespace {
	const float skillSize = 64;
	const float fadeSize = skillSize;
	const float keySize = 32;
	const float numSize = 24;
}


//===== �}�N����` =====



//******************** �X�N���v�g���\�b�h ********************

//========================================
//
//	�J�n��
//
//========================================
void SkillUIScript::Start()
{
	const int skillNum = m_aUIData.size();
	Vector3 pos(SCREEN_CENTER_X / 2, -SCREEN_CENTER_Y + SCREEN_CENTER_Y / 3, 0);
	const char* skillPath[4] = {
	"data/texture/icon/shot.png",
	"data/texture/icon/step.png",
	"data/texture/icon/burst.png",
	"data/texture/icon/bomb.png",
	};
	const char* keyPath[4] = {
		"data/texture/key/m1.png",
		"data/texture/key/m2.png",
		"data/texture/key/e.png",
		"data/texture/key/q.png",
	};

	// UI�f�[�^
	for (int i = 0; i < m_aUIData.size(); ++i)
	{
		auto& data = m_aUIData[i];
		// �X�L���摜
		{
			const auto& sprite = Instantiate<GameObject>()->AddComponent<SpriteRenderer>();
			data.skillImage = sprite;
			sprite->transform().lock()->m_pos = pos;
			sprite->transform().lock()->m_scale = Vector3(skillSize, skillSize, 0);
			sprite->SetDiffuseTexture(skillPath[i]);
			sprite->SetLayer(SpriteRenderer::eUI1);
		}
		// �X�L���L�[�摜
		{
			const auto& sprite = Instantiate<GameObject>()->AddComponent<SpriteRenderer>();
			data.keyImage = sprite;
			sprite->transform().lock()->m_pos = pos - Vector3(0, skillSize / 2 + keySize / 3 * 2, 0);
			sprite->transform().lock()->m_scale = Vector3(keySize, keySize, 0);
			sprite->SetDiffuseTexture(keyPath[i]);
			sprite->SetLayer(SpriteRenderer::eUI1);
		}
		// �X�L�����L���X�g�t�F�[�h
		{
			const auto& sprite = Instantiate<GameObject>()->AddComponent<SpriteRenderer>();
			data.recastFade = sprite;
			sprite->transform().lock()->m_pos = pos;
			sprite->transform().lock()->m_scale = Vector3(fadeSize, fadeSize, 0);
			sprite->SetDiffuseColor(0, 0, 0);
			sprite->SetAlpha(0.5f);
			sprite->SetLayer(SpriteRenderer::eUI2);
		}
		// �X�L�����L���X�g�^�C���̐���
		{
			const auto& number = Instantiate<GameObject>()->AddComponent<NumberScript>();
			data.recastTime = number;
			number->transform().lock()->m_pos = pos;
			number->transform().lock()->m_scale = Vector3(numSize, numSize, 0);
			number->SetNumber(99);
		}

		pos.x += skillSize + 6;
	}
}

//========================================
//
//	�X�V��
//
//========================================
void SkillUIScript::Update()
{
}

//========================================
//
//	��X�V��
//
//========================================
void SkillUIScript::LateUpdate()
{
	for (int i = 0; i < static_cast<int>(PlayerSkill::Max); ++i)
	{
		// ���L���X�g�^�C���̊���
		float skillPer = m_player.lock()->m_aSkillRecastCnt[i] / m_player.lock()->getSkiilRecast(i);
		// ���L���X�g�t�F�[�h
		m_aUIData[i].recastFade.lock()->transform().lock()->m_scale.y = skillPer * fadeSize;
		// ���L���X�g�^�C��
		if(i == 0)
			m_aUIData[i].recastTime.lock()->SetNumber((m_player.lock()->m_aSkillRecastCnt[i]) / 60);
		else
			m_aUIData[i].recastTime.lock()->SetNumber((m_player.lock()->m_aSkillRecastCnt[i] + 55) / 60);
	}
}

//========================================
//
//	�I����
//
//========================================
void SkillUIScript::End()
{
}
