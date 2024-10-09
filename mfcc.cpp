#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#define FRAME_LENGTH 1024
#define FRAME_STEP 256
#define SAMPLE_RATE 44100

// WAV file header structure
#pragma pack(push, 1)
typedef struct {
    char riff[4];                // RIFF Header
    uint32_t chunkSize;          // File size minus 8 bytes
    char wave[4];                // WAVE Header
    char fmt[4];                 // FMT header
    uint32_t subchunk1Size;      // Size of the fmt chunk
    uint16_t audioFormat;        // Audio format (PCM = 1)
    uint16_t numChannels;        // Number of channels
    uint32_t sampleRate;         // Sample rate
    uint32_t byteRate;           // Byte rate
    uint16_t blockAlign;         // Block align
    uint16_t bitsPerSample;      // Bits per sample
    char data[4];                // DATA header
    uint32_t subchunk2Size;      // Size of the data chunk
} WAVHeader;
#pragma pack(pop)

// Function to normalize the audio signal
void normalizeSignal(double* signal, size_t length) {
    double maxVal = 0;
    for (size_t i = 0; i < length; i++) {
        if (fabs(signal[i]) > maxVal) {
            maxVal = fabs(signal[i]);
        }
    }
    if (maxVal > 0) {
        for (size_t i = 0; i < length; i++) {
            signal[i] /= maxVal; // Normalize to range [-1, 1]
        }
    }
}

// Function to apply Hamming window
void applyHammingWindow(double* frame, size_t length) {
    for (size_t n = 0; n < length; n++) {
        frame[n] *= 0.54 - 0.46 * cos((2 * M_PI * n) / (length - 1)); // Hamming window formula
    }
}

void fft(const double* x, double* out_real, double* out_imag, size_t N) {
    if (N <= 1) {
        out_real[0] = x[0];
        out_imag[0] = 0.0;
        return;
    }

    // Split the array into even and odd parts
    double* even_real = (double*)malloc(N / 2 * sizeof(double));
    double* even_imag = (double*)malloc(N / 2 * sizeof(double));
    double* odd_real = (double*)malloc(N / 2 * sizeof(double));
    double* odd_imag = (double*)malloc(N / 2 * sizeof(double));

    for (size_t i = 0; i < N / 2; i++) {
        even_real[i] = x[i * 2];
        even_imag[i] = 0.0; // Imaginary part for even indices
        odd_real[i] = x[i * 2 + 1];
        odd_imag[i] = 0.0;  // Imaginary part for odd indices
    }

    // Recursive FFT
    fft(even_real, out_real, out_imag, N / 2); // FFT for even indexed elements
    fft(odd_real, out_real + N / 2, out_imag + N / 2, N / 2); // FFT for odd indexed elements

    // Combine the results
    for (size_t k = 0; k < N / 2; k++) {
        double t_real = cos(2 * M_PI * k / N) * out_real[k + N / 2] + 
                        sin(2 * M_PI * k / N) * out_imag[k + N / 2];
        double t_imag = -sin(2 * M_PI * k / N) * out_real[k + N / 2] + 
                        cos(2 * M_PI * k / N) * out_imag[k + N / 2];

        out_real[k + N / 2] = out_real[k] - t_real;
        out_imag[k + N / 2] = out_imag[k] - t_imag;
        out_real[k] += t_real;
        out_imag[k] += t_imag;
    }

    // Clean up
    free(even_real);
    free(even_imag);
    free(odd_real);
    free(odd_imag);
}

// Function to compute magnitudes after FFT
void compute_fft_magnitudes(const double* input, size_t num, double* magnitudes) {
    // Allocate output arrays for real and imaginary parts
    double* out_real = (double*)malloc(num * sizeof(double));
    double* out_imag = (double*)malloc(num * sizeof(double));
    
    // Perform FFT
    fft(input, out_real, out_imag, num);

    // Compute magnitudes
    for (size_t i = 0; i < num / 2; i++) {
        magnitudes[i] = sqrt(out_real[i] * out_real[i] + out_imag[i] * out_imag[i]);
    }

    // Clean up
    free(out_real);
    free(out_imag);
}
// Function to compute and save MFCCs (Placeholder)
void computeMFCC(double* spectrogram, size_t num_frames) {
    // Dummy MFCC computation: simply print the spectrogram values as MFCCs
    FILE* mfcc_file = fopen("mfccs_c.txt", "w");
    if (!mfcc_file) {
        fprintf(stderr, "Error opening MFCC output file!\n");
        return;
    }

    for (size_t i = 0; i < num_frames; i++) {
        // For demonstration, writing the first 13 values as MFCCs
        fprintf(mfcc_file, "%f %f %f %f %f %f %f %f %f %f %f %f %f\n",
                spectrogram[i * FRAME_LENGTH], // Replace this with actual MFCC calculation
                spectrogram[i * FRAME_LENGTH + 1],
                spectrogram[i * FRAME_LENGTH + 2],
                spectrogram[i * FRAME_LENGTH + 3],
                spectrogram[i * FRAME_LENGTH + 4],
                spectrogram[i * FRAME_LENGTH + 5],
                spectrogram[i * FRAME_LENGTH + 6],
                spectrogram[i * FRAME_LENGTH + 7],
                spectrogram[i * FRAME_LENGTH + 8],
                spectrogram[i * FRAME_LENGTH + 9],
                spectrogram[i * FRAME_LENGTH + 10],
                spectrogram[i * FRAME_LENGTH + 11],
                spectrogram[i * FRAME_LENGTH + 12]
        );
    }

    fclose(mfcc_file);
    printf("MFCCs saved to mfccs_c.txt\n");
}

double hz_to_mel(double freq) {
    return 2595.0f * log10f(1.0f + freq / 700.0f);
}

double mel_to_hz(double mel) {
    return 700.0f * (powf(10.0f, mel / 2595.0f) - 1.0f);
}

void dct(double *input, double *output, int num_filters, int num_mfccs) {
    for (int i = 0; i < num_mfccs; ++i) {
        output[i] = 0.0f;
        for (int j = 0; j < num_filters; ++j) {
            output[i] += input[j] * cosf(M_PI * i * (j + 0.5f) / num_filters);
        }
    }
}
void apply_mel_filterbank(double *magnitude, double *mel_energies, double *filterbank, int num_filters, int fft_length) {
    for (int i = 0; i < num_filters; ++i) {
        mel_energies[i] = 0.0f;
        for (int j = 0; j < fft_length / 2; ++j) {
            mel_energies[i] += magnitude[j] * filterbank[i * (fft_length / 2) + j];
        }
    }
}

void log_mel_energies(double *mel_energies, int num_filters) {
    for (int i = 0; i < num_filters; ++i) {
        mel_energies[i] = logf(mel_energies[i] + 1e-6f);  // Add small epsilon to avoid log(0)
    }
}

void create_mel_filterbank(double *filterbank, int num_filters, int fft_length, int sample_rate, double min_freq, double max_freq) {
    double mel_min = hz_to_mel(min_freq);
    double mel_max = hz_to_mel(max_freq);

    double mel_step = (mel_max - mel_min) / (num_filters + 1);
    double mel_points[num_filters + 2];
    for (int i = 0; i < num_filters + 2; ++i) {
        mel_points[i] = mel_to_hz(mel_min + mel_step * i);
    }

    for (int i = 0; i < num_filters; ++i) {
        for (int j = 0; j < fft_length / 2; ++j) {
            double freq = (double)j * (sample_rate / 2) / (fft_length / 2);
            if (freq >= mel_points[i] && freq <= mel_points[i + 1]) {
                filterbank[i * (fft_length / 2) + j] = (freq - mel_points[i]) / (mel_points[i + 1] - mel_points[i]);
            } else if (freq >= mel_points[i + 1] && freq <= mel_points[i + 2]) {
                filterbank[i * (fft_length / 2) + j] = (mel_points[i + 2] - freq) / (mel_points[i + 2] - mel_points[i + 1]);
            } else {
                filterbank[i * (fft_length / 2) + j] = 0.0f;
            }
        }
    }
}

// Main function
int main() {
    // Open the WAV file
    FILE* file = fopen("recordings/asistent.wav", "rb");
    if (!file) {
        fprintf(stderr, "Error opening file!\n");
        return -1;
    }

    // Read the WAV header
    WAVHeader header;
    fread(&header, sizeof(WAVHeader), 1, file);

    // Check if the file is a valid WAV file
    if (header.audioFormat != 1) { // PCM format
        fprintf(stderr, "Unsupported audio format!\n");
        fclose(file);
        return -1;
    }

    // Read the audio data
    size_t num_samples = header.subchunk2Size / sizeof(int16_t); // Assuming 16-bit PCM
    double* signal = (double*)malloc(num_samples * sizeof(double));
    if (!signal) {
        fprintf(stderr, "Memory allocation failed!\n");
        fclose(file);
        return -1;
    }

    for (size_t i = 0; i < num_samples; i++) {
        int16_t sample;
        fread(&sample, sizeof(int16_t), 1, file);
        signal[i] = (double)sample / 32768.0; // Normalize to [-1, 1]
    }
    fclose(file);

    printf("Num of samples: %d\n", num_samples);
    for(int i =0; i<10;i++)
    {
        printf("%f ", signal[i]);
    }
    printf("%15.12f", signal[num_samples-1]);
    printf("\n");

    // Normalize the signal
    //normalizeSignal(signal, num_samples);

    // Calculate the number of frames
    int num_frames = (num_samples - FRAME_LENGTH) / FRAME_STEP + 1;
    printf("Num of frames: %d\n", num_frames);

    int num_mel_filters = 80;
    int num_mfccs = 13;

    double mel_energies[num_mel_filters];
    double magnitude[FRAME_LENGTH / 2];
    double filterbank[num_mel_filters * (FRAME_LENGTH / 2)];
    create_mel_filterbank(filterbank, num_mel_filters, FRAME_LENGTH, 44100, 80.0, 7600.0);


    // Process each frame
    for (int i = 0; i < num_frames; i++) {
        double frame[FRAME_LENGTH];
        memcpy(frame, signal + i * FRAME_STEP, FRAME_LENGTH * sizeof(double));
        applyHammingWindow(frame, FRAME_LENGTH);

        compute_fft_magnitudes(frame, FRAME_LENGTH, magnitude);
        
    
        // Apply Mel filter bank to get Mel energies
        apply_mel_filterbank(magnitude, mel_energies, filterbank, num_mel_filters, FRAME_LENGTH);

        // Take log of Mel energies
        log_mel_energies(mel_energies, num_mel_filters);

        // Compute MFCCs using DCT
        double mfcc_frame[num_mfccs];
        dct(mel_energies, mfcc_frame, num_mel_filters, num_mfccs);

        // Store MFCCs for the current frame
        for (int i = 0; i < num_mfccs; ++i) {
            mfcc_out[frame * num_mfccs + i] = mfcc_frame[i];
        }
    }

    // Output spectrogram dimensions
    printf("Spectrogram size: %d frames, %d points per frame.\n", num_frames, FRAME_LENGTH);

    // Compute MFCCs
    computeMFCC(spectrogram_real, num_frames);

    // Clean up
    free(signal);
    free(spectrogram_real);
    free(spectrogram_imag);

    return 0;
}
