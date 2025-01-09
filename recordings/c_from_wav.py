import wave
import numpy as np

# Load the WAV file
input_file = "yes_esp.wav"  # Replace with your WAV file name
output_c_file = "audio_data.h"  # Output C header file

# Open the WAV file and read audio properties
with wave.open(input_file, "rb") as wav_file:
    n_channels = wav_file.getnchannels()
    sample_width = wav_file.getsampwidth()
    frame_rate = wav_file.getframerate()
    n_frames = wav_file.getnframes()
    audio_data = wav_file.readframes(n_frames)

    # Convert audio data to int16 format
    audio_samples = np.frombuffer(audio_data, dtype=np.int16)

    # Handle multi-channel audio (convert to mono if necessary)
    if n_channels > 1:
        audio_samples = audio_samples[::n_channels]  # Take one channel

    # Resample to 16,000 samples if needed
    if frame_rate != 16000 or len(audio_samples) != 16000:
        from scipy.signal import resample
        audio_samples = resample(audio_samples, 16000).astype(np.int16)

# Ensure the output is exactly 16,000 samples
audio_samples = audio_samples[:16000]

# Generate the C header file
with open(output_c_file, "w") as header_file:
    header_file.write("#ifndef AUDIO_DATA_H\n")
    header_file.write("#define AUDIO_DATA_H\n\n")
    header_file.write("#define AUDIO_SAMPLE_RATE 16000\n")
    header_file.write("#define AUDIO_NUM_SAMPLES 16000\n\n")
    header_file.write("const int16_t audio_samples[16000] = {\n")

    # Write the audio samples in C array format
    for i, sample in enumerate(audio_samples):
        if i % 10 == 0:  # Format for readability
            header_file.write("\n    ")
        header_file.write(f"{sample}, ")
    header_file.write("\n};\n\n")
    header_file.write("#endif // AUDIO_DATA_H\n")

print(f"C header file '{output_c_file}' generated successfully!")
