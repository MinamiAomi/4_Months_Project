#include "SkyRenderer.h"

#include "../Core/Helper.h"
#include "../Core/ShaderManager.h"
#include "File/JsonHelper.h"
#include "Graphics/ImGuiManager.h"
namespace {
	const wchar_t kVertexShader[] = L"App/SkyVS.hlsl";
	const wchar_t kPixelShader[] = L"App/SkyPS.hlsl";
}

void SkyRenderer::Initialize(DXGI_FORMAT rtvFormat) {
#pragma region パラメーター
	JSON_OPEN("Resources/Data/Sky/Sky.json");
	JSON_OBJECT("Sky");
	JSON_LOAD(topColor_);
	JSON_LOAD(bottomColor_);
	JSON_LOAD(oppositionTopColor_);
	JSON_LOAD(oppositionBottomColor_);
	JSON_CLOSE();
#pragma endregion
	CD3DX12_DESCRIPTOR_RANGE range{};
	range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER rootParameters[2]{};
	rootParameters[0].InitAsConstantBufferView(0);
	rootParameters[1].InitAsDescriptorTable(1, &range);

	CD3DX12_STATIC_SAMPLER_DESC ssDesc{};
	ssDesc.Init(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

	D3D12_ROOT_SIGNATURE_DESC rsDesc{};
	rsDesc.NumParameters = _countof(rootParameters);
	rsDesc.pParameters = rootParameters;
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pStaticSamplers = &ssDesc;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignature_.Create(L"SkyRenderer RootSignature", rsDesc);


	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};

	pipelineStateDesc.pRootSignature = rootSignature_;

	D3D12_INPUT_ELEMENT_DESC inputElements[] = {
		 { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		 { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		 { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
	D3D12_INPUT_LAYOUT_DESC inputLayout{};
	inputLayout.NumElements = _countof(inputElements);
	inputLayout.pInputElementDescs = inputElements;
	pipelineStateDesc.InputLayout = inputLayout;

	auto shaderManager = ShaderManager::GetInstance();
	auto vs = shaderManager->Compile(kVertexShader, ShaderManager::kVertex);
	auto ps = shaderManager->Compile(kPixelShader, ShaderManager::kPixel);

	pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
	pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());

	pipelineStateDesc.BlendState = Helper::BlendAlpha;
	pipelineStateDesc.DepthStencilState = Helper::DepthStateDisabled;
	pipelineStateDesc.RasterizerState = Helper::RasterizerDefault;
	pipelineStateDesc.RasterizerState.DepthClipEnable = FALSE;
	pipelineStateDesc.NumRenderTargets = 1;
	pipelineStateDesc.RTVFormats[0] = rtvFormat;
	pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineState_.Create(L"SkyRenderer PipelineState", pipelineStateDesc);

	voronoi_.Initialize(1024, 1024, 60000);
	saveState_ = Character::currentCharacterState_;
}

void SkyRenderer::Render(CommandContext& commandContext, const Camera& camera, Matrix4x4 worldMatrix) {

	DebugParam();

	struct Scene {
		Matrix4x4 viewMatrix;
		Matrix4x4 projectionMatrix;
		Matrix4x4 worldMatrix;
		Matrix4x4 worldInverseTransposeMatrix;
		Vector3 topColor;
		float pad;
		Vector3 bottomColor;
	};

	struct Vertex {
		Vector3 position;
		Vector3 normal;
		Vector2 texcoord;
	};

	commandContext.SetRootSignature(rootSignature_);
	commandContext.SetPipelineState(pipelineState_);
	commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Scene scene;
	scene.viewMatrix = camera.GetViewMatrix();
	scene.projectionMatrix = camera.GetProjectionMatrix();
	scene.worldMatrix = worldMatrix;
	scene.worldInverseTransposeMatrix = worldMatrix.Inverse().Transpose();
	//遷移
	if (saveState_ != Character::currentCharacterState_) {
		if (Character::currentCharacterState_ == Character::kChase) {
			//反撃になったら
			t_ += speed_;
			t_ = std::clamp(t_, 0.0f, 1.0f);
			if (t_ >= 1.0f) {
				saveState_ = Character::currentCharacterState_;
			}
		}
		else {
			//逃げる側になったら
			t_ -= speed_;
			t_ = std::clamp(t_, 0.0f, 1.0f);
			if (t_ <= 0.0f) {
				saveState_ = Character::currentCharacterState_;
			}
		}
	}

	scene.topColor = Vector3::Lerp(t_, topColor_, oppositionTopColor_);
	scene.bottomColor = Vector3::Lerp(t_, bottomColor_, oppositionBottomColor_);

	commandContext.SetDynamicConstantBufferView(0, sizeof(scene), &scene);
	commandContext.SetDescriptorTable(1, voronoi_.Get().GetSRV());

	Vector3 positions[8] = {
		{ 0.5f, 0.5f,-0.5f }, // 右上前 0
		{-0.5f, 0.5f,-0.5f }, // 左上前 1
		{ 0.5f, 0.5f, 0.5f }, // 右上奥 2
		{-0.5f, 0.5f, 0.5f }, // 左上奥 3
		{ 0.5f,-0.5f,-0.5f }, // 右下前 4
		{-0.5f,-0.5f,-0.5f }, // 左下前 5
		{ 0.5f,-0.5f, 0.5f }, // 右下奥 6
		{-0.5f,-0.5f, 0.5f }, // 左下奥 7
	};



	std::vector<Vertex> vertices(36);
	size_t index = 0;
	// 上面
	vertices[index++] = { positions[3], Vector3::down, { 0.0f, 1.0f} };
	vertices[index++] = { positions[1], Vector3::down, { 0.0f, 0.0f} };
	vertices[index++] = { positions[0], Vector3::down, { 1.0f, 0.0f} };
	vertices[index++] = { positions[3], Vector3::down, { 0.0f, 1.0f} };
	vertices[index++] = { positions[0], Vector3::down, { 1.0f, 0.0f} };
	vertices[index++] = { positions[2], Vector3::down, { 1.0f, 1.0f} };

	// 下面
	vertices[index++] = { positions[5], Vector3::up, { 0.0f, 1.0f} };
	vertices[index++] = { positions[7], Vector3::up, { 0.0f, 0.0f} };
	vertices[index++] = { positions[6], Vector3::up, { 1.0f, 0.0f} };
	vertices[index++] = { positions[5], Vector3::up, { 0.0f, 1.0f} };
	vertices[index++] = { positions[6], Vector3::up, { 1.0f, 0.0f} };
	vertices[index++] = { positions[4], Vector3::up, { 1.0f, 1.0f} };

	// 右面
	vertices[index++] = { positions[6], Vector3::left, { 0.0f, 1.0f} };
	vertices[index++] = { positions[2], Vector3::left, { 0.0f, 0.0f} };
	vertices[index++] = { positions[0], Vector3::left, { 1.0f, 0.0f} };
	vertices[index++] = { positions[6], Vector3::left, { 0.0f, 1.0f} };
	vertices[index++] = { positions[0], Vector3::left, { 1.0f, 0.0f} };
	vertices[index++] = { positions[4], Vector3::left, { 1.0f, 1.0f} };

	// 左面
	vertices[index++] = { positions[5], Vector3::right, { 0.0f, 1.0f} };
	vertices[index++] = { positions[1], Vector3::right, { 0.0f, 0.0f} };
	vertices[index++] = { positions[3], Vector3::right, { 1.0f, 0.0f} };
	vertices[index++] = { positions[5], Vector3::right, { 0.0f, 1.0f} };
	vertices[index++] = { positions[3], Vector3::right, { 1.0f, 0.0f} };
	vertices[index++] = { positions[7], Vector3::right, { 1.0f, 1.0f} };

	// 前面
	vertices[index++] = { positions[4], Vector3::forward, { 0.0f, 1.0f} };
	vertices[index++] = { positions[0], Vector3::forward, { 0.0f, 0.0f} };
	vertices[index++] = { positions[1], Vector3::forward, { 1.0f, 0.0f} };
	vertices[index++] = { positions[4], Vector3::forward, { 0.0f, 1.0f} };
	vertices[index++] = { positions[1], Vector3::forward, { 1.0f, 0.0f} };
	vertices[index++] = { positions[5], Vector3::forward, { 1.0f, 1.0f} };

	// 後面
	vertices[index++] = { positions[7], Vector3::back, { 0.0f, 1.0f} };
	vertices[index++] = { positions[3], Vector3::back, { 0.0f, 0.0f} };
	vertices[index++] = { positions[2], Vector3::back, { 1.0f, 0.0f} };
	vertices[index++] = { positions[7], Vector3::back, { 0.0f, 1.0f} };
	vertices[index++] = { positions[2], Vector3::back, { 1.0f, 0.0f} };
	vertices[index++] = { positions[6], Vector3::back, { 1.0f, 1.0f} };

	commandContext.SetDynamicVertexBuffer(0, vertices.size(), sizeof(Vertex), vertices.data());
	commandContext.Draw((UINT)vertices.size());
}

void SkyRenderer::DebugParam() {
#ifdef _DEBUG


	ImGui::Begin("Editor");
	if (ImGui::BeginMenu("Sky")) {
		ImGui::DragFloat3("topColor", &topColor_.x, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat3("bottomColor", &bottomColor_.x, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat3("oppositionTopColor_", &oppositionTopColor_.x, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat3("oppositionBottomColor_", &oppositionBottomColor_.x, 0.01f, 0.0f, 1.0f);

		if (ImGui::Button("Save")) {
			JSON_OPEN("Resources/Data/Sky/Sky.json");
			JSON_OBJECT("Sky");
			JSON_SAVE(topColor_);
			JSON_SAVE(bottomColor_);
			JSON_SAVE(oppositionTopColor_);
			JSON_SAVE(oppositionBottomColor_);
			JSON_CLOSE();
		}
		ImGui::EndMenu();
	}
	ImGui::End();
#endif // _DEBUG
}
