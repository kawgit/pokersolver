from pathlib import Path

print("Below I have pasted the contents of all the files in the src directory of my cpp poker solver project so far. Please refrain from using comments if possible within your code. Use snake casing rather than camel casing.")
print("Please help me build table.cpp's print function.")

print("\n\n")

for file in sorted(Path("src").iterdir()):
    if file.is_file():
        print(file.name, end=":\n")
        print(file.read_text().strip(), end="\n\n")
