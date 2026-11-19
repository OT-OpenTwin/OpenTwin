import json
from pathlib import Path

import numpy as np
import matplotlib.pyplot as plt

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
SCRIPT_DIR = Path(__file__).resolve().parent
JSON_PATH = SCRIPT_DIR / "s11_vectors.json"

FREQ_KEY = "freq_Hz"
REAL_KEY = "s11_real"
IMAG_KEY = "s11_imag"

PHASE_UNIT = "deg"       # "deg" or "rad"
MAG_SCALE = "linear"     # "linear" or "dB"
FREQ_UNIT = "GHz"        # "Hz", "kHz", "MHz", or "GHz"


def scale_frequency(freq_hz, unit):
    factors = {
        "Hz": 1.0,
        "kHz": 1e3,
        "MHz": 1e6,
        "GHz": 1e9,
    }
    if unit not in factors:
        raise ValueError(f"Unsupported frequency unit: {unit}")
    return freq_hz / factors[unit]


# ---------------------------------------------------------------------------
# Load JSON
# ---------------------------------------------------------------------------
if not JSON_PATH.exists():
    raise FileNotFoundError(f"JSON file not found: {JSON_PATH}")

with JSON_PATH.open("r", encoding="utf-8") as f:
    data = json.load(f)

required_keys = [FREQ_KEY, REAL_KEY, IMAG_KEY]
missing = [key for key in required_keys if key not in data]
if missing:
    raise KeyError(
        f"Missing keys in JSON: {missing}. "
        f"Available keys: {list(data.keys())}"
    )

freq_hz = np.asarray(data[FREQ_KEY], dtype=float)
real_part = np.asarray(data[REAL_KEY], dtype=float)
imag_part = np.asarray(data[IMAG_KEY], dtype=float)

if not (len(freq_hz) == len(real_part) == len(imag_part)):
    raise ValueError(
        "Array lengths do not match:\n"
        f"freqHz: {len(freq_hz)}\n"
        f"s11real: {len(real_part)}\n"
        f"s11imag: {len(imag_part)}"
    )

z = real_part + 1j * imag_part
freq = scale_frequency(freq_hz, FREQ_UNIT)

mag = np.abs(z)
phase = np.angle(z, deg=(PHASE_UNIT == "deg"))

if MAG_SCALE == "dB":
    tiny = np.finfo(float).tiny
    mag_plot = 20 * np.log10(np.maximum(mag, tiny))
    mag_ylabel = "Magnitude (dB)"
else:
    mag_plot = mag
    mag_ylabel = "Magnitude"

freq_xlabel = f"Frequency ({FREQ_UNIT})"

# ---------------------------------------------------------------------------
# Cartesian plots
# ---------------------------------------------------------------------------
plt.figure(figsize=(10, 5))
plt.plot(freq, mag_plot, color="tab:blue", lw=1.5)
plt.xlabel(freq_xlabel)
plt.ylabel(mag_ylabel)
plt.title("S11 Magnitude vs. Frequency")
plt.grid(True, which="both", alpha=0.35)
plt.tight_layout()
plt.show()

plt.figure(figsize=(10, 5))
plt.plot(freq, phase, color="tab:orange", lw=1.5)
plt.xlabel(freq_xlabel)
plt.ylabel(f"Phase ({PHASE_UNIT})")
plt.title("S11 Phase vs. Frequency")
plt.grid(True, which="both", alpha=0.35)
plt.tight_layout()
plt.show()

plt.figure(figsize=(10, 5))
plt.plot(freq, real_part, color="tab:green", lw=1.5)
plt.xlabel(freq_xlabel)
plt.ylabel("Re(S11)")
plt.title("S11 Real Part vs. Frequency")
plt.grid(True, which="both", alpha=0.35)
plt.tight_layout()
plt.show()

plt.figure(figsize=(10, 5))
plt.plot(freq, imag_part, color="tab:red", lw=1.5)
plt.xlabel(freq_xlabel)
plt.ylabel("Im(S11)")
plt.title("S11 Imaginary Part vs. Frequency")
plt.grid(True, which="both", alpha=0.35)
plt.tight_layout()
plt.show()

# ---------------------------------------------------------------------------
# Polar plot: magnitude vs. phase
# ---------------------------------------------------------------------------
phase_rad = np.deg2rad(phase) if PHASE_UNIT == "deg" else phase

fig = plt.figure(figsize=(7.5, 7.5))
ax = fig.add_subplot(111, projection="polar")
ax.plot(phase_rad, mag, color="tab:purple", lw=1.5)
ax.set_title("S11 Magnitude vs. Phase", va="bottom")
ax.grid(True, alpha=0.4)
plt.tight_layout()
plt.show()

# ---------------------------------------------------------------------------
# Polar plot: magnitude and phase vs. frequency
# Radius = magnitude
# Angle = frequency, normalized to 0...360 degrees
# Color = phase
# ---------------------------------------------------------------------------
freq_min = freq.min()
freq_max = freq.max()

if freq_max > freq_min:
    freq_norm = (freq - freq_min) / (freq_max - freq_min)
else:
    freq_norm = np.zeros_like(freq)

freq_angle = 2 * np.pi * freq_norm

fig = plt.figure(figsize=(8.5, 8.5))
ax = fig.add_subplot(111, projection="polar")

sc = ax.scatter(
    freq_angle,
    mag,
    c=phase,
    cmap="viridis",
    s=18,
    alpha=0.9,
    linewidths=0,
)

cbar = plt.colorbar(sc, ax=ax, pad=0.10)
cbar.set_label(f"Phase ({PHASE_UNIT})")

ax.set_title(
    "S11 Magnitude and Phase vs. Frequency\n"
    "(radius = magnitude, angle = frequency, color = phase)",
    va="bottom",
)
ax.grid(True, alpha=0.4)
plt.tight_layout()
plt.show()

print(f"Loaded {len(z)} S11 samples.")
print(f"Frequency range: {freq.min():.6g} to {freq.max():.6g} {FREQ_UNIT}")