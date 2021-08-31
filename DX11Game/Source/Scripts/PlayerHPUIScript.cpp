/*****************************************************************//**
 * \file   PlayerHPUIScript.h
 * \brief  �v���C���[HPUI
 *
 * \author USAMI KOSHI
 * \date   2021/08/30
 *********************************************************************/


//====== �C���N���[�h�� ======

// ���g
#include "PlayerHPUIScript.h"

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
	const float bgSizeX = 360;
	const float bgSizeY = 40;
	const float gaugeSizeX = bgSizeX - 4;
	const float gaugeSizeY = bgSizeY - 4;
	const float numSize = 24;
	const Vector3 initPos(-SCREEN_CENTER_X / 3 * 2, -SCREEN_CENTER_Y + SCREEN_CENTER_Y / 11 * 3, 0);

}


//===== �}�N����` =====



//******************** �X�N���v�g���\�b�h ********************

//========================================
//
//	�J�n��
//
//========================================
void PlayerHPUIScript::Start()
{
	Vector3 pos(initPos);

	// UI�f�[�^

	// �w�i�摜
	{
		const auto& sprite = Instantiate<GameObject>()->AddComponent<SpriteRenderer>();
		m_bgImage = sprite;
		sprite->transform().lock()->m_pos = pos;
		sprite->transform().lock()->m_scale = Vector3(bgSizeX, bgSizeY, 0);
		sprite->SetDiffuseColor(0, 0, 0);
		sprite->SetAlpha(0.5f);
		sprite->SetLayer(SpriteRenderer::eUI1);
	}
	// �Q�[�W
	{
		const auto& sprite = Instantiate<GameObject>()->AddComponent<SpriteRenderer>();
		m_gaugeImage = sprite;
		sprite->transform().lock()->m_pos = pos;
		sprite->transform().lock()->m_scale = Vector3(gaugeSizeX, gaugeSizeY, 0);
		sprite->SetDiffuseColor(0, 1, 0);
		sprite->SetLayer(SpriteRenderer::eUI2);
	}
	// �X���b�V��
	{
		const auto& sprite = Instantiate<GameObject>()->AddComponent<SpriteRenderer>();
		m_slashImage = sprite;
		sprite->transform().lock()->m_pos = pos;
		sprite->transform().lock()->m_scale = Vector3(numSize, numSize, 0);
		sprite->SetDiffuseTexture("data/texture/slash.png");
		sprite->SetLayer(SpriteRenderer::eUI3);
	}
	// HP�\�L
	{
		const auto& number = Instantiate<GameObject>()->AddComponent<NumberScript>();
		m_hpNum = number;
		number->transform().lock()->m_pos = pos - Vector3(numSize * 2.5f, -2, 0);
		number->transform().lock()->m_scale = Vector3(numSize, numSize, 0);
		number->SetNumber(99);
		number->SetNumScape(-4);
	}
	// �ő�HP�\�L
	{
		const auto& number = Instantiate<GameObject>()->AddComponent<NumberScript>();
		m_maxHpNum = number;
		number->transform().lock()->m_pos = pos + Vector3(numSize * 2.5f, 2, 0);
		number->transform().lock()->m_scale = Vector3(numSize, numSize, 0);
		number->SetNumber(99);
		number->SetNumScape(-4);
	}
}
//========================================
//
//	�X�V��
//
//========================================
void PlayerHPUIScript::Update()
{
}

//========================================
//
//	��X�V��
//
//========================================
void PlayerHPUIScript::LateUpdate()
{
	// HP����
	float HpPer = m_player.lock()->m_HP / m_player.lock()->getMaxHP();
	// �Q�[�W�\�L
	m_gaugeImage.lock()->transform().lock()->m_pos.x = initPos.x - (1.0f - HpPer) * gaugeSizeX / 2;
	m_gaugeImage.lock()->transform().lock()->m_scale.x = HpPer * gaugeSizeX;
	// HP����
	m_hpNum.lock()->SetNumber(m_player.lock()->m_HP);
	m_maxHpNum.lock()->SetNumber(m_player.lock()->getMaxHP());
}

//========================================
//
//	�I����
//
//========================================
void PlayerHPUIScript::End()
{
}
