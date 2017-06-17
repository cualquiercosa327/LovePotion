extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>

	#include <compat-5.2.h>
	#include <luaobj.h>
}

#include <3ds.h>

#include <citro3d.h>
#include <string>

std::string debug;

#include <ivorbiscodec.h>
#include <ivorbisfile.h>

#include "common/version.h"
#include "modules/love/love.h"

#include "modules/audio/source.h"
love::Source * streams[24];

#include "modules/graphics/crendertarget.h"
#include "modules/graphics/graphics.h"

bool LUA_ERROR = false;
bool LOVE_QUIT = false;
bool AUDIO_ENABLED = false;
bool FUSED = false;
bool runThreads = true;


#include <unistd.h>

#include "boot_lua.h"

#define luaL_dobuffer(L, b, n, s) (luaL_loadbuffer(L, b, n, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

#include "common/console.h"

int main(int argc, char ** argv)
{
	lua_State * L = luaL_newstate();

	luaL_openlibs(L);
	luaL_requiref(L, "love", loveInit, 1);

	AUDIO_ENABLED = !ndspInit();

	Result romfs = romfsInit();
	FUSED = (romfs) ? false : true;

	if (FUSED)
	{
		chdir("romfs:/");
	}
	else
	{
		char cwd[256];
		getcwd(cwd, 256);

		strcat(cwd, "game/");

		chdir(cwd);
	}

	love::Console * console = new love::Console();
	console->Enable(GFX_BOTTOM);

	if (!AUDIO_ENABLED)
		console->ThrowError("Audio failed to initialize!\nPlease dump your console's DSPfirm!");

	printf("\e[1;36mLOVE\e[0m %s for 3DS\n\n", love::VERSION);

	osSetSpeedupEnable(true);

	luaL_dobuffer(L, (char *)boot_lua, boot_lua_size, "boot");

	if (luaL_dofile(L, "main.lua"))
		console->ThrowError(L);
	
	if(luaL_dostring(L, "if love.load then love.load() end"))
		console->ThrowError(L);

	//fix delta time bug
	if (luaL_dostring(L, "love.timer.step()"))
		console->ThrowError(L);
	
	while (aptMainLoop())
	{
		if (LOVE_QUIT)
			break;

		if (!LUA_ERROR)
		{
			loveScan(L);

			if (luaL_dostring(L, "love.timer.step()"))
				console->ThrowError(L);
			
			for (int i = 0; i <= 23; i++)
				if (streams[i])
					streams[i]->Update();

			if(luaL_dostring(L, "if love.update then love.update(love.timer.getDelta()) end"))
				console->ThrowError(L);

			if (CLOSE_KEYBOARD)
			{
				luaL_dostring(L, "love.keyboard.setTextInput(false)");
				CLOSE_KEYBOARD =  false;
			}

			love::Graphics::Instance()->Render(GFX_TOP);

			if (luaL_dostring(L, "if love.draw then love.draw() end"))
				console->ThrowError(L);

			//love::Graphics::Instance()->Render(GFX_BOTTOM);
				
			//if (luaL_dostring(L, "if love.draw then love.draw() end"))
			//	console->ThrowError(L);

			love::Graphics::Instance()->SwapBuffers();
		}
		else
		{
			const char * errorMessage = lua_tostring(L, -1);

			if (errorMessage != nullptr)
				console->ThrowError(errorMessage);

			hidScanInput();
			u32 kTempDown = hidKeysDown();
			if (kTempDown & KEY_START)
				break;
		}
	}

	luaL_dostring(L, "love.audio.stop()");

	loveClose(L);

	C3D_Fini();
	gfxExit();
	
	if (AUDIO_ENABLED) 
		ndspExit();
	
	if (FUSED) 
		romfsExit();

	return 0;
}