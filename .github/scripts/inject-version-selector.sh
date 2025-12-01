#!/bin/bash
set -e

# Script to inject version selector into Doxygen HTML documentation
# Usage: ./inject-version-selector.sh <docs_directory>

DOCS_DIR="${1:-static}"
SELECTOR_HTML=".github/scripts/version-selector.html"

if [ ! -d "$DOCS_DIR" ]; then
  echo "Error: Documentation directory '$DOCS_DIR' not found"
  exit 1
fi

if [ ! -f "$SELECTOR_HTML" ]; then
  echo "Error: Version selector file '$SELECTOR_HTML' not found"
  exit 1
fi

echo "Injecting version selector into Doxygen documentation..."

# Create a temporary file for the selector content
TEMP_SELECTOR=$(mktemp)
cat "$SELECTOR_HTML" > "$TEMP_SELECTOR"

# Find all HTML files in the docs directory
find "$DOCS_DIR" -name "*.html" -type f | while read -r html_file; do
  # Check if the file already has the version selector (avoid double injection)
  if grep -q "id=\"version-selector\"" "$html_file"; then
    echo "Skipping $html_file (already has version selector)"
    continue
  fi

  # Inject the version selector just before the closing </body> tag
  if grep -q "</body>" "$html_file"; then
    # Create a temporary output file
    TEMP_OUTPUT=$(mktemp)

    # Use awk to inject the selector before </body>
    awk -v selector="$TEMP_SELECTOR" '
      /<\/body>/ {
        while ((getline line < selector) > 0) {
          print line
        }
        close(selector)
      }
      { print }
    ' "$html_file" > "$TEMP_OUTPUT"

    # Replace the original file
    mv "$TEMP_OUTPUT" "$html_file"
    echo "Injected version selector into: $html_file"
  fi
done

# Clean up
rm -f "$TEMP_SELECTOR"

echo "Version selector injection complete!"
