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

#include "libs/lua/lua.h"
#include "libs/lua/lualib.h"
#include "libs/lua/lauxlib.h"
#include "libs/lua/compat-5.2.h"
#include "libs/luaobj/luaobj.h"

#include "libs/libsf2d/include/sf2d.h"
#include <sfil.h>
#include <sftd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <3ds.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define CONFIG_3D_SLIDERSTATE (*(float*)0x1FF81080)

typedef struct {
	sf2d_texture *texture;
} love_image;

typedef struct {
	sftd_font *font;
	int size;
} love_font;

typedef struct {
	u8* data;
	u32 size;
	u32 format;
	bool used;
} love_source;

extern lua_State *L;
extern int currentScreen;
extern int drawScreen;
extern char dsNames[32][32];
extern char *rootDir;
extern int shouldQuit;
extern love_font *currentFont;
extern int is3D;
extern const char *fontDefaultInit();
extern bool soundEnabled;