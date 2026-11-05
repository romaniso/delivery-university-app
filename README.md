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
4. `Shift+F5` — uruchomienie; program pyta o plik wejściowy, strategię i plik wyjściowy (domyślnie wynik w `output/result.txt`).

Błąd typu „does not match the generator used previously”: `Cmd/Ctrl+Shift+P` → **CMake: Delete Cache and Reconfigure**, potem znowu `F7`.

**Terminal:**

```bash
cmake -S . -B build
cmake --build build
./build/kolejka [-s A|B|C] [plik_wejsciowy] [plik_wyjsciowy]     # Windows + MSVC: build\Debug\kolejka.exe
```

Wszystkie argumenty są opcjonalne. O wszystko, czego nie podano w argumentach, program pyta po uruchomieniu — kolejno o plik wejściowy, strategię i plik wyjściowy:

```
Wybierz plik wejsciowy (.../data):
  1) edge-all-urgent.txt
  ...
  8) input.txt
Wybor [numer, nazwa lub sciezka, Enter = input.txt]:
Wybierz strategie:
  1) A - FIFO
  2) B - priorytet
  3) C - pelny score
Wybor [1-3, Enter = C - pelny score]:
Plik wyjsciowy [nazwa w .../output lub sciezka, Enter = result.txt]:
```

- **Plik wejściowy** — lista plików `.txt` z `data/`, czytana przy każdym uruchomieniu (nowy plik w `data/` pojawi się sam). Można wpisać numer z listy, nazwę pliku z `data/` albo dowolną ścieżkę; `.txt` można pominąć (`edge-full` = `edge-full.txt`). Nieistniejący plik — program pyta ponownie.
- **Strategia** — numer, litera (`A`/`B`/`C`) lub nazwa (`FIFO`/`PRIORITY`/`SCORE`).
- **Plik wyjściowy** — sama nazwa trafia do `output/`, ścieżka z katalogiem jest używana bez zmian. Nazwa bez rozszerzenia dostaje `.txt` (`wynik` → `output/wynik.txt`).

Enter wybiera wartość domyślną (`data/input.txt`, C, `output/result.txt`); tak samo, gdy program działa bez terminala (brak danych na wejściu). Żeby pominąć pytania (np. w skryptach), podaj wszystko w argumentach: `./build/kolejka -s A data/edge-heavy.txt output/heavy.txt`.

Oczekiwany wynik w konsoli:

```
Kolejka Paczek
Wczytano z: .../data/input.txt
Kierowcy: 6
Paczki: 30
Strategia: C - pelny score
Dni symulacji: 5
Doreczone: 30 / 30
Zapisano wynik do: .../output/result.txt
```

## Zadanie 5 — uruchomienie eksperymentów i statystyk

Skrypt [experiments/run_experiments.ps1](./experiments/run_experiments.ps1) uruchamia trzy scenariusze: małe obciążenie, przeciążenie systemu i skokowy napływ zgłoszeń.

1. Zbuduj aktualną wersję programu w VS Code przez **CMake: Build** (`F7`).
2. Otwórz terminal PowerShell w katalogu głównym projektu.
3. Uruchom skrypt, przekazując ścieżkę do zbudowanego programu:

```powershell
.\experiments\run_experiments.ps1 -Executable .\build\Debug\kolejka.exe
```

Na macOS/Linux (bez instalowania PowerShella) użyj odpowiednika w bashu — działa tak samo i zapisuje te same pliki:

```bash
./experiments/run_experiments.sh ./build/kolejka
```

Jeśli plik wykonywalny znajduje się w innym katalogu (np. `build/Release` lub `build`), odpowiednio zmień argument `-Executable`. Wymagany jest PowerShell oraz program z aktualnym modułem statystyk; skrypt nie kompiluje aplikacji.

Skrypt tworzy katalog `output/` i zapisuje:

- `output/experiments.csv` — zbiorczą tabelę wyników trzech scenariuszy;
- `output/result-low_load.txt`, `output/result-overload.txt`, `output/result-arrival_spike.txt` — szczegółowe raporty symulacji;
- `output/metrics.csv` — metryki ostatnio wykonanego scenariusza (nadpisywane przy każdym uruchomieniu programu).

Zbiorcza tabela zawiera kolumny:

```text
scenario,strategy,total_deliveries,delivered,on_time,late,remaining,average_waiting_days,max_waiting_days,executor_utilization_percent
```

Czas oczekiwania to `deliveredDay - arrivalDay`, liczony dla dostarczonych paczek (doręczenie w dniu przyjęcia oznacza 0 dni). `on_time` oznacza doręczenia najpóźniej w terminie, `late` — doręczenia po terminie, a `remaining` — zgłoszenia niedostarczone. Wykorzystanie wykonawców to procent wykorzystanej masy względem łącznej ładowności dostępnych kierowców przez wszystkie dni symulacji.

**Zakres porównania:** skrypt uruchamia trzy scenariusze dla jednej strategii — domyślnie `SCORE` (C). Inną strategię podaj jako argument, bez przebudowywania programu:

```powershell
.\experiments\run_experiments.ps1 -Executable .\build\Debug\kolejka.exe -Strategy A
```

```bash
./experiments/run_experiments.sh ./build/kolejka A
```

Przed kolejnym uruchomieniem zachowaj kopię wyników (np. zmień nazwę `experiments.csv` na `experiments-A.csv`), ponieważ pliki o tych samych nazwach zostaną nadpisane.

Wyniki CSV można wykorzystać do tabel i wykresów w sprawozdaniu. Opis metryk i scenariuszy znajduje się w [SPRAWOZDANIE_ZADANIE_5.md](./SPRAWOZDANIE_ZADANIE_5.md).

## Plik wejściowy

Zwykły plik tekstowy, jeden rekord w linii, pola oddzielone średnikiem `;`. Pierwsze pole to typ rekordu. Puste linie i linie zaczynające się od `#` są pomijane.

```
# DRIVER;imie i nazwisko;ladownosc_kg;dostepny (1/0)
DRIVER;Jan Kowalski;500;1
DRIVER;Piotr Wisniewski;800;0

# DELIVERY;adres;waga_kg;NORMAL|URGENT;dzien_przyjecia;termin
DELIVERY;ul. Długa 5, Kraków;12.5;URGENT;1;2
```

Pola i walidacja: [Logika biznesowa → Rekordy w pliku wejściowym](#rekordy-w-pliku-wejściowym).

Błędna linia (zły typ, brak pola, liczba, której nie da się odczytać, wartość spoza zakresu) przerywa program z komunikatem zawierającym numer linii, np.:

```
Blad: data/input.txt, linia 17: "urgent" nie jest priorytetem (NORMAL/URGENT)
```

## Plik wyjściowy

Pliki w `output/` (ignorowane przez git). `output/result.txt` (dla wybranej strategii) zawiera:

- nagłówek ze strategią i podsumowaniem symulacji (liczba dni, doręczone paczki),
- kierowców posortowanych wg ładowności,
- paczki ze statusem końcowym, kierowcą i dniem doręczenia,
- kursy dzień po dniu — dla każdego kierowcy obciążenie i lista adresów w kolejności doręczania,
- paczki, które zostały w magazynie.

Do zrobienia: metryki, porównanie strategii A/B/C i `output/metrics.csv` z metrykami dzień po dniu do wykresów.

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
enums::Strategy        FIFO, PRIORITY, SCORE   (strategie A, B, C)

model::Driver       id, name, capacityKg, isAvailable
model::Delivery     id, address, weightKg, priority, arrivalDay, deadlineDay,
                    status = WAITING, driverId = 0, deliveredDay = 0
model::DriverQueue  driverId, std::queue<int> deliveries, status = OPEN, loadKg = 0
model::DriverDay    driverId, day, std::vector<int> deliveryIds, loadKg
```

`DriverDay` to kurs kierowcy w jednym dniu. Zapisujemy go przy doręczeniu, bo kolejka kierowcy jest wtedy opróżniana, a raport potrzebuje kolejności doręczania.

Gotowe: enumy, wszystkie klasy modelu, `Warehouse` z kierowcami i paczkami , `assignment::DeliveryManager` (przydział i zdarzenia dnia), strategie A/B/C oraz `simulation::Simulation` (pętla po dniach).

`storage::Warehouse` trzyma `std::vector` kierowców i paczek oraz wyszukuje je po `id` (`driver(id)`, `delivery(id)` — zwracają referencję, zły `id` rzuca `std::out_of_range`). Da się go skopiować, więc każda strategia (A, B, C) działa na własnej kopii tych samych danych.

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

Wagi są stałymi na górze `assignment/DeliveryScore.h`. Paczka `NORMAL` czekająca 10 dni dogania świeżą `URGENT`, więc żadna paczka nie czeka w nieskończoność.

Przykład dla dnia 3:

| Paczka | Priorytet | Przyjęta | Termin | Score |
|--------|-----------|----------|--------|-------|
| P1 | NORMAL | 1 | 3 | 0 + 20 + 60 = **80** |
| P2 | URGENT | 3 | 5 | 100 + 0 + 20 = **120** |
| P3 | NORMAL | 2 | 9 | 0 + 10 + 0 = **10** |

Kolejność wg samego score: P2, P1, P3. Ale P1 i P3 czekają od wcześniejszych dni (`POSTPONED`), a P2 przyszła dziś (`WAITING`), więc ostatecznie: **P1, P3, P2**.

Strategie to funktory w `assignment/Comparators.h`. `std::priority_queue` trzyma na górze „największy” element, więc `operator()(a, b)` zwraca `true`, gdy **a ma być niżej niż b**.

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

Przydział i zdarzenia dnia to metody klasy `assignment::DeliveryManager` (`assignment/DeliveryManager.h`). Menedżer doręczeń trzyma kolejki kierowców (`DriverQueue`, po jednej na kierowcę) i zmienia paczki w `Warehouse`. Sam `Warehouse` tylko przechowuje dane.

`DeliveryManager::assign(dziś, strategia)` — zachłanny algorytm, wspólny dla wszystkich strategii (szablon na typ strategii):

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

| Zdarzenie | Metoda `DeliveryManager` | Kiedy | Co się dzieje |
|-----------|---------------------|-------|---------------|
| `DRIVER_AVAILABLE` | `driversAvailable()` | początek dnia, kierowcy wracający z wczoraj | kolejka znów `OPEN`, `loadKg = 0`. Kierowca z `dostępny = 0` w pliku jest niedostępny przez całą symulację |
| `NEW_DELIVERY` | — (filtr w `assign()`) | początek dnia, paczki z `arrivalDay == dziś` | są `WAITING` od wczytania pliku; `assign()` bierze tylko paczki z `arrivalDay <= dziś`, więc od dziś są widoczne |
| `DEPART` | `depart()` | po `assign()` | każda niepusta kolejka → `DEPARTED`, jej paczki → `IN_TRANSIT` |
| `DELIVERED` | `deliver(dziś)` | koniec dnia | opróżnienie `deliveries` od początku, paczki → `DELIVERED`, `deliveredDay = dziś`. Zwraca `DriverDay` z kolejnością doręczania do raportu |

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

Pętlę prowadzi `simulation::Simulation` (`simulation/Simulation.h`): jedna symulacja = jedna strategia (`enums::Strategy`), wybierana w menu po uruchomieniu albo argumentem `-s` / `--strategy` (domyślnie `DEFAULT_STRATEGY` w `main.cpp`, czyli C). `Simulation::assign` wybiera funktor dla strategii (`FifoOrder`, `PriorityOrder`, `ScoreOrder`) i przekazuje go do `DeliveryManager::assign`.

Żeby porównać strategie, uruchamiamy trzy symulacje, każdą na własnej kopii `Warehouse`:

```cpp
storage::Warehouse copy = warehouse;
simulation::Simulation simulation(copy, enums::Strategy::FIFO);
simulation.run();
```

### Przypadki brzegowe

| Plik | Sytuacja | Oczekiwany wynik |
|------|----------|------------------|
| `data/edge-heavy.txt` | paczka cięższa niż ładowność każdego kierowcy | ostrzeżenie po wczytaniu; paczka zostaje `POSTPONED` do limitu dni i trafia do „Zostały w magazynie”, reszta jest doręczana normalnie |
| `data/edge-no-drivers.txt` | wszyscy kierowcy `dostępny = 0` | ostrzeżenie po wczytaniu, żadnych kursów, koniec po limicie dni |
| `data/edge-full.txt` | paczka `URGENT`, gdy wszyscy kierowcy są pełni | `POSTPONED`; następnego dnia idzie przed wszystkimi paczkami `WAITING` |
| `data/edge-all-urgent.txt` | same paczki `URGENT` | priorytet ich nie rozróżnia — decyduje `POSTPONED`, aging i termin; strategia B działa jak A. Paczka przyjęta w dniu 2 z terminem 2 jedzie w dniu 3 (spóźniona), bo `POSTPONED` idą przed `WAITING` |

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
| Własne komparatory / funktory | `assignment/Comparators.h` — `DriverCapacityGreater` dla `std::sort` oraz strategie `FifoOrder`, `PriorityOrder`, `ScoreOrder` dla `std::priority_queue` |

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
│   ├── assignment/         score, strategie, DeliveryManager (przydział assign() i zdarzenia dnia)
│   └── simulation/         symulacja dzień po dniu dla wybranej strategii
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
| `src/assignment/` | `DeliveryScore.h` — `deliveryScore()` (priorytet + aging + deadline). `Comparators.h` — funktory dla `std::sort` i `std::priority_queue` (strategie). `DeliveryManager.h` — kolejki kierowców (`std::queue`), zachłanny `assign()` z odkładaniem paczek (`POSTPONED`), zdarzenia `DRIVER_AVAILABLE`, `NEW_DELIVERY`, `DEPART`, `DELIVERED`. | 3 | storage |
| `src/simulation/` | `Simulation` — pętla po dniach dla wybranej strategii A (FIFO) / B (priorytet) / C (pełny score), kursy kierowców do raportu. Do zrobienia: metryki zapisywane do `output/`, generator losowych plików wejściowych. | 4 | assignment |

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
