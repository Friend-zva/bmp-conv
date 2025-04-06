find_path(CMOCKA_INCLUDE_DIR NAMES cmocka.h)
set(CMOCKA_INCLUDE_DIRS "${CMOCKA_INCLUDE_DIR}")

find_library(CMOCKA_LIBRARY NAMES cmocka)
set(CMOCKA_LIBRARIES "${CMOCKA_LIBRARY}")

# Set CMocka_FOUND (if all required vars are found).
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CMocka DEFAULT_MSG CMOCKA_INCLUDE_DIRS CMOCKA_LIBRARIES)

# Hide variables from cmake GUIs.
mark_as_advanced(CMOCKA_INCLUDE_DIR CMOCKA_INCLUDE_DIRS CMOCKA_LIBRARY CMOCKA_LIBRARIES)
