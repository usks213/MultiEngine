//==================================================================
//								Renderer.h
//	�����_���[
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/23	ECS�p�ɍĐ݌v
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../System/Math.h"
#include "../ECS/Component/IComponent.h"
#include "../main.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class Transform;
	class GameObject;

	class Renderer : public IComponent
	{
	public:
		Renderer();
		virtual ~Renderer();

		// �I�u�W�F�N�g������
		virtual void OnCreate() override;
		// �I�u�W�F�N�g�j����
		virtual void OnDestroy() override;

		// �e�����_���[�̕`�揈��
		virtual void EarlyDraw(ID3D11DeviceContext* pDC) = 0;
		virtual void LateDraw(ID3D11DeviceContext* pDC) = 0;

		// �J��������̋������i�[
		virtual void LayerUpdate();

		// �\�[�g�p
		static bool swapR(Renderer* lhs, Renderer* rhs) {
			return lhs->m_fLayer > rhs->m_fLayer; // �~���ɂ��Ă݂�
		}

		// �e�N�X�`���}�g���b�N�X�̍X�V
		void UpdateTexMatrix();

		// UV
		Vector3 GetTexPos() { return m_texPos; }
		Vector3 GetTexSize() { return m_texSize; }
		void SetTexPos(Vector3 pos) { m_texPos = pos; }
		void SetTexSize(Vector3 size) { m_texSize = size; }

		// �u�����h�X�e�[�g
		void SetRendererBlendState(EBlendState eBlendState) { m_eState = eBlendState; }
		EBlendState GetRendererBlendState() { return m_eState; }

		// ���C���[�̎擾
		float GetLayer() { return m_fLayer; }
		void SetLayer(float fLayer) { m_fLayer = fLayer; }
		void SetUpdateLayer(bool bUpdate) { m_bUpdateLayer = bUpdate; }

		// �Q�[���I�u�W�F�N�g�擾
		const std::weak_ptr<GameObject>& gameObject() { return m_gameObject; }
		// �g�����X�t�H�[���擾
		const std::weak_ptr<Transform>& transform() { return m_transform; }
	protected:
		// �g�����X�t�H�[��
		std::weak_ptr<Transform> m_transform;
		// �Q�[���I�u�W�F�N�g
		std::weak_ptr<GameObject> m_gameObject;

		XMFLOAT4X4 m_mtxTexture;				// �e�N�X�`�� �}�g���b�N�X

		Vector3 m_texPos;
		Vector3 m_texSize;

		// �u�����h�X�e�[�g
		EBlendState m_eState;

		float m_fLayer;
		bool m_bUpdateLayer = true;
	};
}
