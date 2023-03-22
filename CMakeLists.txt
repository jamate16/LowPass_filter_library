add_library(filter STATIC
    Filter.h
    Filter.cpp
)

# pico_generate_pio_header(encoder ${CMAKE_CURRENT_LIST_DIR}/quadrature_encoder.pio)

# This will link all the required libraries from RP2040's SDK
# in case it has already been linked from another proprietary
# library, the guard clauses in the preprocesor directives
# will avoid duplicate code
target_link_libraries(filter PUBLIC
    pico_stdlib
)

target_include_directories(filter PUBLIC ${CMAKE_CURRENT_LIST_DIR})


