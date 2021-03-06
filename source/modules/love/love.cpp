#include "common/luax.h"

#include "common/version.h"
#include "modules/love.h"

#include "common/backend/input.h"

#include "modules/audio/wrap_audio.h"
#include "modules/data/wrap_datamodule.h"
#include "modules/event/wrap_event.h"
#include "modules/filesystem/wrap_filesystem.h"
#if defined (__SWITCH__)
    #include "modules/font/wrap_fontmodule.h"
#endif
#include "modules/graphics/wrap_graphics.h"
#include "modules/joystick/wrap_joystick.h"
#include "modules/keyboard/wrap_keyboard.h"
#include "modules/math/wrap_mathmodule.h"
#include "modules/sound/wrap_sound.h"
#include "modules/system/wrap_system.h"
#include "modules/thread/wrap_threadmodule.h"
#include "modules/timer/wrap_timer.h"
#include "modules/touch/wrap_touch.h"
#include "modules/window/wrap_window.h"

#include "luasocket/luasocket.h"

#include "boot_lua.h"

/*
** @func Initialize
** Initializes the framework
*/
int Love::Initialize(lua_State * L)
{
    Luax::InsistPinnedThread(L);

    Luax::InsistGlobal(L, "love");

    //--------------CONSTANTS----------------//

    // love._os = "Horizon"
    lua_pushstring(L, "Horizon");
    lua_setfield(L, -2, "_os");

    //love._console_name
    lua_pushstring(L, LOVE_POTION_CONSOLE);
    lua_setfield(L, -2, "_console_name");

    //love._potion_version
    lua_pushstring(L, Version::LOVE_POTION);
    lua_setfield(L, -2, "_potion_version");

    // love._version
    lua_pushstring(L, Version::LOVE);
    lua_setfield(L, -2, "_version");

    // love._(major, minor, revision, codename)
    lua_pushnumber(L, Version::MAJOR);
    lua_setfield(L, -2, "_version_major");

    lua_pushnumber(L, Version::MINOR);
    lua_setfield(L, -2, "_version_minor");

    lua_pushnumber(L, Version::REVISION);
    lua_setfield(L, -2, "_version_revision");

    lua_pushstring(L, Version::CODENAME);
    lua_setfield(L, -2, "_version_codename");

    // End Constants -- namespace functions //

    lua_pushcfunction(L, GetVersion);
    lua_setfield(L, -2, "getVersion");

    lua_pushcfunction(L, _OpenConsole);
    lua_setfield(L, -2, "_openConsole");

    lua_pushcfunction(L, EnableAccelerometerAsJoystick);
    lua_setfield(L, -2, "_setAccelerometerAsJoystick");

    //---------------------------------------//

    const luaL_Reg modules[] =
    {
        { "love.audio",       Wrap_Audio::Register        },
        { "love.data",        Wrap_DataModule::Register   },
        { "love.event",       Wrap_Event::Register,       },
        { "love.graphics",    Wrap_Graphics::Register,    },
        { "love.filesystem",  Wrap_Filesystem::Register,  },
        #if defined(__SWITCH__)
         { "love.font",       Wrap_FontModule::Register  },
        #endif
        { "love.joystick",    Wrap_Joystick::Register,    },
        { "love.keyboard",    Wrap_Keyboard::Register     },
        { "love.math",        Wrap_Math::Register         },
        { "love.sound",       Wrap_Sound::Register        },
        { "love.system",      Wrap_System::Register       },
        { "love.thread",      Wrap_ThreadModule::Register },
        { "love.timer",       Wrap_Timer::Register,       },
        { "love.touch",       Wrap_Touch::Register        },
        { "love.window",      Wrap_Window::Register,      },
        { "love.boot",        Boot                        },
        { 0,                  0 }
    };

    // preload all the modules
    for (size_t i = 0; modules[i].name  != nullptr; i++)
        Luax::Preload(L, modules[i].func, modules[i].name);

    Luax::Require(L, "love.data");
    lua_pop(L, 1);

    // LuaSocket
    love::luasocket::__open(L);

    // lua 5.3 stuff
    Luax::Preload(L, luaopen_luautf8, "utf8");

    return 1;
}

int Love::EnableAccelerometerAsJoystick(lua_State * L)
{
    return 0;
}

/*
** @func Boot
** Boots LOVE
*/
int Love::Boot(lua_State * L)
{
    if (luaL_loadbuffer(L, (const char *)boot_lua, boot_lua_size, "boot.lua") == 0)
        lua_call(L, 0, 1);

    return 1;
}

/*
** @func GetVersion
** Return the version for the framework.
*/
int Love::GetVersion(lua_State * L)
{
    lua_pushnumber(L, Version::MAJOR);
    lua_pushnumber(L, Version::MINOR);
    lua_pushnumber(L, Version::REVISION);
    lua_pushstring(L, Version::CODENAME);

    return 4;
}

/*
** Initialize the 'console'. Console for your console.
** On Switch, redirects printf to nxlink
** On 3DS, redirects to gdb
*/
int Love::_OpenConsole(lua_State * L)
{
    #if defined (__SWITCH__)
        nxlinkStdioForDebug();

        lua_pushboolean(L, true);
    #else
        gdbHioDevInit();
        int success = gdbHioDevRedirectStdStreams(false, true, true);

        lua_pushboolean(L, success == 0);
    #endif

    return 1;
}