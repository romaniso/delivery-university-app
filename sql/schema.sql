-- Schemat bazy SQLite (etap 2).
--
-- Wskazowki:
--   * CREATE TABLE IF NOT EXISTS - skrypt moze byc wykonywany przy kazdym starcie.
--   * SQLite nie ma typow enum/date: TEXT + CHECK, daty w ISO 8601, flagi jako INTEGER 0/1.
--   * Klucze obce sa domyslnie WYLACZONE - PRAGMA foreign_keys = ON przy kazdym polaczeniu.

-- TODO: tabele Driver, DriverQueue, Delivery
