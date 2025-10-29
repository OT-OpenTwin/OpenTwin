// @otlicense

#pragma once
#include <complex>
#include <istream>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace ltspice {

    // Represents a single LTspice RAW variable (index, name, type).
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

    // Container for parsed LTspice RAW data (header + variables + samples).
    class RawData {
    public:
        using cpx = std::complex<double>;

        RawData();

        // Header key/value access (e.g., Title, Date, Plotname, Flags, No. Points, ...).
        void setHeader(const std::map<std::string, std::string>& hdr);
        const std::map<std::string, std::string>& header() const;

        // Flags (e.g., "real", "complex", optionally "float", "forward").
        void setFlags(const std::string& flags);
        const std::string& flags() const;

        // Counts & variable list.
        void setCounts(std::size_t nVars, std::size_t nPts);
        void setVariables(std::vector<RawVariable> vars);
        const std::vector<RawVariable>& variables() const;

        // Dataset properties.
        bool isComplex() const;
        std::size_t points() const;
        std::size_t variablesCount() const;

        // Store/access values by (point, variable index).
        void setReal(std::size_t p, std::size_t v, double x);
        void setComplex(std::size_t p, std::size_t v, cpx z);
        double real(std::size_t p, std::size_t v) const;
        cpx    complex(std::size_t p, std::size_t v) const;

        // Convenience by variable name.
        std::optional<std::size_t> variableIndexByName(const std::string& name) const;
        std::vector<std::string>   variableNames() const;
        double real(std::size_t p, const std::string& varName) const;
        cpx    complex(std::size_t p, const std::string& varName) const;

        // Convenience vectors (first variable is X-axis for real datasets).
        std::vector<double> xValues() const;                       // throws on complex datasets
        std::vector<double> yValues(const std::string& var) const; // throws on complex datasets

        // Optional CSV export.
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

    // High-level reader: detects ASCII vs. Binary and parses accordingly.
    // Include only this header in your code and link against the .cpp.
    class AutoRawReader {
    public:
        // Read from any std::istream positioned at the beginning of the RAW file.
        // Expects an 8-bit (ASCII/UTF-8) header; for UTF-16 input prefer readFromBytes().
        RawData read(std::istream& in);

        // Read from an in-memory byte blob. Detects UTF-16 (LE/BE) with or without BOM,
        // decodes the header to UTF-8, finds the payload offset, then parses payload
        // (binary: supports mixed precision; ASCII Values decoded to UTF-8).
        RawData readFromBytes(const std::string& blob);
    };

} // namespace ltspice
