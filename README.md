# Fantasy Log Simulator

Tiny C++17 program that reads commands from `input.txt` and writes a narrated log to `output.txt`. Supports creating **characters** (fighter/archer/wizard), **items** (weapon/potion/spell), actions (**Attack**, **Cast**, **Drink**), **Dialogue**, and **Show** queries.

## Build & Run
```bash
g++ -std=cpp17 -O2 -o fantasy main.cpp
./fantasy
```

## Command Sketch

- Create character <type> <name> <hp>
- Create item <weapon|potion|spell> <ownerName> <itemName> <val|len> [victim1 ...]
- Attack <attacker> <target> <weaponName>
- Cast <caster> <target> <spellName>
- Drink <supplier> <drinker> <potionName>
- Dialogue <name|Narrator> <wordCount> <w1> ... <wN>
- Show <characters|weapons|potions|spells> [name]

## Notes: 
HP 1–200, item values 1–50 (len 0–50 for spells), capacities depend on class. Invalid input prints Error caught.

