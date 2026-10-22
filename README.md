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

# DELIVERY;adres;waga_kg;NORMAL|URGENT;dzien_przyjecia;termin
DELIVERY;ul. Długa 5, Kraków;12.5;URGENT;1;2
```

Na razie program obsługuje tylko `DRIVER`. `DELIVERY` dochodzi w etapie 2 — pola i walidacja: [Logika biznesowa → Rekordy w pliku wejściowym](#rekordy-w-pliku-wejściowym).

Błędna linia (zły typ, brak pola, liczba, której nie da się odczytać) przerywa program z komunikatem zawierającym numer linii.

## Plik wyjściowy

Pliki w `output/` (ignorowane przez git). Na razie raport zawiera tylko listę kierowców posortowaną wg ładowności. Docelowo:

- `output/result.txt` — kierowcy, dla każdego dnia i kierowcy lista adresów, paczki pozostałe w magazynie i porównanie strategii A/B/C,
- `output/metrics.csv` — metryki dzień po dniu do wykresów.

Szczegóły: [Logika biznesowa → Metryki i wyniki](#metryki-i-wyniki).

## Logika biznesowa

> Stan docelowy — opis tego, jak program ma działać po etapach 1–4 z `TODO.md`. Wagi score i część decyzji to propozycje do dopracowania w trakcie.

### Założenia

| Temat | Decyzja | Dlaczego |
|-------|---------|----------|
| Jednostka czasu | **Dni**, numerowane od 1 | Aging, terminy i symulacja liczą się w całych dniach — wystarczy `int`. |
| Co trzymają kolejki | **`id` paczki (`int`)**, nie kopie `Delivery` | Jedyna kopia danych jest w `Warehouse`, więc zmiana statusu nie rozjedzie się z kopią w kolejce. `id` to 1, 2, 3, … bez usuwania, więc wyszukanie to `deliveries_[id - 1]`. |
| Kolejka kierowcy | **Jedna `std::queue<int>`** | Strategia ustala kolejność przy przydziale, więc kierowca dostaje gotową kolejkę. Kurs trwa jeden dzień, więc kolejność w kolejce nie zmienia dnia doręczenia. |
| Paczki, które się nie zmieściły | **Status `POSTPONED`**, zostają w `Warehouse` | Dane są już w RAM (`std::vector`), więc nie potrzeba osobnej struktury. Następnego dnia idą przed paczkami `WAITING`. |
| Kolejka priorytetowa | **Budowana od nowa każdego dnia** z paczek `WAITING` i `POSTPONED` | Score rośnie z czasem oczekiwania, a `std::priority_queue` nie przestawia elementów, które już w niej są. Kolejka trzymana przez kilka dni miałaby złą kolejność. |
| Kurs kierowcy | **Jeden dzień**: rano wyjazd, doręczenie, następnego dnia powrót | Prosta symulacja; dłuższe kursy można dodać później. |

### Model

```
enums::Priority        NORMAL, URGENT
enums::DeliveryStatus  WAITING, POSTPONED, ASSIGNED, IN_TRANSIT, DELIVERED
enums::QueueStatus     OPEN, DEPARTED

model::Driver       id, name, capacityKg, isAvailable
model::Delivery     id, address, weightKg, priority, arrivalDay, deadlineDay,
                    status = WAITING, driverId = 0, deliveredDay = 0
model::DriverQueue  driverId, std::queue<int> deliveries, status = OPEN, loadKg = 0
```

Gotowe: enumy oraz `Driver`, `Delivery` i `DriverQueue`.

`storage::Warehouse` trzyma `std::vector` kierowców i paczek oraz wyszukuje je po `id`. Da się go skopiować, więc każda strategia (A, B, C) działa na własnej kopii tych samych danych.

### Rekordy w pliku wejściowym

```
DRIVER;Jan Kowalski;500;1
DELIVERY;ul. Długa 5, Kraków;12.5;URGENT;1;2
```

| Rekord | Pola | Walidacja (błąd z numerem linii) |
|--------|------|----------------------------------|
| `DRIVER` | imię i nazwisko; ładowność_kg; dostępny | już zrobione |
| `DELIVERY` | adres; waga_kg; `NORMAL`\|`URGENT`; dzień_przyjęcia; termin | adres niepusty, waga > 0, `priorityFromString`, dzień ≥ 1, termin ≥ dzień przyjęcia |

`id` paczki to jej numer wśród linii `DELIVERY` (1, 2, 3, …), tak jak u kierowców. Cały plik jest wczytywany na starcie, ale paczka z `dzień_przyjęcia = 3` pojawia się dla algorytmu dopiero w dniu 3.

### Cykl życia paczki

```
          NEW_DELIVERY          assign()            DEPART             DELIVERED
(plik) ───────────────► WAITING ────────► ASSIGNED ───────► IN_TRANSIT ─────────► DELIVERED
                           │                  ▲
                  nie      │                  │ assign()
               zmieściła   ▼                  │ następnego dnia
                  się    POSTPONED ───────────┘
                           ▲  │
                           └──┘ znowu się nie zmieściła
```

| Z | Zdarzenie | Do | Skutki |
|---|-----------|----|--------|
| — | nadszedł dzień przyjęcia | `WAITING` | trafia do dzisiejszej kolejki priorytetowej |
| `WAITING` / `POSTPONED` | `assign()` znalazł kierowcę | `ASSIGNED` | ustawiony `driverId`, `id` w kolejce kierowcy, `loadKg += waga` |
| `WAITING` / `POSTPONED` | nikt jej nie weźmie | `POSTPONED` | zostaje w `Warehouse`; jutro idzie przed paczkami `WAITING` |
| `ASSIGNED` | `DEPART` | `IN_TRANSIT` | — |
| `IN_TRANSIT` | `DELIVERED` | `DELIVERED` | `deliveredDay = dziś` |

Każda paczka jest próbowana już w dniu przyjęcia, więc na początku przydziału `WAITING` = przyjęta dziś, a `POSTPONED` = czeka od wcześniejszego dnia.

### Score i strategie

```
score = punktyPriorytet + punktyAging + punktyTermin

punktyPriorytet = URGENT ? 100 : 0
punktyAging     = 10 * (dziś - dzieńPrzyjęcia)
punktyTermin    = max(0, 60 - 20 * (termin - dziś))
```

| Dni do terminu | ≥ 3 | 2 | 1 | 0 (dziś) | −1 (spóźniona) | −2 |
|----------------|-----|---|---|----------|----------------|----|
| punktyTermin | 0 | 20 | 40 | 60 | 80 | 100 |

Wagi są stałymi na górze `assignment/Score.h`. Paczka `NORMAL` czekająca 10 dni dogania świeżą `URGENT`, więc żadna paczka nie czeka w nieskończoność.

Przykład dla dnia 3:

| Paczka | Priorytet | Przyjęta | Termin | Score |
|--------|-----------|----------|--------|-------|
| P1 | NORMAL | 1 | 3 | 0 + 20 + 60 = **80** |
| P2 | URGENT | 3 | 5 | 100 + 0 + 20 = **120** |
| P3 | NORMAL | 2 | 9 | 0 + 10 + 0 = **10** |

Kolejność wg samego score: P2, P1, P3. Ale P1 i P3 czekają od wcześniejszych dni (`POSTPONED`), a P2 przyszła dziś (`WAITING`), więc ostatecznie: **P1, P3, P2**.

Strategie to funktory w `assignment/Strategies.h`. `std::priority_queue` trzyma na górze „największy” element, więc `operator()(a, b)` zwraca `true`, gdy **a ma być niżej niż b**.

Każda strategia porównuje paczki w tej samej kolejności kluczy:

1. **`POSTPONED` przed `WAITING`** — wspólne dla wszystkich strategii,
2. klucz strategii (tabela niżej),
3. remis: wcześniejszy `arrivalDay`, potem mniejsze `id` — wynik jest powtarzalny.

| Strategia | Klucz |
|-----------|-------|
| A — `FifoOrder` | wcześniejszy `arrivalDay` |
| B — `PriorityOrder` | `URGENT` przed `NORMAL` |
| C — `ScoreOrder` | wyższy score w danym dniu |

### Przydział — `assign()`

Zachłanny algorytm, wspólny dla wszystkich strategii (szablon na typ strategii):

```
1. Wszystkie paczki WAITING i POSTPONED z arrivalDay <= dziś → std::priority_queue<int, std::vector<int>, Strategia>.
2. Kandydaci: kierowcy dostępni, z kolejką OPEN.
3. Dopóki kolejka niepusta:
     id = top(); pop()
     kierowca = best fit: spośród kandydatów, u których loadKg + waga <= capacityKg,
                ten, któremu zostanie najmniej wolnego miejsca (remis: mniejsze id)
     znaleziony → ASSIGNED, driverId, push do deliveries, loadKg += waga
     brak       → POSTPONED (zostaje w Warehouse do jutra)
```

Paczki trafiają do `deliveries` w kolejności zdejmowania z kolejki priorytetowej, więc kierowca dostaje gotową listę ułożoną wg strategii.

- **Bez blokowania** — paczka, która się nie mieści, nie zatrzymuje pętli; lżejsze paczki niżej nadal mogą zostać przydzielone.
- **Best fit** wypełnia auta lepiej niż „pierwszy kierowca z listy”. `DriverCapacityGreater` zostaje do sortowania raportu.

### Zdarzenia i cykl dnia

| Zdarzenie | Kiedy | Co się dzieje |
|-----------|-------|---------------|
| `DRIVER_AVAILABLE` | początek dnia, kierowcy wracający z wczoraj | kolejka znów `OPEN`, `loadKg = 0`. Kierowca z `dostępny = 0` w pliku jest niedostępny przez całą symulację |
| `NEW_DELIVERY` | początek dnia, paczki z `arrivalDay == dziś` | stają się widoczne dla `assign()` |
| `DEPART` | po `assign()` | każda niepusta kolejka → `DEPARTED`, jej paczki → `IN_TRANSIT` |
| `DELIVERED` | koniec dnia | opróżnienie `deliveries` od początku, paczki → `DELIVERED`. Ta kolejność to lista adresów kierowcy w raporcie |

```
dla dziś = 1, 2, ...:
    1. DRIVER_AVAILABLE
    2. NEW_DELIVERY
    3. assign(dziś, strategia)   (nieprzydzielone → POSTPONED)
    4. DEPART
    5. DELIVERED (+ zapis list adresów do raportu)
    6. metryki dnia
    koniec: nie ma paczek WAITING/POSTPONED/ASSIGNED i nic więcej nie przyjdzie,
            albo minęło ostatniDzieńPrzyjęcia + 30 dni (ochrona przed paczkami, które nigdy się nie zmieszczą)
```

`main` uruchamia tę pętlę trzy razy — po jednej kopii `Warehouse` na strategię.

### Przypadki brzegowe

| Plik | Sytuacja | Oczekiwany wynik |
|------|----------|------------------|
| `data/edge-heavy.txt` | paczka cięższa niż ładowność każdego kierowcy | zostaje `POSTPONED` do limitu dni, w raporcie jako „nie mieści się u żadnego kierowcy”; ostrzeżenie po wczytaniu |
| `data/edge-no-drivers.txt` | wszyscy kierowcy `dostępny = 0` | nic nie zostaje przydzielone, koniec po limicie dni |
| `data/edge-full.txt` | paczka `URGENT`, gdy wszyscy kierowcy są pełni | `POSTPONED`; następnego dnia idzie przed wszystkimi paczkami `WAITING` |
| `data/edge-all-urgent.txt` | same paczki `URGENT` | priorytet ich nie rozróżnia — decyduje `POSTPONED`, aging i termin; strategia B działa jak A |

### Metryki i wyniki

| Metryka | Definicja |
|---------|-----------|
| Doręczone | liczba paczek `DELIVERED` |
| Średni czas oczekiwania | średnia `deliveredDay - arrivalDay`, osobno dla `URGENT` |
| Spóźnione | doręczone z `deliveredDay > deadlineDay` + czekające po terminie |
| Wykorzystanie ładowności | suma `loadKg` wyjeżdżających ÷ suma ich `capacityKg` |
| Odłożone | paczki, które choć raz dostały `POSTPONED` |
| Zostały w magazynie | paczki `WAITING` / `POSTPONED` na końcu symulacji |

- `output/result.txt` — kierowcy, dla każdego dnia i kierowcy lista adresów (w kolejności ze strategii), paczki pozostałe w magazynie, tabela porównania A/B/C.
- `output/metrics.csv` — `strategy,day,waiting,postponed,delivered,late,utilization` do wykresów.
- `kolejka --generate data/random.txt [ziarno]` — losowy plik wejściowy (`std::mt19937` ze stałym ziarnem, więc wyniki da się powtórzyć).

## Wykorzystanie STL

Projekt korzysta z dedykowanych kontenerów i algorytmów STL zamiast własnych struktur:

| Element STL | Gdzie i do czego |
|-------------|------------------|
| `std::vector` | `storage::Warehouse` — kierowcy i paczki w pamięci (także `POSTPONED`, czekające na kolejny dzień) |
| `std::priority_queue` | Paczki czekające w magazynie (`id`), budowana od nowa każdego dnia — na górze paczka najważniejsza wg wybranej strategii |
| `std::queue` | Kolejka paczek kierowcy (FIFO) — gotowa lista w kolejności ze strategii |
| `std::sort` | Kierowcy wg ładowności w pliku wyjściowym |
| Własne komparatory / funktory | `assignment/Comparators.h` — `DriverCapacityGreater` dla `std::sort`; `assignment/Strategies.h` — `FifoOrder`, `PriorityOrder`, `ScoreOrder` dla `std::priority_queue` |

## Struktura folderów

```
app/
├── CMakeLists.txt          konfiguracja budowania (nie trzeba ruszać przy dodawaniu plików)
├── TODO.md                 plan prac z podziałem na etapy
├── src/
│   ├── main.cpp            punkt wejścia — spina moduły razem
│   ├── enums/              typy wyliczeniowe i ich zamiana na tekst
│   ├── model/              klasy z danymi
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
| `src/enums/` | Enumy `Priority` / `DeliveryStatus` / `QueueStatus` i ich zamiana na tekst (`toString`, `priorityFromString`). | 1 | — |
| `src/model/` | `Driver`, `DriverQueue`, `Delivery`. Same dane, bez logiki. | 1 | enums |
| `src/storage/` | `Warehouse` — listy kierowców i paczek w pamięci (`std::vector`), nadawanie kolejnych `id`. | 2 | model |
| `src/io/` | `InputFile` — wczytanie pliku wejściowego do `Warehouse`. `OutputFile` — zapis raportu do pliku wyjściowego. | 2 | model, storage |
| `src/assignment/` | Liczenie score (priorytet + aging + deadline), komparatory/funktory, `std::priority_queue` paczek, kolejki kierowców (`std::queue`), zachłanny `assign()`, zdarzenia `NEW_DELIVERY`, `DRIVER_AVAILABLE`, `DEPART`, `DELIVERED`, odkładanie paczek (`POSTPONED`). | 3 | storage |
| `src/simulation/` | Symulacja kilku dni, strategie A (FIFO) / B (priorytet) / C (pełny score), generator losowych plików wejściowych, metryki zapisywane do `output/`. | 4 | assignment, io |

Kierunek zależności idzie **w dół tabeli**: `enums` nie zależy od niczego, `model` nie wie nic o magazynie, `storage` nie wie nic o plikach itd. Dzięki temu moduły można robić równolegle, jeśli wcześniej uzgodnimy interfejsy (nagłówki `.h`).

## Konwencje

- Każdy nowy plik `.cpp` w `src/` (także w podfolderach) jest dołączany do builda **automatycznie** — nie trzeba edytować `CMakeLists.txt`.
- Klasy piszemy w całości w plikach `.h` (metody w ciele klasy), na górze `#pragma once`. Plik `.cpp` dodajemy tylko wtedy, gdy naprawdę jest potrzebny. Funkcje poza klasą w `.h` oznaczamy `inline`.
- Nagłówki dołączamy ścieżką od `src/`, np. `#include "model/Delivery.h"`.
- Przestrzeń nazw = nazwa folderu (`enums::`, `model::`, `storage::`, `io::`, `assignment::`, `simulation::`).
- Pliki `.gitkeep` trzymają puste foldery w repozytorium — można je usunąć, gdy w folderze pojawi się kod.

## Praca w zespole

1. `main` zawiera tylko działający szkielet — nikt nie pushuje bezpośrednio.
2. Każdy moduł w osobnej gałęzi, np. `feature/model`, `feature/storage`, `feature/io`, `feature/assignment`, `feature/simulation`.
3. Przed startem modułu ustalamy jego interfejs (pliki `.h`), żeby zależne moduły mogły ruszyć równolegle.
4. Merge do `main` przez pull request, po tym jak projekt się buduje.
