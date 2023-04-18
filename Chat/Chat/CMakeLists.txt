# CMakeList.txt: проект CMake для Chat; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавьте источник в исполняемый файл этого проекта.
add_executable (Chat "Chat.cpp" "User.h" "User.cpp" "sha1.cpp"
  "sha1.h" "message.h" "message.cpp" "socket.h" "socket.cpp"
  "chat_server.h" "chat_server.cpp"
  "chat_client.h" "chat_client.cpp")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
