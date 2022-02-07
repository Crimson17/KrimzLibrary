#pragma once

#include <string>
#include <vector>

#include "KrimzLib/vecmath/ivec2.h"
#include "KrimzLib/graphics/color.h"


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
		int getWidth() const;
		int getHeight() const;
		kl::ivec2 getSize() const;
		kl::color getPixel(const kl::ivec2& point) const;
		int getPixelCount() const;
		byte* pointer() const;

		// Setters
		void setWidth(int width);
		void setHeight(int height);
		void setSize(const kl::ivec2& size);
		void setPixel(const kl::ivec2& point, const kl::color& color);

		// Reads an image file and stores it in the image instance
		void fromFile(const std::string& filePath);

		// Saves the image to a file
		void toFile(const std::string& fileName);

		// Fils the image with solid color
		void fillSolid(const kl::color& color);

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