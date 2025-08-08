#@ Port : {"type": "in", "name": "Time", "label" : "Signal Time"}
#@ Port : {"type": "in", "name": "Voltage", "label" : "Signal Voltage"}
#@ Port : {"type": "out", "name": "Frequency", "label" : "FFT-Frequency"}
#@ Port : {"type": "out", "name": "Amplitude", "label" : "FFT-Amplitude"}

import OpenTwin

import numpy as np
import pandas as pd

from scipy.fft import fft, fftfreq
from scipy.interpolate import interp1d

def __main__(this):

    # Import the time and signal strength columns
    time = OpenTwin.GetPortData("Time")  # First column (time in microseconds)
    signal_strength = OpenTwin.GetPortData("Voltage")  # Second column (signal strength)
    
    # Create a uniform time grid with the desired spacing
    uniform_time = np.linspace(time[0], time[len(time)-1], len(time))
    
    # Interpolate signal_strength to match uniform_time
    interpolator = interp1d(time, signal_strength, kind='linear')
    uniform_signal_strength = interpolator(uniform_time)
    
    # Perform FFT on the signal
    signal_fft = fft(uniform_signal_strength)
    
    # Compute the corresponding frequency values
    sampling_rate = 1 / ((uniform_time[1] - uniform_time[0])*1e-6)  # Convert microseconds to Hz
    frequencies = fftfreq(len(uniform_signal_strength), d=1/sampling_rate)
    
    # Only keep the positive frequencies (FFT is symmetric)
    positive_freqs = frequencies[:len(frequencies)//2]

    positive_fft = signal_fft[:len(frequencies)//2]
    
    # Export the FFT-frequency and amplitude columns
    OpenTwin.SetPortData("Frequency",positive_freqs)
    OpenTwin.SetPortData("Amplitude",np.abs(positive_fft))
