#include <PBRender/core/film.h>

namespace PBRender
{

GeometryFilm::GeometryFilm(Point2i fullResolution, std::string filename)
    : Film(fullResolution, filename),
      data(std::make_unique<Array2D<Pixel>>
    (fullResolution.x, fullResolution.y)) { }

GeometryFilm::~GeometryFilm() { data.reset(); }

void GeometryFilm::AddSample(const Point2i pFilm, const Vector3f &L) {
	Pixel &pixel = (*data)[pFilm];

	for (size_t i = 0; i < 3; ++i) {
		pixel.rgbSum[i] += L[i];
	}

	pixel.weightSum += 1.f;
}

Vector3f GeometryFilm::GetPixel(const Point2i pFilm) const {
	Pixel &pixel = (*data)[pFilm];
	Vector3f rgb;

	for (size_t i = 0; i < 3; ++i) {
		rgb[i] = pixel.rgbSum[i] / pixel.weightSum;
	}

	return rgb;
}

void GeometryFilm::WriteImage() const {
	const size_t W = fullResolution.x;
	const size_t H = fullResolution.y;

	auto buf = std::vector<char>(3 * H * W);

	for (size_t i = 0; i < W; ++i) {
		for (size_t j = 0; j < H; ++j) {
			int offset = i + W * j;

			Vector3f col = GetPixel(Point2i(i, j));
            buf[3 * offset + 0] = (uint8_t) PBRender::Clamp(255.f * col.x, 0.f, 255.f);
            buf[3 * offset + 1] = (uint8_t) PBRender::Clamp(255.f * col.y, 0.f, 255.f);
            buf[3 * offset + 2] = (uint8_t) PBRender::Clamp(255.f * col.z, 0.f, 255.f);
		}
	}

	stbi_write_png("output.png", 
					W, 
					H, 
					3, 
					buf.data(), 
					0);
}

} // namespace PBRender