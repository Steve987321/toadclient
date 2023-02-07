#include "pch.h"
#include "Toad/Toad.h"
#include "modules.h"


void toadll::modules::update()
{
	lPlayer = p_Minecraft->get_localplayer();
	if (lPlayer == nullptr) return;

	/*auto heldItem = lPlayer->get_heldItem();
	if (heldItem != NULL)
	{
		log_Debug("%s", jstring2string(jstring(env->CallObjectMethod(heldItem, env->GetMethodID(env->GetObjectClass(heldItem), "toString", "()Ljava/lang/String;")))).c_str());
		env->DeleteLocalRef(heldItem);
	}
	else
	{
		log_Debug("no");
	}*/

	aa();
	//auto_bridge();
}

void toadll::modules::aa()
{
	if (!toadll::aa::enabled) return;

	if (GetAsyncKeyState(aa::key))
	{
		std::vector <std::pair<float, std::shared_ptr<c_Entity>>> distances = {};

		if (lPlayer->obj == NULL) return;
		
		auto lPlayername = lPlayer->get_name();

		for (const auto& player : p_Minecraft->get_playerList())
		{
			if (player->get_name() == lPlayername) continue;
			distances.emplace_back(lPlayer->get_position().dist(player->get_position()), player);
		}
		if (distances.size() < 2) return; // atleast 2 players in session
		auto t = std::min_element(distances.begin(), distances.end());

		if (t->first > aa::distance) return;

		auto target = t->second;

		auto [yaw, pitch] = get_angles(lPlayer->get_position(), target->get_position());

		auto lyaw = lPlayer->get_rotationYaw();
		auto lpitch = lPlayer->get_rotationPitch();

		float difference = wrap_to_180(-(lyaw - yaw));
		float difference2 = wrap_to_180(-(lpitch - pitch));

		lPlayer->set_rotation(lyaw + difference / (10000.f / aa::speed), lpitch + difference2 / (10000.f / aa::speed));
	}
}

void toadll::modules::esp(const vec3& ePos)
{
	/*vec2 vec2t{0,0};

	GLint viewport[4];
	GLfloat modelview[16];
	GLfloat projection[16];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
	glGetFloatv(GL_PROJECTION_MATRIX, projection);

	if (WorldToScreen(ePos, vec2t, modelview, projection, viewport))
	{
		glBegin(GL_LINE_LOOP);
		glVertex2f(vec2t.x - 10, vec2t.y - 10);
		glVertex2f(vec2t.x + 10, vec2t.y - 10);
		glVertex2f(vec2t.x + 10, vec2t.y + 10);
		glVertex2f(vec2t.x - 10, vec2t.y + 10);
		glEnd();
	}*/


	// Projection (only needs to be set once in most cases)
	//glMatrixMode(GL_PROJECTION);        // Select projection matrix
	//glLoadIdentity();                        // Clear it
	//glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);  // Set your projection

	//// model/view transforms
	//glMatrixMode(GL_MODELVIEW);  // Select modelview matrix
	//glLoadIdentity();                 // Clear it

	//// Draw (shortcut)
	//glRectf(0.1f, 0.1f, 0.9f, 0.9f);
	//glPushMatrix();
	//// Translate to the entities position
	//glTranslated(
	//	// Add 0.5 for tile entities
	//	// There's gotta be a better way to do this, though...
	//	ePos.z - renderposX,
	//	ePos.y - renderposY + /*(e.height / 2),*/ 0.5 ,
	//	ePos.z - renderposZ
	//);

	//glNormal3f(0.0F, 1.0F, 0.0F);


	//// Rotate the ESP "plate" to always be facing the player
	//glRotatef(-yaw, 0.0F, 1.0F, 0.0F);
	//glRotatef(pitch, 1.0F, 0.0F, 0.0F);

	////p_Minecraft->disableLightMap();
	//glDisable(GL_DEPTH_TEST);
	//
	//double extraDist = 0.2;
	//// Main rectangle
	//// TODO: Add a parameter that 'toggles' this?

	//auto width = 1;
	//auto height = 2;

	//glRectf(0.1f, 0.1f, 0.9f, 0.9f);

	////log_Debug(" RENDERPOS(%f,%f,%f) ENTITYPOS(%f,%f,%f)", renderposX, renderposY, renderposZ, ePos.x, ePos.y, ePos.z);

	////draw::drawRect(-width, -height / 2 - extraDist, width * 2, height + extraDist * 2);

	////// Bottom right
	////draw::drawRect(-width, -height / 2 - extraDist, extraDist, extraDist / 10);
	////draw::drawRect(-width, -height / 2 - extraDist, extraDist / 10, extraDist);
	////// Bottom left
	////draw::drawRect(width - extraDist * 1.1, -height / 2 - extraDist, extraDist, extraDist / 10);
	////draw::drawRect(width - extraDist / 10, -height / 2 - extraDist, extraDist / 10, extraDist);
	////// Top right
	////draw::drawRect(-width, height / 2 + extraDist * 0.9, extraDist, extraDist / 10);
	////draw::drawRect(-width, height / 2, extraDist / 10, extraDist);
	////// Top left
	////draw::drawRect(width - extraDist, height / 2 + extraDist * 0.9, extraDist, extraDist / 10);
	////draw::drawRect(width - extraDist / 10, height / 2, extraDist / 10, extraDist * 0.9);

	//glEnable(GL_DEPTH_TEST);
	////p_Minecraft->enableLightMap();
	//glPopMatrix();
	
}

INPUT ip{};
std::once_flag onceFlag;
void toadll::modules::auto_bridge()
{
	// TODO: check if enabled

	// define our ip struct
	std::call_once(onceFlag, []
	{
			ip.type = INPUT_KEYBOARD;
			ip.ki.wScan = 0; 
			ip.ki.time = 0;
			ip.ki.dwExtraInfo = 0;

			ip.ki.wVk = VK_SHIFT; 
		});

	auto world = p_Minecraft->get_world();
	if (world == NULL) return;
	auto lplayerpos = lPlayer->get_position();

	//auto lplayer = p_Minecraft->get_localplayer();
	/*auto blockpos = env->CallObjectMethod(*lplayer->obj, get_mid(*lplayer->obj, mapping::getBlockPosition));
	auto vec3 = to_vec3i(blockpos);*/
	//auto vec3 = lplayer->get_position();
	//vec3.x = floor(vec3.x);
	//vec3.y = floor(vec3.y);
	//vec3.z = floor(vec3.z);
	//log_Debug("vec3: (%f %f %f) absDist: %f", vec3.x, vec3.y, vec3.z, abs(vec3.dist(lplayer->get_position())));

	static bool flag = false;

	// extra checks and settings
	float rotPitch = p_Minecraft->get_localplayer()->get_rotationPitch();
	if (rotPitch < 61.f) return;

	//good for break blocks
	auto lookatobj = env->CallObjectMethod(p_Minecraft->get_mc(), get_mid(p_Minecraft->get_mc(), mapping::getObjectMouseOver));
	auto blokpoz = env->CallObjectMethod(lookatobj, get_mid(lookatobj, mapping::getBlockPos));

	// no dec
	auto jo = to_vec3i(blokpoz);

	if (jo.y != floor(lplayerpos.y) - 1)
	{
		env->DeleteLocalRef(blokpoz);
		env->DeleteLocalRef(lookatobj);
		env->DeleteLocalRef(world);
		return;
	}

	bool isEdge = false;

	if (jo.x > lplayerpos.x && abs(jo.x - lplayerpos.x) > 0.1f)
	{
		jo.x -= 1;
		isEdge = true;
	}
	else if (jo.x < lplayerpos.x && abs(jo.x - lplayerpos.x) > 1)
	{
		jo.x += 1;
		isEdge = true;
	}
	if (jo.z > lplayerpos.z && abs(jo.z - lplayerpos.z) > 0.1f)
	{
		jo.z -= 1;
		isEdge = true;
	}
	else if (jo.z < lplayerpos.z && abs(jo.z - lplayerpos.z) > 1)
	{
		jo.z += 1;
		isEdge = true;
	}

	auto vec3iClass = findclass(curr_client == minecraft_client::Vanilla ? "cj" : "net.minecraft.util.Vec3i");
	if (vec3iClass == NULL)
	{
		env->DeleteLocalRef(blokpoz);
		env->DeleteLocalRef(lookatobj);
		env->DeleteLocalRef(world);
		return;
	}

	auto vec3i = env->NewObject(vec3iClass, env->GetMethodID(vec3iClass, "<init>", "(III)V"), (int)jo.x, (int)jo.y, (int)jo.z);
	if (env->CallBooleanMethod(world, get_mid(world, mapping::isAirBlock), vec3i) && isEdge)
	{

		if (abs(jo.y - lplayerpos.y) == 2)
		{
			ip.ki.dwFlags = 0; // 0 for key press
			SendInput(1, &ip, sizeof(INPUT));
			flag = true;
		}

		if (!flag)
		{
			//std::cout << "down\n";
			
			ip.ki.dwFlags = 0; // 0 for key press
			SendInput(1, &ip, sizeof(INPUT));

			//PostMessage(GetForegroundWindow(), WM_KEYDOWN, VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC));
			Sleep(10);

			flag = !flag;
		}
	}
	else
	{
		if (flag)
		{
				
			ip.ki.dwFlags = KEYEVENTF_KEYUP; // 0 for key press
			SendInput(1, &ip, sizeof(INPUT));

			//keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
			//PostMessage(GetForegroundWindow(), WM_KEYUP, VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC));
			Sleep(10);
			flag = !flag;
		}
	}

	//log_Debug("pos{x=%f, y=%f, z=%f} isairblock %s edge %s", jo.x, jo.y, jo.z, env->CallBooleanMethod(world, get_mid(world, mapping::isAirBlock), vec3i) ? "yes" : "no", isEdge ? "yes" : "no");

	//Sleep(1);
}
