import shutil
import os
import sys

def create_files(num_files):
    template_file = "template.cpp"

    for i in range(num_files):
        # Generate the new file name (a.cpp, b.cpp, c.cpp, ...)
        new_file_name = f"{chr(ord('a') + i)}.cpp"

        # Copy the template file to the new file name
        shutil.copyfile(template_file, new_file_name)

        print(f"Created file: {new_file_name}")

if __name__ == "__main__":
    # Check if the correct number of command line arguments is provided
    if len(sys.argv) != 2:
        print("Usage: python script.py <number_of_files>")
        sys.exit(1)

    try:
        # Get the number of files from the command line argument
        num_files = int(sys.argv[1])

        # Call the function to create the files
        create_files(num_files)
    except ValueError:
        print("Please provide a valid integer for the number of files.")
        sys.exit(1)

