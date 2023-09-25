#include "ppm_image.h"

#include <array>
#include <algorithm>
#include <fstream>
#include <string_view>
#include <cmath>

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

    void HMirrInplace(img_lib::Image &image) {
        for (int j = 0; j < image.GetHeight() / 2; j++)
            for (int i = 0; i < image.GetWidth(); i++)
                std::swap(image.GetPixel(i, j), image.GetPixel(i, image.GetHeight() - 1 - j));
    }

    void VMirrInplace(img_lib::Image &image) {
        for (int j = 0; j < image.GetHeight(); j++)
            for (int i = 0; i < image.GetWidth() / 2; i++)
                std::swap(image.GetPixel(i, j), image.GetPixel(image.GetWidth() - i - 1, j));
    }

    img_lib::Image Sobel(const img_lib::Image &image, img_lib::Image& result) {
        for (int j = 1; j < image.GetHeight() - 1; j++) {   // i - столбец, j - строка
            for (int i = 1; i < image.GetWidth() - 1; i++) {
                auto tl = image.GetPixel(i - 1, j - 1);
                auto tc = image.GetPixel(i, j - 1);
                auto tr = image.GetPixel(i + 1, j - 1);
                auto cl = image.GetPixel(i - 1, j);
                auto bl = image.GetPixel(i - 1, j + 1);
                auto bc = image.GetPixel(i, j + 1);
                auto br = image.GetPixel(i + 1, j + 1);
                auto cr = image.GetPixel(i + 1, j);
                auto r = sqrt((-(int) tl.r - 2 * (int) tc.r - (int) tr.r + (int) bl.r + 2 * (int) bc.r + (int) br.r) *
                              (-(int) tl.r - 2 * (int) tc.r - (int) tr.r + (int) bl.r + 2 * (int) bc.r + (int) br.r) +
                              (-(int) tl.r - 2 * (int) cl.r - (int) bl.r + (int) tr.r + 2 * (int) cr.r + (int) br.r) *
                              (-(int) tl.r - 2 * (int) cl.r - (int) bl.r + (int) tr.r + 2 * (int) cr.r + (int) br.r));
                clamp((int) r, 0, 255);
                auto b = sqrt(
                        (-(int) tl.b - 2 * (int) tc.b - (int) tr.b + (int) bl.b + 2 * (int) bc.b + (int) br.b) *
                        (-(int) tl.b - 2 * (int) tc.b - (int) tr.b + (int) bl.b + 2 * (int) bc.b + (int) br.b) +
                        (-(int) tl.b - 2 * (int) cl.b - (int) bl.b + (int) tr.b + 2 * (int) cr.b + (int) br.b) *
                        (-(int) tl.b - 2 * (int) cl.b - (int) bl.b + (int) tr.b + 2 * (int) cr.b + (int) br.b));
                clamp((int) b, 0, 255);
                auto g = (sqrt(
                        (-(int) tl.g - 2 * (int) tc.g - (int) tr.g + (int) bl.g + 2 * (int) bc.g + (int) br.g) *
                        (-(int) tl.g - 2 * (int) tc.g - (int) tr.g + (int) bl.g + 2 * (int) bc.g + (int) br.g) +
                        (-(int) tl.g - 2 * (int) cl.g - (int) bl.g + (int) tr.g + 2 * (int) cr.g + (int) br.g) *
                        (-(int) tl.g - 2 * (int) cl.g - (int) bl.g + (int) tr.g + 2 * (int) cr.g +
                         (int) br.g)));
                clamp((int) g, 0, 255);
                result.GetPixel(i, j).r = std::byte(r);
                result.GetPixel(i, j).b = std::byte(b);
                result.GetPixel(i, j).g = std::byte(g);
            }
        }
        return result;
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
