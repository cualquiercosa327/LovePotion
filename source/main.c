// This code is licensed under the MIT Open Source License.

// Copyright (c) 2015 Ruairidh Carmichael - ruairidhcarmichael@live.co.uk

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "shared.h"

char *rootDir = "game";

lua_State *L;

int initLove(lua_State *L);

bool errorOccured = false;

void displayError() {

	errorOccured = true;
	printf(lua_tostring(L, -1));

}

int main() {

	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requiref(L, "love", initLove, 1);

	sf2d_init(); // 2D Drawing lib.
	sftd_init(); // Text Drawing lib.

	sf2d_set_clear_color(RGBA8(0x0, 0x0, 0x0, 0xFF)); // Reset background color.

	// consoleInit(GFX_BOTTOM, NULL);

	luaL_dostring(L, "_defaultFont_ = love.graphics.newFont(); love.graphics.setFont(_defaultFont_)");

	luaL_dostring(L, "print(''); print('\x1b[1;36mLovePotion 1.0.5 BETA\x1b[0m (LOVE for 3DS)'); print('')"); // Ew.

	luaL_dostring(L, "package.path = 'game/?.lua;game/?/init.lua'"); // Set default requiring path.
	luaL_dostring(L, "package.cpath = 'game/?.lua;game/?/init.lua'");

	luaL_dostring(L, 
		"function love.errhand(msg)\n"
		"love.graphics.setBackgroundColor(89, 157, 220)\n"
		"love.graphics.setScreen('top')\n"
		"love.graphics.setFont(_defaultFont_)\n"
		"love.graphics.print('Oops, a Lua error has occured', 25, 25)\n"
		"love.graphics.print('Press Start to quit', 25, 40)\n"
		"love.graphics.printf(msg, 25, 70, love.graphics.getWidth() - 50)\n"
		"end"
	); // default love.errhand()

	if (luaL_dofile(L, "game/main.lua")) displayError();

	if (luaL_dostring(L, "if love.load then love.load() end")) displayError();

	while (aptMainLoop()) {

		if (shouldQuit == 1) break; // Quit event

		if (!errorOccured) {

			if (luaL_dostring(L,
				"love.keyboard.scan()\n"
				"love.timer.step()\n"
				"if love.update then love.update(love.timer.getDelta()) end")) {
					displayError();
			}

			// Top screen

			sf2d_start_frame(GFX_TOP, GFX_LEFT);

				if (luaL_dostring(L, "if love.draw then love.draw() end")) displayError();

			sf2d_end_frame();

			if (is3D) {

				sf2d_start_frame(GFX_TOP, GFX_RIGHT);

					if (luaL_dostring(L, "if love.draw then love.draw() end")) displayError();

				sf2d_end_frame();

			}

			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);

				if (luaL_dostring(L, "if love.draw then love.draw() end")) displayError();

			sf2d_end_frame();

			luaL_dostring(L, "love.graphics.present()");

		} else {

			hidScanInput();
			u32 kTempDown = hidKeysDown();
			if (kTempDown & KEY_START) shouldQuit = 1;

			char *errhandler[512];
			snprintf(errhandler, sizeof errhandler, "%s%s%s", "love.errhand(\"", lua_tostring(L, -1), "\")");

			sf2d_start_frame(GFX_TOP, GFX_LEFT);

				luaL_dostring(L, errhandler);

			sf2d_end_frame();

			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);

				luaL_dostring(L, errhandler);

			sf2d_end_frame();

			luaL_dostring(L, "love.graphics.present()");

		}

	}

	lua_close(L);

	sftd_fini();
	sf2d_fini();

	if (soundEnabled) csndExit();

	return 0;

}