#pragma once

#include "icy/time.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


namespace icy::bench {


struct Metric
{
    std::string key;
    std::string value;
};


struct Measurement
{
    double nsPerOp = 0.0;
    double bestNsPerOp = 0.0;
    uint64_t iterations = 0;
    uint32_t samples = 0;
};


struct Result
{
    std::string name;
    Measurement measurement;
    std::vector<Metric> metrics;
};


struct Options
{
    bool json = false;
    uint32_t samples = 3;
    uint32_t warmupSamples = 1;
};


inline Options parseOptions(int argc, char** argv)
{
    Options opts;

    for (int i = 1; i < argc; ++i) {
        const std::string_view arg(argv[i]);
        if (arg == "--json") {
            opts.json = true;
            continue;
        }

        constexpr std::string_view samplesPrefix = "--samples=";
        if (arg.rfind(samplesPrefix, 0) == 0) {
            opts.samples = static_cast<uint32_t>(
                std::max(1, std::atoi(argv[i] + static_cast<int>(samplesPrefix.size()))));
            continue;
        }

        constexpr std::string_view warmupPrefix = "--warmups=";
        if (arg.rfind(warmupPrefix, 0) == 0) {
            opts.warmupSamples = static_cast<uint32_t>(
                std::max(0, std::atoi(argv[i] + static_cast<int>(warmupPrefix.size()))));
        }
    }

    return opts;
}


template <typename Sample>
inline Measurement measure(const Options& opts, uint64_t iterations, Sample&& sample)
{
    for (uint32_t warmup = 0; warmup < opts.warmupSamples; ++warmup)
        sample();

    std::vector<double> samples;
    samples.reserve(opts.samples);
    double best = 0.0;

    for (uint32_t index = 0; index < opts.samples; ++index) {
        const uint64_t start = time::hrtime();
        sample();
        const uint64_t end = time::hrtime();

        const double nsPerOp = (end - start) * 1.0 / static_cast<double>(iterations);
        samples.push_back(nsPerOp);
        if (index == 0 || nsPerOp < best)
            best = nsPerOp;
    }

    std::sort(samples.begin(), samples.end());
    const double median = samples[samples.size() / 2];

    return {
        .nsPerOp = median,
        .bestNsPerOp = best,
        .iterations = iterations,
        .samples = opts.samples,
    };
}


class Reporter
{
public:
    explicit Reporter(int argc, char** argv)
        : _opts(parseOptions(argc, argv))
    {
    }

    [[nodiscard]] const Options& options() const { return _opts; }

    void add(Result result)
    {
        _results.push_back(std::move(result));
    }

    int finish() const
    {
        if (_opts.json)
            printJson();
        else
            printHuman();
        return 0;
    }

private:
    void printHuman() const
    {
        for (const auto& result : _results) {
            std::cout << result.name << ": "
                      << result.measurement.nsPerOp << "ns per op"
                      << " (best=" << result.measurement.bestNsPerOp
                      << ", samples=" << result.measurement.samples;
            for (const auto& metric : result.metrics)
                std::cout << ", " << metric.key << '=' << metric.value;
            std::cout << ")\n";
        }
    }

    static void printJsonEscaped(std::string_view value)
    {
        for (const char ch : value) {
            switch (ch) {
            case '\\':
                std::cout << "\\\\";
                break;
            case '"':
                std::cout << "\\\"";
                break;
            case '\n':
                std::cout << "\\n";
                break;
            case '\r':
                std::cout << "\\r";
                break;
            case '\t':
                std::cout << "\\t";
                break;
            default:
                std::cout << ch;
                break;
            }
        }
    }

    void printJson() const
    {
        std::cout << "[\n";
        for (size_t i = 0; i < _results.size(); ++i) {
            const auto& result = _results[i];
            std::cout << "  {\n";
            std::cout << "    \"name\": \"";
            printJsonEscaped(result.name);
            std::cout << "\",\n";
            std::cout << "    \"ns_per_op\": " << result.measurement.nsPerOp << ",\n";
            std::cout << "    \"best_ns_per_op\": " << result.measurement.bestNsPerOp << ",\n";
            std::cout << "    \"iterations\": " << result.measurement.iterations << ",\n";
            std::cout << "    \"samples\": " << result.measurement.samples << ",\n";
            std::cout << "    \"metrics\": {";
            for (size_t metricIndex = 0; metricIndex < result.metrics.size(); ++metricIndex) {
                const auto& metric = result.metrics[metricIndex];
                if (metricIndex == 0)
                    std::cout << '\n';
                else
                    std::cout << ",\n";
                std::cout << "      \"";
                printJsonEscaped(metric.key);
                std::cout << "\": \"";
                printJsonEscaped(metric.value);
                std::cout << '"';
            }
            if (!result.metrics.empty())
                std::cout << '\n' << "    ";
            std::cout << "}\n";
            std::cout << "  }";
            if (i + 1 != _results.size())
                std::cout << ',';
            std::cout << '\n';
        }
        std::cout << "]\n";
    }

    Options _opts;
    std::vector<Result> _results;
};


inline Metric metric(std::string key, std::string value)
{
    return Metric{std::move(key), std::move(value)};
}


inline Metric metric(std::string key, uint64_t value)
{
    return metric(std::move(key), std::to_string(value));
}


} // namespace icy::bench
