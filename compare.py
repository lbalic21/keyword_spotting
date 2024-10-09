import numpy as np

def load_mfccs(filename):
    """Load MFCCs from a given filename."""
    return np.loadtxt(filename)

def compare_mfccs(mfccs_python, mfccs_c):
    """Compare two sets of MFCCs and print the results."""
    # Check if shapes are the same
    if mfccs_python.shape != mfccs_c.shape:
        print("MFCC shapes are different:")
        print(f"Python MFCCs shape: {mfccs_python.shape}")
        print(f"C MFCCs shape: {mfccs_c.shape}")
        return False

    # Calculate the difference
    difference = np.abs(mfccs_python - mfccs_c)
    
    # Set a threshold for "closeness" (e.g., 1e-5)
    threshold = 1e-5
    count_differences = np.sum(difference >= threshold)

    if count_differences == 0:
        print("MFCCs are approximately equal within the threshold.")
    else:
        print(f"MFCCs differ beyond the threshold in {count_differences} elements.")

    return count_differences == 0  # Return True if they are equal, False otherwise

def main():
    # Load MFCCs from both files
    mfccs_python = load_mfccs("mfccs_python.txt")
    mfccs_c = load_mfccs("mfccs_c.txt")

    # Compare MFCCs
    are_equal = compare_mfccs(mfccs_python, mfccs_c)
    
    if are_equal:
        print("The MFCC outputs from Python and C are equivalent.")
    else:
        print("The MFCC outputs from Python and C are NOT equivalent.")

if __name__ == "__main__":
    main()
