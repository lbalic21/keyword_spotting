#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#define ON_LINUX  0

#if ON_LINUX == 1
    #include <fftw3.h>
#endif

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define FRAME_LENGTH            1024
#define SPECTROGRAM_LENGTH      ((FRAME_LENGTH / 2) + 1)
#define NUM_OF_MEL_BINS         80
#define FRAME_STEP              256
#define SAMPLE_RATE             44100
#define NUM_OF_MFCCS            13
#define LOWER_F                 80.0
#define HIGHER_F                7600.0

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

void dct(double *input, double *output) {
    for (int i = 0; i < NUM_OF_MFCCS; ++i) {
        output[i] = 0.0;
        for (int j = 0; j < NUM_OF_MEL_BINS; ++j) {
            output[i] += input[j] * cos(M_PI * i * (j + 0.5) / NUM_OF_MEL_BINS);
        }
        // Normalize the DCT coefficients
        if (i == 0) {
            output[i] *= sqrt(1.0 / NUM_OF_MEL_BINS); // First coefficient normalization
        } else {
            output[i] *= sqrt(2.0 / NUM_OF_MEL_BINS); // Other coefficients normalization
        }
    }
}
void log_mel_energies(double *mel_energies) {
    for (int i = 0; i < NUM_OF_MEL_BINS; ++i) {
        mel_energies[i] = logf(mel_energies[i] + 1e-6f);  // Add small epsilon to avoid log(0)
    }
}

void spectro_to_mel_spectro(double* spectrogram, double* mel_spectrogram)
{
    double lower_mel = hz_to_mel(LOWER_F);
    double upper_mel = hz_to_mel(HIGHER_F);

    double mel_step = (upper_mel - lower_mel) / (NUM_OF_MEL_BINS + 1);
    double* mel_points = (double*)malloc((NUM_OF_MEL_BINS + 2) * sizeof(double));
    for (int i = 0; i < NUM_OF_MEL_BINS + 2; i++) {
        mel_points[i] = mel_to_hz(lower_mel + i * mel_step);
    }

    // Initialize the mel spectrogram
    for (int m = 0; m < NUM_OF_MEL_BINS; m++) {
        mel_spectrogram[m] = 0.0;  // Initialize the mel spectrogram bins to zero
        double left = mel_points[m];
        double center = mel_points[m + 1];
        double right = mel_points[m + 2];

        // Apply the triangular filter to compute the mel spectrogram value
        for (int k = 0; k < SPECTROGRAM_LENGTH; k++) {
            double frequency = (double)k * SAMPLE_RATE / (2.0 * (SPECTROGRAM_LENGTH - 1));  // Convert index to frequency

            if (frequency >= left && frequency <= center) {
                mel_spectrogram[m] += (frequency - left) / (center - left) * spectrogram[k];  // Left side of triangle
            } else if (frequency > center && frequency <= right) {
                mel_spectrogram[m] += (right - frequency) / (right - center) * spectrogram[k];  // Right side of triangle
            }
        }
    }
}

FILE* open_file(const char* path, const char* rwb)
{
    FILE* file = fopen(path, rwb);
    if(!file)
    {
        fprintf(stderr, "Error opening file! -> %s\n", path);
        exit(EXIT_FAILURE);
    }
    else
    {
        return file;
    }
}

// Main function
int main() {

    /* Reading wav file */
    FILE* wav = open_file("recordings/asistent.wav", "rb");
    WAVHeader header;
    fread(&header, sizeof(WAVHeader), 1, wav);
    if (header.audioFormat != 1) { 
        fprintf(stderr, "Unsupported audio format!\n");
        fclose(wav);
        return -1;
    }

    size_t num_samples = header.subchunk2Size / sizeof(int16_t); // Assuming 16-bit PCM
    printf("Num of samples: %ld\n", num_samples);
    int num_frames = (num_samples - FRAME_LENGTH) / FRAME_STEP + 1;
    printf("Num of frames: %d\n", num_frames);

    double* signal = (double*)malloc(num_samples * sizeof(double));
    if (!signal) {
        fprintf(stderr, "Memory allocation failed!\n");
        fclose(wav);
        return -1;
    }
    for (size_t i = 0; i < num_samples; i++) {
        int16_t sample;
        fread(&sample, sizeof(int16_t), 1, wav);
        signal[i] = (double)sample / 32768.0; // Normalize to [-1, 1]
    }
    fclose(wav);

    /********************************/

    FILE* out_spectrogram = open_file("spectrogram_c.txt", "rw");
    FILE *out_mel_spectrogram = open_file("mel_spectrogram_c.txt", "w");
    FILE *out_log_mel_spectrogram = open_file("log_mel_spectrogram_c.txt", "w"); 
    FILE *out_mfccs = open_file("mfccs_c.txt", "w");

    double frame[FRAME_LENGTH];
    double spectrogram[SPECTROGRAM_LENGTH];
    double mel_spectro[NUM_OF_MEL_BINS];
    double mfccs[NUM_OF_MFCCS];

    #if ON_LINUX == 1
        fftw_plan plan;
        fftw_complex fft_output[SPECTROGRAM_LENGTH]; 
    #endif

    /* Process each frame */
    for (int i = 0; i < num_frames; i++) {

        #if ON_LINUX == 1

            memcpy(frame, signal + i * FRAME_STEP, FRAME_LENGTH * sizeof(double));
            applyHammingWindow(frame, FRAME_LENGTH);

            plan = fftw_plan_dft_r2c_1d(FRAME_LENGTH, frame, fft_output, FFTW_ESTIMATE);
            fftw_execute(plan);
            
            for (int j = 0; j < SPECTROGRAM_LENGTH; j++) {
                double real = fft_output[j][0]; // Real part
                double imag = fft_output[j][1]; // Imaginary part
                spectrogram[j] = sqrt(real * real + imag * imag); // Magnitude
            }

            for (int j = 0; j < SPECTROGRAM_LENGTH; j++) {
               fprintf(out_spectrogram, "%f ", spectrogram[j]);  // Write each coefficient
            }
            fprintf(out_spectrogram, "\n"); 

        #else

            for (int j = 0; j < SPECTROGRAM_LENGTH; j++)
            {
                fscanf(out_spectrogram, "%lf", &spectrogram[j]);
            }

        #endif

        spectro_to_mel_spectro(spectrogram, mel_spectro);
        for(int j = 0; j < NUM_OF_MEL_BINS; j++)
        {
            fprintf(out_mel_spectrogram, "%f ", mel_spectro[j]);
        }
        fprintf(out_mel_spectrogram, "\n"); 

        log_mel_energies(mel_spectro);
        for(int j = 0; j < NUM_OF_MEL_BINS; j++)
        {
            fprintf(out_log_mel_spectrogram, "%f ", mel_spectro[j]);
        }
        fprintf(out_log_mel_spectrogram, "\n");

        dct(mel_spectro, mfccs);
        for(int j=0; j<NUM_OF_MFCCS; j++)
        {
            fprintf(out_mfccs, "%f ", mfccs[j]);
        }
        fprintf(out_mfccs, "\n");
    }

    free(signal);

    #if ON_LINUX == 1
        fftw_destroy_plan(plan);
        fftw_cleanup();
    #endif

    fclose(out_spectrogram);
    fclose(out_mel_spectrogram);
    fclose(out_log_mel_spectrogram);
    fclose(out_mfccs);

    return 0;
}
