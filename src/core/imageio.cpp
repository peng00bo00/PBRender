#include "imageio.h"
#include "spectrum.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace PBRender {

// ImageIO Declarations
std::unique_ptr<RGBSpectrum[]> ReadImage(const std::string &filename, Point2i *resolution) {
    float *data;
    RGBSpectrum *data_s;

    if (filename != "") {
        int imageWidth, imageHeight, nrComponents;

        stbi_set_flip_vertically_on_load(true);
        data = stbi_loadf(filename.c_str(), &imageWidth, &imageHeight, &nrComponents, 0);

        if (!data) return nullptr;

        data_s = new RGBSpectrum[imageWidth * imageHeight];

        for (size_t i = 0; i < imageWidth; ++i) {
            for (size_t j = 0; j < imageHeight; ++j) {
                RGBSpectrum r;

                r[0] = data[(i + j * imageWidth) * nrComponents + 0];
                r[1] = data[(i + j * imageWidth) * nrComponents + 1];
                r[2] = data[(i + j * imageWidth) * nrComponents + 2];

                data_s[i + j * imageWidth] = r;
            }
        }
        
        resolution->x = imageWidth;
        resolution->y = imageHeight;
        free(data);
        
        return std::unique_ptr<RGBSpectrum[]>(data_s);
    }

    return nullptr;
}

void WriteImage(const char* filename, const char *rgb,
                const Point2i &fullResolution) {
    stbi_write_png(filename, fullResolution.x, fullResolution.y, 3, rgb, 0);
}

void WriteImage(const char* name, const char *rgb,
                const Vector2f &resolution) {
    const Point2i fullResolution((int)resolution.x, (int)resolution.y);
    WriteImage(name, rgb, fullResolution);
}

}