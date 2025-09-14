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
		data += "?QP0001oOc`0o009@F809009GD8090<9GDH090L97c`090097QP0600600000000";
		data += "H000000L6400620R5T@>95A14GkO95A15WkO95AA640>97PRH400o41L00000000";
		data += "Of@0001oBGH3O7l9BG8344P9BE8024P9CUX314PIB4h314PY<4`0131600000000";
		data += "7200000V8V8DB3Q9@DXnE4A9@DXDE4A9@DhnE4A9@GHDE4A98S8082Pb00000000";
		data += "Oc000001@CPT1301@B`^14P1@BI[?TQo@Gi[A4P18WhjA4P1720B07l100000000";
		data += "Obi2000OBFhU?3PPBDXB@5A0BDX8@5A0BDXT@5A0BGYBO5A0@C0Q05Qo00000000";
		data += "Oc`P00032Gif70P<2DY?80P`2DYI@7i02DYf80T`2GXP70T<0C1H00X300000000";
		data += "70H0?01o8PH0@9P`@F808:@<BG8F6:@2BAX>8:@<B@h0@:@`>@H0?7ao00000000";

		data += "?C@0101127x0Q7}C24(>:0PT24)440P824*2:0PT27Z2Q71C?C@0@01100000000";
		data += "00p00001@Ux0W002@U*2X004?U*2X7Yx@U)4X004@Wxx?00203p0000100000000";

		font.resize(128, 48);

		int px = 0, py = 0;
		for (size_t b = 0; b < 64 * 10; b += 4) {
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
}