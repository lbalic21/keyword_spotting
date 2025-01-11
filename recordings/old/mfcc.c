#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

/**
 * @brief START_FROM => 0 means raw signal is used
 *                      1 means windowed spectrogram is loaded from TF implementation
 *                      2 means log mel-spectrogram is loaded from TF implementation
 */
#define START_FROM 0

#if START_FROM == 0
    #include <fftw3.h>
#endif

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define FRAME_LENGTH            512
#define SPECTROGRAM_LENGTH      ((FRAME_LENGTH / 2) + 1)
#define FRAME_STEP              256

#define NUM_OF_MEL_BINS         80
#define NUM_OF_MFCCS            13

#define SAMPLE_RATE             44100
#define LOWER_F                 80.0
#define HIGHER_F                7600.0

#define MEL_BREAK_FREQUENCY_HERTZ      700.0
#define MEL_HIGH_FREQUENCY_Q           1127.0

// WAV file header structure
#pragma pack(push, 1)
typedef struct 
{
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

/* STEP 0 */
void applyHammingWindow(float* frame, size_t length) 
{
    for (size_t n = 0; n < length; n++) {
        frame[n] *= 0.54 - 0.46 * cos((2 * M_PI * n) / (length - 1)); // Hamming window formula
    }
}

/* STEP 1 */
float hz_to_mel(float freq)
{
    return MEL_HIGH_FREQUENCY_Q * log10f(1.0f + freq / MEL_BREAK_FREQUENCY_HERTZ);
}

float mel_to_hz(float mel)
{
    return MEL_BREAK_FREQUENCY_HERTZ * (powf(10.0f, mel / MEL_HIGH_FREQUENCY_Q) - 1.0f);
}

void log_mel_energies(float *mel_energies) 
{
    for (int i = 0; i < NUM_OF_MEL_BINS; ++i) {
        mel_energies[i] = logf(mel_energies[i] + 1e-6f);  // Add small epsilon to avoid log(0)
    }
}

// Compute the Mel coefficients for one spectrogram frame
void compute_mel_coeffs(const float *spectrum, float *mel_coeffs,
                        int num_mel_bins, int num_spectrogram_bins,
                        float sample_rate, float lower_edge_hertz,
                        float upper_edge_hertz) {
    // Nyquist frequency (half the sampling rate)
    float nyquist_hertz = sample_rate / 2.0;

    // Calculate Mel frequency range
    float lower_edge_mel = hz_to_mel(lower_edge_hertz);
    float upper_edge_mel = hz_to_mel(upper_edge_hertz);

    // Create an array to store the Mel bin edges in Hertz
    float *mel_bin_edges_hertz = (float *)malloc((num_mel_bins + 2) * sizeof(float));

    // Calculate the Mel bin edges
    for (int i = 0; i < num_mel_bins + 2; i++) {
        float mel = lower_edge_mel + (upper_edge_mel - lower_edge_mel) * i / (num_mel_bins + 1);
        mel_bin_edges_hertz[i] = mel_to_hz(mel);
    }

    // Clear the mel_coeffs array
    for (int i = 0; i < num_mel_bins; i++) {
        mel_coeffs[i] = 0.0;
    }

    // Linear frequencies corresponding to the spectrogram bins
    for (int j = 0; j < num_spectrogram_bins; j++) {
        float linear_freq_hz = nyquist_hertz * (float)j / (num_spectrogram_bins - 1);

        // Find the contribution of this spectrogram bin to each Mel bin
        for (int i = 1; i <= num_mel_bins; i++) {
            float lower_hz = mel_bin_edges_hertz[i - 1];
            float center_hz = mel_bin_edges_hertz[i];
            float upper_hz = mel_bin_edges_hertz[i + 1];

            if (linear_freq_hz >= lower_hz && linear_freq_hz <= upper_hz) {
                float weight = 0.0;

                if (linear_freq_hz <= center_hz) {
                    weight = (linear_freq_hz - lower_hz) / (center_hz - lower_hz);
                } else {
                    weight = (upper_hz - linear_freq_hz) / (upper_hz - center_hz);
                }

                // Accumulate the weighted contribution of this frequency bin
                mel_coeffs[i - 1] += weight * spectrum[j];
            }
        }
    }

    // Free memory used for Mel bin edges
    free(mel_bin_edges_hertz);
}

void spectro_to_mel_spectro(float* spectrogram, float* mel_spectrogram) 
{
    float lower_mel = hz_to_mel(LOWER_F);
    float upper_mel = hz_to_mel(HIGHER_F);

    float mel_step = (upper_mel - lower_mel) / (NUM_OF_MEL_BINS + 1);
    float* mel_points = (float*)malloc((NUM_OF_MEL_BINS + 2) * sizeof(float));
    if (!mel_points) 
    {
        fprintf(stderr, "Memory allocation failed for mel_points!\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_OF_MEL_BINS + 2; i++) 
    {
        mel_points[i] = mel_to_hz(lower_mel + i * mel_step);
    }

    // Initialize the mel spectrogram
    for (int m = 0; m < NUM_OF_MEL_BINS; m++) 
    {
        mel_spectrogram[m] = 0.0;  // Initialize the mel spectrogram bins to zero
        float left = mel_points[m];
        float center = mel_points[m + 1];
        float right = mel_points[m + 2];

        // Apply the triangular filter to compute the mel spectrogram value
        for (int k = 0; k < SPECTROGRAM_LENGTH; k++) 
        {
            float frequency = (float)k * SAMPLE_RATE / (2.0 * (SPECTROGRAM_LENGTH - 1));  // Convert index to frequency

            if (frequency >= left && frequency <= center) 
            {
                mel_spectrogram[m] += (frequency - left) / (center - left) * spectrogram[k];  // Left side of triangle
            } 
            else if (frequency > center && frequency <= right) 
            {
                mel_spectrogram[m] += (right - frequency) / (right - center) * spectrogram[k];  // Right side of triangle
            }
        }
    }

    free(mel_points); // Free the allocated memory
}

/* STEP 2 */
void dct(float *input, float *output)
{
    for (int i = 0; i < NUM_OF_MFCCS; i++) {
        output[i] = 0.0;
        for (int j = 0; j < NUM_OF_MEL_BINS; j++)
        {
            output[i] += input[j] * cos(M_PI * i * (j + 0.5) / NUM_OF_MEL_BINS);
        }
        output[i] *= sqrt(2.0 / NUM_OF_MEL_BINS);
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
    return file;
}

// Main function
int main() {
    /* Reading wav file */
    FILE* wav = open_file("../recordings/ugasi/ugasi.wav", "rb");
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
    printf("Spectrogram length: %d\n", SPECTROGRAM_LENGTH);

    float* signal = (float*)malloc(num_samples * sizeof(float));
    if (!signal) {
        fprintf(stderr, "Memory allocation failed!\n");
        fclose(wav);
        return -1;
    }
    FILE *write_test = open_file("samples512.txt", "w");
  
    int16_t testSignal[512];
    int signalNum = 0;
    for (size_t i = 0; i < num_samples; i++) {
        int16_t sample;
        if (fread(&sample, sizeof(int16_t), 1, wav) != 1) {
            fprintf(stderr, "Error reading sample data!\n");
            free(signal);
            fclose(wav);
            return -1;
        }

        if(i >= 20001 && i <= 20512)
        {
            //printf("%d ", sample);
            //fprintf(write_test, "%d\n", sample);
            testSignal[signalNum++] = sample;
            
        }
        signal[i] = (float)sample / 32768.0; // Normalize to [-1, 1]
    }
    fclose(wav);
   
    


    FILE* out_spectrogram = open_file("spectrogram_c.txt", "w");
    FILE *out_mel_spectrogram = open_file("mel_spectrogram_c.txt", "w");
    FILE *out_log_mel_spectrogram = open_file("log_mel_spectrogram_c.txt", "w"); 
    FILE *out_mfccs = open_file("mfccs_c.txt", "w");

    FILE* spectro_py = open_file("spectro_python.txt", "r");
    FILE *log_mel_py = open_file("log_mel_spectro_python.txt", "r");

    float frame[FRAME_LENGTH];
    float spectrogram[SPECTROGRAM_LENGTH];
    float mel_spectro[NUM_OF_MEL_BINS];
    float mfccs[NUM_OF_MFCCS];

    #if START_FROM == 0
        fftw_plan plan;
        fftw_complex fft_output[SPECTROGRAM_LENGTH]; 
    #endif

    /* Process each frame */
    for (int i = 0; i < num_frames; i++) 
    {
        #if START_FROM == 0
            //memcpy(frame, signal + i * FRAME_STEP, FRAME_LENGTH * sizeof(float));
            for(int w=0;w<512;w++)
            {
                frame[w] = (float)testSignal[w] / 32768.0;
                //printf("%f ",frame[w]);
            
            }
            applyHammingWindow(frame, FRAME_LENGTH);
            printf("\nWINDOWED\n");
            for(int w=0;w<512;w++)
            {
               
            //printf("%d - %f\n",w,frame[w]);
            }
            printf("\n");

            plan = fftw_plan_dft_r2c_1d(FRAME_LENGTH, (double*)frame, fft_output, FFTW_ESTIMATE);
            fftw_execute(plan);
            
            for (int j = 0; j < SPECTROGRAM_LENGTH; j++) 
            {
                float real = fft_output[j][0]; // Real part
                float imag = fft_output[j][1]; // Imaginary part
                spectrogram[j] = sqrt(real * real + imag * imag); // Magnitude
            }
            printf("\nFFT\n");
            for(int w=0;w<SPECTROGRAM_LENGTH;w++){printf("FFT%d -> %f\n",w, spectrogram[w]);}
            printf("\n");

            for (int j = 0; j < SPECTROGRAM_LENGTH; j++) 
            {
                fprintf(out_spectrogram, "%f\n", spectrogram[j]);  // Write each coefficient
            }
            fprintf(out_spectrogram, "\n"); 

            spectro_to_mel_spectro(spectrogram, mel_spectro);
            printf("\nMEL\n");
            for(int j = 0; j < NUM_OF_MEL_BINS; j++) 
            {
            printf("MEL%d -> %f\n",j, mel_spectro[j]);

                fprintf(out_mel_spectrogram, "%f\n", mel_spectro[j]);
            }
            fprintf(out_mel_spectrogram, "\n");

            log_mel_energies(mel_spectro);
            for(int j = 0; j < NUM_OF_MEL_BINS; j++) 
            {
               fprintf(out_log_mel_spectrogram, "%f ", mel_spectro[j]); 
            }
            fprintf(out_mel_spectrogram, "\n");

            dct(mel_spectro, mfccs);
            for(int j = 0; j < NUM_OF_MFCCS; j++) 
            {
                fprintf(out_mfccs, "%f ", mfccs[j]);
            }
            fprintf(out_mfccs, "\n");

            break;

        #endif

        #if START_FROM == 1
            for (int j = 0; j < SPECTROGRAM_LENGTH; j++) 
            {
                fscanf(spectro_py, "%f", &spectrogram[j]);
                fprintf(write_test, "%f ", spectrogram[j]); 
            }
            fprintf(write_test, "\n");

            spectro_to_mel_spectro(spectrogram, mel_spectro);
            //compute_mel_coeffs(spectrogram, mel_spectro, NUM_OF_MEL_BINS, SPECTROGRAM_LENGTH, SAMPLE_RATE, LOWER_F, HIGHER_F);
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
            fprintf(out_mel_spectrogram, "\n");

            dct(mel_spectro, mfccs);
            for(int j = 0; j < NUM_OF_MFCCS; j++) 
            {
                fprintf(out_mfccs, "%f ", mfccs[j]);
            }
            fprintf(out_mfccs, "\n");

        #endif

        #if START_FROM == 2
            for(int j = 0; j < NUM_OF_MEL_BINS; j++) 
            {
               fscanf(log_mel_py, "%f", &mel_spectro[j]);
               fprintf(write_test, "%f ", mel_spectro[j]); 
            }
            fprintf(write_test, "\n");

            dct(mel_spectro, mfccs);
            for(int j = 0; j < NUM_OF_MFCCS; j++) 
            {
                fprintf(out_mfccs, "%f ", mfccs[j]);
            }
            fprintf(out_mfccs, "\n");

        #endif
    }

    free(signal);

    #if START_FROM == 0
        fftw_destroy_plan(plan);
        fftw_cleanup();
    #endif

    fclose(out_spectrogram);
    fclose(out_mel_spectrogram);
    fclose(out_log_mel_spectrogram);
    fclose(out_mfccs);
    fclose(spectro_py);
    fclose(log_mel_py);
    fclose(write_test);

    return 0;
}
