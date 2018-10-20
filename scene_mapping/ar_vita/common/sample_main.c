/* SCE CONFIDENTIAL
 PlayStation(R)Vita Programmer Tool Runtime Library Release 03.570.011
 * Copyright (C) 2011 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

/*
 * Title:  Sample main.
 * File: sample_main.c
 * Description:
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <libdbg.h>
#include <sceerror.h>
#include <kernel.h>
#include <display.h>
#include <ctrl.h>
#include <gxm.h>
#include <power.h>

#include <camera.h>
#include <motion.h>
#include <math.h>
#include <float.h>
#include <libsysmodule.h>
#include <apputil.h>

//#define ENABLE_DBGPRINTF
#include "common.h"
#include "sample_main.h"

//#define ENABLE_DBGFONT

#define ENABLE_SAVEDATA

//#define DISABLE_SMART


#if defined(ENABLE_DBGFONT)
#	include <libdbgfont.h>
#	pragma comment(lib, "SceDbgFont")
#endif

#pragma comment(lib, "SceSysmodule_stub")
#pragma comment(lib, "SceDisplay_stub")
#pragma comment(lib, "SceAppUtil_stub")
#pragma comment(lib, "SceCamera_stub")
#pragma comment(lib, "SceMotion_stub")
#pragma comment(lib, "SceCtrl_stub")
#pragma comment(lib, "SceGxm_stub")
#pragma comment(lib, "ScePower_stub")
#pragma comment(lib, "SceDbg_stub")


#define SAVEDATA_SLOT_ID			(0)

// Define the width and height to render at the native resolution on ES2 hardware.
#define DISPLAY_WIDTH				960
#define DISPLAY_HEIGHT				544
#define DISPLAY_STRIDE_IN_PIXELS	1024

//	Define the libgxm color format to render to.  This should be kept in sync
//	with the display format to use with the SceDisplay library.
#define DISPLAY_COLOR_FORMAT		SCE_GXM_COLOR_FORMAT_A8B8G8R8
#define DISPLAY_PIXEL_FORMAT		SCE_DISPLAY_PIXELFORMAT_A8B8G8R8

// 	Define the number of back buffers to use with this sample.  Most applications
//	should use a value of 2 (double buffering) or 3 (triple buffering).
#define DISPLAY_BUFFER_COUNT		3

// Define the maximum number of queued swaps that the display queue will allow.
// This limits the number of frames that the CPU can get ahead of the GPU,
// and is independent of the actual number of back buffers.  The display
// queue will block during sceGxmDisplayQueueAddEntry if this number of swaps
// have already been queued.
#define DISPLAY_MAX_PENDING_SWAPS	1

// Define the MSAA mode.  This can be changed to 4X on ES1 hardware to use 4X
// multi-sample anti-aliasing, and can be changed to 4X or 2X on ES2 hardware.
#define MSAA_MODE					SCE_GXM_MULTISAMPLE_NONE

// Helper macro to align a value
#define ALIGN(x, a)					(((x) + ((a) - 1)) & ~((a) - 1))

//E @brief Main entry point for the application
//E @return Error code result of processing during execution: <c> SCE_OK </c> on success, 
//E or another code depending upon the error
//J @brief アプリケーションのメインエントリポイント
//J @return 実行結果を示すエラーコード: 成功時は <c> SCE_OK </c>, 
//J エラー発生時はエラーに依存して他のコード
int main(void);

//E @brief Main initialization function to setup up application data
//E @return Result of processing during execution
//J @brief アプリケーションデータをセットアップするためのメイン初期化関数
//J @return 実行結果
static int init(void);

//E @brief Initializes the graphics services and the libgxm graphics library
//E @return Error code result of processing during execution: <c> SCE_OK </c> on success, 
//E or another code depending upon the error
//J @brief グラフィックスサービスとlibgxmグラフィックライブラリの初期化
//J @return 実行結果を示すエラーコード: 成功時は <c> SCE_OK </c>, 
//J エラー発生時はエラーに依存して他のコード
static int initGxm(void);

//E @brief Main update function called from main
//J @brief メイン関数から呼ばれるメインの更新関数
static void update(void);

//E @brief Function to manager any controller input
//J @brief コントローラからの入力を管理する関数
static void processController(void);

//E @brief Function to update the debug menu
//J @brief デバッグメニューを更新するための関数
static void updateDebugMenu(void);

//E @brief Main rendering function to draw graphics to the display
//J @brief グラフィックを描画し、表示するためのメインのレンダリング関数
static void render(void);

//E @brief Function to draw any profile data to the screen
//J @brief プロファイルデータをスクリーンに描画するための関数
static void renderProfileData(void);

//E @brief Function to display the debug text
//J @brief デバッグテキストを表示するための関数
static void renderDebugText(void);

//E @brief Function to draw the debug menu after update
//J @brief update後にデバッグメニューを描画するための関数
static void renderDebugMenu(void);

//E @brief Main shutdown function to tidy up
//E @return Error code result of processing during execution: <c> SCE_OK </c> on success, 
//E or another code depending upon the error
//J @brief クリーンアップのためのメイン終了関数
//J @return 実行結果を示すエラーコード: 成功時は <c> SCE_OK </c>, 
//J エラー発生時はエラーに依存して他のコード
static int terminate(void);

//E @brief Function to shut down libgxm and the graphics display services
//E @return Error code result of processing during execution: <c> SCE_OK </c> on success, 
//E or another code depending upon the error
//J @brief libgxmとグラフィックサービスのクリーンアップのためのメイン終了関数
//J @return 実行結果を示すエラーコード: 成功時は <c> SCE_OK </c>, 
//J エラー発生時はエラーに依存して他のコード
static int terminateGxm(void);

//E @brief Function to manage pause state
//J @brief ポーズ状態を管理する関数
static void handlePause(void);

//E @brief Simple query to determine current pause state
//E @return boolean value <c> true </c> if paused, <c> false </c> if not
//J @brief 現在のポーズ状態を確認するための関数
//J @return boolean 値 ポーズ中の場合は<c> true </c>, ポーズ中ではない場合は<c> false </c>
bool isPaused(void);

//E @brief Simple query to determine current execution state
//E @return boolean value <c> true </c> if running, <c> false </c> if not
//J @brief 現在の実行状態を確認するための関数
//J @return boolean 値 実行中の場合は<c> true </c>, 実行中ではない場合は<c> false </c>
bool isRunning(void);

//E @brief function to handle a quit event
//J @brief 終了イベントをハンドルするための関数
void quit(void);

//E @brief Memory allocation callback function used by the libgxm shader patcher
//E @param size Size in bytes of memory block to allocate
//E @return void pointer to the allocated memory, on success, <c> NULL </c> on failure
//J @brief libgxmのシェーダーパッチャーが利用するメモリーアロケーションコールバック関数
//J @param size 確保するメモリのバイトサイズ
//J @return 確保されたメモリに対するvoidポインタ, 失敗時は <c> NULL </c>
static void *patcherHostAlloc(void *userData, uint32_t size);

//E @brief Memory de-allocation callback function used by the libgxm shader patcher
//E @param mem void pointer to memory block to be freed
//J @brief libgxmのシェーダーパッチャーが利用するメモリ割当解除コールバック関数
//J @param mem 開放するメモリに対するvoidポインタ
static void patcherHostFree(void *userData, void *mem);

// Data structure to pass through the display queue.  This structure is
// serialized during sceGxmDisplayQueueAddEntry, and is used to pass
// arbitrary data to the display callback function, called from an internal
// thread once the back buffer is ready to be displayed.
typedef struct DisplayData
{
	void *address;
} DisplayData;

// Callback function for displaying a buffer
static void displayCallback(const void *callbackData);

//E @brief Helper function to allocate memory and map it for the GPU
static void *graphicsAlloc(SceKernelMemBlockType type, uint32_t size, uint32_t alignment, uint32_t attribs, SceUID *uid);

//E @brief Helper function to free memory mapped to the GPU
static void graphicsFree(SceUID uid);

//E @brief Helper function to allocate memory and map it as vertex USSE code for the GPU
static void *vertexUsseAlloc(uint32_t size, SceUID *uid, uint32_t *usseOffset);

//E @brief Helper function to free memory mapped as vertex USSE code for the GPU
static void vertexUsseFree(SceUID uid);

//E @brief Helper function to allocate memory and map it as fragment USSE code for the GPU
static void *fragmentUsseAlloc(uint32_t size, SceUID *uid, uint32_t *usseOffset);

//E @brief Helper function to free memory mapped as fragment USSE code for the GPU
static void fragmentUsseFree(SceUID uid);


// sample_smart
int sampleInitialize(void);
void sampleRelease(void);
void sampleUpdate(void);
void sampleRender(void);
void sampleDisplayCallback(DisplayData const* data);



// Mark variable as used
#define	UNUSED(a)					(void)(a)

// Game state
static bool g_isPaused = false;			///< Global pause state flag
static bool g_isRunning = true;			///< Global execution state flag
static bool g_showMenu = false;			///< Global menu rendering state flag
static SceCtrlData g_controllerData;	//E< Global controller state

// Structures required by libgxm 
static	SceGxmContextParams			g_ctxParams;				///< libgxm graphics context parameter structure
static	SceUID						g_vdmRingBufferUid = 0;
static	SceUID						g_vertexRingBufferUid = 0;
static	SceUID						g_fragmentRingBufferUid = 0;
static	SceUID						g_fragmentUsseRingBufferUid = 0;

static	SceGxmShaderPatcherParams	g_patcherParams;			///< libgxm graphics shader patcher parameter structure
static	SceUID						g_patcherBufferUid = 0;
static	SceUID						g_patcherVertexUsseUid = 0;
static	SceUID						g_patcherFragmentUsseUid = 0;

static	SceGxmContext				*g_pContext = NULL;			///< libgxm graphics context pointer
static	SceGxmShaderPatcher			*g_pShaderPatcher = NULL;	///< libgxm graphics shader patcher pointer
static  SceGxmRenderTargetParams	g_renderTargetParams;
static  SceGxmRenderTarget			*g_pRenderTarget = NULL;	///< libgxm render target setup pointer

// allocate memory and sync objects for display buffers
static	void						*g_pDisplayBufferData[DISPLAY_BUFFER_COUNT] = { 0 };
static	void						*g_pDepthBufferData = NULL;
static	SceUID						g_displayBufferUid[DISPLAY_BUFFER_COUNT] = { 0 };
static	SceUID						g_depthBufferUid = 0;
static	SceGxmColorSurface			g_displaySurface[DISPLAY_BUFFER_COUNT];
static	SceGxmDepthStencilSurface	g_depthSurface;
static	SceGxmSyncObject			*g_pDisplayBufferSync[DISPLAY_BUFFER_COUNT] = { 0 };
static	uint32_t					g_displayFrontBufferIndex = DISPLAY_BUFFER_COUNT - 1;
static	uint32_t					g_displayBackBufferIndex = 0;

#if 0
// Main entry point of program
int main(void)
{
	int returnCode = SCE_OK;

	returnCode = init();
	if(returnCode == SCE_CAMERA_ERROR_NOT_MOUNTED){
		printf("Camera is not supported.\n");
		int errCode = terminate();
		SCE_DBG_ASSERT(errCode >= SCE_OK);
		return SCE_OK;
	}
	SCE_DBG_ASSERT(SCE_OK == returnCode);
	if(returnCode < SCE_OK){
		int errCode = terminate();
		SCE_DBG_ASSERT(errCode >= SCE_OK);
		return returnCode;
	}
	
	// Message fou SDK sample auto test
	printf("## %s: INIT SUCCEEDED ##\n", smartSampleName());
	
	// main loop 
	while (isRunning())
	{
		update();
		render();

		// queue the display swap for this frame
		DisplayData displayData;
		displayData.address = g_pDisplayBufferData[g_displayBackBufferIndex];
		sceGxmDisplayQueueAddEntry(
			g_pDisplayBufferSync[g_displayFrontBufferIndex],	// front buffer is OLD buffer
			g_pDisplayBufferSync[g_displayBackBufferIndex],		// back buffer is NEW buffer
			&displayData);

		// update buffer indices
		g_displayFrontBufferIndex = g_displayBackBufferIndex;
		g_displayBackBufferIndex = (g_displayBackBufferIndex + 1) % DISPLAY_BUFFER_COUNT;		
	}

	returnCode = terminate();
	SCE_DBG_ASSERT(SCE_OK == returnCode);
	
	// Message fou SDK sample auto test
	//printf("## %s: FINISHED ##\n", smartSampleName());

	return returnCode;
}
#endif

// Initialise system - memory, graphics, etc.
int init(void)
{   
	int returnCode = SCE_OK;

	g_isPaused = false;
	g_isRunning = true;
	g_showMenu = false;

	// Initialize controller state
	memset(&g_controllerData, 0, sizeof(g_controllerData));

	// Init graphics
	returnCode = initGxm();
	if(returnCode < SCE_OK){
		return returnCode;
	}

	// Init sample
	returnCode = sampleInitialize();
	if(returnCode < SCE_OK){
		return returnCode;
	}

	return returnCode;
}

// Initialize the graphics display services and libgxm
int initGxm()
{
	// set up initialise params
	SceGxmInitializeParams initializeParams;
	memset(&initializeParams, 0, sizeof(SceGxmInitializeParams));
	initializeParams.flags							= 0;
	initializeParams.displayQueueMaxPendingCount	= DISPLAY_MAX_PENDING_SWAPS;
	initializeParams.displayQueueCallback			= displayCallback;
	initializeParams.displayQueueCallbackDataSize	= sizeof(DisplayData);
	initializeParams.parameterBufferSize			= SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE;
	
	// start libgxm
	SceGxmErrorCode errCode = sceGxmInitialize(&initializeParams);
	SCE_DBG_ASSERT(errCode == SCE_OK);
	if(errCode < SCE_OK){
		return errCode;
	}

	// allocate ring buffer memory using default sizes
	void *vdmRingBuffer = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
		SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&g_vdmRingBufferUid);
	if(vdmRingBuffer == NULL){
		return -1;
	}

	void *vertexRingBuffer = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
		SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&g_vertexRingBufferUid);
	if(vertexRingBuffer == NULL){
		return -1;
	}

	void *fragmentRingBuffer = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
		SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&g_fragmentRingBufferUid);
	if(fragmentRingBuffer == NULL){
		return -1;
	}

	uint32_t fragmentUsseRingBufferOffset;
	void *fragmentUsseRingBuffer = fragmentUsseAlloc(
		SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE,
		&g_fragmentUsseRingBufferUid,
		&fragmentUsseRingBufferOffset);
	if(fragmentUsseRingBuffer == NULL){
		return -1;
	}

	// set params for rendering context
	memset(&g_ctxParams, 0, sizeof(g_ctxParams));
	g_ctxParams.hostMem							= malloc(SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE);
	g_ctxParams.hostMemSize						= SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE;
	g_ctxParams.vdmRingBufferMem				= vdmRingBuffer;
	g_ctxParams.vdmRingBufferMemSize			= SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE;
	g_ctxParams.vertexRingBufferMem				= vertexRingBuffer;
	g_ctxParams.vertexRingBufferMemSize			= SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE;
	g_ctxParams.fragmentRingBufferMem			= fragmentRingBuffer;
	g_ctxParams.fragmentRingBufferMemSize		= SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE;
	g_ctxParams.fragmentUsseRingBufferMem		= fragmentUsseRingBuffer;
	g_ctxParams.fragmentUsseRingBufferMemSize	= SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE;
	g_ctxParams.fragmentUsseRingBufferOffset	= fragmentUsseRingBufferOffset;

	// create a rendering context
	errCode = sceGxmCreateContext(&g_ctxParams, &g_pContext);
	SCE_DBG_ASSERT(SCE_OK == errCode);
	SCE_DBG_ASSERT(g_pContext);
	if(errCode < SCE_OK){
		return errCode;
	}

	// set up render target parameters
	memset(&g_renderTargetParams, 0, sizeof(SceGxmRenderTargetParams));
	g_renderTargetParams.flags					= 0;
	g_renderTargetParams.width					= DISPLAY_WIDTH;
	g_renderTargetParams.height					= DISPLAY_HEIGHT;
	g_renderTargetParams.scenesPerFrame			= 1;
	g_renderTargetParams.multisampleMode		= MSAA_MODE;
	g_renderTargetParams.multisampleLocations	= 0;
	g_renderTargetParams.driverMemBlock			= SCE_UID_INVALID_UID;

	// create the render target with the above parameters
	errCode = sceGxmCreateRenderTarget(&g_renderTargetParams, &g_pRenderTarget);
	SCE_DBG_ASSERT(errCode == SCE_OK);
	if(errCode < SCE_OK){
		return errCode;
	}

	// set buffer sizes for this sample
	const uint32_t patcherBufferSize		= 64*1024;
	const uint32_t patcherVertexUsseSize 	= 64*1024;
	const uint32_t patcherFragmentUsseSize 	= 64*1024;
	
	// allocate memory for buffers and USSE code
	void *patcherBuffer = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
		patcherBufferSize,
		4, 
		SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
		&g_patcherBufferUid);
	if(patcherBuffer == NULL){
		return -1;
	}

	uint32_t patcherVertexUsseOffset;
	void *patcherVertexUsse = vertexUsseAlloc(
		patcherVertexUsseSize,
		&g_patcherVertexUsseUid,
		&patcherVertexUsseOffset);
	if(patcherVertexUsse == NULL){
		return -1;
	}

	uint32_t patcherFragmentUsseOffset;
	void *patcherFragmentUsse = fragmentUsseAlloc(
		patcherFragmentUsseSize,
		&g_patcherFragmentUsseUid,
		&patcherFragmentUsseOffset);
	if(patcherFragmentUsse == NULL){
		return -1;
	}

	// create a shader patcher
	memset(&g_patcherParams, 0, sizeof(SceGxmShaderPatcherParams));
	g_patcherParams.userData					= NULL;
	g_patcherParams.hostAllocCallback			= &patcherHostAlloc;
	g_patcherParams.hostFreeCallback			= &patcherHostFree;
	g_patcherParams.bufferAllocCallback			= NULL;
	g_patcherParams.bufferFreeCallback			= NULL;
	g_patcherParams.bufferMem					= patcherBuffer;
	g_patcherParams.bufferMemSize				= patcherBufferSize;
	g_patcherParams.vertexUsseAllocCallback		= NULL;
	g_patcherParams.vertexUsseFreeCallback		= NULL;
	g_patcherParams.vertexUsseMem				= patcherVertexUsse;
	g_patcherParams.vertexUsseMemSize			= patcherVertexUsseSize;
	g_patcherParams.vertexUsseOffset			= patcherVertexUsseOffset;
	g_patcherParams.fragmentUsseAllocCallback	= NULL;
	g_patcherParams.fragmentUsseFreeCallback	= NULL;
	g_patcherParams.fragmentUsseMem				= patcherFragmentUsse;
	g_patcherParams.fragmentUsseMemSize			= patcherFragmentUsseSize;
	g_patcherParams.fragmentUsseOffset			= patcherFragmentUsseOffset;

	// allocate memory and sync objects for display buffers
	for (uint32_t i = 0; i < DISPLAY_BUFFER_COUNT; ++i) {
		// allocate memory with large (1MiB) alignment to ensure physical contiguity
		g_pDisplayBufferData[i] = graphicsAlloc(
			SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA,
			ALIGN(4*DISPLAY_STRIDE_IN_PIXELS*DISPLAY_HEIGHT, 1*1024*1024),
			SCE_GXM_COLOR_SURFACE_ALIGNMENT,
			SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
			&g_displayBufferUid[i]);
		if(g_pDisplayBufferData[i] == NULL){
			return -1;
		}

		// memset the buffer to a noticeable debug color
		for (uint32_t y = 0; y < DISPLAY_HEIGHT; ++y) {
			uint32_t *row = (uint32_t *)g_pDisplayBufferData[i] + y*DISPLAY_STRIDE_IN_PIXELS;
			for (uint32_t x = 0; x < DISPLAY_WIDTH; ++x) {
				row[x] = 0xffff00ff;
			}
		}

		// initialize a color surface for this display buffer
		errCode = sceGxmColorSurfaceInit(
			&g_displaySurface[i],
			DISPLAY_COLOR_FORMAT,
			SCE_GXM_COLOR_SURFACE_LINEAR,
			(MSAA_MODE == SCE_GXM_MULTISAMPLE_NONE) ? SCE_GXM_COLOR_SURFACE_SCALE_NONE : SCE_GXM_COLOR_SURFACE_SCALE_MSAA_DOWNSCALE,
			SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
			DISPLAY_WIDTH,
			DISPLAY_HEIGHT,
			DISPLAY_STRIDE_IN_PIXELS,
			g_pDisplayBufferData[i]);
		SCE_DBG_ASSERT(errCode == SCE_OK);
		if(errCode < SCE_OK){
			return errCode;
		}

		// create a sync object that we will associate with this buffer
		errCode = sceGxmSyncObjectCreate(&g_pDisplayBufferSync[i]);
		SCE_DBG_ASSERT(errCode == SCE_OK);
		if(errCode < SCE_OK){
			return errCode;
		}
	}
	{
		g_pDepthBufferData = graphicsAlloc(
			SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA,
			ALIGN(4*DISPLAY_STRIDE_IN_PIXELS*DISPLAY_HEIGHT, 1*1024*1024),
			SCE_GXM_DEPTHSTENCIL_SURFACE_ALIGNMENT,
			SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
			&g_depthBufferUid);
		if(g_pDepthBufferData == NULL){
			return -1;
		}

		errCode = sceGxmDepthStencilSurfaceInit(
			&g_depthSurface,
			SCE_GXM_DEPTH_STENCIL_FORMAT_S8D24,
			SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
			DISPLAY_STRIDE_IN_PIXELS,
			g_pDepthBufferData,
			NULL);
		SCE_DBG_ASSERT(errCode == SCE_OK);
		if(errCode < SCE_OK){
			return errCode;
		}
	}
	// create a shader patcher
	errCode = sceGxmShaderPatcherCreate(&g_patcherParams, &g_pShaderPatcher);
	SCE_DBG_ASSERT(SCE_OK == errCode);
	SCE_DBG_ASSERT(g_pShaderPatcher);
	if(errCode < SCE_OK){
		return errCode;
	}

	return errCode;
}

// Main update function - process controllers, update objects
void update(void)
{	
	// Process controller input
	processController();
	
	if (g_showMenu)
	{
		updateDebugMenu();
	}

	if (!isPaused())
	{
		// Update AI
		// Foo();

		// Update object positions
		// Bar();
	}

	sampleUpdate();
}

// Update controller state
void processController(void)
{
	// Handle inputs etc.
	sceCtrlReadBufferPositive(0, &g_controllerData, 1);

	// Pause or resume if START button pressed
	if (g_controllerData.buttons & SCE_CTRL_START)
	{   
		handlePause();
	}

	// Toggle menu if TRIANGLE button pressed
	if (g_controllerData.buttons & SCE_CTRL_TRIANGLE)
	{
		g_showMenu = !g_showMenu;
	}
}

// Function to update menu state based on controller input
void updateDebugMenu(void)
{
	// Process buttons to update menu state
}

// Main render function
void render(void)
{
	// Construct and draw 3D scene
	// ...

	sampleRender();

	// Draw 2D elements
	renderProfileData();
	renderDebugText();
	renderDebugMenu();

	if (g_showMenu)
	{
		//renderMenu();
	}
}

// Profile data rendering function
void renderProfileData(void)
{
	// Draws the profiler bars
}

// Debug text rendering function
void renderDebugText(void)
{
	// Draws the debug text
}

// Debug menu rendering function
void renderDebugMenu(void)
{
	// Draws on screen menu 
}

// Cleanup memory, free resources, shutdown
int terminate(void)
{
	int returnCode = SCE_OK;

	// Clean up 
	// returnCode = FooCleanupFunction();

	sampleRelease();

	returnCode = terminateGxm();

	return returnCode;
}

// Shutdown libgxm and the graphics display services
int terminateGxm()
{
	int returnCode = SCE_OK;

	// wait until rendering is done
	if(g_pContext != NULL){
		returnCode = sceGxmFinish(g_pContext);
		SCE_DBG_ASSERT(returnCode == SCE_OK);
	}

	// wait until display queue is finished before deallocating display buffers
	returnCode = sceGxmDisplayQueueFinish();
	SCE_DBG_ASSERT(returnCode == SCE_OK);

	// destroy the shader patcher
	if(g_pShaderPatcher != NULL){
		returnCode = sceGxmShaderPatcherDestroy(g_pShaderPatcher);
		SCE_DBG_ASSERT(returnCode == SCE_OK);
		g_pShaderPatcher = NULL;
	}

	if(g_depthBufferUid > 0){
		graphicsFree(g_depthBufferUid);
		g_pDepthBufferData = NULL;
		g_depthBufferUid = 0;
	}

	// clean up display queue
	for (uint32_t i = 0; i < DISPLAY_BUFFER_COUNT; ++i) {
		// clear the buffer then deallocate
		if(g_displayBufferUid[i] > 0){
			memset(g_pDisplayBufferData[i], 0, DISPLAY_HEIGHT*DISPLAY_STRIDE_IN_PIXELS*4);
			graphicsFree(g_displayBufferUid[i]);
			g_pDisplayBufferData[i] = NULL;
			g_displayBufferUid[i] = NULL;
		}
		// destroy the sync object
		if(g_pDisplayBufferSync[i] != NULL){
			returnCode = sceGxmSyncObjectDestroy(g_pDisplayBufferSync[i]);
			SCE_DBG_ASSERT(returnCode == SCE_OK);
			g_pDisplayBufferSync[i] = NULL;
		}
	}

	if(g_patcherFragmentUsseUid > 0){
		fragmentUsseFree(g_patcherFragmentUsseUid);
		g_patcherFragmentUsseUid = 0;
	}
	if(g_patcherVertexUsseUid > 0){
		vertexUsseFree(g_patcherVertexUsseUid);
		g_patcherVertexUsseUid = 0;
	}
	if(g_patcherBufferUid > 0){
		graphicsFree(g_patcherBufferUid);
		g_patcherBufferUid = 0;
	}

	// destroy render target
	if(g_pRenderTarget != NULL){
		returnCode = sceGxmDestroyRenderTarget(g_pRenderTarget);
		SCE_DBG_ASSERT(returnCode == SCE_OK);
		g_pRenderTarget = NULL;
	}

	// destroy the rendering context
	if(g_pContext != NULL){
		returnCode = sceGxmDestroyContext(g_pContext);
		SCE_DBG_ASSERT(returnCode == SCE_OK);
		g_pContext = NULL;
	}
	if(g_ctxParams.hostMem != NULL){
		free(g_ctxParams.hostMem);
		g_ctxParams.hostMem = NULL;
	}
	if(g_fragmentUsseRingBufferUid > 0){
		fragmentUsseFree(g_fragmentUsseRingBufferUid);
		g_fragmentUsseRingBufferUid = 0;
	}
	if(g_fragmentRingBufferUid > 0){
		graphicsFree(g_fragmentRingBufferUid);
		g_fragmentRingBufferUid = 0;
	}
	if(g_vertexRingBufferUid > 0){
		graphicsFree(g_vertexRingBufferUid);
		g_vertexRingBufferUid = 0;
	}
	if(g_vdmRingBufferUid > 0){
		graphicsFree(g_vdmRingBufferUid);
		g_vdmRingBufferUid = 0;
	}

	// terminate libgxm
	returnCode = sceGxmTerminate();

	return returnCode;
}

// Handle pause button being pressed (toggle pause and unpaused)
void handlePause(void)
{	
	g_isPaused = !g_isPaused; 

	if (g_isPaused)
	{
		// Processing to perform on pause event 
	}
	else
	{
		// Processing to be performed on resume event
	}
}

// Returns paused or not paused state
bool isPaused(void)
{
	return g_isPaused;
}

// Terminate the program
void quit(void)
{
	g_isRunning = false;
}

// Returns running or terminated state
bool isRunning(void)
{
	return g_isRunning;
}

// Callback function for flip operation
void displayCallback(const void *callbackData)
{
	SceDisplayFrameBuf framebuf;
	int err = SCE_OK;
	TOUCH(err);

	// Cast the parameters back
	const DisplayData *displayData = (const DisplayData *)callbackData;

	sampleDisplayCallback(displayData);

	// Swap to the new buffer on the next VSYNC
	memset(&framebuf, 0x00, sizeof(SceDisplayFrameBuf));
	framebuf.size        = sizeof(SceDisplayFrameBuf);
	framebuf.base        = displayData->address;
	framebuf.pitch       = DISPLAY_STRIDE_IN_PIXELS;
	framebuf.pixelformat = DISPLAY_PIXEL_FORMAT;
	framebuf.width       = DISPLAY_WIDTH;
	framebuf.height      = DISPLAY_HEIGHT;
	err = sceDisplaySetFrameBuf(&framebuf, SCE_DISPLAY_UPDATETIMING_NEXTVSYNC);
	SCE_DBG_ASSERT(err == SCE_OK);

	// Block this callback until the swap has occurred and the old buffer
	// is no longer displayed
	err = sceDisplayWaitVblankStart();
	SCE_DBG_ASSERT(err == SCE_OK);
}

// Allocation callback for the libgxm shader patcher
static void *patcherHostAlloc(void *userData, uint32_t size)
{
	UNUSED(userData);
	return malloc(size);
}

// Free callback for the libgxm shader patcher
static void patcherHostFree(void *userData, void *mem)
{
	UNUSED(userData);
	free(mem);
}

static void *graphicsAlloc(SceKernelMemBlockType type, uint32_t size, uint32_t alignment, uint32_t attribs, SceUID *uid)
{
	int err = SCE_OK;
	TOUCH(err);
	TOUCH(alignment);

	if (type == SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA) {
		// CDRAM memblocks must be 256KiB aligned
		SCE_DBG_ASSERT(alignment <= 256*1024);
		size = ALIGN(size, 256*1024);

	} else if((type == SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_RW) || (type == SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_NC_RW)) {
		// must be 1024KiB aligned
		SCE_DBG_ASSERT(alignment <= 1024*1024);
		size = ALIGN(size, 1024*1024);

	} else {
		// LPDDR memblocks must be 4KiB aligned
		SCE_DBG_ASSERT(alignment <= 4*1024);
		size = ALIGN(size, 4*1024);
	}

	// allocate some memory
	*uid = sceKernelAllocMemBlock("basic", type, size, NULL);
	SCE_DBG_ASSERT(*uid >= SCE_OK);
	if(*uid < SCE_OK){
		return NULL;
	}

	// grab the base address
	void *mem = NULL;
	err = sceKernelGetMemBlockBase(*uid, &mem);
	SCE_DBG_ASSERT(err == SCE_OK);
	if(err < SCE_OK){
		err = sceKernelFreeMemBlock(*uid);
		SCE_DBG_ASSERT(err == SCE_OK);
		return NULL;
	}

	// map for the GPU
	err = sceGxmMapMemory(mem, size, attribs);
	SCE_DBG_ASSERT(err == SCE_OK);
	if(err < SCE_OK){
		err = sceKernelFreeMemBlock(*uid);
		SCE_DBG_ASSERT(err == SCE_OK);
		return NULL;
	}

	// done
	return mem;
}

static void graphicsFree(SceUID uid)
{
	int err = SCE_OK;
	TOUCH(err);

	// grab the base address
	void *mem = NULL;
	err = sceKernelGetMemBlockBase(uid, &mem);
	SCE_DBG_ASSERT(err == SCE_OK);

	// unmap memory
	err = sceGxmUnmapMemory(mem);
	SCE_DBG_ASSERT(err == SCE_OK);

	// free the memory block
	err = sceKernelFreeMemBlock(uid);
	SCE_DBG_ASSERT(err == SCE_OK);
}

static void *vertexUsseAlloc(uint32_t size, SceUID *uid, uint32_t *usseOffset)
{
	int err = SCE_OK;
	TOUCH(err);

	// align to memblock alignment for LPDDR
	size = ALIGN(size, 4096);
	
	// allocate some memory
	*uid = sceKernelAllocMemBlock("basic", SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, size, NULL);
	SCE_DBG_ASSERT(*uid >= SCE_OK);
	if(*uid < SCE_OK){
		return NULL;
	}

	// grab the base address
	void *mem = NULL;
	err = sceKernelGetMemBlockBase(*uid, &mem);
	SCE_DBG_ASSERT(err == SCE_OK);
	if(err < SCE_OK){
		err = sceKernelFreeMemBlock(*uid);
		SCE_DBG_ASSERT(err == SCE_OK);
		return NULL;
	}

	// map as vertex USSE code for the GPU
	err = sceGxmMapVertexUsseMemory(mem, size, usseOffset);
	SCE_DBG_ASSERT(err == SCE_OK);
	if(err < SCE_OK){
		err = sceKernelFreeMemBlock(*uid);
		SCE_DBG_ASSERT(err == SCE_OK);
		return NULL;
	}

	// done
	return mem;
}

static void vertexUsseFree(SceUID uid)
{
	int err = SCE_OK;
	TOUCH(err);

	// grab the base address
	void *mem = NULL;
	err = sceKernelGetMemBlockBase(uid, &mem);
	SCE_DBG_ASSERT(err == SCE_OK);

	// unmap memory
	err = sceGxmUnmapVertexUsseMemory(mem);
	SCE_DBG_ASSERT(err == SCE_OK);

	// free the memory block
	err = sceKernelFreeMemBlock(uid);
	SCE_DBG_ASSERT(err == SCE_OK);
}

static void *fragmentUsseAlloc(uint32_t size, SceUID *uid, uint32_t *usseOffset)
{
	int err = SCE_OK;
	TOUCH(err);

	// align to memblock alignment for LPDDR
	size = ALIGN(size, 4096);
	
	// allocate some memory
	*uid = sceKernelAllocMemBlock("basic", SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE, size, NULL);
	SCE_DBG_ASSERT(*uid >= SCE_OK);
	if(*uid < SCE_OK){
		return NULL;
	}

	// grab the base address
	void *mem = NULL;
	err = sceKernelGetMemBlockBase(*uid, &mem);
	SCE_DBG_ASSERT(err == SCE_OK);
	if(err < SCE_OK){
		err = sceKernelFreeMemBlock(*uid);
		SCE_DBG_ASSERT(err == SCE_OK);
		return NULL;
	}

	// map as fragment USSE code for the GPU
	err = sceGxmMapFragmentUsseMemory(mem, size, usseOffset);
	SCE_DBG_ASSERT(err == SCE_OK);
	if(err < SCE_OK){
		err = sceKernelFreeMemBlock(*uid);
		SCE_DBG_ASSERT(err == SCE_OK);
		return NULL;
	}

	// done
	return mem;
}

static void fragmentUsseFree(SceUID uid)
{
	int err = SCE_OK;
	TOUCH(err);

	// grab the base address
	void *mem = NULL;
	err = sceKernelGetMemBlockBase(uid, &mem);
	SCE_DBG_ASSERT(err == SCE_OK);

	// unmap memory
	err = sceGxmUnmapFragmentUsseMemory(mem);
	SCE_DBG_ASSERT(err == SCE_OK);

	// free the memory block
	err = sceKernelFreeMemBlock(uid);
	SCE_DBG_ASSERT(err == SCE_OK);
}




const char		sceUserMainThreadName[]		= "sample_smart";
int				sceUserMainThreadPriority	= SCE_KERNEL_DEFAULT_PRIORITY_USER;
unsigned int	sceUserMainThreadStackSize	= SCE_KERNEL_STACK_SIZE_DEFAULT_USER_MAIN;


typedef struct{
	float x;
	float y;
	float z;
	float w;
	uint32_t col;
}VertexC;

typedef struct{
	float x;
	float y;
	float z;
	float w;
	float u;
	float v;
}VertexT;

#define SET_XYZW(_a, _x, _y, _z, _w)		(_a).x = (_x), (_a).y = (_y), (_a).z = (_z), (_a).w = (_w)
#define SET_UV(_a, _u, _v)					(_a).u = (_u), (_a).v = (_v)

struct CameraBuffer{
	CameraBuffer* prev;
	CameraBuffer* next;
	void* buf;
	SceUID uID;
	SceCameraRead read;
	SceGxmTexture tex_yuv;

	MyAtomic32 ref;

	SceMotionSensorState motion[SCE_MOTION_MAX_NUM_STATES];
};

typedef struct AppData AppData;

struct AppData{
	AppData* prev;
	AppData* next;

	UInt ref;

	SceUInt32 frame;
	CameraBuffer* currentImage;

	struct{
		MyMatrix viewMat;
		MyMatrix projMat;
	}world;

	struct{
		VertexC* vtx;
		SceUID vUID;
		UInt vnum;
	}lines;

	struct{
		VertexC* vtx;
		SceUID vUID;
		UInt vnum;
	}trianglesCol;

	struct{
		VertexT* vtx;
		SceUID vUID;
		UInt vnum;
	}trianglesTex;

	struct{
		char text[1024];
		char statusLine[256];
	}debug;
};

struct SaveDataStream{
	char* buf;
	size_t size;
	size_t wpos;
	size_t rpos;
	char path[1024];
};

typedef struct{
	struct{
		struct{
			struct{
				SceGxmVertexProgram* vp;
				SceGxmShaderPatcherId vpId;
				SceGxmFragmentProgram* fp;
				SceGxmShaderPatcherId fpId;
			}color;
			struct{
				SceGxmVertexProgram* vp;
				SceGxmShaderPatcherId vpId;
				SceGxmFragmentProgram* fp;
				SceGxmShaderPatcherId fpId;
			}tex;
		}shader;

		struct{
			struct{
				SceUInt16* iDat;
				SceUID iUID;
			}index;
			struct{
				VertexC* vDat;
				SceUID vUID;
			}clear;
		}vertex;
	}gxm;

	struct{
		Bool volatile exit;
		MyThread thread;
		MyEvent evt;
	}vsync;

	struct{
		SceCameraInfo info;
		CameraBuffer db[CAMERA_BUFFER_COUNT];
		CameraBuffer* head;
		CameraBuffer* tail;

		MyMutex mtx;
		int initStep;
	}camera;

	struct{
		int initStep;
	}motion;

	struct{
		SceCtrlData data;
		SceUInt32 raw;
		SceUInt32 trg;
		float lx;
		float ly;
		float rx;
		float ry;
	}ctrl;

	struct{
		struct{
			UInt vcount;
			float fps;
			struct{
				SceUInt32 pt;
			}update;
			struct{
				SceUInt32 pt;
			}render;
			char statusLine[256];
		}debug;

		struct{
			SceUInt32 t1;
			SceUInt32 frame;
		}time;

		struct{
			MyMutex mtx;
			AppData buf[APP_DATA_COUNT];
			AppData* head;
			AppData* tail;
			int initStep;
		}data;

		struct{
			AppData* current;
		}update;

		struct{
			AppData* data;
			MyEvent flip_evt;
		}render;
	}app;

	struct{
		SaveDataStream stream;
	}savedata;

	int initStep;
}SampleState;

static SampleState s_sampleState;

static int _appDataInitDat(AppData* dat){
	//SampleState* const s = &s_sampleState;
	{
		MyMatrix pm;
		MyMatrix sm;
		myMatrixPerspective(&pm, PROJECTION_FOV, (float)PROJECTION_WIDTH / (float)PROJECTION_HEIGHT, PROJECTION_ZNEAR, PROJECTION_ZFAR);
		myMatrixScaling(&sm, VIEW_SCALE_H, VIEW_SCALE_V, 1.0f);
		myMatrixMul(&dat->world.projMat, &pm, &sm);
		myMatrixIdentity(&dat->world.viewMat);
	}
	{
		dat->lines.vtx = (VertexC*)graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA, sizeof(VertexC) * LINES_VERTEX_SIZE, 4, SCE_GXM_MEMORY_ATTRIB_READ, &dat->lines.vUID);
		if(dat->lines.vtx == NULL){
			return -1;
		}
		memset(dat->lines.vtx, 0, sizeof(VertexC) * LINES_VERTEX_SIZE);
	}
	{
		dat->trianglesCol.vtx = (VertexC*)graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA, sizeof(VertexC) * TRIANGLES_COL_VERTEX_SIZE, 4, SCE_GXM_MEMORY_ATTRIB_READ, &dat->trianglesCol.vUID);
		if(dat->trianglesCol.vtx == NULL){
			return -1;
		}
		memset(dat->trianglesCol.vtx, 0, sizeof(VertexC) * TRIANGLES_COL_VERTEX_SIZE);
	}
	{
		dat->trianglesTex.vtx = (VertexT*)graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA, sizeof(VertexT) * TRIANGLES_TEX_VERTEX_SIZE, 4, SCE_GXM_MEMORY_ATTRIB_READ, &dat->trianglesTex.vUID);
		if(dat->trianglesTex.vtx == NULL){
			return -1;
		}
		memset(dat->trianglesTex.vtx, 0, sizeof(VertexT) * TRIANGLES_TEX_VERTEX_SIZE);
	}
	return SCE_OK;
}
static void _appDataReleaseDat(AppData* dat){
	//SampleState* const s = &s_sampleState;
	if(dat->trianglesTex.vUID > 0){
		graphicsFree(dat->trianglesTex.vUID);
		dat->trianglesTex.vUID = -1;
		dat->trianglesTex.vtx = NULL;
	}
	if(dat->trianglesCol.vUID > 0){
		graphicsFree(dat->trianglesCol.vUID);
		dat->trianglesCol.vUID = -1;
		dat->trianglesCol.vtx = NULL;
	}
	if(dat->lines.vUID > 0){
		graphicsFree(dat->lines.vUID);
		dat->lines.vUID = -1;
		dat->lines.vtx = NULL;
	}
}

static int _appDataInit(void){
	SampleState* const s = &s_sampleState;
	UInt i;
	int returnCode;
	returnCode = myMutexCreate(&s->app.data.mtx);
	SCE_DBG_ASSERT(returnCode >= SCE_OK);
	if(returnCode < SCE_OK){
		return returnCode;
	}
	s->app.data.initStep = 1;
	{
		s->app.data.head = &s->app.data.buf[0];
		s->app.data.buf[0].prev = NULL;
		for(i = 1; i < bulkof(s->app.data.buf); i++){
			s->app.data.buf[i].prev = &s->app.data.buf[i - 1];
			s->app.data.buf[i - 1].next = &s->app.data.buf[i];
		}
		s->app.data.buf[i - 1].next = NULL;
		s->app.data.tail = &s->app.data.buf[i - 1];
	}

	for(i = 0; i < bulkof(s->app.data.buf); i++){
		returnCode = _appDataInitDat(&s->app.data.buf[i]);
		SCE_DBG_ASSERT(returnCode >= SCE_OK);
		if(returnCode < SCE_OK){
			return returnCode;
		}
	}
	s->app.data.initStep = 2;
	return SCE_OK;
}


static void _appDataRelease(void){
	SampleState* const s = &s_sampleState;
	UInt i;
	for(i = 0; i < bulkof(s->app.data.buf); i++){
		_appDataReleaseDat(&s->app.data.buf[i]);
	}
	if(s->app.data.initStep >= 1){
		myMutexRelease(&s->app.data.mtx);
	}
}

static AppData* _appDataReserve(void){
	SampleState* const s = &s_sampleState;
	AppData* dat;
	myMutexLock(&s->app.data.mtx);
	{
		dat = s->app.data.tail;
		while(dat != NULL){
			if(dat->ref == 0){
				break;
			}
			dat = dat->prev;
		}
		if(dat != NULL){
			if(dat->next != NULL){
				dat->next->prev = dat->prev;
			}
			if(dat->prev != NULL){
				dat->prev->next = dat->next;
			}
			if(dat == s->app.data.head){
				s->app.data.head = dat->next;
			}
			if(dat == s->app.data.tail){
				s->app.data.tail = dat->prev;
			}
			dat->next = NULL;
			dat->prev = NULL;
		}
	}
	myMutexUnlock(&s->app.data.mtx);
	return dat;
}

static void _appDataRegist(AppData* dat){
	SampleState* const s = &s_sampleState;
	myMutexLock(&s->app.data.mtx);
	{
		ASSERT(dat != NULL);
		ASSERT(dat->prev == NULL);
		ASSERT(dat->next == NULL);
		ASSERT(s->app.data.head != NULL);
		ASSERT(s->app.data.head->prev == NULL);
		ASSERT(s->app.data.tail != NULL);
		ASSERT(s->app.data.tail->next == NULL);
		dat->next = s->app.data.head;
		s->app.data.head->prev = dat;
		s->app.data.head = dat;
	}
	myMutexUnlock(&s->app.data.mtx);
}

static AppData* _appDataLockHead(void){
	SampleState* const s = &s_sampleState;
	AppData* dat;
	myMutexLock(&s->app.data.mtx);
	{
		dat = s->app.data.head;
		ASSERT(dat->ref == 0);
		dat->ref++;
	}
	myMutexUnlock(&s->app.data.mtx);
	return dat;
}
static void _appDataUnlockData(AppData* dat){
	SampleState* const s = &s_sampleState;
	myMutexLock(&s->app.data.mtx);
	{
		ASSERT(dat->ref > 0);
		dat->ref--;
	}
	myMutexUnlock(&s->app.data.mtx);
}


static CameraBuffer* _cameraReserveBuffer(void){
	SampleState* const s = &s_sampleState;
	CameraBuffer* buf = NULL;
	myMutexLock(&s->camera.mtx);
	{
		buf = s->camera.tail;
		while(buf != NULL){
			if(buf->ref == 0){
				break;
			}
			buf = buf->prev;
		}
		if(buf != NULL){
			if(buf->next != NULL){
				buf->next->prev = buf->prev;
			}
			if(buf->prev != NULL){
				buf->prev->next = buf->next;
			}
			if(buf == s->camera.head){
				s->camera.head = buf->next;
			}
			if(buf == s->camera.tail){
				s->camera.tail = buf->prev;
			}
		}
	}
	myMutexUnlock(&s->camera.mtx);
	return buf;
}

static void _cameraCancelBuffer(CameraBuffer* buf){
	SampleState* const s = &s_sampleState;
	myMutexLock(&s->camera.mtx);
	{
		if(s->camera.tail == NULL){
			ASSERT(s->camera.head == NULL);
			buf->prev = NULL;
			buf->next = NULL;
			s->camera.head = buf;
			s->camera.tail = buf;
		}else{
			ASSERT(s->camera.tail->next == NULL);
			s->camera.tail->next = buf;
			buf->prev = s->camera.tail;
			buf->next = NULL;
			s->camera.tail = buf;
		}
	}
	myMutexUnlock(&s->camera.mtx);
}

static void _cameraRegistBuffer(CameraBuffer* buf){
	SampleState* const s = &s_sampleState;
	myMutexLock(&s->camera.mtx);
	{
		if(s->camera.head == NULL){
			ASSERT(s->camera.tail == NULL);
			buf->prev = NULL;
			buf->next = NULL;
			s->camera.head = buf;
			s->camera.tail = buf;
		}else{
			ASSERT(s->camera.head->prev == NULL);
			s->camera.head->prev = buf;
			buf->prev = NULL;
			buf->next = s->camera.head;
			s->camera.head = buf;
		}
	}
	myMutexUnlock(&s->camera.mtx);
}

CameraBuffer* cameraGetLatestImage(void){
	SampleState* const s = &s_sampleState;
	CameraBuffer* buf = NULL;
	myMutexLock(&s->camera.mtx);
	{
		buf = s->camera.head;
		if(buf != NULL){
#if 1
			myAtomic32Inc(&buf->ref);
#else
			buf->ref++;
#endif
		}
	}
	myMutexUnlock(&s->camera.mtx);
	return buf;
}

void cameraBufferAddRef(CameraBuffer* buf){
	//SampleState* const s = &s_sampleState;
	if(buf == NULL){
		return;
	}
	ASSERT(buf->ref > 0);
#if 1
	myAtomic32Inc(&buf->ref);
#else
	myMutexLock(&s->camera.mtx);
	{
		buf->ref++;
	}
	myMutexUnlock(&s->camera.mtx);
#endif
}

void cameraBufferRelease(CameraBuffer* buf){
	//SampleState* const s = &s_sampleState;
	ASSERT(buf->ref > 0);
#if 1
	myAtomic32Dec(&buf->ref);
#else
	myMutexLock(&s->camera.mtx);
	{
		buf->ref_count--;
	}
	myMutexUnlock(&s->camera.mtx);
#endif
}


int cameraInitialize(void){
	SampleState* const s = &s_sampleState;
	int returnCode;
	UInt i;
	UInt uv_size;

	memset(&s->camera.info, 0, sizeof(s->camera.info));
	s->camera.info.sizeThis = sizeof(SceCameraInfo);
	s->camera.info.wPriority = SCE_CAMERA_PRIORITY_SHARE;
	s->camera.info.wFormat = CAMERA_FORMAT;
	s->camera.info.wResolution = CAMERA_RESOLUTION;
	s->camera.info.wFramerate = CAMERA_FRAMERATE;
	s->camera.info.wRange = SCE_CAMERA_DATA_RANGE_FULL;
	s->camera.info.wBuffer = SCE_CAMERA_BUFFER_SETBYREAD;
	s->camera.info.wPitch = 0;
	uv_size = CAMERA_WIDTH * CAMERA_HEIGHT / 4;

	for(i = 0; i < CAMERA_BUFFER_COUNT; i++){
		memset(&s->camera.db[i].read, 0, sizeof(s->camera.db[i].read));
		s->camera.db[i].read.sizeThis = sizeof(SceCameraRead);
		s->camera.db[i].read.dwMode = SCE_CAMERA_READ_MODE_WAIT_NEXTFRAME_ON;
		s->camera.db[i].read.dwExposureTimeMode = SCE_CAMERA_READ_GET_EXPOSURE_TIME_OFF;
		s->camera.db[i].read.sizeIBase = CAMERA_WIDTH * CAMERA_HEIGHT;
		s->camera.db[i].read.sizeUBase = uv_size;
		s->camera.db[i].read.sizeVBase = uv_size;
		//s->camera.db[i].buf = graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_RW, CAMERA_WIDTH * CAMERA_HEIGHT + uv_size * 2, SCE_GXM_TEXTURE_ALIGNMENT, SCE_GXM_MEMORY_ATTRIB_READ, &s->camera.db[i].uID);
		s->camera.db[i].buf = graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA, CAMERA_WIDTH * CAMERA_HEIGHT + uv_size * 2, SCE_GXM_TEXTURE_ALIGNMENT, SCE_GXM_MEMORY_ATTRIB_READ, &s->camera.db[i].uID);
		if(s->camera.db[i].buf == NULL){
			return -1;
		}
		memset(s->camera.db[i].buf, 0, CAMERA_WIDTH * CAMERA_HEIGHT + uv_size * 2);
		{
			void* base = (void*)((UIntPtr)s->camera.db[i].buf);
			s->camera.db[i].read.pvIBase = base;
			s->camera.db[i].read.pvUBase = (void*)((UIntPtr)base + CAMERA_WIDTH * CAMERA_HEIGHT);
			s->camera.db[i].read.pvVBase = (void*)((UIntPtr)base + CAMERA_WIDTH * CAMERA_HEIGHT + uv_size);
		}
		{
			void* base = (void*)((UIntPtr)s->camera.db[i].buf);
			SCECHK(sceGxmTextureInitLinear(&s->camera.db[i].tex_yuv, base, SCE_GXM_TEXTURE_FORMAT_YUV420P3_CSC0, CAMERA_WIDTH, CAMERA_HEIGHT, 0));
		}
		SCECHK(sceGxmTextureSetMinFilter(&s->camera.db[i].tex_yuv, SCE_GXM_TEXTURE_FILTER_LINEAR));
		SCECHK(sceGxmTextureSetMagFilter(&s->camera.db[i].tex_yuv, SCE_GXM_TEXTURE_FILTER_LINEAR));
		SCECHK(sceGxmTextureSetMipFilter(&s->camera.db[i].tex_yuv, SCE_GXM_TEXTURE_MIP_FILTER_DISABLED));
		SCECHK(sceGxmTextureSetUAddrMode(&s->camera.db[i].tex_yuv, SCE_GXM_TEXTURE_ADDR_CLAMP));
		SCECHK(sceGxmTextureSetVAddrMode(&s->camera.db[i].tex_yuv, SCE_GXM_TEXTURE_ADDR_CLAMP));
	}
	{
		CameraBuffer* prv = &s->camera.db[0];
		prv->prev = NULL;
		for(i = 1; i < CAMERA_BUFFER_COUNT; i++){
			CameraBuffer* cur = &s->camera.db[i];
			prv->next = cur;
			cur->prev = prv;
			prv = cur;
		}
		prv->next = NULL;
		s->camera.head = &s->camera.db[0];
		s->camera.tail = &s->camera.db[CAMERA_BUFFER_COUNT - 1];
	}
	s->camera.initStep = 1;

	returnCode = myMutexCreate(&s->camera.mtx);
	SCE_DBG_ASSERT(returnCode >= SCE_OK);
	if(returnCode < SCE_OK){
		return returnCode;
	}
	s->camera.initStep = 2;

#if 1
	returnCode = sceCameraOpen(CAMERA_DEVICE, &s->camera.info);
#else
	returnCode = SCE_CAMERA_ERROR_NOT_MOUNTED;
#endif
	//SCE_DBG_ASSERT(returnCode >= SCE_OK);
	if(returnCode < SCE_OK){
		return returnCode;
	}
	s->camera.initStep = 3;

	SCECHK(sceCameraStart(CAMERA_DEVICE));

	SCECHK(sceCameraSetAutoControlHold(CAMERA_DEVICE, SCE_CAMERA_ATTRIBUTE_AUTOCONTROLHOLD_OFF));

	SCECHK(sceCameraSetISO(CAMERA_DEVICE, CAMERA_ATTRIBUTE_ISO));

	SCECHK(sceCameraSetWhiteBalance(CAMERA_DEVICE, CAMERA_ATTRIBUTE_WHITEBALANCE));

	//SCECHK(sceCameraSetNightmode(CAMERA_DEVICE,SCE_CAMERA_ATTRIBUTE_NIGHTMODE_LESS10));

	s->camera.initStep = 4;
	return SCE_OK;
}

void cameraRelease(void){
	SampleState* const s = &s_sampleState;
	UInt i;

	if(s->camera.initStep >= 4){
		SCECHK(sceCameraStop(CAMERA_DEVICE));
	}
	if(s->camera.initStep >= 3){
		SCECHK(sceCameraClose(CAMERA_DEVICE));
	}

	if(s->camera.initStep >= 2){
		myMutexRelease(&s->camera.mtx);
	}

	if(s->camera.initStep >= 1){
		for(i = 0; i < CAMERA_BUFFER_COUNT; i++){
			if(s->camera.db[i].uID > 0){
				graphicsFree(s->camera.db[i].uID);
				s->camera.db[i].uID = -1;
				s->camera.db[i].buf = NULL;
			}
		}
	}
	memset(&s->camera.info, 0, sizeof(s->camera.info));
}

Bool cameraUpdate(void){
	SampleState* const s = &s_sampleState;
	CameraBuffer* buf = NULL;
	TOUCH(s);

	buf = _cameraReserveBuffer();
	ASSERT(buf != NULL);
	if(buf == NULL){
		return FALSE;
	}

	{
		int res;
		res = sceCameraRead(CAMERA_DEVICE, &buf->read);

		if(res != SCE_OK){
			DEBUG_PRINTF("sceCameraRead() = %08x\n", res);
			_cameraCancelBuffer(buf);
			return FALSE;
		}
		if(buf->read.dwStatus < SCE_CAMERA_STATUS_IS_ACTIVE){
			DEBUG_PRINTF("dwStatus < SCE_CAMERA_STATUS_IS_ACTIVE: %08x\n", buf->read.dwStatus);
			_cameraCancelBuffer(buf);
			return FALSE;
		}
#if 1
		if((buf->read.dwStatus != SCE_CAMERA_STATUS_IS_ACTIVE) && (buf->read.dwStatus != SCE_CAMERA_STATUS_IS_NOT_STABLE)){
			//DEBUG_PRINTF("dwStatus: %08x: drop\n", buf->read.dwStatus);
			_cameraCancelBuffer(buf);
			return FALSE;
		}
#endif

		SCECHK(sceMotionGetSensorState(buf->motion, bulkof(buf->motion)));

		_cameraRegistBuffer(buf);
	}
	return TRUE;
}

int saveDataStreamInitialize(void){
	SampleState* const s = &s_sampleState;
	SaveDataStream* stream = &s->savedata.stream;
	memset(&s->savedata, 0, sizeof(s->savedata));
	stream->buf = (char*)malloc(SAVEDATA_SIZE);
	if(stream->buf == NULL){
		return -1;
	}
	ASSERT(stream->buf != NULL);
	stream->size = SAVEDATA_SIZE;
	stream->rpos = 0;
	stream->wpos = 0;
	stream->path[0] = '\0';
	return SCE_OK;
}
void saveDataStreamRelease(void){
	SampleState* const s = &s_sampleState;
	SaveDataStream* stream = &s->savedata.stream;
	if(stream->buf != NULL){
		free(stream->buf);
	}
	stream->buf = NULL;
	memset(&s->savedata, 0, sizeof(s->savedata));
}
SaveDataStream* sampleSaveDataStreamOpen(char const* path, Bool read){
	SampleState* const s = &s_sampleState;
	SaveDataStream* stream = &s->savedata.stream;
	ASSERT(stream->path[0] == '\0');
	if(read != FALSE){
#if !defined(ENABLE_SAVEDATA)
		stream->rpos = 0;
		if(stream->wpos == 0){
			return NULL;
		}
#else
		SceInt32 res;
		SceAppUtilSaveDataSlotId slotId = SAVEDATA_SLOT_ID;
		SceAppUtilSaveDataSlotParam slotParam;
		size_t len;
		FILE* fp;

		memset(&slotParam, 0, sizeof(slotParam));
		res = sceAppUtilSaveDataSlotGetParam(slotId, &slotParam, NULL);
		if(res == SCE_APPUTIL_ERROR_NOT_MOUNTED){
			return NULL;
		}
		if(res == SCE_APPUTIL_ERROR_SAVEDATA_SLOT_NOT_FOUND){
			return NULL;
		}
		SCECHK(res);
		if(res < SCE_OK){
			return NULL;
		}
		if(slotParam.status != SCE_APPUTIL_SAVEDATA_SLOT_STATUS_AVAILABLE){
			return NULL;
		}

		fp = fopen(path, "rb");
		if(fp == NULL){
			return NULL;
		}
		len = fread(stream->buf, 1, stream->size, fp);
		if(len == 0){
			return NULL;
		}
		stream->rpos = 0;
		stream->wpos = len;
#endif
		stream->path[0] = '\0';
	}else{
		SceInt32 res;
		SceAppUtilSaveDataSlotId slotId = SAVEDATA_SLOT_ID;
		SceAppUtilSaveDataSlotParam slotParam;

		memset(&slotParam, 0, sizeof(slotParam));
		res = sceAppUtilSaveDataSlotGetParam(slotId, &slotParam, NULL);
		if(res == SCE_APPUTIL_ERROR_NOT_MOUNTED){
			return NULL;
		}
		if((res < SCE_OK) && (res != SCE_APPUTIL_ERROR_SAVEDATA_SLOT_NOT_FOUND)){
			SCECHK(res);
			return NULL;
		}
		stream->wpos = 0;
		snprintf(stream->path, sizeof(stream->path), "%s", path);
	}
	return stream;
}
void sampleSaveDataStreamClose(SaveDataStream* stream){
	SampleState* const s = &s_sampleState;
	TOUCH(s);
	ASSERT(stream == &s->savedata.stream);
	if(stream->path[0] == '\0'){
		return;
	}
#if !defined(ENABLE_SAVEDATA)
	// nop
#else
	{
		SceInt32 res;
		SceAppUtilSaveDataSlotId slotId = SAVEDATA_SLOT_ID;
		SceAppUtilSaveDataSlotParam slotParam;
		SceAppUtilSaveDataDataSaveItem items[1];
		SceSize required;
		SceSize pos;
		char const* path = stream->path;
		char const* p = strchr(path, ':');

		memset(&slotParam, 0, sizeof(slotParam));
		res = sceAppUtilSaveDataSlotGetParam(slotId, &slotParam, NULL);
		if(res == SCE_APPUTIL_ERROR_SAVEDATA_SLOT_NOT_FOUND){
			snprintf(slotParam.title, sizeof(slotParam.title), "api_libsmart");
			snprintf(slotParam.subTitle, sizeof(slotParam.subTitle), "%s", smartSampleName());
			snprintf(slotParam.detail, sizeof(slotParam.detail), "Sample");
			snprintf(slotParam.iconPath, sizeof(slotParam.iconPath), "savedata0:sce_sys/icon0.png");
			slotParam.status = SCE_APPUTIL_SAVEDATA_SLOT_STATUS_BROKEN;
			res = sceAppUtilSaveDataSlotCreate(slotId, &slotParam, NULL);
			SCECHK(res);
			if(res < SCE_OK){
				return;
			}
			res = sceAppUtilSaveDataSlotGetParam(slotId, &slotParam, NULL);
		}
		SCECHK(res);
		if(res < SCE_OK){
			return;
		}

		slotParam.status = SCE_APPUTIL_SAVEDATA_SLOT_STATUS_BROKEN;

		res = sceAppUtilSaveDataSlotSetParam(slotId, &slotParam, NULL);
		SCECHK(res);
		if(res < SCE_OK){
			return;
		}

		ASSERT(stream != NULL);
		ASSERT(stream->buf != NULL);
		memset(&items, 0 ,sizeof(items));
		items[0].dataPath = (SceChar8 const*)((p == NULL) ? path : (p + 1));
		items[0].mode = SCE_APPUTIL_SAVEDATA_DATA_SAVE_MODE_FILE_TRUNCATE;

		pos = 0;
		do{
			SceSize n = stream->wpos - pos;
			Bool last = TRUE;
			if(n > SCE_APPUTIL_SAVEDATA_DATA_SAVE_MAXSIZE){
				n = SCE_APPUTIL_SAVEDATA_DATA_SAVE_MAXSIZE;
				last = FALSE;
			}
			items[0].buf = &stream->buf[pos];
			items[0].bufSize = n;
			items[0].offset = pos;
			if(last != FALSE){
				SceAppUtilSaveDataDataSlot slot;
				memset(&slot, 0, sizeof(slot));
				slot.id = slotId;
				slot.slotParam = &slotParam;
				res = sceAppUtilSaveDataDataSave(&slot, items, bulkof(items), NULL, &required);
			}else{
				res = sceAppUtilSaveDataDataSave(NULL, items, bulkof(items), NULL, &required);
			}
			if(res != SCE_OK){
				DEBUG_PRINTF("sceAppUtilSaveDataDataSave: failed: %08x: %u\n", res, required);
				break;
			}
			items[0].mode = SCE_APPUTIL_SAVEDATA_DATA_SAVE_MODE_FILE;
			pos += n;
		}while(pos != stream->wpos);
	}
#endif
	stream->path[0] = '\0';
}
size_t sampleSaveDataStreamWrite(SaveDataStream* stream, void const* buf, size_t size){
	//SampleState* const s = &s_sampleState;
	size_t n = size;
	ASSERT(stream != NULL);
	ASSERT(stream == &s_sampleState.savedata.stream);
	ASSERT(stream->buf != NULL);
	ASSERT(stream->wpos <= stream->size);
	ASSERT((stream->wpos + n) <= stream->size);	// overflow
	if((stream->wpos + n) > stream->size){
		n = stream->size - stream->wpos;
	}
	memcpy(&stream->buf[stream->wpos], buf, n);
	stream->wpos += n;
	return n;
}
size_t sampleSaveDataStreamRead(SaveDataStream* stream, void* buf, size_t size){
	//SampleState* const s = &s_sampleState;
	size_t n = size;
	ASSERT(stream != NULL);
	ASSERT(stream == &s_sampleState.savedata.stream);
	ASSERT(stream->buf != NULL);
	ASSERT(stream->wpos <= stream->size);
	ASSERT(stream->rpos <= stream->wpos);
	if((stream->rpos + n) > stream->wpos){
		n = stream->wpos - stream->rpos;
	}
	memcpy(buf, &stream->buf[stream->rpos], n);
	stream->rpos += n;
	return n;
}

static DECLARE_THREAD_PROC(_vsyncThread){
	SampleState* const s = &s_sampleState;
	UNUSED_THREAD_ARG();

	DEBUG_PRINTF("%s: start\n", __FUNCTION__);

	for(;;){
		SCECHK(sceDisplayWaitVblankStart());

		if(s->vsync.exit != FALSE){
			break;
		}

#if 1
		s->app.debug.vcount++;
#endif
		myEventRaise(&s->vsync.evt);
	}

	DEBUG_PRINTF("%s: exit\n", __FUNCTION__);

	THREAD_RETURN(0);
}

int vsyncInitialize(void){
	SampleState* const s = &s_sampleState;
	int returnCode;
	memset(&s->vsync, 0, sizeof(s->vsync));
	returnCode = myEventCreate(&s->vsync.evt);
	SCE_DBG_ASSERT(returnCode >= SCE_OK);
	if(returnCode < SCE_OK){
		return returnCode;
	}
	{
		s->vsync.exit = FALSE;
		returnCode = myThreadStartEx(&s->vsync.thread, "vsync", _vsyncThread, 0, VSYNC_THREAD_PRIORITY, VSYNC_THREAD_STACK_SIZE);
		SCE_DBG_ASSERT(returnCode >= SCE_OK);
		if(returnCode < SCE_OK){
			return returnCode;
		}
	}
	return SCE_OK;
}

void vsyncRelease(void){
	SampleState* const s = &s_sampleState;
	s->vsync.exit = TRUE;
	if(myThreadIsValid(&s->vsync.thread) != FALSE){
		myThreadJoin(&s->vsync.thread);
	}
	if(myEventIsValid(&s->vsync.evt) != FALSE){
		myEventRelease(&s->vsync.evt);
	}
	memset(&s->vsync, 0, sizeof(s->vsync));
}

void vsyncWait(void){
	SampleState* const s = &s_sampleState;
	myEventWait(&s->vsync.evt, INFINITE);
}

int motionInitialize(void){
	SampleState* const s = &s_sampleState;
	int returnCode;
	memset(&s->motion, 0, sizeof(s->motion));
	returnCode = sceMotionStartSampling();
	SCE_DBG_ASSERT(returnCode >= SCE_OK);
	if(returnCode < SCE_OK){
		return returnCode;
	}
	s->motion.initStep = 1;
	return SCE_OK;
}

void motionRelease(void){
	SampleState* const s = &s_sampleState;
	if(s->motion.initStep >= 1){
		SCECHK(sceMotionStopSampling());
	}
	memset(&s->motion, 0, sizeof(s->motion));
}

void motionUpdate(void){
	//SampleState* const s = &s_sampleState;
	// -> cameraUpdate
	//SCECHK(sceMotionGetSensorState(s->motion.state, bulkof(s->motion.state)));
}

static int ctrlInitialize(void){
	//SampleState* const s = &s_sampleState;
	int returnCode;
	returnCode = sceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITALANALOG);
	SCE_DBG_ASSERT(returnCode >= SCE_OK);
	if(returnCode < SCE_OK){
		return returnCode;
	}
	return SCE_OK;
}

static void ctrlRelease(void){
	//SampleState* const s = &s_sampleState;
}

static void ctrlAnalogAdjust(float* ox, float* oy, SceUInt8 ix, SceUInt8 iy){
	float tx = ((float)ix - 128.0f) * (1.0f / 128.0f);
	float ty = ((float)iy - 128.0f) * (1.0f / 128.0f);
	float l = sqrtf(tx * tx + ty * ty);
	float r;
	float min = 0.1f;
	float max = 0.9f;
	if(l <= min){
		r = 0.0f;
	}else if(l >= max){
		r = 1.0f / l;
	}else{
		r = (l - min) * (1.0f / (max - min));
	}
	*ox = tx * r;
	*oy = ty * r;
}

void ctrlUpdate(void){
	SampleState* const s = &s_sampleState;
	SCECHK(sceCtrlPeekBufferPositive(0, &s->ctrl.data, 1));
	{
		SceUInt32 prv = s->ctrl.raw;
		SceUInt32 now = s->ctrl.data.buttons;
		s->ctrl.raw = now;
		s->ctrl.trg = (prv ^ now);// & now;
		ctrlAnalogAdjust(&s->ctrl.lx, &s->ctrl.ly, s->ctrl.data.lx, s->ctrl.data.ly);
		ctrlAnalogAdjust(&s->ctrl.rx, &s->ctrl.ry, s->ctrl.data.rx, s->ctrl.data.ry);
	}
}

SceUInt32 sampleGetButtonsRaw(void){
	SampleState* const s = &s_sampleState;
	return s->ctrl.raw;
}
SceUInt32 sampleGetButtonsTrg(void){
	SampleState* const s = &s_sampleState;
	return s->ctrl.trg;
}
void sampleGetAnalogLeft(float* ox, float* oy){
	SampleState* const s = &s_sampleState;
	*ox = s->ctrl.lx;
	*oy = s->ctrl.ly;
}
void sampleGetAnalogRight(float* ox, float* oy){
	SampleState* const s = &s_sampleState;
	*ox = s->ctrl.rx;
	*oy = s->ctrl.ry;
}

CameraBuffer* sampleGetCameraImage(void){
	//SampleState* const s = &s_sampleState;
	return cameraGetLatestImage();
}
SceMotionSensorState* sampleGetCameraMotion(CameraBuffer* buf){
	if(buf == NULL){
		return NULL;
	}
	return buf->motion;
}
SceCameraRead* sampleGetCameraBufferRead(CameraBuffer* buf){
	if(buf == NULL){
		return NULL;
	}
	return &buf->read;
}
void sampleLockCameraBuffer(CameraBuffer* buf){
	//SampleState* const s = &s_sampleState;
	if(buf == NULL){
		return;
	}
	cameraBufferAddRef(buf);
}
void sampleUnlockCameraBuffer(CameraBuffer* buf){
	//SampleState* const s = &s_sampleState;
	if(buf == NULL){
		return;
	}
	cameraBufferRelease(buf);
}
void sampleSetCameraAutoControlHold(Bool hold){
	//DEBUG_PRINTF((hold != FALSE) ? "camera auto control hold on\n" : "camera auto control hold off\n");
	SCECHK(sceCameraSetAutoControlHold(CAMERA_DEVICE, (hold != FALSE) ? SCE_CAMERA_ATTRIBUTE_AUTOCONTROLHOLD_ON : SCE_CAMERA_ATTRIBUTE_AUTOCONTROLHOLD_OFF));
}


void sampleOverrideCameraImage(CameraBuffer* image){
	SampleState* const s = &s_sampleState;
	AppData* dat = s->app.update.current;
	ASSERT(dat != NULL);
	if(image != NULL){
		cameraBufferAddRef(image);
	}
	if(dat->currentImage != NULL){
		cameraBufferRelease(dat->currentImage);
	}
	dat->currentImage = image;
}

void sampleClearLines(void){
	SampleState* const s = &s_sampleState;
	AppData* dat = s->app.update.current;
	ASSERT(dat != NULL);
	dat->lines.vnum = 0;
	dat->trianglesTex.vnum = 0;
	dat->trianglesCol.vnum = 0;
}
void sampleAddLine(float x0, float y0, float z0, UInt32 c0, float x1, float y1, float z1, UInt32 c1){
	SampleState* const s = &s_sampleState;
	AppData* dat = s->app.update.current;
	ASSERT(dat != NULL);
	if((dat->lines.vnum + 2) <= LINES_VERTEX_SIZE){
		UInt vi = dat->lines.vnum;
		SET_XYZW(dat->lines.vtx[vi + 0], x0 * (2.0f / DISPLAY_WIDTH) - 1.0f, y0 * (2.0f / DISPLAY_HEIGHT) - 1.0f, z0, 1.0f);
		SET_XYZW(dat->lines.vtx[vi + 1], x1 * (2.0f / DISPLAY_WIDTH) - 1.0f, y1 * (2.0f / DISPLAY_HEIGHT) - 1.0f, z1, 1.0f);
		dat->lines.vtx[vi + 0].col = c0;
		dat->lines.vtx[vi + 1].col = c1;
		dat->lines.vnum += 2;
	}
}
void sampleAddLineP(MyMatrix const* m0, float x0, float y0, float z0, UInt32 c0, float x1, float y1, float z1, UInt32 c1){
	SampleState* const s = &s_sampleState;
	AppData* dat = s->app.update.current;
	ASSERT(dat != NULL);
	if((dat->lines.vnum + 2) <= LINES_VERTEX_SIZE){
		UInt vi = dat->lines.vnum;
		MyVectorEx v0;
		MyVectorEx v1;
		myVectorSet(&v0.v, x0, y0, z0);
		myVectorSet(&v1.v, x1, y1, z1);
		if(m0 != NULL){
			myMatrixMulVector(&v0.v, m0, &v0.v);
			myMatrixMulVector(&v1.v, m0, &v1.v);
		}
		if((v0.s.w > -FLT_EPSILON) && (v0.s.w < +FLT_EPSILON)){
			return;
		}
		if((v1.s.w > -FLT_EPSILON) && (v1.s.w < +FLT_EPSILON)){
			return;
		}
		SET_XYZW(dat->lines.vtx[vi + 0], v0.s.x, v0.s.y, v0.s.z, v0.s.w);
		SET_XYZW(dat->lines.vtx[vi + 1], v1.s.x, v1.s.y, v1.s.z, v1.s.w);
		dat->lines.vtx[vi + 0].col = c0;
		dat->lines.vtx[vi + 1].col = c1;
		dat->lines.vnum += 2;
	}
}
void sampleAddLineV(MyMatrix const* m0, MyVector const* v0, UInt32 c0, MyVector const* v1, UInt32 c1){
	SampleState* const s = &s_sampleState;
	AppData* dat = s->app.update.current;
	ASSERT(dat != NULL);
	if((dat->lines.vnum + 2) <= LINES_VERTEX_SIZE){
		UInt vi = dat->lines.vnum;
		MyVectorEx vx0;
		MyVectorEx vx1;
		if(m0 != NULL){
			myMatrixMulVector(&vx0.v, m0, v0);
			myMatrixMulVector(&vx1.v, m0, v1);
		}else{
			vx0.v = *v0;
			vx1.v = *v1;
		}
		if((vx0.s.w > -FLT_EPSILON) && (vx0.s.w < +FLT_EPSILON)){
			return;
		}
		if((vx1.s.w > -FLT_EPSILON) && (vx1.s.w < +FLT_EPSILON)){
			return;
		}
		SET_XYZW(dat->lines.vtx[vi + 0], vx0.s.x, vx0.s.y, vx0.s.z, vx0.s.w);
		SET_XYZW(dat->lines.vtx[vi + 1], vx1.s.x, vx1.s.y, vx1.s.z, vx1.s.w);
		dat->lines.vtx[vi + 0].col = c0;
		dat->lines.vtx[vi + 1].col = c1;
		dat->lines.vnum += 2;
	}
}
void sampleAddTriangleT(MyMatrix const* m0, MyVector const* v0, MyVector const* t0, MyVector const* v1, MyVector const* t1, MyVector const* v2, MyVector const* t2){
	SampleState* const s = &s_sampleState;
	AppData* dat = s->app.update.current;
	ASSERT(dat != NULL);
	if((dat->trianglesTex.vnum + 3) <= TRIANGLES_TEX_VERTEX_SIZE){
		UInt vi = dat->trianglesTex.vnum;
		MyVectorEx vx0;
		MyVectorEx vx1;
		MyVectorEx vx2;
		MyVectorEx tx0;
		MyVectorEx tx1;
		MyVectorEx tx2;
		if(m0 != NULL){
			myMatrixMulVector(&vx0.v, m0, v0);
			myMatrixMulVector(&vx1.v, m0, v1);
			myMatrixMulVector(&vx2.v, m0, v2);
		}else{
			vx0.v = *v0;
			vx1.v = *v1;
			vx2.v = *v2;
		}
		if((vx0.s.w > -FLT_EPSILON) && (vx0.s.w < +FLT_EPSILON)){
			return;
		}
		if((vx1.s.w > -FLT_EPSILON) && (vx1.s.w < +FLT_EPSILON)){
			return;
		}
		if((vx2.s.w > -FLT_EPSILON) && (vx2.s.w < +FLT_EPSILON)){
			return;
		}
		tx0.v = *t0;
		tx1.v = *t1;
		tx2.v = *t2;
		SET_XYZW(dat->trianglesTex.vtx[vi + 0], vx0.s.x, vx0.s.y, vx0.s.z, vx0.s.w);
		SET_XYZW(dat->trianglesTex.vtx[vi + 1], vx1.s.x, vx1.s.y, vx1.s.z, vx1.s.w);
		SET_XYZW(dat->trianglesTex.vtx[vi + 2], vx2.s.x, vx2.s.y, vx2.s.z, vx2.s.w);
		SET_UV(dat->trianglesTex.vtx[vi + 0], tx0.s.x, tx0.s.y);
		SET_UV(dat->trianglesTex.vtx[vi + 1], tx1.s.x, tx1.s.y);
		SET_UV(dat->trianglesTex.vtx[vi + 2], tx2.s.x, tx2.s.y);
		dat->trianglesTex.vnum += 3;
	}
}
void sampleAddTriangleC(MyMatrix const* m0, MyVector const* v0, UInt32 c0, MyVector const* v1, UInt32 c1, MyVector const* v2, UInt32 c2){
	SampleState* const s = &s_sampleState;
	AppData* dat = s->app.update.current;
	ASSERT(dat != NULL);
	if((dat->trianglesCol.vnum + 3) <= TRIANGLES_COL_VERTEX_SIZE){
		UInt vi = dat->trianglesCol.vnum;
		MyVectorEx vx0;
		MyVectorEx vx1;
		MyVectorEx vx2;
		if(m0 != NULL){
			myMatrixMulVector(&vx0.v, m0, v0);
			myMatrixMulVector(&vx1.v, m0, v1);
			myMatrixMulVector(&vx2.v, m0, v2);
		}else{
			vx0.v = *v0;
			vx1.v = *v1;
			vx2.v = *v2;
		}
		if((vx0.s.w > -FLT_EPSILON) && (vx0.s.w < +FLT_EPSILON)){
			return;
		}
		if((vx1.s.w > -FLT_EPSILON) && (vx1.s.w < +FLT_EPSILON)){
			return;
		}
		if((vx2.s.w > -FLT_EPSILON) && (vx2.s.w < +FLT_EPSILON)){
			return;
		}
		SET_XYZW(dat->trianglesCol.vtx[vi + 0], vx0.s.x, vx0.s.y, vx0.s.z, vx0.s.w);
		SET_XYZW(dat->trianglesCol.vtx[vi + 1], vx1.s.x, vx1.s.y, vx1.s.z, vx1.s.w);
		SET_XYZW(dat->trianglesCol.vtx[vi + 2], vx2.s.x, vx2.s.y, vx2.s.z, vx2.s.w);
		dat->trianglesCol.vtx[vi + 0].col = c0;
		dat->trianglesCol.vtx[vi + 1].col = c1;
		dat->trianglesCol.vtx[vi + 2].col = c2;
		dat->trianglesCol.vnum += 3;
	}
}
void sampleSetViewMatrix(MyMatrix const* mat){
	SampleState* const s = &s_sampleState;
	AppData* dat = s->app.update.current;
	ASSERT(dat != NULL);
	dat->world.viewMat = *mat;
}
void _sampleGetViewMatrix(MyMatrix* mat){
	SampleState* const s = &s_sampleState;
	AppData* dat = s->app.update.current;
	ASSERT(dat != NULL);
	*mat = dat->world.viewMat;
}
void sampleGetProjMatrix(MyMatrix* mat){
	SampleState* const s = &s_sampleState;
	AppData* dat = s->app.update.current;
	ASSERT(dat != NULL);
	*mat = dat->world.projMat;
}

void sampleDebugout(char const* str){
	SampleState* const s = &s_sampleState;
	AppData* dat = s->app.update.current;
#if defined(_DEBUG)
	MyMemInfo info;
	myMemGetInfo(&info);
	snprintf(dat->debug.text, sizeof(dat->debug.text),
		"Frame: %u\nFPS: %5.2f[update:%3u render:%3u]\n%s\nMemory:%lu<%lu>/%u [%u]\n",
		s->app.time.frame/*s->app.debug.vcount*/,
		s->app.debug.fps, s->app.debug.update.pt / 1000, s->app.debug.render.pt / 1000, str, info.used, info.max, info.units, info.counts);
#else
	snprintf(dat->debug.text, sizeof(dat->debug.text),
		"Frame: %u\nFPS: %5.2f[update:%3u render:%3u]\n%s",
		s->app.time.frame/*s->app.debug.vcount*/,
		s->app.debug.fps, s->app.debug.update.pt / 1000, s->app.debug.render.pt / 1000, str);
#endif
}

void sampleSetStatusLine(char const* text, ...){
	SampleState* const s = &s_sampleState;
	if(text != NULL){
		va_list list;
		va_start(list, text);
		s->app.debug.statusLine[0] = '>';
		s->app.debug.statusLine[1] = ' ';
		vsnprintf(s->app.debug.statusLine + 2, sizeof(s->app.debug.statusLine) - 2, text, list);
		DEBUG_PRINTF("%s", s->app.debug.statusLine + 2);
		va_end(list);
	}
}

uint32_t getShaderAttribute(SceGxmShaderPatcherId id, const char* name){
	uint32_t ret;
	SceGxmProgram const* program;
	SceGxmProgramParameter const* param;
	SceGxmParameterCategory	cat;

	program = sceGxmShaderPatcherGetProgramFromId(id);
	ASSERT(program != NULL);

	param = sceGxmProgramFindParameterByName(program, name);
	ASSERT(param != NULL);

	cat = sceGxmProgramParameterGetCategory(param);
	ASSERT(cat == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE);
	TOUCH(cat);

	ret = sceGxmProgramParameterGetResourceIndex(param);
	return ret;
}

extern SceGxmProgram const _binary_color_v_gxp_start;
extern SceGxmProgram const _binary_color_f_gxp_start;
extern SceGxmProgram const _binary_tex_v_gxp_start;
extern SceGxmProgram const _binary_tex_f_gxp_start;

static int _sampleInitializeShaders(void){
	SampleState* const s = &s_sampleState;
	int returnCode;

	{
		SceGxmVertexAttribute attr[2];
		SceGxmVertexStream stream[1];

		returnCode = sceGxmShaderPatcherRegisterProgram(g_pShaderPatcher, &_binary_color_v_gxp_start, &s->gxm.shader.color.vpId);
		SCE_DBG_ASSERT(returnCode >= SCE_OK);
		if(returnCode < SCE_OK){
			return returnCode;
		}
		returnCode = sceGxmShaderPatcherRegisterProgram(g_pShaderPatcher, &_binary_color_f_gxp_start, &s->gxm.shader.color.fpId);
		SCE_DBG_ASSERT(returnCode >= SCE_OK);
		if(returnCode < SCE_OK){
			return returnCode;
		}

		attr[0].streamIndex = 0;
		attr[0].offset = 0;
		attr[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		attr[0].componentCount = 4;
		attr[0].regIndex = getShaderAttribute(s->gxm.shader.color.vpId, "ix.position");

		attr[1].streamIndex = 0;
		attr[1].offset = 16;
		attr[1].format = SCE_GXM_ATTRIBUTE_FORMAT_U8N;
		attr[1].componentCount = 4;
		attr[1].regIndex = getShaderAttribute(s->gxm.shader.color.vpId, "ix.color0");

		stream[0].stride = sizeof(VertexC);
		stream[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

		returnCode = sceGxmShaderPatcherCreateVertexProgram(g_pShaderPatcher,
														s->gxm.shader.color.vpId,
														attr, 2, stream, 1,
														&s->gxm.shader.color.vp);
		SCE_DBG_ASSERT(returnCode >= SCE_OK);
		if(returnCode < SCE_OK){
			return returnCode;
		}


		returnCode = sceGxmShaderPatcherCreateFragmentProgram(g_pShaderPatcher,
														s->gxm.shader.color.fpId,
														SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,
														SCE_GXM_MULTISAMPLE_NONE,
														NULL,
														sceGxmShaderPatcherGetProgramFromId(s->gxm.shader.color.vpId),
														&s->gxm.shader.color.fp);
		SCE_DBG_ASSERT(returnCode >= SCE_OK);
		if(returnCode < SCE_OK){
			return returnCode;
		}
	}

	{
		SceGxmVertexAttribute attr[2];
		SceGxmVertexStream stream[1];

		returnCode = sceGxmShaderPatcherRegisterProgram(g_pShaderPatcher, &_binary_tex_v_gxp_start, &s->gxm.shader.tex.vpId);
		SCE_DBG_ASSERT(returnCode >= SCE_OK);
		if(returnCode < SCE_OK){
			return returnCode;
		}
		returnCode = sceGxmShaderPatcherRegisterProgram(g_pShaderPatcher, &_binary_tex_f_gxp_start, &s->gxm.shader.tex.fpId);
		SCE_DBG_ASSERT(returnCode >= SCE_OK);
		if(returnCode < SCE_OK){
			return returnCode;
		}

		attr[0].streamIndex = 0;
		attr[0].offset = 0;
		attr[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		attr[0].componentCount = 4;
		attr[0].regIndex = getShaderAttribute(s->gxm.shader.tex.vpId, "ix.position");

		attr[1].streamIndex = 0;
		attr[1].offset = 16;
		attr[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
		attr[1].componentCount = 2;
		attr[1].regIndex = getShaderAttribute(s->gxm.shader.tex.vpId, "ix.texcoord0");

		stream[0].stride = sizeof(VertexT);
		stream[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

		returnCode = sceGxmShaderPatcherCreateVertexProgram(g_pShaderPatcher,
														s->gxm.shader.tex.vpId,
														attr, 2, stream, 1,
														&s->gxm.shader.tex.vp);
		SCE_DBG_ASSERT(returnCode >= SCE_OK);
		if(returnCode < SCE_OK){
			return returnCode;
		}

		returnCode = sceGxmShaderPatcherCreateFragmentProgram(g_pShaderPatcher,
														s->gxm.shader.tex.fpId,
														SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,
														SCE_GXM_MULTISAMPLE_NONE,
														NULL,
														sceGxmShaderPatcherGetProgramFromId(s->gxm.shader.tex.vpId),
														&s->gxm.shader.tex.fp);
		SCE_DBG_ASSERT(returnCode >= SCE_OK);
		if(returnCode < SCE_OK){
			return returnCode;
		}
	}

	{
		SceUInt16 i;
		s->gxm.vertex.index.iDat = (SceUInt16*)graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA, sizeof(SceUInt16) * 32768, 4, SCE_GXM_MEMORY_ATTRIB_READ, &s->gxm.vertex.index.iUID);
		if(s->gxm.vertex.index.iDat == NULL){
			return -1;
		}
		for(i = 0; i < 32768; i++){
			s->gxm.vertex.index.iDat[i] = i;
		}
	}
	{
		s->gxm.vertex.clear.vDat = (VertexC*)graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA, sizeof(VertexC) * 3, 4, SCE_GXM_MEMORY_ATTRIB_READ, &s->gxm.vertex.clear.vUID);
		if(s->gxm.vertex.clear.vDat == NULL){
			return -1;
		}
		SET_XYZW(s->gxm.vertex.clear.vDat[0], -1.0f, -1.0f, 1.0f, 1.0f);
		SET_XYZW(s->gxm.vertex.clear.vDat[1],  3.0f, -1.0f, 1.0f, 1.0f);
		SET_XYZW(s->gxm.vertex.clear.vDat[2], -1.0f,  3.0f, 1.0f, 1.0f);
#if defined(_DEBUG)
		s->gxm.vertex.clear.vDat[0].col = 0x0000ff00U;
		s->gxm.vertex.clear.vDat[1].col = 0x000000ffU;
		s->gxm.vertex.clear.vDat[2].col = 0x00ff0000U;
#else
		s->gxm.vertex.clear.vDat[0].col = 0x00000000;
		s->gxm.vertex.clear.vDat[1].col = 0x00000000;
		s->gxm.vertex.clear.vDat[2].col = 0x00000000;
#endif
	}
	return SCE_OK;
}

static void _sampleReleaseShaders(void){
	SampleState* const s = &s_sampleState;

	if(g_pContext != NULL){
		sceGxmFinish(g_pContext);
	}

	SCECHK(sceGxmDisplayQueueFinish());

	if(s->gxm.vertex.index.iUID >= SCE_OK){
		graphicsFree(s->gxm.vertex.index.iUID);
		s->gxm.vertex.index.iUID = -1;
		s->gxm.vertex.index.iDat = NULL;
	}
	if(s->gxm.vertex.clear.vUID >= SCE_OK){
		graphicsFree(s->gxm.vertex.clear.vUID);
		s->gxm.vertex.clear.vUID = -1;
		s->gxm.vertex.clear.vDat = NULL;
	}

	if(s->gxm.shader.color.fp != NULL){
		SCECHK(sceGxmShaderPatcherReleaseFragmentProgram(g_pShaderPatcher, s->gxm.shader.color.fp));
		s->gxm.shader.color.fp = NULL;
	}
	if(s->gxm.shader.color.vp != NULL){
		sceGxmShaderPatcherReleaseVertexProgram(g_pShaderPatcher, s->gxm.shader.color.vp);
		s->gxm.shader.color.vp = NULL;
	}
	if(s->gxm.shader.color.fpId >= SCE_OK){
		SCECHK(sceGxmShaderPatcherUnregisterProgram(g_pShaderPatcher, s->gxm.shader.color.fpId));
		s->gxm.shader.color.fpId = NULL;
	}
	if(s->gxm.shader.color.vpId >= SCE_OK){
		SCECHK(sceGxmShaderPatcherUnregisterProgram(g_pShaderPatcher, s->gxm.shader.color.vpId));
		s->gxm.shader.color.vpId = NULL;
	}

	if(s->gxm.shader.tex.fp != NULL){
		SCECHK(sceGxmShaderPatcherReleaseFragmentProgram(g_pShaderPatcher, s->gxm.shader.tex.fp));
		s->gxm.shader.tex.fp = NULL;
	}

	if(s->gxm.shader.tex.vp != NULL){
		SCECHK(sceGxmShaderPatcherReleaseVertexProgram(g_pShaderPatcher, s->gxm.shader.tex.vp));
		s->gxm.shader.tex.vp = NULL;
	}
	if(s->gxm.shader.tex.fpId >= SCE_OK){
		SCECHK(sceGxmShaderPatcherUnregisterProgram(g_pShaderPatcher, s->gxm.shader.tex.fpId));
		s->gxm.shader.tex.fpId = NULL;
	}
	if(s->gxm.shader.tex.vpId >= SCE_OK){
		SCECHK(sceGxmShaderPatcherUnregisterProgram(g_pShaderPatcher, s->gxm.shader.tex.vpId));
		s->gxm.shader.tex.vpId = NULL;
	}
}

static int _myDebugPrintfInitialize(void);
static void _myDebugPrintfRelease(void);


extern void(*custom_display_callback)(const void *callbackData);

int sampleInitialize(void){
	SampleState* const s = &s_sampleState;
	int returnCode;

	memset(&s_sampleState, 0, sizeof(s_sampleState));

	returnCode = _myDebugPrintfInitialize();
	if(returnCode < SCE_OK){
		return returnCode;
	}
	s->initStep = 1;

	custom_display_callback = displayCallback;

	{
		//SceAppUtilInitParam init_param;
		//SceAppUtilBootParam boot_param;

		//memset(&init_param, 0, sizeof(init_param));
		//memset(&boot_param, 0, sizeof(boot_param));
		//returnCode = sceAppUtilInit(&init_param, &boot_param);
		//if(returnCode < SCE_OK){
		//	return returnCode;
		//}
	}
	s->initStep = 2;

#if defined(ENABLE_DBGFONT)
	{
		SceDbgFontConfig config;
		memset(&config, 0, sizeof(SceDbgFontConfig));
		config.fontSize = SCE_DBGFONT_FONTSIZE_LARGE;
		returnCode = sceDbgFontInit(&config);
		if(returnCode < SCE_OK){
			return returnCode;
		}
	}
	s->initStep = 3;
#endif

	//returnCode = _sampleInitializeShaders();
	//if(returnCode < SCE_OK){
	//	return returnCode;
	//}
	s->initStep = 4;

	//returnCode = ctrlInitialize();
	//if(returnCode < SCE_OK){
	//	return returnCode;
	//}
	s->initStep = 5;

	returnCode = vsyncInitialize();
	if(returnCode < SCE_OK){
		return returnCode;
	}
	s->initStep = 6;

	returnCode = cameraInitialize();
	if(returnCode < SCE_OK){
		return returnCode;
	}
	s->initStep = 7;

	returnCode = motionInitialize();
	if(returnCode < SCE_OK){
		return returnCode;
	}
	s->initStep = 8;

	returnCode = saveDataStreamInitialize();
	if(returnCode < SCE_OK){
		return returnCode;
	}
	s->initStep = 9;

#if !defined(DISABLE_SMART)
	returnCode = smartInitialize();
	if(returnCode < SCE_OK){
		return returnCode;
	}
	s->initStep = 10;
#endif

	returnCode = _appDataInit();
	if(returnCode < SCE_OK){
		return returnCode;
	}
	s->initStep = 11;

	//scePowerSetConfigurationMode(SCE_POWER_CONFIGURATION_MODE_B);

	{
		returnCode = myEventCreate(&s->app.render.flip_evt);
		if(returnCode < SCE_OK){
			return returnCode;
		}
		myEventRaise(&s->app.render.flip_evt);
	}
	s->initStep = 12;
	return SCE_OK;
}

void sampleRelease(void){
	SampleState* const s = &s_sampleState;

	//if(g_pContext != NULL){
	//	sceGxmFinish(g_pContext);
	//}

	//SCECHK(sceGxmDisplayQueueFinish());

	if(s->initStep >= 12){
		myEventRelease(&s->app.render.flip_evt);
	}

	if(s->initStep >= 11){
		_appDataRelease();
	}

#if !defined(DISABLE_SMART)
	if(s->initStep >= 10){
		smartRelease();
	}
#endif

	if(s->initStep >= 9){
		saveDataStreamRelease();
	}

	if(s->initStep >= 8){
		motionRelease();
	}

	if(s->initStep >= 7){
		cameraRelease();
	}

	if(s->initStep >= 6){
		vsyncRelease();
	}

	if(s->initStep >= 5){
//		ctrlRelease();
	}

	if(s->initStep >= 4){
//		_sampleReleaseShaders();
	}

#if defined(ENABLE_DBGFONT)
	if(s->initStep >= 3){
		SCECHK(sceDbgFontExit());
	}
#endif

	if(s->initStep >= 2){
//		SCECHK(sceAppUtilShutdown());
	}

	if(s->initStep >= 1){
		_myDebugPrintfRelease();
	}

	memset(&s_sampleState, 0, sizeof(s_sampleState));
}

void sampleUpdate(void){
	SampleState* const s = &s_sampleState;
	SceUInt32 span;
	AppData* dat;

	vsyncWait();

	{
		dat = _appDataReserve();
		ASSERT(dat != NULL);
	}

	{
		if(dat->currentImage != NULL){
			cameraBufferRelease(dat->currentImage);
		}
		cameraUpdate();
		dat->currentImage = cameraGetLatestImage();
		dat->frame = s->app.time.frame;
	}

//	ctrlUpdate();

	{
		SceUInt32 t0 = sceKernelGetProcessTimeLow();
		SceUInt32 t1 = s->app.time.t1;
		span = t0 - t1;
		s->app.time.t1 = t0;
		s->app.time.frame++;
		s->app.debug.fps = 1000000.0f / (t0 - t1);
	}

	{
		s->app.update.current = dat;

//		sampleClearLines();

#if !defined(DISABLE_SMART)
		smartUpdate(span, dat->currentImage);
#else
		TOUCH(span);
#endif

		s->app.update.current = NULL;
	}

	{
		SceUInt32 t0 = sceKernelGetProcessTimeLow();
		s->app.debug.update.pt = t0 - s->app.time.t1;
	}

	{
		strcpy(dat->debug.statusLine, s->app.debug.statusLine);
	}

	{
		_appDataRegist(dat);
	}
}

void sampleRenderBegin()
{
	SampleState* const s = &s_sampleState;
	AppData* dat;
	SceUInt32 t1;

	myEventWait(&s->app.render.flip_evt, INFINITE);

	t1 = sceKernelGetProcessTimeLow();

	{
		if (s->app.render.data != NULL) {
			_appDataUnlockData(s->app.render.data);
		}
		s->app.render.data = _appDataLockHead();
		dat = s->app.render.data;
	}
}

void sampleRenderEnd()
{
}

bool sampleIsCameraImageReady()
{
	SampleState* const s = &s_sampleState;
	AppData* dat;
	dat = s->app.render.data;

	return (dat->currentImage != NULL);
}

SceGxmTexture sampleGetCameraImageTexture()
{
	SampleState* const s = &s_sampleState;
	AppData* dat;
	dat = s->app.render.data;
	return dat->currentImage->tex_yuv;
}

void sampleRender(void){
	SampleState* const s = &s_sampleState;
	AppData* dat;
	SceUInt32 t1;

	myEventWait(&s->app.render.flip_evt, INFINITE);

	t1 = sceKernelGetProcessTimeLow();

	{
		if(s->app.render.data != NULL){
			_appDataUnlockData(s->app.render.data);
		}
		s->app.render.data = _appDataLockHead();
		dat = s->app.render.data;
	}

	SCECHK(sceGxmBeginScene(g_pContext, 0, g_pRenderTarget, NULL, NULL, g_pDisplayBufferSync[g_displayBackBufferIndex], &g_displaySurface[g_displayBackBufferIndex], &g_depthSurface));
	{
		sceGxmSetCullMode(g_pContext, SCE_GXM_CULL_CW);
		//sceGxmSetFrontDepthWriteEnable(g_pContext, SCE_GXM_DEPTH_WRITE_ENABLED);
#if 1
		{ // clear
			sceGxmSetFrontDepthFunc(g_pContext, SCE_GXM_DEPTH_FUNC_ALWAYS);
			sceGxmSetFrontPolygonMode(g_pContext, SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);
			sceGxmSetVertexProgram(g_pContext, s->gxm.shader.color.vp);
			sceGxmSetFragmentProgram(g_pContext, s->gxm.shader.color.fp);
			SCECHK(sceGxmSetVertexStream(g_pContext, 0, s->gxm.vertex.clear.vDat));
			SCECHK(sceGxmDraw(g_pContext, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, s->gxm.vertex.index.iDat, 3));
		}
#endif
#if 1
		if((dat->trianglesTex.vnum != 0) && (dat->currentImage != NULL)){
			sceGxmSetFrontDepthFunc(g_pContext, SCE_GXM_DEPTH_FUNC_LESS_EQUAL);
			sceGxmSetFrontPolygonMode(g_pContext, SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);
			sceGxmSetVertexProgram(g_pContext, s->gxm.shader.tex.vp);
			sceGxmSetFragmentProgram(g_pContext, s->gxm.shader.tex.fp);
			SCECHK(sceGxmSetFragmentTexture(g_pContext, 0, &dat->currentImage->tex_yuv));
			SCECHK(sceGxmSetVertexStream(g_pContext, 0, dat->trianglesTex.vtx));
			SCECHK(sceGxmDraw(g_pContext, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, s->gxm.vertex.index.iDat, dat->trianglesTex.vnum));
		}
#endif
#if 1
		if(dat->lines.vnum != 0){
			//sceGxmSetFrontDepthFunc(g_pContext, SCE_GXM_DEPTH_FUNC_ALWAYS);
			sceGxmSetFrontDepthFunc(g_pContext, SCE_GXM_DEPTH_FUNC_LESS_EQUAL);
			sceGxmSetFrontPolygonMode(g_pContext, SCE_GXM_POLYGON_MODE_LINE);
			sceGxmSetVertexProgram(g_pContext, s->gxm.shader.color.vp);
			sceGxmSetFragmentProgram(g_pContext, s->gxm.shader.color.fp);
			SCECHK(sceGxmSetVertexStream(g_pContext, 0, dat->lines.vtx));
			SCECHK(sceGxmDraw(g_pContext, SCE_GXM_PRIMITIVE_LINES, SCE_GXM_INDEX_FORMAT_U16, s->gxm.vertex.index.iDat, dat->lines.vnum));
		}
#endif
#if 1
		if(dat->trianglesCol.vnum != 0){
			sceGxmSetFrontDepthFunc(g_pContext, SCE_GXM_DEPTH_FUNC_LESS_EQUAL);
			//sceGxmSetFrontDepthFunc(g_pContext, SCE_GXM_DEPTH_FUNC_ALWAYS);
			sceGxmSetFrontPolygonMode(g_pContext, SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);
			sceGxmSetVertexProgram(g_pContext, s->gxm.shader.color.vp);
			sceGxmSetFragmentProgram(g_pContext, s->gxm.shader.color.fp);
			SCECHK(sceGxmSetVertexStream(g_pContext, 0, dat->trianglesCol.vtx));
			SCECHK(sceGxmDraw(g_pContext, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, s->gxm.vertex.index.iDat, dat->trianglesCol.vnum));
		}
#endif
	}
	SCECHK(sceGxmEndScene(g_pContext, NULL, NULL));

	{
		SceUInt32 t0 = sceKernelGetProcessTimeLow();
		s->app.debug.render.pt = t0 - t1;
	}
}

void sampleDisplayCallback(DisplayData const* data){
	SampleState* const s = &s_sampleState;
	AppData* dat = s->app.render.data;

#if defined(ENABLE_DBGFONT)
	{

		SceDbgFontFrameBufInfo info;
		memset(&info, 0, sizeof(SceDbgFontFrameBufInfo));
		info.frameBufAddr = (SceUChar8*)data->address;
		info.frameBufPixelformat = DISPLAY_PIXEL_FORMAT;
		info.frameBufWidth = DISPLAY_WIDTH;
		info.frameBufHeight = DISPLAY_HEIGHT;
		info.frameBufPitch = DISPLAY_STRIDE_IN_PIXELS;

		SCECHK(sceDbgFontPrint(16, 16, 0xffeeeeeeU, dat->debug.text));
		SCECHK(sceDbgFontPrint(0, 512, 0xff8080eeU, dat->debug.statusLine));
		SCECHK(sceDbgFontFlush(&info));
	}
#endif

	myEventRaise(&s->app.render.flip_evt);
}

#if 1
static struct{
	MyMutex myMutex;
	SceUInt32 timestamp;
	int initStep;
	char work[1024];
}s_myDebugPrintfState;

static int _myDebugPrintfInitialize(void){
	int returnCode;
	memset(&s_myDebugPrintfState, 0, sizeof(s_myDebugPrintfState));
	returnCode = myMutexCreate(&s_myDebugPrintfState.myMutex);
	if(returnCode < SCE_OK){
		return returnCode;
	}
	s_myDebugPrintfState.initStep = 1;
	s_myDebugPrintfState.timestamp = sceKernelGetProcessTimeLow();
	s_myDebugPrintfState.initStep = 2;
	return SCE_OK;
}
static void _myDebugPrintfRelease(void){
	if(s_myDebugPrintfState.initStep >= 1){
		myMutexRelease(&s_myDebugPrintfState.myMutex);
	}
	memset(&s_myDebugPrintfState, 0, sizeof(s_myDebugPrintfState));
}

int myDebugPrintf(char const* function, int line, char const* format, ...){
	int ret;
	myMutexLock(&s_myDebugPrintfState.myMutex);
	{
		SceUInt32 t0 = s_myDebugPrintfState.timestamp;
		SceUInt32 t1 = sceKernelGetProcessTimeLow();
		SceUInt32 dt = t1 - t0;
		va_list list;
		va_start(list, format);
		vsnprintf(s_myDebugPrintfState.work, sizeof(s_myDebugPrintfState.work), format, list);
		ret = printf("%06u: %s: %d: %s", ((dt <= 999999) ? dt : 999999), function, line, s_myDebugPrintfState.work);
		s_myDebugPrintfState.timestamp = t1;
		va_end(list);
	}
	myMutexUnlock(&s_myDebugPrintfState.myMutex);
	return ret;
}

#if 1
#include <libperf.h>
#pragma comment(lib, "libScePerf_stub_weak.a")
void myDebugStamp(char const* str, ...){
	static __thread SceUInt64 t1 = 0;
	SceUInt64 t0 = scePerfGetTimebaseValue();
	if(str != NULL){
		static char work[256];
		va_list list;
		va_start(list, str);
		vsnprintf(work, sizeof(work), str, list);
		printf("%s: %d\n", work, (int)(t0 - t1));
		va_end(list);
	}
	t1 = scePerfGetTimebaseValue();
}
#else
void myDebugStamp(char const* str, ...){
	static __thread UInt32 t1 = 0;
	UInt32 t0 = system_time();
	if(str != NULL){
		static char work[256];
		va_list list;
		va_start(list, str);
		vsnprintf(work, sizeof(work), str, list);
		printf("%s: %d\n", work, (int)(t0 - t1));
		va_end(list);
	}
	t1 = system_time();
}
#endif

void myDebugBreak(void){
	SCE_BREAK();
}

void myDebugAbort(void){
	SCE_STOP();
}

#endif

