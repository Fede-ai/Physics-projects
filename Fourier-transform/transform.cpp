#include "transform.hpp"
#include <algorithm>

std::vector<Point> Transform::smoothenPoints(const std::vector<Point>& raw) const
{
	std::vector<Point> x = raw;
	x.push_back(x[0]);

    std::vector<float> arc(x.size());
    arc[0] = 0;

    for (int i = 1; i < x.size(); i++)
        arc[i] = arc[i - 1] + std::abs(x[i] - x[i - 1]);
    float total_length = arc[x.size() - 1];

    int N = x.size() * 2;
    std::vector<Point> uniform(N);

    int j = 0;
    for (int k = 0; k < N; k++) {

        float target = total_length * (float(k) / float(N));

        while (j < x.size() - 2 && arc[j + 1] < target)
            j++;

        float t = (target - arc[j]) / (arc[j + 1] - arc[j]);
        uniform[k] = x[j] * double(1 - t) + x[j + 1] * double(t);
    }

    return uniform;
}

void Transform::performDFT(const std::vector<Point>& x)
{
    const size_t N = x.size();
    spectrum_ = std::vector<Point>(N);

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

void Transform::orderSpectrum()
{
    orderedSpectrum_.clear();
    const size_t N = spectrum_.size();
    for (size_t k = 0; k < N; k++) {
        Freq freq;
        freq.m = (k <= N / 2) ? k : (k - N);
        freq.magnitude = std::abs(spectrum_[k]) / N;
        freq.phase = std::arg(spectrum_[k]);
        orderedSpectrum_.push_back(freq);
    }

    std::sort(orderedSpectrum_.begin() + 1, orderedSpectrum_.end(),
              [](const Freq& a, const Freq& b) {
                  return a.magnitude > b.magnitude;
		});
}

std::vector<Point> Transform::performIDFT()
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
