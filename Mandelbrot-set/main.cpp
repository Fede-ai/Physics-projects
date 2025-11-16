#include <iostream>
#include <cmath>
#include "SFML/Graphics.hpp"
#include <omp.h>
#pragma warning(disable: 6993)

static constexpr size_t iter = 5'000;
static constexpr unsigned int xSize = 4'000;
static constexpr double colorScale = 1 - 0.7;

inline static bool inBulbs(double x, double y) {
    //period-2 bulb: center (-1, 0), radius 1/4
    if ((x + 1) * (x + 1) + y * y < 0.0625 - 0.0025)
        return true;

    //main cardioid
    double xr = (x - 0.25 + 0.005) * 1.01;
    double q = xr * xr + y * y;
    if (q * (q + xr) < 0.25 * y * y * 0.98) 
        return true;

    return false;
}

inline static size_t diverges(double cr, double ci) {
    if (cr * cr + ci * ci < 0.005 * 0.005)
        return size_t(-1);

    if (inBulbs(cr, ci))
        return iter;
        //return size_t(-1);

    double zr = 0.0, zi = 0.0;
    double zr2 = 0.0, zi2 = 0.0;

    for (size_t a = 0; a < iter; ++a) {
        zi = 2.0 * zr * zi + ci;
        zr = zr2 - zi2 + cr;

        zr2 = zr * zr;
        zi2 = zi * zi;

        if (zr2 + zi2 > 4.0)
            return a;
    }

    return iter;
}

int main() {
    double ratio = std::sqrt(7.0) / 3.0;
    double xView = 3.0;

    sf::Vector2u imgSize(xSize, unsigned(xSize * ratio));
    sf::Image img({ imgSize.x, imgSize.y }, sf::Color::Black);

    //precompute scaling
    double scale = xView / imgSize.x;
    int midX = imgSize.x / 2;
    int midY = imgSize.y / 2;
	int columsCompleted = 0, lastUpdate = 0;

    #pragma omp parallel for schedule(dynamic)
    for (int x = 0; x < int(imgSize.x); x++) {
        double cr = (x - midX) * scale - 0.5;

        for (unsigned y = 0; y <= imgSize.y / 2; y++) {
            double ci = (int(y) - midY) * scale;
            size_t n = diverges(cr, ci);

            sf::Color c;
            if (n == size_t(-1))
                c = sf::Color::Red;
            else if (n < iter) {
                double s = std::pow(double(n) / iter, colorScale);
                uint8_t col = uint8_t(255 * s);
                c = sf::Color(0, col, col);
            }
            else
                c = sf::Color::White;

            img.setPixel({ unsigned(x), y }, c);
            img.setPixel({ unsigned(x), imgSize.y - y - 1 }, c);
        }
        columsCompleted++;

        #pragma omp critical
        if (int(50.0 * columsCompleted / imgSize.x) > int(50.0 * lastUpdate / imgSize.x)) {
            std::cout << 100.0 * columsCompleted / imgSize.x << "%\n";
            lastUpdate = columsCompleted;
        }
    }

    std::cout << "100.0%\n";
    auto _ = img.saveToFile("image.png");
    return 0;
}
