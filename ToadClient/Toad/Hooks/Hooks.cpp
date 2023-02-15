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
	//toadll::setup_ortho();
	//GLubyte col[3] = { 1, 1, 1 };
	//for (const auto& vec : toadll::entities)
	//{
	//	toadll::setup_ortho();
	//	auto f = toadll::WorldToScreen(vec, toadll::render_pos_vec3, toadll::render_rotation.y, toadll::render_rotation.x, 0, 1920, 1080);
	//	//std::cout << f.x << " " << f.y << std::endl;
	//	glBegin(GL_QUADS);
	//	glColor4f(1, 1, 1, 1);
	//	glVertex3f(f.x, f.y, 0);
	//	glVertex3f(f.x + 10, f.y, 0);
	//	glVertex3f(f.x + 10, f.y + 10, 0);
	//	glVertex3f(f.x, f.y + 10, 0);
	//	glEnd();
	//	glColor4f(1, 1, 1, 1);
	//	toadll::restore_GL();
	//	//toadll::draw_outline(100, 100, 50.f, 50.f, 2.5f, col);
	//}

	


	
	//glPushMatrix();
	//glMatrixMode(GL_PROJECTION);
	//glLoadMatrixf(:ActiveRenderInfo:PROJECTION);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadMatrixf(:MOVELVIEW);
	//EventSystem::fire(EventRenderOverlay3D());
	//glPopMatrix();

	//if (toadll::modules::entitiepositions.empty())
	//	return owglSwapBuffers(hDc);
	//auto players = toadll::modules::entitiepositions;
	//toadll::draw::drawRect(470, 700, 490, 510);
	//if (players.empty())
	//	return owglSwapBuffers(hDc);

	//for (const auto& e : players)
	//	toadll::modules::esp(e);

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

bool toadll::c_Hooks::init()
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

void toadll::c_Hooks::enable()
{
	enabled = true;
	MH_EnableHook(optr);
}

void toadll::c_Hooks::disable()
{
	enabled = false;
	MH_DisableHook(optr);
}

void toadll::c_Hooks::dispose() const
{
	if (enabled)
		MH_DisableHook(optr);
	MH_Uninitialize();
}
