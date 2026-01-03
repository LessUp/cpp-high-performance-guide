# Tests CMakeLists.txt
# Unit tests and property-based tests

include(GoogleTest)

# Add test subdirectories
add_subdirectory(unit)
add_subdirectory(property)
