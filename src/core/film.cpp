// STB (note only to include in .cpp file)
#define STB_IMAGE_IMPLEMENTATION
#include <ext/stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <ext/stb/stb_image_write.h>

#include <PBRender/core/film.h>

namespace PBRender
{

RGBFilm::RGBFilm(Point2i fullResolution, std::string filename)
    : Film(fullResolution, filename),
      data(std::make_unique<Array2D<Pixel>>
    (fullResolution.x, fullResolution.y)) { }

RGBFilm::~RGBFilm() { data.reset(); }

void RGBFilm::AddSample(const Point2i pFilm, const Spectrum &L) {
	Pixel &pixel = (*data)[pFilm];

	for (size_t i = 0; i < 3; ++i) {
		pixel.rgbSum[i] += L[i];
	}

	pixel.weightSum += 1.f;
}

Spectrum RGBFilm::GetPixel(const Point2i pFilm) const {
	Pixel &pixel = (*data)[pFilm];
	Spectrum rgb;

	for (size_t i = 0; i < 3; ++i) {
		rgb[i] = pixel.rgbSum[i] / pixel.weightSum;
	}

	return rgb;
}

void RGBFilm::WriteImage() const {
	const size_t W = fullResolution.x;
	const size_t H = fullResolution.y;

	auto buf = std::vector<char>(3 * W * H);

	for (size_t i = 0; i < W; ++i) {
		for (size_t j = 0; j < H; ++j) {
			int offset = i + W * j;

			Spectrum rgb = GetPixel(Point2i(i, j)).Clamp(0.f, 255.f);
			
			for (size_t c = 0; c < 3; ++c) 
				buf[3 * offset + c] = static_cast<uint8_t>(rgb[c]);
		}
	}

	// save to .png file
	std::string filename_png = filename + ".png";

	stbi_write_png(filename_png.data(), 
					W, 
					H, 
					3, 
					buf.data(), 
					0);
}

GeometryFilm::GeometryFilm(Point2i fullResolution, std::string filename)
    : Film(fullResolution, filename),
      data(std::make_unique<Array2D<Pixel>>
    (fullResolution.x, fullResolution.y)) { }

GeometryFilm::~GeometryFilm() { data.reset(); }

void GeometryFilm::AddSample(const Point2i pFilm, const Spectrum &L) {
	Pixel &pixel = (*data)[pFilm];

	for (size_t i = 0; i < 3; ++i) {
		pixel.rgbSum[i] += L[i];
	}

	pixel.weightSum += 1.f;
}

Spectrum GeometryFilm::GetPixel(const Point2i pFilm) const {
	Pixel &pixel = (*data)[pFilm];
	Spectrum rgb;

	for (size_t i = 0; i < 3; ++i) {
		rgb[i] = pixel.rgbSum[i] / pixel.weightSum;
	}

	return rgb;
}

void GeometryFilm::WriteImage() const {
	const size_t W = fullResolution.x;
	const size_t H = fullResolution.y;

	auto buf = std::vector<char>(3 * W * H);

	for (size_t i = 0; i < W; ++i) {
		for (size_t j = 0; j < H; ++j) {
			int offset = i + W * j;

			Spectrum rgb = GetPixel(Point2i(i, j)) * 255.f;
			rgb = rgb.Clamp(0.f, 255.f);

			for (size_t c = 0; c < 3; ++c) 
				buf[3 * offset + c] = static_cast<uint8_t>(rgb[c]);
		}
	}

	// save to .png file
	std::string filename_png = filename + ".png";

	stbi_write_png(filename_png.data(), 
					W, 
					H, 
					3, 
					buf.data(), 
					0);
}

} // namespace PBRender