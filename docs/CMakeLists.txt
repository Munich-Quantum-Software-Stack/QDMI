# Set the Doxygen configuration file. In the Doxyfile.in file, there are some
# placeholders that are replaced by CMake. That is also the reason for the
# extension `.in`. For details see below for the command `configure_file`.
set(DOXYGEN_CONFIG_FILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)

# Set the input directories for doxygen, i.e., all files that contain source
# code with documentation.
set(DOXYGEN_INPUT_DIRS ${PROJECT_SOURCE_DIR}/include)
# This variable is only needed to setup the dependency tracking of CMake
# correctly and is not passed to Doxygen. Whenever such a file is modified,
# CMake detects that change and will rerun the Doxygen command instead of using
# the cached result when the target `qdmi_docs`is built.
file(GLOB_RECURSE DOXYGEN_INPUT_FILES ${PROJECT_SOURCE_DIR}/include/*.h
     ${CMAKE_CURRENT_SOURCE_DIR}/*.md)
# After the variable above is correctly initialized, we can add the additional
# files that are not source code but still part of the documentation. IMPORTANT:
# The order the files with the static content are added here influences their
# order in the generated documentation.
set(DOXYGEN_INPUT_DIRS
    ${DOXYGEN_INPUT_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/index.md
    ${CMAKE_CURRENT_SOURCE_DIR}/faq.md
    ${CMAKE_CURRENT_SOURCE_DIR}/support.md
    ${CMAKE_CURRENT_SOURCE_DIR}/examples.md
    ${CMAKE_CURRENT_SOURCE_DIR}/guide.md)
# By default the files are separated by a semicolon. This is not what Doxygen
# wants, so we replace it with a space.
string(REPLACE ";" " " DOXYGEN_INPUT_DIRS "${DOXYGEN_INPUT_DIRS}")

# Such that the content of other files than files listed in DOXYGEN_INPUT_DIRS
# can be included with the `\include`or `\snippet` command, we need to add the
# directories where these files are located.
set(DOXYGEN_EXAMPLE_DIRS
    ${PROJECT_SOURCE_DIR}/examples ${PROJECT_SOURCE_DIR}/.github
    ${PROJECT_SOURCE_DIR}/README.md)
# Again, replace the semicolon with a space such that Doxygen can handle it.
string(REPLACE ";" " " DOXYGEN_EXAMPLE_DIRS "${DOXYGEN_EXAMPLE_DIRS}")

# Set the output directory for the generated documentation: Place it in the
# CMake build directory.
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Set the Doxygen configured configuration file. This will point to the Doxyfile
# that is written out by CMake with all placeholders substituted.
set(DOXYGEN_CONFIG_FILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Save the location the files of the project doxygen-awesome-css were cloned
# into. This allows us to get the path to doxygen-awesome.css. For its usage
# also see the occurrences of @AWESOME_CSS_DIR@ in the Doxyfile.in.
FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

# Replace variables inside @@ with the current values, e.g., the placeholder
# @@DOXYGEN_INPUT_DIRS@@ is replaced by the content of the variable
# DOXYGEN_INPUT_DIRS.
configure_file(${DOXYGEN_CONFIG_FILE_IN} ${DOXYGEN_CONFIG_FILE_OUT} @ONLY)

# Create a custom command to run Doxygen
add_custom_command(
  OUTPUT ${DOXYGEN_OUTPUT_DIR}/html/index.html
  DEPENDS ${DOXYGEN_INPUT_FILES}
          ${DOXYGEN_CONFIG_FILE_IN}
          ${CMAKE_CURRENT_SOURCE_DIR}/header.html
          ${CMAKE_CURRENT_SOURCE_DIR}/layout.xml
          ${CMAKE_CURRENT_SOURCE_DIR}/style.css
          ${CMAKE_SOURCE_DIR}/README.md
          ${CMAKE_SOURCE_DIR}/.github/support.md
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG_FILE_OUT}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Generating API documentation with Doxygen"
  VERBATIM)

# Create a custom target
add_custom_target(qdmi_docs ALL DEPENDS ${DOXYGEN_OUTPUT_DIR}/html/index.html)
