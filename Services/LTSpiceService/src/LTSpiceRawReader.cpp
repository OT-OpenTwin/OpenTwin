#include "LTSpiceRawReader.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace ltspice {

    // ---------- RawVariable ----------
    RawVariable::RawVariable(int index, std::string name, std::string type)
        : index_(index), name_(std::move(name)), type_(std::move(type)) {
    }

    int RawVariable::index() const { return index_; }
    const std::string& RawVariable::name() const { return name_; }
    const std::string& RawVariable::type() const { return type_; }

    // ---------- helpers (internal) ----------
    namespace {
        inline std::string trim(const std::string& s) {
            std::size_t a = 0, b = s.size();
            while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
            while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
            return s.substr(a, b - a);
        }

        inline void parseHeaderLine(const std::string& line,
            std::map<std::string, std::string>& header) {
            auto pos = line.find(':');
            if (pos == std::string::npos) return;
            std::string key = trim(line.substr(0, pos));
            std::string val = trim(line.substr(pos + 1));
            header[key] = val;
        }

        inline std::vector<std::string> splitWS(const std::string& s) {
            std::istringstream iss(s);
            std::vector<std::string> out;
            std::string tok;
            while (iss >> tok) out.push_back(tok);
            return out;
        }

        inline std::string valueOr(const std::map<std::string, std::string>& m, const std::string& k) {
            auto it = m.find(k);
            return it == m.end() ? std::string() : it->second;
        }

        inline std::size_t toSize(const std::string& s) {
            if (s.empty()) return 0;
            return static_cast<std::size_t>(std::stoll(s));
        }

        // Read a native little-endian double from stream.
        // (LTspice writes IEEE754 doubles in little-endian order.)
        inline void readDouble(std::istream& in, double& x) {
            in.read(reinterpret_cast<char*>(&x), sizeof(double));
            if (!in) throw std::runtime_error("Unexpected end of file while reading binary doubles");
        }

        // Header parser result shared by ASCII/Binary payload readers.
        struct HeaderParseResult {
            std::map<std::string, std::string> header;
            std::vector<RawVariable> variables;
            std::size_t nVars{ 0 };
            std::size_t nPts{ 0 };
            bool isBinary{ false };
            std::string flags;
            std::string marker; // "Binary:" or "Values:"
        };

        // Parse header & variable list; stop right AFTER the marker line.
        // Leaves stream positioned at the start of the payload.
        HeaderParseResult readHeaderAndVariables(std::istream& in) {
            HeaderParseResult r;
            std::string line;
            bool inVars = false;

            while (std::getline(in, line)) {
                std::string t = trim(line);
                if (t.empty()) continue;

                if (!inVars && t == "Variables:") { inVars = true; continue; }
                if (t == "Binary:" || t == "Values:") {
                    r.marker = t;
                    r.isBinary = (t == "Binary:");
                    break; // position is right after the marker line
                }

                if (inVars) {
                    // Expected: "<idx>  <name>  <type>"
                    auto parts = splitWS(t);
                    if (parts.size() >= 3) {
                        int idx = std::stoi(parts[0]);
                        r.variables.emplace_back(idx, parts[1], parts[2]);
                    }
                    continue;
                }

                // Generic header key:value
                parseHeaderLine(t, r.header);
            }

            r.flags = valueOr(r.header, "Flags");
            r.nVars = toSize(valueOr(r.header, "No. Variables"));
            r.nPts = toSize(valueOr(r.header, "No. Points"));
            if (r.nVars == 0 && !r.variables.empty()) r.nVars = r.variables.size();
            if (r.marker.empty()) throw std::runtime_error("RAW header missing 'Binary:' or 'Values:' marker");
            return r;
        }

        // Parse ASCII payload given the header result; stream at payload start.
        void parseAsciiPayload(std::istream& in, const HeaderParseResult& hdr, RawData& data) {
            const bool isComplex = data.isComplex();
            const std::size_t nVars = data.variablesCount();
            const std::size_t nPts = data.points();

            std::string line;
            for (std::size_t p = 0; p < nPts; ++p) {
                // Index line: "<idx>  <value for var0>"
                if (!std::getline(in, line)) throw std::runtime_error("Unexpected EOF in ASCII data (index line)");
                line = trim(line);
                while (line.empty()) {
                    if (!std::getline(in, line)) throw std::runtime_error("Unexpected EOF in ASCII data (blank lines)");
                    line = trim(line);
                }

                // split once: first token = index (ignored), rest = value
                std::string rest;
                {
                    std::istringstream iss(line);
                    int idxParsed = 0;
                    if (!(iss >> idxParsed)) throw std::runtime_error("Failed to parse point index in ASCII data");
                    std::getline(iss, rest);
                    rest = trim(rest);
                }

                auto parseReal = [](std::string s)->double {
                    while (!s.empty() && (s.back() == ',' || s.back() == ';')) s.pop_back();
                    auto comma = s.find(',');
                    if (comma != std::string::npos && s.find('(') == std::string::npos) {
                        std::replace(s.begin(), s.end(), ',', '.');
                    }
                    return std::stod(s);
                    };
                auto parseCplx = [](const std::string& s)->std::complex<double> {
                    auto a = s.find('(');
                    auto b = s.find(')', a == std::string::npos ? 0 : a + 1);
                    if (a == std::string::npos || b == std::string::npos || b <= a + 1)
                        throw std::runtime_error("Invalid complex literal: " + s);
                    std::string inside = s.substr(a + 1, b - a - 1);
                    auto comma = inside.find(',');
                    if (comma == std::string::npos) throw std::runtime_error("Invalid complex literal (no comma): " + s);
                    auto re = trim(inside.substr(0, comma));
                    auto im = trim(inside.substr(comma + 1));
                    return { std::stod(re), std::stod(im) };
                    };

                if (isComplex) data.setComplex(p, 0, parseCplx(rest));
                else           data.setReal(p, 0, parseReal(rest));

                // Following lines: one per remaining variable
                for (std::size_t v = 1; v < nVars; ++v) {
                    if (!std::getline(in, line)) throw std::runtime_error("Unexpected EOF in ASCII data (variable lines)");
                    line = trim(line);
                    if (line.empty()) { --v; continue; }

                    std::string token;
                    auto posParen = line.find('(');
                    if (posParen != std::string::npos) {
                        auto posEnd = line.find(')', posParen);
                        if (posEnd == std::string::npos) throw std::runtime_error("Malformed complex value in ASCII data");
                        token = line.substr(posParen, posEnd - posParen + 1);
                    }
                    else {
                        auto parts = splitWS(line);
                        if (parts.empty()) { --v; continue; }
                        token = parts.back();
                    }

                    if (isComplex) data.setComplex(p, v, parseCplx(token));
                    else           data.setReal(p, v, parseReal(token));
                }
            }
        }

        // Parse Binary payload given the header result; stream at payload start.
        void parseBinaryPayload(std::istream& in, const HeaderParseResult& hdr, RawData& data) {
            const bool isComplex = data.isComplex();
            const std::size_t nVars = data.variablesCount();
            const std::size_t nPts = data.points();

            for (std::size_t p = 0; p < nPts; ++p) {
                for (std::size_t v = 0; v < nVars; ++v) {
                    double a = 0.0, b = 0.0;
                    readDouble(in, a);
                    if (isComplex) {
                        readDouble(in, b);
                        data.setComplex(p, v, RawData::cpx(a, b));
                    }
                    else {
                        data.setReal(p, v, a);
                    }
                }
            }
        }

    } // namespace (helpers)

    // ---------- RawData ----------
    RawData::RawData() = default;

    void RawData::setHeader(const std::map<std::string, std::string>& hdr) { header_ = hdr; }
    const std::map<std::string, std::string>& RawData::header() const { return header_; }

    void RawData::setFlags(const std::string& flags) { flags_ = flags; }
    const std::string& RawData::flags() const { return flags_; }

    void RawData::setCounts(std::size_t nVars, std::size_t nPts) {
        nVars_ = nVars; nPts_ = nPts;
        dataReal_.clear(); dataCplx_.clear();
        if (isComplex()) dataCplx_.assign(nPts_, std::vector<cpx>(nVars_));
        else             dataReal_.assign(nPts_, std::vector<double>(nVars_));
    }

    void RawData::setVariables(std::vector<RawVariable> vars) { variables_ = std::move(vars); }
    const std::vector<RawVariable>& RawData::variables() const { return variables_; }

    bool RawData::isComplex() const {
        std::string f = flags_;
        std::transform(f.begin(), f.end(), f.begin(), [](unsigned char c) { return std::tolower(c); });
        return f.find("complex") != std::string::npos;
    }

    std::size_t RawData::points() const { return nPts_; }
    std::size_t RawData::variablesCount() const { return nVars_; }

    void RawData::setReal(std::size_t p, std::size_t v, double x) {
        if (isComplex()) throw std::logic_error("Attempting to store real into complex dataset");
        dataReal_.at(p).at(v) = x;
    }
    void RawData::setComplex(std::size_t p, std::size_t v, cpx z) {
        if (!isComplex()) throw std::logic_error("Attempting to store complex into real dataset");
        dataCplx_.at(p).at(v) = z;
    }

    double RawData::real(std::size_t p, std::size_t v) const { return dataReal_.at(p).at(v); }
    RawData::cpx RawData::complex(std::size_t p, std::size_t v) const { return dataCplx_.at(p).at(v); }

    void RawData::exportCSV(const std::string& path) const {
        std::ofstream out(path, std::ios::binary);
        if (!out) throw std::runtime_error("Failed to open CSV for writing: " + path);

        // header
        if (isComplex()) {
            for (std::size_t i = 0; i < variables_.size(); ++i) {
                if (i) out << ',';
                out << '"' << variables_[i].name() << ".re\","
                    << '"' << variables_[i].name() << ".im\"";
                if (i + 1 < variables_.size()) out << ',';
            }
        }
        else {
            for (std::size_t i = 0; i < variables_.size(); ++i) {
                if (i) out << ',';
                out << '"' << variables_[i].name() << '"';
            }
        }
        out << "\n";

        out.setf(std::ios::scientific);
        out << std::setprecision(16);
        for (std::size_t p = 0; p < points(); ++p) {
            if (isComplex()) {
                for (std::size_t v = 0; v < variables_.size(); ++v) {
                    if (v) out << ',';
                    auto z = dataCplx_[p][v];
                    out << z.real() << ',' << z.imag();
                }
            }
            else {
                for (std::size_t v = 0; v < variables_.size(); ++v) {
                    if (v) out << ',';
                    out << dataReal_[p][v];
                }
            }
            out << "\n";
        }
    }

    // ---------- AutoRawReader ----------
    RawData AutoRawReader::read(std::istream& in) {
        // Read header & variables; stream remains positioned at payload start.
        HeaderParseResult hdr = readHeaderAndVariables(in);

        RawData data;
        data.setHeader(hdr.header);
        data.setFlags(hdr.flags);
        if (hdr.nVars == 0) hdr.nVars = hdr.variables.size();
        data.setCounts(hdr.nVars, hdr.nPts);
        data.setVariables(hdr.variables);

        if (hdr.isBinary) {
            parseBinaryPayload(in, hdr, data);
        }
        else {
            if (hdr.marker != "Values:")
                throw std::runtime_error("Expected 'Values:' section for ASCII RAW");
            parseAsciiPayload(in, hdr, data);
        }
        return data;
    }

} // namespace ltspice
