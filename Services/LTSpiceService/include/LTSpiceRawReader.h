#pragma once
#include <complex>
#include <istream>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace ltspice {

    // Represents a single variable (index, name, type).
    class RawVariable {
    public:
        RawVariable(int index, std::string name, std::string type);
        int index() const;
        const std::string& name() const;
        const std::string& type() const;

    private:
        int index_;
        std::string name_;
        std::string type_;
    };

    // Container for parsed RAW data (header + variables + samples).
    class RawData {
    public:
        using cpx = std::complex<double>;

        RawData();

        void setHeader(const std::map<std::string, std::string>& hdr);
        const std::map<std::string, std::string>& header() const;

        void setFlags(const std::string& flags);
        const std::string& flags() const;

        void setCounts(std::size_t nVars, std::size_t nPts);
        void setVariables(std::vector<RawVariable> vars);
        const std::vector<RawVariable>& variables() const;

        bool isComplex() const;
        std::size_t points() const;
        std::size_t variablesCount() const;

        void setReal(std::size_t p, std::size_t v, double x);
        void setComplex(std::size_t p, std::size_t v, cpx z);

        double real(std::size_t p, std::size_t v) const;
        cpx complex(std::size_t p, std::size_t v) const;

        // Optional: export current dataset to CSV (one row per point).
        void exportCSV(const std::string& path) const;

    private:
        std::map<std::string, std::string> header_;
        std::string flags_;
        std::size_t nVars_{ 0 };
        std::size_t nPts_{ 0 };
        std::vector<RawVariable> variables_;
        std::vector<std::vector<double>> dataReal_;
        std::vector<std::vector<cpx>>    dataCplx_;
    };

    // High-level entry point. Detects ASCII vs. Binary and parses accordingly.
    // You only need to include this header in your code to use AutoRawReader.
    class AutoRawReader {
    public:
        // Read from any std::istream (e.g., std::stringstream with the entire file content).
        // The stream should be positioned at the beginning of the RAW file.
        RawData read(std::istream& in);
    };

} // namespace ltspice
