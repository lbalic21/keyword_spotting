#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <fftw3.h>

#define FRAME_LENGTH 1024
#define Spectro_LEN  513
#define NUM_OF_MEl_BINS 80

#define FRAME_STEP 256
#define SAMPLE_RATE 44100
#define NUM_OF_MFCCS 13
#define LOWER_F 80.0
#define HUGHER_F 7600.0

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

// Function to apply Hamming window
void applyHammingWindow(double* frame, size_t length) {
    for (size_t n = 0; n < length; n++) {
        frame[n] *= 0.54 - 0.46 * cos((2 * M_PI * n) / (length - 1)); // Hamming window formula
    }
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

    FILE *out_spectrogram = fopen("spectrogram_c.txt", "w");  // Open the file for writing

    if (out_spectrogram== NULL) {
        printf("Error opening file out!\n");
        return 1;  // Exit if file can't be opened
    }

    for (size_t i = 0; i < num_samples; i++) {
        int16_t sample;
        fread(&sample, sizeof(int16_t), 1, file);
        signal[i] = (double)sample / 32768.0; // Normalize to [-1, 1]
    }
    fclose(file);

    printf("Num of samples: %ld\n", num_samples);
    for(int i =0; i<10;i++)
    {
        printf("%15.12f ", signal[i]);
    }
    printf("%15.12f", signal[num_samples-1]);
    printf("\n");

    // Calculate the number of frames
    int num_frames = (num_samples - FRAME_LENGTH) / FRAME_STEP + 1;
    printf("Num of frames: %d\n", num_frames);



    double frame[FRAME_LENGTH];
    double spectrogram[Spectro_LEN];
    double mel_spectro[NUM_OF_MEl_BINS];
    double mfccs[NUM_OF_MFCCS];

    fftw_plan plan;
    fftw_complex fft_output[Spectro_LEN]; 
   
    //double filterbank[num_mel_filters * (FRAME_LENGTH / 2)];
    //create_mel_filterbank(filterbank, num_mel_filters, FRAME_LENGTH, 44100, 80.0, 7600.0);

    // Process each frame
    for (int i = 0; i < num_frames; i++) {

        memcpy(frame, signal + i * FRAME_STEP, FRAME_LENGTH * sizeof(double));
        applyHammingWindow(frame, FRAME_LENGTH);

        plan = fftw_plan_dft_r2c_1d(FRAME_LENGTH, frame, fft_output, FFTW_ESTIMATE);
        fftw_execute(plan);
        // Step 4: Compute the magnitudes of the FFT output
        for (int i = 0; i < Spectro_LEN; i++) {
            double real = fft_output[i][0]; // Real part
            double imag = fft_output[i][1]; // Imaginary part
            spectrogram[i] = sqrt(real * real + imag * imag); // Magnitude
        }

        for (int j = 0; j < Spectro_LEN; j++) {
           fprintf(out_spectrogram, "%f ", spectrogram[j]);  // Write each coefficient
        }
        fprintf(out_spectrogram, "\n"); 

        //spectro_to_mel_spectro(spectrogram, mel_spectro);

        // Apply Mel filter bank to get Mel energies
        //apply_mel_filterbank(magnitude, mel_energies, filterbank, num_mel_filters, FRAME_LENGTH);

        // Take log of Mel energies
        //log_mel_energies(mel_energies, num_mel_filters);

        // Compute MFCCs using DCT
        //double mfcc_frame[num_mfccs];
        //dct(mel_energies, mfcc_frame, num_mel_filters, num_mfccs);

        // Store MFCCs for the current frame
        //for (int i = 0; i < num_mfccs; i++) {
        //fprintf(out, "%f ", mfcc_frame[i]);  // Write each coefficient
    //}
        
    }

    // Output spectrogram dimensions
   

    // Clean up
    free(signal);

    fftw_destroy_plan(plan);
    fftw_cleanup();
    fclose(out_spectrogram);
    
    return 0;
}
