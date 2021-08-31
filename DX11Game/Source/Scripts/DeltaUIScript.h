//==================================================================
//								DeltaUIScript.h
//	�����\��UI
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/15	���\��UI�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"
#include "MakeEnemyScript.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class DeltaUIScript;

	class DeltaUIData :public Script
	{
		friend DeltaUIScript;
	private:
		// ----- �����o -----
		std::weak_ptr<SpriteRenderer> m_sprite;
		int m_nAnimNo;

	public:
		// ----- ���\�b�h -----

		// �J�n���ɌĂ΂�܂�
		void Start() override;

	};

	class DeltaUIScript : public Script
	{
	private:
		// ----- �����o -----

		// �v���C���[
		std::weak_ptr<MakeEnemyScript> m_maker;

		// ����
		std::vector<std::weak_ptr<DeltaUIData>> m_data;

	public:
		void SetMaker(std::weak_ptr<MakeEnemyScript> maker) { m_maker = maker; }

	protected:
		// ----- ���\�b�h -----

		// �J�n���ɌĂ΂�܂�
		void Start() override;
		// ���t���[���Ă΂�܂�
		void Update() override;
		// ���t���[���X�V��ɌĂ΂�܂�
		void LateUpdate() override;
		// �I�����ɌĂ΂�܂�
		void End() override;

		// UV�X�V
		void UpdateTexCoord();
	};
}
