#include <Windows.h>
#include <cassert>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>

// スキルツリーマネージャーとImGuiのインクルード
#include "SkillTreeManager.h"
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

// ImGuiのメッセージハンドラー外部宣言
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	if (msg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

// ヘルパー関数（ディスクリプタヒープ生成）
ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	return descriptorHeap;
}

// エントリーポイント
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// 1. ウィンドウ生成
	WNDCLASS wc{};
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = L"SkillTreeWindowClass";
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	RegisterClass(&wc);

	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;
	RECT wrc = {0, 0, kClientWidth, kClientHeight};
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindow(
	    wc.lpszClassName, L"スキルツリー 開発画面", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, wrc.right - wrc.left, wrc.bottom - wrc.top, nullptr, nullptr, wc.hInstance, nullptr);

	// 2. グラフィックス基盤最低限の初期化 (ImGuiを動かすため)
	IDXGIFactory7* dxgiFactory = nullptr;
	CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));

	IDXGIAdapter4* useAdapter = nullptr;
	dxgiFactory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter));

	ID3D12Device* device = nullptr;
	D3D12CreateDevice(useAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));

	// コマンド関連
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));

	ID3D12CommandAllocator* commandAllocator = nullptr;
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));

	ID3D12GraphicsCommandList* commandList = nullptr;
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));

	// スワップチェーン設定
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = kClientWidth;
	swapChainDesc.Height = kClientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));

	ID3D12DescriptorHeap* rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	ID3D12DescriptorHeap* srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	ID3D12Resource* swapChainResources[2] = {nullptr};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	uint32_t descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < 2; ++i) {
		swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainResources[i]));
		rtvHandles[i].ptr = rtvStartHandle.ptr + (i * descriptorSize);
		device->CreateRenderTargetView(swapChainResources[i], nullptr, rtvHandles[i]);
	}

	// フェンス（同期用）
	ID3D12Fence* fence = nullptr;
	uint64_t fenceValue = 0;
	device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// スキルツリーマネージャーの初期化
	SkillTreeManager skillTreeManager;
	skillTreeManager.Initialize();

	ShowWindow(hwnd, SW_SHOW);

	// ImGui初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(
	    device, swapChainDesc.BufferCount, DXGI_FORMAT_R8G8B8A8_UNORM, srvDescriptorHeap, srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
	    srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	// 日本語フォント（メイリオ）をここで読み込みます
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\meiryo.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	io.Fonts->Build();

	// メインループ
	MSG msg{};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// スキルツリー画面を描画
			skillTreeManager.Draw();

			ImGui::Render();

			// バックバッファの切り替えと画面クリア
			UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Transition.pResource = swapChainResources[backBufferIndex];
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			commandList->ResourceBarrier(1, &barrier);

			commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
			float clearColor[] = {0.15f, 0.15f, 0.15f, 1.0f};
			commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

			// ImGuiの描画コマンドを実行
			ID3D12DescriptorHeap* descriptorHeaps[] = {srvDescriptorHeap};
			commandList->SetDescriptorHeaps(1, descriptorHeaps);
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			commandList->ResourceBarrier(1, &barrier);

			commandList->Close();
			ID3D12CommandList* commandLists[] = {commandList};
			commandQueue->ExecuteCommandLists(1, commandLists);

			swapChain->Present(1, 0);

			// 同期
			fenceValue++;
			commandQueue->Signal(fence, fenceValue);
			if (fence->GetCompletedValue() < fenceValue) {
				fence->SetEventOnCompletion(fenceValue, fenceEvent);
				WaitForSingleObject(fenceEvent, INFINITE);
			}

			commandAllocator->Reset();
			commandList->Reset(commandAllocator, nullptr);
		}
	}

	// 解放処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (fenceEvent)
		CloseHandle(fenceEvent);
	if (fence)
		fence->Release();
	if (rtvDescriptorHeap)
		rtvDescriptorHeap->Release();
	if (srvDescriptorHeap)
		srvDescriptorHeap->Release();
	for (int i = 0; i < 2; ++i) {
		if (swapChainResources[i])
			swapChainResources[i]->Release();
	}
	if (swapChain)
		swapChain->Release();
	if (commandList)
		commandList->Release();
	if (commandAllocator)
		commandAllocator->Release();
	if (commandQueue)
		commandQueue->Release();
	if (device)
		device->Release();
	if (useAdapter)
		useAdapter->Release();
	if (dxgiFactory)
		dxgiFactory->Release();

	CoUninitialize();
	return 0;
}