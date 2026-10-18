# Kolejka Paczek

Prototyp systemu, który rozładowuje kolejkę paczek czekających w magazynie: ustala ich ważność (priorytet, czas oczekiwania, termin) i rozdziela je między dostępnych kierowców w granicach ich ładowności. Tras nie wyznaczamy — kierowca dostaje listę adresów.

Stos: **C++17 + CMake + STL**, bez bazy danych. Program wczytuje dane z **pliku wejściowego**, przetwarza je w pamięci (RAM) i zapisuje wynik do **pliku wyjściowego**.

> To jest **szkielet projektu**. Każdy moduł robimy w osobnej gałęzi.

## Wymagania

| System  | Co zainstalować |
|---------|-----------------|
| Windows | [CMake](https://cmake.org/download/) oraz kompilator: **Visual Studio Build Tools** („Programowanie aplikacji klasycznych w C++”) albo MinGW z MSYS2 |
| macOS   | `xcode-select --install` oraz `brew install cmake` |
| Linux   | `sudo apt install build-essential cmake` (lub odpowiednik) |

W VS Code: rozszerzenia **C/C++** i **CMake Tools** — VS Code zaproponuje je sam po otwarciu folderu.

## Uruchomienie

**VS Code:**

1. `File → Open Folder…` → folder projektu.
2. Wybierz kit (kompilator), np. na macOS **Clang … arm64-apple-darwin**.
3. `F7` — build.
4. `Shift+F5` — uruchomienie (wynik w `output/result.txt`).

Błąd typu „does not match the generator used previously”: `Cmd/Ctrl+Shift+P` → **CMake: Delete Cache and Reconfigure**, potem znowu `F7`.

**Terminal:**

```bash
cmake -S . -B build
cmake --build build
./build/kolejka [plik_wejsciowy] [plik_wyjsciowy]     # Windows + MSVC: build\Debug\kolejka.exe
```

Oba argumenty są opcjonalne. Domyślnie program czyta `data/input.txt` i zapisuje `output/result.txt` (ścieżki względem katalogu projektu).

Oczekiwany wynik w konsoli:

```
Kolejka Paczek
Wczytano z: .../data/input.txt
Kierowcy: 6
Zapisano wynik do: .../output/result.txt
```

## Plik wejściowy

Zwykły plik tekstowy, jeden rekord w linii, pola oddzielone średnikiem `;`. Pierwsze pole to typ rekordu. Puste linie i linie zaczynające się od `#` są pomijane.

```
# DRIVER;imie i nazwisko;ladownosc_kg;dostepny (1/0)
DRIVER;Jan Kowalski;500;1
DRIVER;Piotr Wisniewski;800;0

# DELIVERY;... (format ustalimy razem z klasa Delivery)
```

Błędna linia (zły typ, brak pola, liczba, której nie da się odczytać) przerywa program z komunikatem zawierającym numer linii.

## Plik wyjściowy

Raport tekstowy w `output/` (ignorowany przez git): lista kierowców posortowana wg ładowności, a docelowo — przydział paczek do kierowców i paczki, które zostały w magazynie.

## Wykorzystanie STL

Projekt korzysta z dedykowanych kontenerów i algorytmów STL zamiast własnych struktur:

| Element STL | Gdzie i do czego |
|-------------|------------------|
| `std::vector` | `storage::Warehouse` — listy kierowców i paczek w pamięci |
| `std::priority_queue` | Paczki czekające w magazynie — na górze zawsze paczka o najwyższym score |
| `std::queue` | Kolejka paczek kierowcy (FIFO). Paczki `URGENT` trafiają do osobnej kolejki, obsługiwanej przed zwykłymi |
| `std::sort` | Kierowcy wg ładowności przed przydziałem, sortowanie list w pliku wyjściowym |
| Własne komparatory / funktory | `assignment/Comparators.h` — np. `DriverCapacityGreater` dla `std::sort`, komparator score paczek dla `std::priority_queue` |

## Struktura folderów

```
app/
├── CMakeLists.txt          konfiguracja budowania (nie trzeba ruszać przy dodawaniu plików)
├── TODO.md                 plan prac z podziałem na etapy
├── src/
│   ├── main.cpp            punkt wejścia — spina moduły razem
│   ├── model/              klasy z danymi + typy wyliczeniowe
│   ├── storage/            magazyn danych w pamięci
│   ├── io/                 odczyt pliku wejściowego i zapis pliku wyjściowego
│   ├── assignment/         score, komparatory, algorytm przydziału assign(), obsługa zdarzeń
│   └── simulation/         symulacja dni pracy, strategie, metryki
├── data/                   pliki wejściowe (input.txt)
├── output/                 pliki wyjściowe (ignorowane przez git)
└── .vscode/                wspólne ustawienia edytora
```

### Co gdzie trafia

| Folder | Odpowiedzialność | Etap | Zależy od |
|--------|------------------|------|-----------|
| `src/model/` | `Driver`, `DriverQueue`, `Delivery`, enumy `Priority` / `DeliveryStatus` / `QueueStatus` i ich zamiana na tekst. Same dane, bez logiki. | 1 | — |
| `src/storage/` | `Warehouse` — listy kierowców i paczek w pamięci (`std::vector`), nadawanie kolejnych `id`. | 2 | model |
| `src/io/` | `InputFile` — wczytanie pliku wejściowego do `Warehouse`. `OutputFile` — zapis raportu do pliku wyjściowego. | 2 | model, storage |
| `src/assignment/` | Liczenie score (priorytet + aging + deadline), komparatory/funktory, `std::priority_queue` paczek, kolejki kierowców (`std::queue`), zachłanny `assign()`, zdarzenia `NEW_DELIVERY`, `DRIVER_AVAILABLE`, `DELIVERY_CANCELLED`, `DEPART`, `DELIVERED`. | 3 | storage |
| `src/simulation/` | Symulacja kilku dni, strategie A (FIFO) / B (priorytet) / C (pełny score), generator losowych plików wejściowych, metryki zapisywane do `output/`. | 4 | assignment, io |

Kierunek zależności idzie **w dół tabeli**: `model` nie wie nic o magazynie, `storage` nie wie nic o plikach itd. Dzięki temu moduły można robić równolegle, jeśli wcześniej uzgodnimy interfejsy (nagłówki `.h`).

## Konwencje

- Każdy nowy plik `.cpp` w `src/` (także w podfolderach) jest dołączany do builda **automatycznie** — nie trzeba edytować `CMakeLists.txt`.
- Klasy piszemy w całości w plikach `.h` (metody w ciele klasy), na górze `#pragma once`. Plik `.cpp` dodajemy tylko wtedy, gdy naprawdę jest potrzebny. Funkcje poza klasą w `.h` oznaczamy `inline`.
- Nagłówki dołączamy ścieżką od `src/`, np. `#include "model/Delivery.h"`.
- Przestrzeń nazw = nazwa folderu (`model::`, `storage::`, `io::`, `assignment::`, `simulation::`).
- Pliki `.gitkeep` trzymają puste foldery w repozytorium — można je usunąć, gdy w folderze pojawi się kod.

## Praca w zespole

1. `main` zawiera tylko działający szkielet — nikt nie pushuje bezpośrednio.
2. Każdy moduł w osobnej gałęzi, np. `feature/model`, `feature/storage`, `feature/io`, `feature/assignment`, `feature/simulation`.
3. Przed startem modułu ustalamy jego interfejs (pliki `.h`), żeby zależne moduły mogły ruszyć równolegle.
4. Merge do `main` przez pull request, po tym jak projekt się buduje.
