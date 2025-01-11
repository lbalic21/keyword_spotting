from pydub import AudioSegment
from pydub.silence import detect_nonsilent
import matplotlib.pyplot as plt
import numpy as np
from scipy.io.wavfile import read

from pydub import AudioSegment
from pydub.silence import detect_nonsilent

# Load the audio file
audio = AudioSegment.from_wav("yes/rec.wav")

# Parameters for silence detection
min_silence_len = 300  # Minimum silence length in milliseconds
silence_thresh = audio.dBFS - 14  # Adjust threshold based on file's dBFS

# Detect non-silent ranges (where "yes" might be spoken)
nonsilent_ranges = detect_nonsilent(audio, min_silence_len, silence_thresh)

print("Detected non-silent ranges:", nonsilent_ranges)

# Desired length in milliseconds (1 second)
desired_length = 1000

if nonsilent_ranges:
    start, end = nonsilent_ranges[0]  # Use the first detected segment

    # Center the range and expand it to 1 second
    midpoint = (start + end) // 2
    new_start = max(0, midpoint - desired_length // 2)
    new_end = min(len(audio), midpoint + desired_length // 2)

    # Trim the audio to the new range
    trimmed_audio = audio[new_start:new_end]

    # Resample to 16 kHz
    trimmed_audio = trimmed_audio.set_frame_rate(16000)

    # Save the trimmed audio
    trimmed_audio.export("output.wav", format="wav")
    print("Trimmed audio saved as 'output.wav'")
else:
    print("No non-silent segments detected.")



rate, data = read("output.wav")
time = np.linspace(0., len(data) / rate, len(data))

plt.plot(time, data)
plt.title("Audio Waveform")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
plt.show()
