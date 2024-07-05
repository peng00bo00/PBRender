// STB (note only to include in .cpp file)
#define STB_IMAGE_IMPLEMENTATION
#include <ext/stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <ext/stb/stb_image_write.h>

// tbb
#include <tbb/parallel_for.h>
#include <tbb/task_arena.h>
#include <tbb/blocked_range.h>

#include <PBRender/core/film.h>

namespace PBRender
{

RGBFilm::RGBFilm(Point2i fullResolution, std::string filename)
    : Film(fullResolution, filename),
      data(std::make_unique<Array2D<Pixel>>(fullResolution.x, fullResolution.y)) 
	{ }

RGBFilm::~RGBFilm() { data.reset(); }

void RGBFilm::AddSample(const Point2i pFilm, const Spectrum &L, float weight) {
	Pixel &pixel = (*data)[pFilm];

	for (size_t i = 0; i < 3; ++i) {
		pixel.rgbSum[i] += L[i] * weight;
	}

	pixel.weightSum += weight;
}

Spectrum RGBFilm::GetPixel(const Point2i pFilm) const {
	const Pixel pixel = (*data)[pFilm];
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

	// write to buf
	tbb::task_arena ta;
    ta.execute([&] {
        tbb::blocked_range<int> range(0, W*H);
        tbb::parallel_for(
            range,
            [&](const tbb::blocked_range<int> r) {
                for (int i=r.begin(); i<r.end(); ++i) {
                    int x = i % W;
					int y = i / W;

					Spectrum rgb = GetPixel(Point2i(x, y)) * 255.f;
					rgb = rgb.Clamp(0.f, 255.f);

					for (size_t c = 0; c < 3; ++c) 
						buf[3*i + c] = static_cast<uint8_t>(rgb[c]);
                }
            }
        );
    });

	// save to .png file
	std::string filename_png = filename + ".png";

	stbi_write_png(filename_png.data(), 
					W, 
					H, 
					3, 
					buf.data(), 
					0);
}

GBuffferFilm::GBuffferFilm(Point2i fullResolution, std::string filename)
    : Film(fullResolution, filename),
      data(std::make_unique<Array2D<Pixel>>(fullResolution.x, fullResolution.y)) 
	{ }

GBuffferFilm::~GBuffferFilm() { data.reset(); }

void GBuffferFilm::AddSample(const Point2i pFilm, const Spectrum &L, float weight) {
	Pixel &pixel = (*data)[pFilm];

	for (size_t i = 0; i < 3; ++i) {
		pixel.rgbSum[i] += L[i] * weight;
	}

	pixel.weightSum += weight;
}

Spectrum GBuffferFilm::GetPixel(const Point2i pFilm) const {
	const Pixel pixel = (*data)[pFilm];
	Spectrum rgb;

	for (size_t i = 0; i < 3; ++i) {
		rgb[i] = pixel.rgbSum[i] / pixel.weightSum;
	}

	return rgb;
}

void GBuffferFilm::WriteImage() const {
	const size_t W = fullResolution.x;
	const size_t H = fullResolution.y;

	auto buf = std::vector<char>(3 * W * H);

	// write to buf
	std::cout << "Writing pixels to buffer..." << std::endl;
	tbb::task_arena ta;
    ta.execute([&] {
        tbb::blocked_range<int> range(0, W*H);
        tbb::parallel_for(
            range,
            [&](const tbb::blocked_range<int> r) {
                for (int i=r.begin(); i<r.end(); ++i) {
                    int x = i % W;
					int y = i / W;

					Spectrum rgb = GetPixel(Point2i(x, y)) * 255.f;
					rgb = rgb.Clamp(0.f, 255.f);

					for (size_t c = 0; c < 3; ++c) 
						buf[3*i + c] = static_cast<uint8_t>(rgb[c]);
                }
            }
        );
    });

	std::cout << "Writing to buffer is finished!" << std::endl;

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