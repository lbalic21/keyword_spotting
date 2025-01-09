import numpy as np
import argparse

def load_file(filename):
    """Load file from a given filename."""
    try:
        return np.loadtxt(filename)
    except Exception as e:
        print(f"Error loading {filename}: {e}")
        return None

def compare_files(file1, file2, diff_filename, threshold):
    """Compare two sets of files and save the differences in the same format."""
    # Check if shapes are the same
    if file1.shape != file2.shape:
        print("Shapes are different:")
        print(f"File 1 shape: {file1.shape}")
        print(f"File 2 shape: {file2.shape}")
        return False

    # Calculate the difference
    difference = np.abs(file1 - file2)

    count_differences = np.sum(difference >= threshold)
    num_of_elements = file2.shape[0] * file2.shape[1]

    if count_differences == 0:
        print("Files are approximately equal within the threshold.")
    else:
        print(f"Files differ beyond the threshold in {count_differences}/{num_of_elements} elements => {count_differences / num_of_elements * 100:.2f}%")

    # Save differences to a file in the same format
    np.savetxt(diff_filename, difference, fmt='%.6f')
    print(f"Differences written to {diff_filename}")

def main():
    # Set up argument parser
    parser = argparse.ArgumentParser(description="Compare two mel spectrogram files and output the differences.")
    parser.add_argument("file1", type=str, help="Path to the first mel spectrogram file.")
    parser.add_argument("file2", type=str, help="Path to the second mel spectrogram file.")
    #parser.add_argument("output_file", type=str, help="Path to save the output differences file.")
    parser.add_argument("-t", type=float, default=0.0001, help="Threshold for considering values different.")

    # Parse the arguments
    args = parser.parse_args()

    # Load the files
    file1 = load_file(args.file1)
    file2 = load_file(args.file2)
    output_file = "difference.txt"

    print("Threshold: ", args.t)

    if file1 is not None and file2 is not None:
        # Compare files and write differences to a file
        compare_files(file1, file2, output_file, args.t)
    else:
        print("Failed to load one or both files.")

if __name__ == "__main__":
    main()
