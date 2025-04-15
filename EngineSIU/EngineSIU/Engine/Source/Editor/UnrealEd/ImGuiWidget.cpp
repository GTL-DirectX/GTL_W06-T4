#include "ImGuiWidget.h"

#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "ImGUI/imgui_internal.h"

void FImGuiWidget::DrawVec3Control(const std::string& label, FVector& values, float resetValue, float columnWidth)
{
     ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());

        // 현재 윈도우 크기 가져오기
        float windowWidth = ImGui::GetWindowWidth();
        
        // 컨트롤러 하나의 너비 계산 (대략적인 값)
        float controlWidth = columnWidth + // 라벨 컬럼
                            3.0f * (5.0f + ImGui::CalcItemWidth() + 5.0f) + // 버튼(5) + DragFloat + 간격(5)
                            GImGui->Style.ItemSpacing.x * 2; // 추가적인 아이템 간격
        
        // 가운데 정렬을 위한 오프셋 계산
        float offset = (windowWidth - controlWidth) * 0.5f;
        if (offset > 0) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
        }

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { 5.0f, lineHeight };

        // X 컨트롤
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("I", buttonSize))
            values.X = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.X, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine(0, 5);

        // Y 컨트롤
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.5f, 0.0f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("II", buttonSize))
            values.Y = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.Y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine(0, 5);

        // Z 컨트롤
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 1.0f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("III", buttonSize))
            values.Z = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.Z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar(2);
        ImGui::Columns(1);
        ImGui::PopID();
}

void FImGuiWidget::DrawRot3Control(const std::string& label, FRotator& values, float resetValue, float columnWidth)
{
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());

        // 현재 윈도우 크기 가져오기
        float windowWidth = ImGui::GetWindowWidth();
        
        // 컨트롤러 하나의 너비 계산 (대략적인 값)
        float controlWidth = columnWidth + // 라벨 컬럼
                            3.0f * (5.0f + ImGui::CalcItemWidth() + 5.0f) + // 버튼(5) + DragFloat + 간격(5)
                            GImGui->Style.ItemSpacing.x * 2; // 추가적인 아이템 간격
        
        // 가운데 정렬을 위한 오프셋 계산
        float offset = (windowWidth - controlWidth) * 0.5f;
        if (offset > 0) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
        }

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { 5.0f, lineHeight };

        // Roll 컨트롤
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("I", buttonSize))
            values.Roll = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.Roll, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine(0, 5);

        // Pitch 컨트롤
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.5f, 0.0f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("II", buttonSize))
            values.Pitch = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.Pitch, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine(0, 5);

        // Yaw 컨트롤
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 1.0f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("III", buttonSize))
            values.Yaw = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.Yaw, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar(2);
        ImGui::Columns(1);
        ImGui::PopID();
}
float FImGuiWidget::GetAdaptiveDragSpeed(float value)
{
    if (value == 0.0f)
        return 0.01f;
    value = abs(value);
    float exponent = floor(log10(std::max(value, 0.0001f)));
    return pow(10.0f, exponent - 1); // 한 단계 더 미세하게 조절하고 싶으면 -1 유지
}

bool FImGuiWidget::DrawFloatWithSliderAndDrag(const char* label, float& value, float min, float max, const char* format)
{
    bool bChanged = false;

    // 레이블 한 줄 위에 출력
    ImGui::Text("%s", label);

    float dragSpeed = GetAdaptiveDragSpeed(value);

    // 한 줄 아래 수평 정렬
    ImGui::PushItemWidth(100);
    bChanged |= ImGui::DragFloat(std::string("##drag_").append(label).c_str(), &value, dragSpeed, min, max, format);
    ImGui::PopItemWidth();

    ImGui::SameLine();

    ImGui::PushItemWidth(200);
    bChanged |= ImGui::SliderFloat(std::string("##slider_").append(label).c_str(), &value, min, max, format);
    ImGui::PopItemWidth();

    return bChanged;
}
