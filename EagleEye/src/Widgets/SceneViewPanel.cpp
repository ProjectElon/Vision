#include "SceneViewPanel.h"

#include "Vision/Renderer/FrameBuffer.h"
#include "Vision/Entity/Scene.h"
#include "Vision/IO/Assets.h"

#include <imgui.h>
#include <ImGuizmo.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Vision/Platform/Input.h"

namespace Vision
{
	static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
	{
		// From glm::decompose in matrix_decompose.inl

		using namespace glm;
		using T = float;

		mat4 LocalMatrix(transform);

		// Normalize the matrix.
		if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
			return false;

		// First, isolate perspective.  This is the messiest.
		if (
			epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
		{
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		translation = vec3(LocalMatrix[3]);
		LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

		vec3 Row[3];

		// Now get scale and shear.
		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);
		Row[0] = detail::scale(Row[0], static_cast<T>(1));
		scale.y = length(Row[1]);
		Row[1] = detail::scale(Row[1], static_cast<T>(1));
		scale.z = length(Row[2]);
		Row[2] = detail::scale(Row[2], static_cast<T>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

		rotation.y = asin(-Row[0][2]);
		if (cos(rotation.y) != 0) {
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else {
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}


		return true;
	}


	void SceneViewPanel::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Scene Viewport");

		ImGui::PopStyleVar();

		IsViewportResized = false;
		IsInteractable = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();

		ImVec2 currentViewportSize = ImGui::GetContentRegionAvail();

		uint32 textureID = FrameBuffer->ColorAttachments[0];
		ImGui::Image((void*)(intptr_t)textureID, { currentViewportSize.x, currentViewportSize.y }, ImVec2(0, 1), ImVec2(1, 0));

		auto windowSize = ImGui::GetWindowSize();
		ImVec2 minBound = ImGui::GetWindowPos();

		// auto viewportOffset = ImGui::GetCursorPos();
		// minBound.x += viewportOffset.x;
		// minBound.y += viewportOffset.y;

		ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };

		ViewportBounds[0] = { minBound.x, minBound.y };
		ViewportBounds[1] = { maxBound.x, maxBound.y };

		if (ViewportSize != glm::vec2(currentViewportSize.x, currentViewportSize.y) &&
				currentViewportSize.x > 0.0f &&
				currentViewportSize.y > 0.0f)
		{
			IsViewportResized = true;
			ViewportSize = { currentViewportSize.x, currentViewportSize.y };
			ResizeFrameBuffer(FrameBuffer, (uint32)currentViewportSize.x, (uint32)currentViewportSize.y);
		}

		EditorState editorState = Scene::EditorState;

		if (ActiveScene && !editorState.SelectedEntityTag.empty() && GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImVec2 windowPosition = ImGui::GetWindowPos();
			ImGuizmo::SetRect(windowPosition.x,
			                  windowPosition.y,
							  maxBound.x - minBound.x,
							  maxBound.y - minBound.y);

			auto& sceneCamera = editorState.SceneCamera;

			Scene* scene = Assets::GetScene(ActiveScene);
			Entity selectedEntity = scene->QueryEntity(editorState.SelectedEntityTag);

			if (selectedEntity != entity::null)
			{
				if (scene->HasComponent<TransformComponent>(selectedEntity))
				{
					auto& tc = scene->GetComponent<TransformComponent>(selectedEntity);

					glm::mat4 rotation = glm::toMat4(glm::quat(tc.Rotation));

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.Position) *
							  rotation *
							  glm::scale(glm::mat4(1.0f), tc.Scale);

					bool snap = Input::IsKeyDown(Key::LeftControl);

					float32 snapValue = 0.5f;

					if (GizmoType == ImGuizmo::OPERATION::ROTATE)
					{
						snapValue = 45.0f;
					}

					float32 snapValues[] = { snapValue, snapValue, snapValue };

					ImGuizmo::Manipulate(glm::value_ptr(sceneCamera.View),
										 glm::value_ptr(sceneCamera.Projection),
										 (ImGuizmo::OPERATION)GizmoType,
										 ImGuizmo::MODE::LOCAL,
										 glm::value_ptr(transform),
										 nullptr,
										 snap ? snapValues : nullptr);

					if (ImGuizmo::IsUsing())
					{
						glm::vec3 position;
						glm::vec3 rotation;
						glm::vec3 scale;

						if (DecomposeTransform(transform, position, rotation, scale))
						{
							tc.Position = position;
							glm::vec3 deltaRotation = rotation - tc.Rotation;
							tc.Rotation += deltaRotation;
							tc.Scale = scale;
						}
					}
				}
			}
		}

		ImGui::End();
	}
}