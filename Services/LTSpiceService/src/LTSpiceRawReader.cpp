// @otlicense

#include "LTSpiceRawReader.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace ltspice {

    // -------------------- RawVariable --------------------
    RawVariable::RawVariable(int index, std::string name, std::string type)
        : index_(index), name_(std::move(name)), type_(std::move(type)) {
    }

    int RawVariable::index() const { return index_; }
    const std::string& RawVariable::name() const { return name_; }
    const std::string& RawVariable::type() const { return type_; }

    // -------------------- internal helpers --------------------
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

        inline std::complex<double> parseComplexFlexible(std::string s) {
            // Accept "(re,im)" or "re,im"  (any whitespace tolerated)
            auto ltrim = [](std::string& x) {
                size_t i = 0; while (i < x.size() && std::isspace((unsigned char)x[i])) ++i; x.erase(0, i);
                };
            auto rtrim = [](std::string& x) {
                while (!x.empty() && std::isspace((unsigned char)x.back())) x.pop_back();
                };

            ltrim(s); rtrim(s);
            // strip optional parentheses
            if (!s.empty() && s.front() == '(' && s.back() == ')') {
                s = s.substr(1, s.size() - 2);
                ltrim(s); rtrim(s);
            }
            // tolerate trailing separators
            while (!s.empty() && (s.back() == ',' || s.back() == ';')) s.pop_back();

            // split on the first comma (delimiter between re and im)
            auto comma = s.find(',');
            if (comma == std::string::npos) {
                throw std::runtime_error("Invalid complex literal (expected re,im): " + s);
            }
            std::string re = s.substr(0, comma);
            std::string im = s.substr(comma + 1);
            ltrim(re); rtrim(re);
            ltrim(im); rtrim(im);
            // also strip trailing separators from parts
            while (!re.empty() && (re.back() == ',' || re.back() == ';')) re.pop_back();
            while (!im.empty() && (im.back() == ',' || im.back() == ';')) im.pop_back();

            // NOTE: We assume dot-decimal here (as LTspice writes "1.23e+00").
            // If your locale uses decimal commas globally, imbue classic "C" locale or pre-normalize.
            return { std::stod(re), std::stod(im) };
        }

        // Read a native little-endian double. (LTspice writes IEEE754 doubles little-endian.)
        inline void readDouble(std::istream& in, double& x) {
            in.read(reinterpret_cast<char*>(&x), sizeof(double));
            if (!in) throw std::runtime_error("Unexpected end of file while reading binary doubles");
        }

        // Result of header parsing (text part up to "Binary:"/"Values:").
        struct HeaderParseResult {
            std::map<std::string, std::string> header;
            std::vector<RawVariable> variables;
            std::size_t nVars{ 0 };
            std::size_t nPts{ 0 };
            bool isBinary{ false };
            std::string flags;
            std::string marker; // "Binary:" or "Values:"
        };

        // Parse header & variable list; stream stops right after marker line.
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
                    break;
                }

                if (inVars) {
                    // "<idx>  <name>  <type>"
                    auto parts = splitWS(t);
                    if (parts.size() >= 3) {
                        int idx = std::stoi(parts[0]);
                        r.variables.emplace_back(idx, parts[1], parts[2]);
                    }
                    continue;
                }

                parseHeaderLine(t, r.header);
            }

            r.flags = valueOr(r.header, "Flags");
            r.nVars = toSize(valueOr(r.header, "No. Variables"));
            r.nPts = toSize(valueOr(r.header, "No. Points"));
            if (r.nVars == 0 && !r.variables.empty()) r.nVars = r.variables.size();
            if (r.marker.empty()) throw std::runtime_error("RAW header missing 'Binary:' or 'Values:' marker");
            return r;
        }

        // ASCII payload parser.
        void parseAsciiPayload(std::istream& in, const HeaderParseResult& /*hdr*/, class RawData& data) {
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

                std::string rest;
                {
                    std::istringstream iss(line);
                    int idxParsed = 0;
                    if (!(iss >> idxParsed)) throw std::runtime_error("Failed to parse point index in ASCII data");
                    std::getline(iss, rest);
                    rest = trim(rest);
                }

                auto parseReal = [](std::string s)->double {
                    // Accept "1.23e+00", tolerate trailing ',' or ';'
                    while (!s.empty() && (s.back() == ',' || s.back() == ';')) s.pop_back();
                    // If someone used decimal comma in a REAL value line (rare), normalize:
                    if (s.find('(') == std::string::npos && s.find(',') != std::string::npos) {
                        // Only convert comma to dot if there's exactly one comma and no 'e' sign confusion
                        // but safest is: replace all commas by dots when there are no parentheses.
                        std::replace(s.begin(), s.end(), ',', '.');
                    }
                    return std::stod(s);
                };
                // accept "(re,im)" OR "re,im"
                auto parseCplx = [](const std::string& s)->std::complex<double> {
                    return parseComplexFlexible(s);
                };

                if (isComplex) data.setComplex(p, 0, parseCplx(rest));
                else           data.setReal(p, 0, parseReal(rest));

                // Following lines: one per remaining variable
                for (std::size_t v = 1; v < nVars; ++v) {
                    if (!std::getline(in, line)) throw std::runtime_error("Unexpected EOF in ASCII data (variable lines)");
                    line = trim(line);
                    if (line.empty()) { --v; continue; }

                    std::string token = trim(line);
                    // If the whole line is just the value, keep it.
                    // Otherwise, fall back to last whitespace-separated token.
                    if (token.find(',') == std::string::npos && token.find('(') == std::string::npos) {
                        auto parts = splitWS(token);
                        if (parts.empty()) { --v; continue; }
                        token = parts.back();
                    }
                    // now token is either "(re,im)" or "re,im" or a single real

                    if (isComplex) data.setComplex(p, v, parseCplx(token));
                    else           data.setReal(p, v, parseReal(token));
                }
            }
        }

        // ---------- UTF-16 support (BOM/No-BOM detection & header decoding) ----------
        enum class Utf16Bom { None, LE, BE };

        // BOM-based detection.
        Utf16Bom detect_utf16_bom(const std::string& s) {
            if (s.size() >= 2) {
                const unsigned char b0 = static_cast<unsigned char>(s[0]);
                const unsigned char b1 = static_cast<unsigned char>(s[1]);
                if (b0 == 0xFF && b1 == 0xFE) return Utf16Bom::LE;
                if (b0 == 0xFE && b1 == 0xFF) return Utf16Bom::BE;
            }
            return Utf16Bom::None;
        }

        // Heuristic for UTF-16 without BOM: look for alternating NULs and ASCII.
        Utf16Bom detect_utf16_no_bom_heuristic(const std::string& s) {
            const std::size_t N = std::min<std::size_t>(s.size(), 512);
            if (N < 4) return Utf16Bom::None;

            auto is_ascii = [](unsigned char c) {
                return (c == 0x09 || c == 0x0A || c == 0x0D || (c >= 0x20 && c <= 0x7E));
                };

            std::size_t le_hits = 0, be_hits = 0, pairs = 0;
            for (std::size_t i = 0; i + 1 < N; i += 2) {
                unsigned char a = static_cast<unsigned char>(s[i]);
                unsigned char b = static_cast<unsigned char>(s[i + 1]);
                if (is_ascii(a) && b == 0x00) le_hits++;
                if (a == 0x00 && is_ascii(b)) be_hits++;
                pairs++;
            }

            if (pairs >= 8) {
                if (le_hits >= pairs * 0.75) return Utf16Bom::LE;
                if (be_hits >= pairs * 0.75) return Utf16Bom::BE;
            }
            return Utf16Bom::None;
        }

        // BMP-only conversion (good enough for LTspice headers).
        static std::string codeunit_to_utf8(uint16_t w) {
            if (w < 0x80)        return std::string(1, static_cast<char>(w));
            else if (w < 0x800)  return std::string{ char(0xC0 | (w >> 6)), char(0x80 | (w & 0x3F)) };
            else                 return std::string{ char(0xE0 | (w >> 12)),
                                                     char(0x80 | ((w >> 6) & 0x3F)),
                                                     char(0x80 | (w & 0x3F)) };
        }

        // Decode UTF-16 header text to UTF-8, stop after marker, return payload offset.
        struct Utf16HeaderDecodeResult {
            std::string utf8_header;
            std::size_t payload_offset_bytes{ 0 };
            bool isBinary{ false };
        };

        Utf16HeaderDecodeResult decode_utf16_header_until_marker(const std::string& s,
            Utf16Bom bom,
            std::size_t start) {
            Utf16HeaderDecodeResult out;

            auto read_u16 = [&](std::size_t at)->uint16_t {
                if (bom == Utf16Bom::LE) return uint16_t(uint8_t(s[at]) | (uint16_t(uint8_t(s[at + 1])) << 8));
                else                     return uint16_t((uint16_t(uint8_t(s[at])) << 8) | uint8_t(s[at + 1]));
                };
            auto equals_word = [&](std::size_t at, const char* ascii)->bool {
                std::size_t i = 0;
                while (ascii[i] != '\0') {
                    if (at + 1 >= s.size()) return false;
                    uint16_t w = read_u16(at);
                    if (w != static_cast<unsigned char>(ascii[i])) return false;
                    at += 2; i++;
                }
                return true;
                };

            std::size_t pos = start;
            while (pos + 1 < s.size()) {
                if (equals_word(pos, "Binary:") || equals_word(pos, "Values:")) {
                    bool isBin = equals_word(pos, "Binary:");
                    // advance to end-of-line
                    std::size_t p = pos;
                    while (p + 1 < s.size()) {
                        uint16_t w = read_u16(p); p += 2;
                        if (w == u'\n') break;
                        if (w == u'\r') { if (p + 1 < s.size() && read_u16(p) == u'\n') p += 2; break; }
                    }
                    out.payload_offset_bytes = p;
                    out.isBinary = isBin;
                    break;
                }
                // collect line → UTF-8
                while (pos + 1 < s.size()) {
                    uint16_t w = read_u16(pos); pos += 2;
                    if (w == u'\n') { out.utf8_header.push_back('\n'); break; }
                    if (w == u'\r') { if (pos + 1 < s.size() && read_u16(pos) == u'\n') pos += 2; out.utf8_header.push_back('\n'); break; }
                    out.utf8_header += codeunit_to_utf8(w);
                }
            }

            if (out.payload_offset_bytes == 0)
                throw std::runtime_error("UTF-16 RAW: missing 'Binary:' or 'Values:' marker in header");
            return out;
        }

        // --- Binary layout inference & parsing (supports mixed precision) ---

        inline double readFloat32AsDouble(std::istream& in) {
            std::uint32_t u = 0;
            in.read(reinterpret_cast<char*>(&u), sizeof(u));
            if (!in) throw std::runtime_error("Unexpected EOF while reading float32");
            float f;
            std::memcpy(&f, &u, sizeof(f));
            return static_cast<double>(f);
        }

        enum class LayoutKind { Uniform, MixedVar0Double };

        struct BinaryLayout {
            LayoutKind kind{ LayoutKind::Uniform };
            std::size_t bytesPerScalarOthers{ 8 }; // for Uniform: width for all; for Mixed: width for vars 1..N-1
            std::size_t nVarsEffective{ 0 };
        };

        inline std::optional<BinaryLayout> infer_layout_from_payload(std::size_t payloadBytes,
            std::size_t headerVars,
            std::size_t nPts,
            bool isComplex,
            bool flagsHasFloat)
        {
            // Helpers to check candidates
            auto matches_mixed = [&](std::size_t nv, std::size_t bpsOthers)->bool {
                // Mixed = var0 (time) as double, others with bpsOthers (float32 or double)
                // NOTE: Only meaningful for real datasets (time domain). For complex datasets we won't use it.
                const std::size_t scalarsOthers = isComplex ? 2 : 1;
                const std::size_t perPoint = 8 + (nv > 1 ? (nv - 1) * scalarsOthers * bpsOthers : 0);
                return nPts * perPoint == payloadBytes;
                };
            auto matches_uniform = [&](std::size_t nv, std::size_t bps)->bool {
                const std::size_t scalarsPerVar = isComplex ? 2 : 1;
                return nPts * nv * scalarsPerVar * bps == payloadBytes;
                };

            // Candidate variable counts: header, header+1, header-1
            std::vector<std::size_t> nvCandidates;
            nvCandidates.push_back(headerVars);
            nvCandidates.push_back(headerVars + 1);
            if (headerVars > 0) nvCandidates.push_back(headerVars - 1);

            // ---------- Complex datasets: ONLY uniform (freq is float or double like others) ----------
            if (isComplex) {
                // Use flags hint first (float), then double, then the other width with ±1 var
                if (flagsHasFloat) {
                    for (auto nv : nvCandidates) if (matches_uniform(nv, 4)) return BinaryLayout{ LayoutKind::Uniform, 4, nv };
                    for (auto nv : nvCandidates) if (matches_uniform(nv, 8)) return BinaryLayout{ LayoutKind::Uniform, 8, nv };
                }
                else {
                    for (auto nv : nvCandidates) if (matches_uniform(nv, 8)) return BinaryLayout{ LayoutKind::Uniform, 8, nv };
                    for (auto nv : nvCandidates) if (matches_uniform(nv, 4)) return BinaryLayout{ LayoutKind::Uniform, 4, nv };
                }
                return std::nullopt;
            }

            // ---------- Real datasets: prefer Mixed (var0=double, others float/double), then Uniform ----------
            // 1) Mixed var0=double, others=float32
            for (auto nv : nvCandidates) {
                if (matches_mixed(nv, 4)) return BinaryLayout{ LayoutKind::MixedVar0Double, 4, nv };
            }
            // 2) Mixed var0=double, others=double
            for (auto nv : nvCandidates) {
                if (matches_mixed(nv, 8)) return BinaryLayout{ LayoutKind::MixedVar0Double, 8, nv };
            }
            // 3) Uniform: prefer header hint from flags, then the other width
            if (flagsHasFloat) {
                for (auto nv : nvCandidates) if (matches_uniform(nv, 4)) return BinaryLayout{ LayoutKind::Uniform, 4, nv };
                for (auto nv : nvCandidates) if (matches_uniform(nv, 8)) return BinaryLayout{ LayoutKind::Uniform, 8, nv };
            }
            else {
                for (auto nv : nvCandidates) if (matches_uniform(nv, 8)) return BinaryLayout{ LayoutKind::Uniform, 8, nv };
                for (auto nv : nvCandidates) if (matches_uniform(nv, 4)) return BinaryLayout{ LayoutKind::Uniform, 4, nv };
            }

            return std::nullopt;
        }

        void parseBinaryPayloadWithLayout(std::istream& in,
            class RawData& data,
            const BinaryLayout& layout)
        {
            const bool isComplex = data.isComplex();
            const std::size_t nVars = data.variablesCount();
            const std::size_t nPts = data.points();

            auto readOtherScalar = [&](double& out) {
                if (layout.bytesPerScalarOthers == 4) {
                    // float32 -> double
                    std::uint32_t u = 0;
                    in.read(reinterpret_cast<char*>(&u), sizeof(u));
                    if (!in) throw std::runtime_error("Unexpected EOF while reading float32");
                    float f;
                    std::memcpy(&f, &u, sizeof(f));
                    out = static_cast<double>(f);
                }
                else {
                    // double
                    in.read(reinterpret_cast<char*>(&out), sizeof(double));
                    if (!in) throw std::runtime_error("Unexpected EOF while reading double");
                }
                };

            for (std::size_t p = 0; p < nPts; ++p) {

                if (!isComplex) {
                    // -------- Real dataset (transient): var0 (time) is DOUBLE (8B)
                    double t = 0.0;
                    in.read(reinterpret_cast<char*>(&t), sizeof(double));
                    if (!in) throw std::runtime_error("Unexpected EOF while reading time (double)");
                    data.setReal(p, 0, t);

                    // remaining variables: single scalars (float32 or double)
                    for (std::size_t v = 1; v < nVars; ++v) {
                        double val = 0.0;
                        readOtherScalar(val);
                        data.setReal(p, v, val);
                    }
                }
                else {
                    // -------- Complex dataset (AC/Noise): var0 (frequency) is COMPLEX as well
                    double f_re = 0.0, f_im = 0.0;
                    readOtherScalar(f_re);
                    readOtherScalar(f_im);
                    data.setComplex(p, 0, { f_re, f_im });

                    // remaining variables: complex pairs (float32/double each component)
                    for (std::size_t v = 1; v < nVars; ++v) {
                        double re = 0.0, im = 0.0;
                        readOtherScalar(re);
                        readOtherScalar(im);
                        data.setComplex(p, v, { re, im });
                    }
                }
            }
        }

        // Optional safety: detect & repair UTF-16-ized binary tails (each low byte stored with 0x00 high).
        bool looksUtf16izedBinaryTail(const std::string& blob, std::size_t payloadOffset) {
            std::size_t N = std::min<std::size_t>(blob.size() - payloadOffset, 1024);
            if (N < 32) return false;
            std::size_t hits = 0, pairs = 0;
            for (std::size_t i = 0; i + 1 < N; i += 2) {
                unsigned char hi = static_cast<unsigned char>(blob[payloadOffset + i + 1]);
                if (hi == 0x00) ++hits;
                ++pairs;
            }
            return pairs >= 16 && hits >= pairs * 0.8;
        }

        std::string deUtf16izeBinaryTailLE(const std::string& blob, std::size_t payloadOffset) {
            std::string out;
            out.reserve((blob.size() - payloadOffset) / 2);
            for (std::size_t i = payloadOffset; i + 1 < blob.size(); i += 2) {
                out.push_back(blob[i]); // keep low byte
            }
            return out;
        }

    } // anonymous namespace

    // -------------------- RawData --------------------
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

    std::optional<std::size_t> RawData::variableIndexByName(const std::string& name) const {
        for (std::size_t i = 0; i < variables_.size(); ++i)
            if (variables_[i].name() == name) return i;
        return std::nullopt;
    }
    std::vector<std::string> RawData::variableNames() const {
        std::vector<std::string> names; names.reserve(variables_.size());
        for (const auto& v : variables_) names.push_back(v.name());
        return names;
    }
    double RawData::real(std::size_t p, const std::string& varName) const {
        auto idx = variableIndexByName(varName);
        if (!idx) throw std::out_of_range("Variable not found: " + varName);
        if (isComplex()) throw std::logic_error("Dataset is complex; use complex()");
        return real(p, *idx);
    }
    RawData::cpx RawData::complex(std::size_t p, const std::string& varName) const {
        auto idx = variableIndexByName(varName);
        if (!idx) throw std::out_of_range("Variable not found: " + varName);
        if (!isComplex()) throw std::logic_error("Dataset is real; use real()");
        return complex(p, *idx);
    }
    std::vector<double> RawData::xValues() const {
        if (isComplex()) throw std::logic_error("xValues() not available for complex dataset");
        if (variables_.empty()) return {};
        std::vector<double> xs(nPts_);
        for (std::size_t i = 0; i < nPts_; ++i) xs[i] = dataReal_[i][0];
        return xs;
    }
    std::vector<double> RawData::yValues(const std::string& varName) const {
        if (isComplex()) throw std::logic_error("yValues() only valid for real dataset");
        auto idx = variableIndexByName(varName);
        if (!idx) throw std::out_of_range("Variable not found: " + varName);
        std::vector<double> ys(nPts_);
        for (std::size_t i = 0; i < nPts_; ++i) ys[i] = dataReal_[i][*idx];
        return ys;
    }

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

    // -------------------- AutoRawReader --------------------

    // 8-bit text path (ASCII/UTF-8 header). Stream must be at file start.
    // For maximum robustness (UTF-16 + layout inference) prefer readFromBytes().
    RawData AutoRawReader::read(std::istream& in) {
        HeaderParseResult hdr = readHeaderAndVariables(in);

        RawData data;
        data.setHeader(hdr.header);
        data.setFlags(hdr.flags);
        if (hdr.nVars == 0) hdr.nVars = hdr.variables.size();
        data.setCounts(hdr.nVars, hdr.nPts);
        data.setVariables(hdr.variables);

        if (hdr.isBinary) {
            // Default to uniform doubles here (no payload size known in generic istream).
            // If your input might be mixed/float32, use readFromBytes() instead.
            BinaryLayout lay;
            lay.kind = LayoutKind::Uniform;
            lay.bytesPerScalarOthers = 8;
            lay.nVarsEffective = data.variablesCount();
            parseBinaryPayloadWithLayout(in, data, lay);
        }
        else {
            if (hdr.marker != "Values:")
                throw std::runtime_error("Expected 'Values:' section for ASCII RAW");
            parseAsciiPayload(in, hdr, data);
        }
        return data;
    }

    // In-memory path with UTF-16 handling and robust binary layout inference (incl. mixed precision).
    RawData AutoRawReader::readFromBytes(const std::string& blob) {
        // Detect UTF-16 header
        Utf16Bom bom = detect_utf16_bom(blob);
        if (bom == Utf16Bom::None) {
            bom = detect_utf16_no_bom_heuristic(blob);
        }

        if (bom == Utf16Bom::None) {
            // Not UTF-16 → parse as usual via 8-bit stream
            std::istringstream iss(blob);
            iss.seekg(0, std::ios::beg);
            return read(iss);
        }

        // UTF-16 header path
        std::size_t start = 0;
        if (blob.size() >= 2) {
            unsigned char b0 = static_cast<unsigned char>(blob[0]);
            unsigned char b1 = static_cast<unsigned char>(blob[1]);
            if ((bom == Utf16Bom::LE && b0 == 0xFF && b1 == 0xFE) ||
                (bom == Utf16Bom::BE && b0 == 0xFE && b1 == 0xFF)) {
                start = 2; // skip BOM
            }
        }

        // Decode header until marker and capture payload offset.
        Utf16HeaderDecodeResult dec = decode_utf16_header_until_marker(blob, bom, start);

        // Parse the decoded (UTF-8) header using existing logic.
        std::string syntheticHeader = dec.utf8_header + (dec.isBinary ? "Binary:\n" : "Values:\n");
        std::istringstream headerStream(syntheticHeader);
        HeaderParseResult hdr = readHeaderAndVariables(headerStream);

        // Prepare dataset from header
        RawData data;
        data.setHeader(hdr.header);
        data.setFlags(hdr.flags);
        if (hdr.nVars == 0) hdr.nVars = hdr.variables.size();
        data.setCounts(hdr.nVars, hdr.nPts);
        data.setVariables(hdr.variables);

        if (dec.isBinary) {
            // Prepare payload bytes; repair if binary tail looks UTF-16-ized
            std::string payload;
            if (looksUtf16izedBinaryTail(blob, dec.payload_offset_bytes)) {
                payload = deUtf16izeBinaryTailLE(blob, dec.payload_offset_bytes);
            }
            else {
                payload = blob.substr(dec.payload_offset_bytes);
            }

            // Header hint for float (may be absent even for float32 payloads)
            bool flagsHasFloat = false;
            {
                auto it = hdr.header.find("Flags");
                if (it != hdr.header.end()) {
                    std::string f = it->second;
                    std::transform(f.begin(), f.end(), f.begin(), [](unsigned char c) { return std::tolower(c); });
                    flagsHasFloat = (f.find("float") != std::string::npos);
                }
            }

            // Infer layout: supports Mixed (var0=double, others float/double) & Uniform, with ±1 var tolerance
            auto layoutOpt = infer_layout_from_payload(payload.size(),
                data.variablesCount(),
                data.points(),
                data.isComplex(),
                flagsHasFloat);
            if (!layoutOpt) {
                throw std::runtime_error("Cannot reconcile payload size ("
                    + std::to_string(payload.size()) + " bytes) with header counts: "
                    "No. Points=" + std::to_string(data.points()) +
                    " No. Variables=" + std::to_string(data.variablesCount()) +
                    " Flags=" + data.flags());
            }
            BinaryLayout layout = *layoutOpt;

            // If effective #vars differs from header, adjust storage and add placeholder names
            if (layout.nVarsEffective != data.variablesCount()) {
                const std::size_t nPts = data.points();
                const std::size_t nVarsEff = layout.nVarsEffective;

                data.setCounts(nVarsEff, nPts);

                auto vars = data.variables(); // copy existing names
                while (vars.size() < nVarsEff) {
                    int idx = static_cast<int>(vars.size());
                    vars.emplace_back(idx, "__extra" + std::to_string(idx), "unknown");
                }
                data.setVariables(std::move(vars));
            }

            // Sanity: expected bytes vs. available
            {
                const bool isComplex = data.isComplex();
                const std::size_t nv = data.variablesCount();
                std::size_t expected = 0;
                if (layout.kind == LayoutKind::MixedVar0Double) {
                    const std::size_t scalarsOthers = isComplex ? 2 : 1;
                    const std::size_t perPoint = 8 + (nv > 1 ? (nv - 1) * scalarsOthers * layout.bytesPerScalarOthers : 0);
                    expected = data.points() * perPoint;
                }
                else {
                    const std::size_t scalarsPerVar = isComplex ? 2 : 1;
                    expected = data.points() * nv * scalarsPerVar * layout.bytesPerScalarOthers;
                }
                if (payload.size() < expected) {
                    throw std::runtime_error("Binary payload too short: expected " +
                        std::to_string(expected) + " bytes, got " + std::to_string(payload.size()));
                }
            }

            std::istringstream payloadStream(payload);
            parseBinaryPayloadWithLayout(payloadStream, data, layout);
        }
        else {
            // ASCII Values are UTF-16 too → decode remainder to UTF-8 then parse
            std::string utf8_values;
            auto read_u16 = [&](std::size_t at)->uint16_t {
                if (bom == Utf16Bom::LE) return uint16_t(uint8_t(blob[at]) | (uint16_t(uint8_t(blob[at + 1])) << 8));
                else                     return uint16_t((uint16_t(uint8_t(blob[at])) << 8) | uint8_t(blob[at + 1]));
                };
            std::size_t pos = dec.payload_offset_bytes;
            while (pos + 1 < blob.size()) {
                uint16_t w = read_u16(pos); pos += 2;
                if (w == u'\r') { if (pos + 1 < blob.size() && read_u16(pos) == u'\n') pos += 2; utf8_values.push_back('\n'); continue; }
                if (w == u'\n') { utf8_values.push_back('\n'); continue; }
                if (w != 0) utf8_values += codeunit_to_utf8(w);
            }
            std::istringstream asciiStream(utf8_values);
            parseAsciiPayload(asciiStream, hdr, data);
        }

        return data;
    }

} // namespace ltspice
