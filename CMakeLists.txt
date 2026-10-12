cmake_minimum_required(VERSION 3.16)

project(KolejkaPaczek
    VERSION 0.1.0
    DESCRIPTION "Symulator przydzialu paczek do kierowcow"
    LANGUAGES C CXX)

# ---------------------------------------------------------------------------
# Ustawienia ogolne
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# compile_commands.json -> podpowiedzi IntelliSense w VS Code
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Domyslnie budujemy w trybie Debug (dla generatorow jednokonfiguracyjnych)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Typ builda" FORCE)
endif()

# ---------------------------------------------------------------------------
# SQLite - wkompilowane z zrodel (amalgamation), bez instalacji w systemie
# ---------------------------------------------------------------------------
add_library(sqlite3 STATIC third_party/sqlite/sqlite3.c)
target_include_directories(sqlite3 PUBLIC third_party/sqlite)
target_compile_definitions(sqlite3 PRIVATE
    SQLITE_OMIT_LOAD_EXTENSION   # nie potrzebujemy rozszerzen -> brak zaleznosci od libdl
    SQLITE_DQS=0                 # "tekst" w cudzyslowie nie jest literalem, tylko 'tekst'
)
find_package(Threads REQUIRED)
target_link_libraries(sqlite3 PUBLIC Threads::Threads)

# Kod SQLite nie jest nasz - wyciszamy jego ostrzezenia
if(MSVC)
    target_compile_options(sqlite3 PRIVATE /W0)
else()
    target_compile_options(sqlite3 PRIVATE -w)
endif()

# ---------------------------------------------------------------------------
# Aplikacja
# ---------------------------------------------------------------------------
# Kazdy nowy plik .cpp w src/ jest dolaczany automatycznie
# (CONFIGURE_DEPENDS = CMake sam wykryje nowe pliki przy kolejnym buildzie)
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(kolejka ${APP_SOURCES})
target_include_directories(kolejka PRIVATE src)
target_link_libraries(kolejka PRIVATE sqlite3)

# Sciezka do katalogu projektu - dzieki temu program znajdzie sql/ i data/
# niezaleznie od tego, z jakiego katalogu zostal uruchomiony
target_compile_definitions(kolejka PRIVATE PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")

if(MSVC)
    target_compile_options(kolejka PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(kolejka PRIVATE -Wall -Wextra -Wpedantic)
endif()
