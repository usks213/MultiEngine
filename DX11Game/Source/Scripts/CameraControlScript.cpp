/*****************************************************************//**
 * \file   CameraControlScript.h
 * \brief
 *
 * \author USAMI KOSHI
 * \date   2021/09/09
 *********************************************************************/


//====== �C���N���[�h�� ======

// ���g
#include "CameraControlScript.h"

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
void CameraControlScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("Camera");
	gameObject().lock()->SetTag("Camera");

	transform().lock()->m_scale = Vector3(100, 100, 100);
	transform().lock()->m_rot = Quaternion();

	// �R���|�[�l���g�̒ǉ�

	// ���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	m_rb = rb;
	rb->SetMass(2);
	rb->SetUseGravity(false);
	rb->SetDrag(Vector3(0.05f, 0.05f, 0.05f));

	// �}�E�X
	SetCursorPos(SCREEN_CENTER_X, SCREEN_CENTER_Y);

	Update();
	LateUpdate();
}

//========================================
//
//	�X�V��
//
//========================================
void CameraControlScript::Update()
{
	// ���_�ړ�
	static POINT m_oldMousePos;
	POINT* mousePos = GetMousePosition();
	POINT mouseDist = {
		mousePos->x - m_oldMousePos.x,
		mousePos->y - m_oldMousePos.y,
	};
	m_oldMousePos = *mousePos;

	if (!GetKeyPress(VK_RBUTTON)) return;

	const float speed = 1.0f;
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
		m_rb.lock()->AddForce(forward);
	}
	if (GetKeyPress(VK_S))
	{
		m_rb.lock()->AddForce(-forward);
	}
	if (GetKeyPress(VK_D))
	{
		m_rb.lock()->AddForce(-right);
	}
	if (GetKeyPress(VK_A))
	{
		m_rb.lock()->AddForce(right);
	}

	// ���{�^��(�J������荞��
	static bool isRot = true;
	if (GetKeyTrigger(VK_MBUTTON))
	{
		isRot ^= 1;
		//SetShowCursor(!isRot);
	}
	if(true)
	{
		// �}�E�X�Œ�
		//SetCursorPos(SCREEN_CENTER_X, SCREEN_CENTER_Y);
		//m_oldMousePos = POINT{ SCREEN_CENTER_X, SCREEN_CENTER_Y };
		//SetShowCursor(false);

		// ��]��
		float angleX = 360.f * mouseDist.x / SCREEN_WIDTH * 0.5f;	// ��ʈ����360�x��](��ʃT�C�Y�̔�����180�x��])
		float angleY = 180.f * mouseDist.y / SCREEN_HEIGHT * 0.5f;	// ��ʈ����180�x��](��ʃT�C�Y�̔�����90�x��])

		//Vector3::Transform(right, Matrix::CreateFromAxisAngle(up, XMConvertToRadians(angleX)));
		trans->m_rot *= Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(-angleY));
		trans->m_rot *= Quaternion::CreateFromAxisAngle(up, XMConvertToRadians(angleX));
	}
	else
	{
		//SetShowCursor(true);
	}
}

//========================================
//
//	��X�V��
//
//========================================
void CameraControlScript::LateUpdate()
{
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

}

//========================================
//
//	�I����
//
//========================================
void CameraControlScript::End()
{

}

