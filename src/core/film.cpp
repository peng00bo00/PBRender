#include <PBRender/core/film.h>

namespace PBRender
{

GeometryFilm::GeometryFilm(Point2i fullResolution, std::string filename)
    : Film(fullResolution, filename),
      data(std::make_unique<Array2D<Pixel>>
    (fullResolution.x, fullResolution.y)) { }

GeometryFilm::~GeometryFilm() { }

} // namespace PBRender