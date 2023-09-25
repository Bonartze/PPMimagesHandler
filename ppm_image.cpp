#include "ppm_image.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

    static const string_view PPM_SIG = "P6"sv;
    static const int PPM_MAX = 255;

    bool SavePPM(const Path &file, const Image &image) {
        fstream f(file, ios::binary | ios::out);
        f << PPM_SIG << '\n' << image.GetWidth() << ' ' << image.GetHeight() << '\n' << PPM_MAX << '\n';
        for (int y = 0; y < image.GetHeight(); y++) {
            const Color *line = image.GetLine(y);
            for (int x = 0; x < image.GetWidth(); x++) {
                if (!f.put(static_cast<char>(line[x].r)))
                    return false;
                if (!(f.put(static_cast<char>(line[x].g))))
                    return false;
                if (!(f.put(static_cast<char>(line[x].b))))
                    return false;
            }
        }
        return true;
    }

    void NegateInplace(img_lib::Image &image) {
        for (int y = 0; y < image.GetHeight(); y++)
            for (int x = 0; x < image.GetWidth(); x++)
                image.GetPixel(x, y).r = static_cast<std::byte>(255 - static_cast<char> (image.GetPixel(x, y).r));
    }

    Image LoadPPM(const Path &file) {
        ifstream ifs(file, ios::binary);
        std::string sign;
        int w, h, color_max;

        ifs >> sign >> w >> h >> color_max;
        if (sign != PPM_SIG || color_max != PPM_MAX) {
            return {};
        }

        const char next = ifs.get();
        if (next != '\n') {
            return {};
        }

        Image result(w, h, Color::Black());
        std::vector<char> buff(w * 3);

        for (int y = 0; y < h; ++y) {
            Color *line = result.GetLine(y);
            ifs.read(buff.data(), w * 3);

            for (int x = 0; x < w; ++x) {
                line[x].r = static_cast<byte>(buff[x * 3 + 0]);
                line[x].g = static_cast<byte>(buff[x * 3 + 1]);
                line[x].b = static_cast<byte>(buff[x * 3 + 2]);
            }
        }

        return result;
    }

}  // namespace img_lib
