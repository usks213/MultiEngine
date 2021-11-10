/*****************************************************************//**
 * \file   OBBTestScript.h
 * \brief  OBB�e�X�g�v���O����
 *
 * \author USAMI KOSHI
 * \date   2021/09/09
 *********************************************************************/


//====== �C���N���[�h�� ======

// ���g
#include "OBBTestScript.h"

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
void OBBTestScript::Start()
{
	// OBB�R���C�_�[
	gameObject().lock()->AddComponent<BoxCollider>();

	// �����_���[
	const auto& renderer = gameObject().lock()->AddComponent<MeshRenderer>();
	renderer->MakeCube("OBB");
	renderer->SetDiffuseColor(Vector4(1, 0, 0, 0.5f));
	renderer->SetEmissiveColor(Vector4(1, 0, 0, 0.5f));
	renderer->SetRendererBlendState(BS_ALPHABLEND);
	renderer->SetTransparent(true);

}

//========================================
//
//	�X�V��
//
//========================================
void OBBTestScript::Update()
{
}

//========================================
//
//	��X�V��
//
//========================================
void OBBTestScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void OBBTestScript::End()
{
}


//******************** �R�[���o�b�N�֐� ********************


//========================================
//
// ����������
//
//========================================
void OBBTestScript::OnCollisionEnter(Collider* collider)
{
	const auto& renderer = gameObject().lock()->GetComponent<MeshRenderer>();
	renderer->SetDiffuseColor(Vector4(0, 1, 0, 0.5f));
	renderer->SetEmissiveColor(Vector4(0, 1, 0, 0.5f));
}

//========================================
//
// �������Ă����
//
//========================================
void OBBTestScript::OnCollisionStay(Collider* collider)
{
	OnCollisionEnter(collider);
}

//========================================
//
// ���ꂽ��
//
//========================================
void OBBTestScript::OnCollisionExit(Collider* collider)
{
	const auto& renderer = gameObject().lock()->GetComponent<MeshRenderer>();
	renderer->SetDiffuseColor(Vector4(1, 0, 0, 0.5f));
	renderer->SetEmissiveColor(Vector4(1, 0, 0, 0.5f));
}

