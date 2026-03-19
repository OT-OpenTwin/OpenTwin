enum class UnitKind
{
    Linear,   // standard multiplicative unit
    Affine,   // linear + offset (degC, degF)
    LogPower, // 10*log10 (dB for power: watts, intensity)
    LogAmplitude  // 20*log10 (dB for amplitude: voltage, S-params)
};
