#include "chemuPixelEngine.h"
#include <iostream>

// O------------------------------------------------------------------------------O
// | pixelEngine INTERFACE IMPLEMENTATION (CORE)							      |
// O------------------------------------------------------------------------------O
namespace cpe
{
	// O------------------------------------------------------------------------------O
	// | cpe::Pixel IMPLEMENTATION                                                    |
	// O------------------------------------------------------------------------------O
	Pixel::Pixel() {
		r = 0;
		g = 0;
		b = 0;
	}

	Pixel::Pixel(uint8_t red, uint8_t green, uint8_t blue) {
		r = red;
		g = green;
		b = blue;
	}



	// O------------------------------------------------------------------------------O
	// | cpe::Sprite IMPLEMENTATION                                                   |
	// O------------------------------------------------------------------------------O

	Sprite::Sprite() {
		width = 0;
		height = 0;
	}

	Sprite::Sprite(int w, int h) {
		width = w;
		height = h;

		data.resize(width * height);
	}

	void Sprite::resize(int w, int h) {
		width = w;
		height = h;

		data.resize(width * height);
	}

	void Sprite::toTexture(SDL_Renderer* renderer, SDL_Texture*& texture) {
		std::vector<uint32_t> sdlPixels(width * height);
		for (int i = 0; i < width * height; i++) {
			const cpe::Pixel& p = data[i];
			sdlPixels[i] = (255 << 24) | (p.r << 16) | (p.g << 8) | (p.b);
		}

		texture = SDL_CreateTexture(renderer,
									SDL_PIXELFORMAT_ARGB8888,
									SDL_TEXTUREACCESS_STATIC,
									width,
									height);

		SDL_UpdateTexture(texture, nullptr, sdlPixels.data(), width * sizeof(uint32_t));
	}


	void Sprite::setPixel(int x, int y, Pixel color) {
		data[y * width + x] = color;
	}

	Pixel& Sprite::getPixel(int x, int y) {
		return data[y * width + x];
	}

	// O------------------------------------------------------------------------------O
	// | cpe::pixelEngine IMPLEMENTATION                                              |
	// O------------------------------------------------------------------------------O
	pixelEngine::pixelEngine() {
		constructFontSheet();
	}

	void pixelEngine::constructFontSheet() {
		std::string data;
		data += "?QP0001oOc`0o009@F809009GD8090<9GDH090L97c`090097QP0600600000000"; // " 0@P`p"
		data += "H000000L6400620R5T@>95A14GkO95A15WkO95AA640>97PRH400o41L00000000"; // "!1AQaq"
		data += "Of@0001oBGH3O7l9BG8344P9BE8024P9CUX314PIB4h314PY<4`0131600000000"; // ""2BRbr"
		data += "7200000V8V8DB3Q9@DXnE4A9@DXDE4A9@DhnE4A9@GHDE4A98S8082Pb00000000"; // "#3CScs"
		data += "Oc000001@CPT1301@B`^14P1@BI[?TQo@Gi[A4P18WhjA4P1720B07l100000000"; // "$4DTdt"
		data += "Obi2000OBFhU?3PPBDXB@5A0BDX8@5A0BDXT@5A0BGYBO5A0@C0Q05Qo00000000"; // "%5EUeu"
		data += "Oc`P00032Gif70P<2DY?80P`2DYI@7i02DYf80T`2GXP70T<0C1H00X300000000"; // "&6FVfv"
		data += "70H0?01o8PH0@9P`@F808:@<BG8F6:@2BAX>8:@<B@h0@:@`>@H0?7ao00000000"; // "'7GWgw"
		data += "Oc@0101127h0A7mS24Xh:0PD24Y440P824Z2:0PD27j2A71SOc@0@01100000000"; // "(8HXhx"
		data += "00`00001@Eh0W002@E:2X004Oe:2X7Yh@E94X004@GhhO00203`0000100000000"; // ")9IYiy"
		data += "<0000011@@0:A01Q@@04I81A@FH>E819?fH4E7Y50@0:C0130@00A01100000000"; // "*:JZjz"
		data += "O`0007l0400821002008MbQo3;Hn@BQ14WH8@DA18@0804@0@000000000000000"; // "+;K[k{"
		data += "O`000001@0000002@0P00004@1B`Ogl8@29`000@@440000P0000001000000000"; // ",<L\l|"
		data += "O`0007`00P0800@011@8@@A121@8@GQ111@8M`Ao0Q@820@0O`0007P000000000"; // "-=M]m}"
		data += "O`0000080P0047`4144020@2229P20@141AP40@280P047P4O`00200800000000"; // ".>N^n~"
		data += "70I000008PLPN3Q0@@<@A4A0@FT8@TA0@Fd4A4A08PL2N4A070L103P000800000"; // "/?O_o(del)"

		font.resize(128, 48);

		int px = 0, py = 0;
		for (size_t b = 0; b < 64 * 16; b += 4) {
			uint32_t sym1 = (uint32_t)data[b + 0] - 48;
			uint32_t sym2 = (uint32_t)data[b + 1] - 48;
			uint32_t sym3 = (uint32_t)data[b + 2] - 48;
			uint32_t sym4 = (uint32_t)data[b + 3] - 48;
			uint32_t r = sym1 << 18 | sym2 << 12 | sym3 << 6 | sym4;

			for (int i = 0; i < 24; i++) {
				int k = r & (1 << i) ? 255 : 0;
				font.setPixel(px, py, cpe::Pixel(k, k, k));
				if (++py == 48) {
					px++;
					py = 0;
				}
			}
		}
	}

	void pixelEngine::DrawString(int x, int y, std::string text, Pixel color) {
		//x += 161 * SCALE; // DEBUG_X
		//y += 1 * SCALE;
		SDL_SetTextureColorMod(fontTexture, color.r, color.g, color.b);
		int init_x = x;
		for (size_t i = 0; i < text.length(); i++) {
			
			char c = text[i];
			if (c == '\n') {
				x = init_x;
				y += (2 * SCALE);
				continue;
			}
			int index = c - 32;
			int srcX = (index % 16) * 8;
			int srcY = (index / 16) * 8;
			
			SDL_FRect srcRect = { static_cast<float>(srcX), static_cast<float>(srcY), 8, 8 };
			SDL_FRect dstRect = { static_cast<float>(x), static_cast<float>(y), 16, 16 };
			x += (2 * SCALE);

			SDL_RenderTexture(renderer, fontTexture, &srcRect, &dstRect);
		}
	}
}