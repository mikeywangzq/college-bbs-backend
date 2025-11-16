# 工具程序编译配置

# 密码生成工具
add_executable(generate_password
    generate_password.cpp
    ../utils/PasswordUtil.cc
)

# 链接OpenSSL库
find_package(OpenSSL REQUIRED)
target_link_libraries(generate_password PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# 设置输出目录
set_target_properties(generate_password PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)

message(STATUS "Password generation tool will be built at: ${CMAKE_BINARY_DIR}/tools/generate_password")
