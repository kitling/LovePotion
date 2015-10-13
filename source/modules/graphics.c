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

#include "../shared.h"

int currentR = 0xFF;
int currentG = 0xFF;
int currentB = 0xFF;
int currentA = 0xFF;

int currentScreen = GFX_BOTTOM;

love_font *currentFont;

int transX = 0;
int transY = 0;
int isPushed = 0;

int is3D = 0;

int currentLineWidth = 1;

int currentDepth = 0;

u32 getCurrentColor() {

	return RGBA8(currentR, currentG, currentB, currentA);
 
}

int translateCoords(int *x, int *y) {

	// Emulates the functionality of lg.translate

	if (isPushed) {

		*x += transX;
		*y += transY;

	}

	// Sets depth of objects

	if (is3D) {

		float slider = CONFIG_3D_SLIDERSTATE;

		if (sf2d_get_current_side() == GFX_LEFT) *x -= (slider * currentDepth);
		if (sf2d_get_current_side() == GFX_RIGHT) *x += (slider * currentDepth);

	}

	return 0;

}

static int graphicsSetBackgroundColor(lua_State *L) { // love.graphics.setBackgroundColor()

	int r = luaL_checkinteger(L, 1);
	int g = luaL_checkinteger(L, 2);
	int b = luaL_checkinteger(L, 3);

	sf2d_set_clear_color(RGBA8(r, g, b, 0xFF));

	return 0;

}

static int graphicsSetColor(lua_State *L) { // love.graphics.setColor()

	int argc = lua_gettop(L);

	int r = luaL_checkinteger(L, 1);
	int g = luaL_checkinteger(L, 2);
	int b = luaL_checkinteger(L, 3);
	int a = NULL;

	if (argc > 3) {
		a = luaL_checkinteger(L, 4);
	}

	if (a == NULL) {
		a = currentA;
	}

	currentR = r;
	currentG = g;
	currentB = b;
	currentA = a;

	return 0;

}

static int graphicsGetColor(lua_State *L) { // love.graphics.getColor()

	lua_pushnumber(L, currentR);
	lua_pushnumber(L, currentG);
	lua_pushnumber(L, currentB);
	lua_pushnumber(L, currentA);

	return 4;

}

static int graphicsRectangle(lua_State *L) { // love.graphics.rectangle()

	if (sf2d_get_current_screen() == currentScreen) {

		char *mode = luaL_checkstring(L, 1);

		int x = luaL_checkinteger(L, 2);
		int y = luaL_checkinteger(L, 3);

		translateCoords(&x, &y);

		int w = luaL_checkinteger(L, 4);
		int h = luaL_checkinteger(L, 5);

		if (strcmp(mode, "fill") == 0) {
			sf2d_draw_rectangle(x, y, w, h, getCurrentColor());
		} else if (strcmp(mode, "line") == 0) {
			sf2d_draw_line(x, y, x, y + h, currentLineWidth, getCurrentColor());
			sf2d_draw_line(x, y, x + w, y, currentLineWidth, getCurrentColor());

			sf2d_draw_line(x + w, y, x + w, y + h, currentLineWidth, getCurrentColor());
			sf2d_draw_line(x, y + h, x + w, y + h, currentLineWidth, getCurrentColor());
		}

	}

	return 0;

}

static int graphicsCircle(lua_State *L) { // love.graphics.circle()

	if (sf2d_get_current_screen() == currentScreen) {

		int step = 15;

		char *mode = luaL_checkstring(L, 1);
		int x = luaL_checkinteger(L, 2);
		int y = luaL_checkinteger(L, 3);
		int r = luaL_checkinteger(L, 4);

		translateCoords(&x, &y);

		sf2d_draw_line(x, y, x, y, 1, getCurrentColor()); // Fixes weird circle bug.
		sf2d_draw_fill_circle(x, y, r, getCurrentColor());

	}

	return 0;

}

static int graphicsLine(lua_State *L) { // love.graphics.line() -- Semi-Broken

	if (sf2d_get_current_screen() == currentScreen) {

		int argc = lua_gettop(L);
		int i = 0;

		if ((argc/2)*2 == argc) {
			for( i; i < argc / 2; i++) {

				int t = i * 4;

				int x1 = luaL_checkinteger(L, t + 1);
				int y1 = luaL_checkinteger(L, t + 2);
				int x2 = luaL_checkinteger(L, t + 3);
				int y2 = luaL_checkinteger(L, t + 4);

				translateCoords(&x1, &y1);
				translateCoords(&x2, &y2);

				sf2d_draw_line(x1, y1, x2, y2, currentLineWidth, getCurrentColor());

			}
		}

	}

	return 0;

}

static int graphicsGetScreen(lua_State *L) { // love.graphics.getScreen()

	if (currentScreen == GFX_TOP) {
		lua_pushstring(L, "top");
	} else if (currentScreen == GFX_BOTTOM) {
		lua_pushstring(L, "bottom");
	}

	return 1;

}

static int graphicsSetScreen(lua_State *L) { // love.graphics.setScreen()

	char *screen = luaL_checkstring(L, 1);

	if (strcmp(screen, "top") == 0) {
		currentScreen = GFX_TOP;
	} else if (strcmp(screen, "bottom") == 0) {
		currentScreen = GFX_BOTTOM;
	}

	return 0;

}

static int graphicsGetSide(lua_State *L) { // love.graphics.getSide()

	if (sf2d_get_current_side() == GFX_LEFT) {
		lua_pushstring(L, "left");
	} else if (sf2d_get_current_side() == GFX_RIGHT) {
		lua_pushstring(L, "right");
	}

	return 1;

}

static int graphicsPresent(lua_State *L) { // love.graphics.present()

	sf2d_swapbuffers();

	return 0;

}

static int graphicsGetWidth(lua_State *L) { // love.graphics.getWidth()

	int topWidth = 400;
	int botWidth = 320;

	int returnWidth;

	// TODO: Take screen sides into account.

	if (currentScreen == GFX_TOP) {

		returnWidth = topWidth;

	} else if (currentScreen == GFX_BOTTOM) {

		returnWidth = botWidth;

	}

	lua_pushnumber(L, returnWidth);

	return 1;

}

static int graphicsGetHeight(lua_State *L) { // love.graphics.getHeight()

	int returnWidth = 240;

	lua_pushnumber(L, returnWidth);

	return 1;

}

static int graphicsDraw(lua_State *L) { // love.graphics.draw()

	if (sf2d_get_current_screen() == currentScreen) {

		love_image *img = luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGE);

		int x = luaL_checkinteger(L, 2);
		int y = luaL_checkinteger(L, 3);
		float rad = luaL_optnumber(L, 4, 0);

		translateCoords(&x, &y);

		if (rad == 0) {

			sf2d_draw_texture_blend(img->texture, x, y, getCurrentColor());

		} else {

			sf2d_draw_texture_rotate_blend(img->texture, x + img->texture->width / 2, y + img->texture->height / 2, rad, getCurrentColor());

		}

	}

	return 0;

}

static int graphicsSetFont(lua_State *L) { // love.graphics.setFont()

	currentFont = luaobj_checkudata(L, 1, LUAOBJ_TYPE_FONT);

	return 0;

}

static int graphicsPrint(lua_State *L) { // love.graphics.print()

	if (sf2d_get_current_screen() == currentScreen) {

		if (currentFont) {

			char *printText = luaL_checkstring(L, 1);
			int x = luaL_checkinteger(L, 2);
			int y = luaL_checkinteger(L, 3);

			translateCoords(&x, &y);

			sftd_draw_text(currentFont->font, x, y, getCurrentColor(), currentFont->size, printText);

		}

	}

	return 0;

}

static int graphicsPrintFormat(lua_State *L) {

	if (sf2d_get_current_screen() == currentScreen) {

		if (currentFont) {

			char *printText = luaL_checkstring(L, 1);
			int x = luaL_checkinteger(L, 2);
			int y = luaL_checkinteger(L, 3);
			int limit = luaL_checkinteger(L, 4);
			char *align = luaL_optstring(L, 5, "left");

			int width = sftd_get_text_width(currentFont->font, currentFont->size, printText);

			if (strcmp(align, "center") == 0) {

				if (width < limit) {
					x += (limit / 2) - (width / 2);
				}

			} else if (strcmp(align, "right") == 0) {

				if (width < limit) {
					x += limit - width;
				}

			}

			translateCoords(&x, &y);

			if (x > 0) limit += x; // Quick text wrap fix, needs removing once sf2dlib is updated.

			sftd_draw_text_wrap(currentFont->font, x, y, getCurrentColor(), currentFont->size, limit, printText);

		}

	}

	return 0;

}

static int graphicsPush(lua_State *L) { // love.graphics.push()

	if (sf2d_get_current_screen() == currentScreen) {

		isPushed = 1;

	}

	return 0;

}

static int graphicsPop(lua_State *L) { // love.graphics.pop()

	if (sf2d_get_current_screen() == currentScreen) {

		isPushed = 0;

	}

	return 0;

}

static int graphicsOrigin(lua_State *L) { // love.graphics.origin()

	if (sf2d_get_current_screen() == currentScreen) {

		transX = 0;
		transY = 0;

	}

	return 0;

}

static int graphicsTranslate(lua_State *L) { // love.graphics.translate()

	if (sf2d_get_current_screen() == currentScreen) {

		int dx = luaL_checkinteger(L, 1);
		int dy = luaL_checkinteger(L, 2);

		transX = dx;
		transY = dy;

	}

	return 0;

}

static int graphicsSet3D(lua_State *L) { // love.graphics.set3D()

	is3D = lua_toboolean(L, 1);
	sf2d_set_3D(is3D);

	return 0;

}

static int graphicsGet3D(lua_State *L) { // love.graphics.get3D()

	lua_pushboolean(L, is3D);

	return 1;

}

static int graphicsSetDepth(lua_State *L) { // love.graphics.setDepth()

	currentDepth = luaL_checkinteger(L, 1);

	return 0;

}

static int graphicsGetDepth(lua_State *L) {

	lua_pushnumber(L, currentDepth);

	return 1;

}

static int graphicsSetLineWidth(lua_State *L) { // love.graphics.setLineWidth()

	currentLineWidth = luaL_checkinteger(L, 1);

	return 0;

}

static int graphicsGetLineWidth(lua_State *L) { // love.graphics.getLineWidth()

	lua_pushnumber(L, currentLineWidth);

	return 1;

}

int imageNew(lua_State *L);
int fontNew(lua_State *L);

const char *fontDefaultInit(love_font *self, int size);

int initLoveGraphics(lua_State *L) {

	luaL_Reg reg[] = {
		{ "setBackgroundColor",	graphicsSetBackgroundColor	},
		{ "setColor",			graphicsSetColor			},
		{ "getColor",			graphicsGetColor			},
		{ "rectangle",			graphicsRectangle			},
		{ "circle",				graphicsCircle				},
		{ "line",				graphicsLine				},
		{ "getScreen",			graphicsGetScreen			},
		{ "setScreen",			graphicsSetScreen			},
		{ "getSide",			graphicsGetSide				},
		{ "present",			graphicsPresent				},
		{ "getWidth",			graphicsGetWidth			},
		{ "getHeight",			graphicsGetHeight			},
		{ "newImage",			imageNew					},
		{ "newFont",			fontNew						},
		{ "draw",				graphicsDraw				},
		{ "setFont",			graphicsSetFont				},
		{ "print",				graphicsPrint				},
		{ "printf",				graphicsPrintFormat			},
		{ "push",				graphicsPush				},
		{ "pop",				graphicsPop					},
		{ "origin",				graphicsOrigin				},
		{ "translate",			graphicsTranslate			},
		{ "set3D",				graphicsSet3D				},
		{ "get3D",				graphicsGet3D				},
		{ "setDepth",			graphicsSetDepth			},
		{ "getDepth",			graphicsGetDepth			},
		{ "setLineWidth",		graphicsSetLineWidth		},
		{ "getLineWidth",		graphicsGetLineWidth		},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);

	return 1;

}