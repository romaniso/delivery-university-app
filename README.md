# Kolejka Paczek

Prototyp systemu, który rozładowuje kolejkę paczek czekających w magazynie: ustala ich ważność (priorytet, czas oczekiwania, termin) i rozdziela je między dostępnych kierowców w granicach ich ładowności. Tras nie wyznaczamy — kierowca dostaje listę adresów.

Stos: **C++17 + CMake + SQLite** (SQLite wkompilowane w projekt — nic nie trzeba instalować osobno).

> To jest **szkielet projektu**. Nie ma tu jeszcze żadnej implementacji — tylko konfiguracja budowania i struktura folderów. Każdy moduł robimy w osobnej gałęzi.

## Wymagania

| System  | Co zainstalować |
|---------|-----------------|
| Windows | [CMake](https://cmake.org/download/) oraz kompilator: **Visual Studio Build Tools** („Programowanie aplikacji klasycznych w C++”) albo MinGW z MSYS2 |
| macOS   | `xcode-select --install` oraz `brew install cmake` |
| Linux   | `sudo apt install build-essential cmake` (lub odpowiednik) |

W VS Code: rozszerzenia **C/C++** i **CMake Tools** — VS Code zaproponuje je sam po otwarciu folderu.

## Uruchomienie

**VS Code:** `File → Open Folder…` → wybierz kit (kompilator) → **Build** (`F7`) → ▶.

**Terminal:**

```bash
cmake -S . -B build
cmake --build build
./build/kolejka            # Windows + MSVC: build\Debug\kolejka.exe
```

Pierwszy build trwa dłużej (kompiluje się SQLite). Oczekiwany wynik:

```
Kolejka Paczek - setup OK
SQLite 3.x.x
Katalog projektu: .../kolejka-paczek
```

## Struktura folderów

```
app/
├── CMakeLists.txt          konfiguracja budowania (nie trzeba ruszać przy dodawaniu plików)
├── src/
│   ├── main.cpp            punkt wejścia — spina moduły razem
│   ├── model/              struktury danych odpowiadające tabelom + typy wyliczeniowe
│   ├── db/                 dostęp do SQLite: połączenie, zapytania, transakcje
│   ├── structures/         własne struktury: kopiec (kolejka priorytetowa) i lista wiązana
│   ├── assignment/         score, algorytm przydziału assign(), obsługa zdarzeń
│   └── simulation/         generator danych, symulacja dni pracy, strategie, metryki
├── sql/
│   ├── schema.sql          schemat bazy
│   └── seed.sql            dane testowe
├── data/                   plik bazy .db (tworzony w runtime, ignorowany przez git)
├── output/                 wyniki symulacji, CSV do wykresów (ignorowane przez git)
├── third_party/sqlite/     źródła SQLite — nie edytować
└── .vscode/                wspólne ustawienia edytora
```

### Co gdzie trafia

| Folder | Odpowiedzialność | Etap | Zależy od |
|--------|------------------|------|-----------|
| `src/model/` | `Driver`, `DriverQueue`, `Delivery`, enumy `Priority` / `DeliveryStatus` / `QueueStatus` i ich zamiana na tekst w bazie. Same dane, bez logiki. | 1 | — |
| `sql/` | `schema.sql` (tabele, `CHECK`, klucze obce, indeksy) i `seed.sql` (kierowcy + ~30 paczek). | 2 | model |
| `src/db/` | Otwarcie bazy (`PRAGMA foreign_keys = ON`!), wykonanie skryptów SQL, zapytania, transakcje, mapowanie wierszy na obiekty modelu. | 2 | model, sql |
| `src/structures/` | Kopiec binarny (`push`, `pop`, `peek`, `size`, `isEmpty`) i kolejka kierowcy jako lista wiązana (`enqueue`, `insertUrgent`, `dequeue`, `peek`, `toList`, `depart`). Kopiec żyje w RAM, lista wiązana jest trwała w bazie (`head` / `tail` / `next`). | 3 | model, db |
| `src/assignment/` | Liczenie score (priorytet + aging + deadline), zachłanny `assign()`, zdarzenia `NEW_DELIVERY`, `DRIVER_AVAILABLE`, `DELIVERY_CANCELLED`, `DEPART`, `DELIVERED`. | 4 | structures, db |
| `src/simulation/` | Generator losowych paczek i kierowców, symulacja kilku dni, strategie A (FIFO) / B (priorytet) / C (pełny score), metryki zapisywane do `output/`. | 5 | assignment |

Kierunek zależności idzie **w dół tabeli**: `model` nie wie nic o bazie, `structures` nie wie nic o algorytmie itd. Dzięki temu moduły można robić równolegle, jeśli wcześniej uzgodnimy interfejsy (nagłówki `.h`).

## Konwencje

- Każdy nowy plik `.cpp` w `src/` (także w podfolderach) jest dołączany do builda **automatycznie** — nie trzeba edytować `CMakeLists.txt`.
- Klasy piszemy w całości w plikach `.h` (metody w ciele klasy), na górze `#pragma once`. Plik `.cpp` dodajemy tylko wtedy, gdy naprawdę jest potrzebny.
- Nagłówki dołączamy ścieżką od `src/`, np. `#include "model/Delivery.h"`.
- Przestrzeń nazw = nazwa folderu (`model::`, `db::`, `structures::`, `assignment::`, `simulation::`).
- Pliki `.gitkeep` trzymają puste foldery w repozytorium — można je usunąć, gdy w folderze pojawi się kod.

## Praca w zespole

1. `main` zawiera tylko działający szkielet — nikt nie pushuje bezpośrednio.
2. Każdy moduł w osobnej gałęzi, np. `feature/model`, `feature/db`, `feature/structures`, `feature/assignment`, `feature/simulation`.
3. Przed startem modułu ustalamy jego interfejs (pliki `.h`), żeby zależne moduły mogły ruszyć równolegle.
4. Merge do `main` przez pull request, po tym jak projekt się buduje.

## Uwagi

- Żeby zacząć z czystą bazą, usuń plik z `data/` — zostanie utworzony na nowo.
- Podgląd bazy: [DB Browser for SQLite](https://sqlitebrowser.org/) albo rozszerzenie VS Code „SQLite Viewer”.
