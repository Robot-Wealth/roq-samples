set(TARGET_NAME "${PROJECT_NAME}-example-8")

add_subdirectory(flags)

add_executable("${TARGET_NAME}" application.cpp config.cpp strategy.cpp main.cpp)

target_link_libraries("${TARGET_NAME}" PRIVATE ${TARGET_NAME}-flags roq-client::roq-client
                                               roq-logging::roq-logging absl::flags fmt::fmt)

target_compile_features("${TARGET_NAME}" PUBLIC cxx_std_20)

target_compile_definitions("${TARGET_NAME}" PRIVATE ROQ_PACKAGE_NAME="${PROJECT_NAME}")

install(TARGETS ${TARGET_NAME})
