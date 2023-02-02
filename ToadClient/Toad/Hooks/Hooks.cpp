#include "pch.h"
#include "Hooks.h"

#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"
#include "Toad/Toad.h"

typedef BOOL(__fastcall* twglSwapBuffers) (_In_ HDC hDc);
twglSwapBuffers owglSwapBuffers = nullptr;

int init = 0;
bool showMenu = false;

HWND hwnd;
HGLRC ctx = nullptr;

BOOL hwglSwapBuffers(HDC hDc)
{
	
	//if (toadll::modules::entitiepositions.empty())
	//	return owglSwapBuffers(hDc);

	/*GLint viewport[4];
	GLfloat modelview[16];
	GLfloat projection[16];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
	glGetFloatv(GL_PROJECTION_MATRIX, projection);

	for (const auto& player : toadll::modules::entitiepositions)
	{
		toadll::vec2 vec2{0,0};

		if (WorldToScreen(player, vec2, modelview, projection, viewport))
		{
			glBegin(GL_LINE_LOOP);
			glVertex2f(vec2.x - 10, vec2.y - 10);
			glVertex2f(vec2.x + 10, vec2.y - 10);
			glVertex2f(vec2.x + 10, vec2.y + 10);
			glVertex2f(vec2.x - 10, vec2.y + 10);
			glEnd();
			std::cout << vec2.x << " " << vec2.y << std::endl;
		}
	}*/
	

	/*hwnd = WindowFromDC(hDc);
	HGLRC oCtx = wglGetCurrentContext();

	if (init==0)
	{
		ctx = wglCreateContext(hDc);
		wglMakeCurrent(hDc, ctx);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		GLint m_viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_viewport);

		glOrtho(0, m_viewport[2], m_viewport[3], 0, 1, -1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClearColor(0, 0, 0, 1);

		init = 1;
	}

	wglMakeCurrent(hDc, ctx);

	if (init == 1)
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplOpenGL2_Init();
		init = 3;
	}

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("nigga client v0.1");

	ImGui::Text("Black af?");

	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	wglMakeCurrent(hDc, oCtx);
	*/

	//glReadPixels()
	//GLint viewport[4];
	//GLfloat modelview[16];
	//GLfloat projection[16];
	//glGetIntegerv(GL_VIEWPORT, viewport);
	//glGetFloatv(GL_MODELVIEW_MATRIX, modelview );
	//glGetFloatv(GL_PROJECTION_MATRIX, projection);

	//for (auto player : toadll::p_Minecraft->get_playerList())
	//{
	//}
	
	/*
	std::cout << "viewport ";
	for (int i = 0; i < 4; i++)
	{
		std::cout << viewport[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "modelview ";
	for (int i = 0; i < 16; i++)
	{
		std::cout << modelview[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "projection ";
	for (int i = 0; i < 16; i++)
	{
		std::cout << projection[i] << " ";
	}
	std::cout << std::endl;*/


	
	return owglSwapBuffers(hDc);
}

bool c_Hooks::init()
{
	if (MH_Initialize() != MH_OK)
		return false;

	optr = GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");
	log_Debug("wglswapbuffers: %p", optr);

	if (optr == nullptr)
		return false;

	//return MH_CreateHookApi(L"opengl32.dll", "wglSwapBuffers", &hwglSwapBuffers, reinterpret_cast<LPVOID*>(&owglSwapBuffers)) == MH_OK;
	return MH_CreateHook(optr, &hwglSwapBuffers, reinterpret_cast<LPVOID*>(&owglSwapBuffers)) == MH_OK;
}

void c_Hooks::enable()
{
	enabled = true;
	MH_EnableHook(optr);
}

void c_Hooks::disable()
{
	enabled = false;
	MH_DisableHook(optr);
}

void c_Hooks::dispose() const
{
	if (enabled)
		MH_DisableHook(optr);
	MH_Uninitialize();
}
