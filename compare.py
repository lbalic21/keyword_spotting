import numpy as np

THRESHOLD = 0.1

file1 = "spectro_python.txt"
file2 = "spectrogram_c.txt"
output_file = "spectro_diff.txt"

def load_file(filename):
    """Load file from a given filename."""
    try:
        return np.loadtxt(filename)
    except Exception as e:
        print(f"Error loading {filename}: {e}")
        return None

def compare_files(file_python, file_c, diff_filename):
    """Compare two sets of files and save the differences in the same format."""
    # Check if shapes are the same
    if file_python.shape != file_c.shape:
        print("Shapes are different:")
        print(f"Python file shape: {file_python.shape}")
        print(f"C file shape: {file_c.shape}")
        return False

    # Calculate the difference
    difference = np.abs(file_python - file_c)

    count_differences = np.sum(difference >= THRESHOLD)

    if count_differences == 0:
        print("File are approximately equal within the threshold.")
    else:
        print(f"Files differ beyond the threshold in {count_differences} elements.")
    
    # Save differences to a file in the same format
    np.savetxt(diff_filename, difference, fmt='%.6f')
    
    print(f"Differences written to {diff_filename}")
    
    return True

def main():

    file_python = load_file(file1)
    file_c = load_file(file2)

    if file_python is not None and file_c is not None:
        # Compare files and write differences to a file
        are_equal = compare_files(file_python, file_c, output_file)
        
        if are_equal:
            print("The outputs from Python and C have been compared and differences saved.")
        else:
            print("The outputs from Python and C are NOT equivalent.")
    else:
        print("Failed to load one or both files.")

if __name__ == "__main__":
    main()
