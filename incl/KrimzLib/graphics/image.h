#pragma once

#include <string>
#include <vector>

#include "KrimzLib/vecmath/ivec2.h"
#include "KrimzLib/graphics/color.h"

#pragma comment(lib, "gdiplus.lib")


namespace kl {
	class image {
	private:
		int width;
		int height;
		std::vector<kl::color> pixels;

	public:
		// Constructor
		image();
		image(const kl::ivec2& size, const kl::color& color = {});
		image(const char* fileName);

		// Getters
		int gWidth() const;
		int gHeight() const;
		kl::ivec2 gSize() const;
		kl::color gPixel(const kl::ivec2& point) const;
		int pixelCount() const;
		byte* pointer() const;
		kl::image gRect(kl::ivec2 a, kl::ivec2 b) const;

		// Setters
		void sWidth(int width);
		void sHeight(int height);
		void sSize(const kl::ivec2& size);
		void sPixel(const kl::ivec2& point, const kl::color& color);

		// Reads an image file and stores it in the image instance
		void fromFile(const std::string& filePath);

		// Saves the image to a file
		void toFile(const std::string& fileName);

		// Fils the image with solid color
		void fill(const kl::color& color);

		// Flips the pixel on x axis
		void flipHorizontal();

		// Flips the pixel on y axis
		void flipVertical();

		// Draws a line between 2 points
		void drawLine(const kl::ivec2& a, const kl::ivec2& b, const kl::color& col);

		// Draws a triangle between 3 points
		void drawTriangle(kl::ivec2 a, kl::ivec2 b, kl::ivec2 c, const kl::color& col, bool fill = false);

		// Draws a rectangle between 2 points
		void drawRectangle(kl::ivec2 a, kl::ivec2 b, const kl::color& col, bool fill = false);

		// Draws a circle with the given center point and radius
		void drawCircle(const kl::ivec2& p, float r, const kl::color& col, bool fill = false);
		// Draws a circle between 1 center and 1 outer point
		void drawCircle(const kl::ivec2& a, const kl::ivec2& b, const kl::color& col, bool fill = false);

		// Converts an image to an ASCII frame
		std::string toASCII(const kl::ivec2& frameSize);
	};
}