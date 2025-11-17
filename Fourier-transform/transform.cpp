#include "transform.hpp"

void Transform::performDFT(const List& x)
{
    const size_t N = x.size();
    spectrum_ = List(N);

    const double twoPiOverN = 2.0 * PI / N;

    for (size_t k = 0; k < N; k++) {
        std::complex<double> sum(0.0, 0.0);

        for (size_t n = 0; n < N; n++) {
            double angle = -twoPiOverN * double(n * k);
            std::complex<double> w(std::cos(angle), std::sin(angle));
            sum += x[n] * w;
        }

        spectrum_[k] = sum;
    }
}

List Transform::performIDFT()
{
    const size_t N = spectrum_.size();
    std::vector<std::complex<double>> x(N);

    const double twoPiOverN = 2.0 * PI / N;

    for (size_t n = 0; n < N; n++) {
        std::complex<double> sum(0.0, 0.0);

        for (size_t k = 0; k < N; k++) {
            double angle = twoPiOverN * double(n * k);
            std::complex<double> w(std::cos(angle), std::sin(angle));
            sum += spectrum_[k] * w;
        }

        x[n] = sum / double(N);
    }

    return x;
}
