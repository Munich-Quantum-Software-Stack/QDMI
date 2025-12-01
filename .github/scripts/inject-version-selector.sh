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

# Read the version selector HTML
SELECTOR_CONTENT=$(cat "$SELECTOR_HTML")

# Find all HTML files in the docs directory
find "$DOCS_DIR" -name "*.html" -type f | while read -r html_file; do
  # Check if the file already has the version selector (avoid double injection)
  if grep -q "id=\"version-selector\"" "$html_file"; then
    echo "Skipping $html_file (already has version selector)"
    continue
  fi

  # Inject the version selector just before the closing </body> tag
  if grep -q "</body>" "$html_file"; then
    # Use perl for in-place editing to handle multi-line replacements
    perl -i -pe "s|</body>|${SELECTOR_CONTENT}\n</body>|" "$html_file"
    echo "Injected version selector into: $html_file"
  fi
done

echo "Version selector injection complete!"
