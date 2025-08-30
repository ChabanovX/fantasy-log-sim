//
// Created by IVAN CHABANOV DSAI-03 on 05/04/2024.
//
// Overview: C++ program
// that simulates a medieval fantasy story.
// Your program will create characters, weapons, and spells
// that interact within a dynamic story.
//
// The story progresses through a series of events such as attacks, spawns, and deaths,
// with each event written to an output file to narrate the unfolding adventure
//
// More overview - https://codeforces.com/group/x7EpYPdPGy/contest/514065/problem/A
//

#include "fstream"
#include "string"
#include "iostream"
#include "memory"
#include "sstream"
#include "iterator"
#include "map"
#include "set"
#include "vector"


std::ofstream outputFile("output.txt");

// Forward declarations
class PhysicalItem;
class Weapon;
class Potion;
class Spell;


// Container class template for managing collections of items
template<typename T>
class Container {
private:
    std::map<std::string, std::shared_ptr<T>> container;  // Map to store items
    int maxSize;    // Maximum size of the container
    int size;       // Current size of the container

public:
    explicit Container(int maxsize) : maxSize(maxsize), size(0) {}    // Constructor with maximum size
    explicit Container() : maxSize(0), size(0) {}    // Default constructor

    // Add an item to the container
    void addItem(std::shared_ptr<T>& item) {
        if (size < maxSize) {   // Check if container is not full
            container[item->getName()] = item;    // Add item to the container
            size += 1;  // Increment size
        }
        else {
            outputFile << "Error caught\n";    // Output error if container is full
        }
    }

    // Find an item in the container by label
    std::shared_ptr<T> find(std::string label) {
        auto it = container.find(label);    // Find item in the container
        if (it != container.end()) {
            return it->second;  // Return item if found
        }
        return nullptr; // Return null if item not found
    }

    // Delete an item from the container by label
    void deleteItem(std::string label) {
        container.erase(label); // Erase item from the container
        size -= 1;  // Decrement size
    }

    // Getters for container attributes
    std::map<std::string, std::shared_ptr<T>>& getContainer() {
        return container;   // Return the container
    }
    int getRemainingSize() {
        return maxSize - size;  // Return remaining size of the container
    }
    int getMaxSize() {
        return maxSize; // Return maximum size of the container
    }
};

// Base class for characters
class Character {
protected:
    Container<Weapon> guns;     // Container for weapons
    Container<Potion> drugs;    // Container for potions
    Container<Spell> swears;    // Container for spells

    int hp;                     // Hit points
    std::string name;           // Name of the character
    std::string type;           // Type of the character

    // Functions for managing health
    void takeDamage(int damage) {}
    void heal(int heal) {}

public:
    // Constructor
    Character(std::string name, int hp, std::string type,
              int MAX_ALLOWED_GUNS, int MAX_ALLOWED_POTIONS, int MAX_ALLOWED_SPELLS):
            guns(MAX_ALLOWED_GUNS), drugs(MAX_ALLOWED_POTIONS), swears(MAX_ALLOWED_SPELLS) {
        this->hp = hp;
        this->name = name;
        this->type = type;
    }
    virtual ~Character() = default; // Destructor
    // Getters and setters for character attributes
    int getHP() const {
        return hp;
    }
    void setHP(int hp) {
        this->hp = hp;
    }
    std::string getName() {
        return name;
    }
    std::string getType() {
        return type;
    }
    // Virtual functions for character actions
    virtual void attack(std::shared_ptr<Character> character, std::string itemName) = 0;
    virtual void potion(std::shared_ptr<Character> character, std::string itemName) = 0;
    virtual void spell(std::shared_ptr<Character> character, std::string itemName) = 0;
    virtual void assignItem(std::shared_ptr<PhysicalItem> item, std::string type) = 0;
    virtual void assignItem(std::shared_ptr<PhysicalItem> item, std::map<std::string, std::shared_ptr<Character>> victims) = 0;
    virtual Container<Weapon> getWeapons() = 0;
    virtual Container<Potion> getPotions() = 0;
    virtual Container<Spell> getSpells() = 0;
};


// Base class for physical items
class PhysicalItem {
private:
    std::shared_ptr<Character> owner;  // Owner of the item
    std::string name;   // Name of the item
    friend class Character; // Friend class declaration

public:
    // Constructor
    PhysicalItem(std::shared_ptr<Character>& owner, std::string name) : owner(owner), name(name) {}

    virtual ~PhysicalItem() {} ;    // Destructor
    // Virtual function for using the item
    virtual void use(Character *user, std::shared_ptr<Character> target) = 0;
    // Getter for item name
    std::string getName() {
        return name;    // Return the name of the item
    }
    // Getter for owner of the item
    std::shared_ptr<Character> getOwner() {
        return owner;   // Return the owner of the item
    };
};

// Derived class for weapons
class Weapon : public PhysicalItem {
private:
    int damage;
public:
    Weapon(std::shared_ptr<Character> owner, std::string name, int damage) : PhysicalItem(owner, name), damage(damage) {
    }

    void use(Character *user, std::shared_ptr<Character> target) override {
        target->setHP(target->getHP() - damage);
    }
    int getDamage() {
        return damage;
    }
};

// Derived class for potions
class Potion : public PhysicalItem {
private:
    int heal;
public:
    Potion(std::shared_ptr<Character> owner, std::string name, int heal) : PhysicalItem(owner, name), heal(heal) {
    }
    void use(Character *user, std::shared_ptr<Character> target) override {
        target->setHP(target->getHP() + heal);
    }
    int getHeal() {
        return heal;
    }
};

// Derived class for spells
class Spell : public PhysicalItem {
private:
    std::map<std::string, std::shared_ptr<Character>> victims;
public:
    Spell(std::shared_ptr<Character> owner, std::string name, std::map<std::string, std::shared_ptr<Character>> victims) :
            PhysicalItem(owner, name), victims(std::move(victims)) {};

    void use(Character *user, std::shared_ptr<Character> target) override {
        target.reset();
    }
    std::map<std::string, std::shared_ptr<Character>> getVictims() {
        return victims;
    }
};

// Derived class for fighters
class Fighter : public Character {
public:
    Fighter(std::string name, int hp) : Character(std::move(name), hp, "fighter",
                                                  MAX_ALLOWED_GUNS, MAX_ALLOWED_POTIONS, MAX_ALLOWED_SPELLS) {};

    const static int MAX_ALLOWED_GUNS = 3;
    const static int MAX_ALLOWED_POTIONS = 5;
    const static int MAX_ALLOWED_SPELLS = 0;

    void assignItem(std::shared_ptr<PhysicalItem> item, std::string type) override {
        if (type == "weapon") {
            std::shared_ptr<Weapon> weaponItem = std::dynamic_pointer_cast<Weapon>(item);
            guns.addItem(weaponItem);
        }
        if (type == "potion") {
            std::shared_ptr<Potion> potionItem = std::dynamic_pointer_cast<Potion>(item);
            drugs.addItem(potionItem);
        }
        if (type == "spell") {
            std::shared_ptr<Spell> spellItem = std::dynamic_pointer_cast<Spell>(item);
            swears.addItem(spellItem);
        }
    }

    void assignItem(std::shared_ptr<PhysicalItem> item, std::map<std::string, std::shared_ptr<Character>> victims) override {
        {}; // CANT be assigned.
    }

    void attack (std::shared_ptr<Character> character, std::string itemName) override {
        guns.find(itemName)->use(this, character);

    }
    void potion (std::shared_ptr<Character> character, std::string itemName) override{
        drugs.find(itemName)->use(this, character);
        drugs.deleteItem(itemName);
    }
    void spell (std::shared_ptr<Character> character, std::string itemName) override {
        swears.find(itemName)->use(this, character);
        swears.deleteItem(itemName);
    }

    Container<Weapon> getWeapons() override {
        return guns;
    }
    Container<Potion> getPotions() override {
        return drugs;
    }
    Container<Spell> getSpells() override {
        return swears;
    }


};

// Derived class for archers
class Archer : public Character {
public:
    Archer(std::string name, int hp) : Character(std::move(name), hp, "archer",
                                                 MAX_ALLOWED_GUNS, MAX_ALLOWED_POTIONS, MAX_ALLOWED_SPELLS) {};

    const static int MAX_ALLOWED_GUNS = 2;
    const static int MAX_ALLOWED_POTIONS = 3;
    const static int MAX_ALLOWED_SPELLS = 2;

    void assignItem(std::shared_ptr<PhysicalItem> item, std::string type) override {
        if (type == "weapon") {
            std::shared_ptr<Weapon> weaponItem = std::dynamic_pointer_cast<Weapon>(item);
            guns.addItem(weaponItem);
        }
        if (type == "potion") {
            std::shared_ptr<Potion> potionItem = std::dynamic_pointer_cast<Potion>(item);
            drugs.addItem(potionItem);
        }
        if (type == "spell") {
            std::shared_ptr<Spell> spellItem = std::dynamic_pointer_cast<Spell>(item);
            swears.addItem(spellItem);
        }
    }
    void assignItem(std::shared_ptr<PhysicalItem> item, std::map<std::string, std::shared_ptr<Character>> victims) override {
        std::shared_ptr<Spell> spell = std::dynamic_pointer_cast<Spell>(item);
        swears.addItem(spell);
    }
    void attack (std::shared_ptr<Character> character, std::string itemName) override {
        guns.find(itemName)->use(this, character);
    }
    void potion (std::shared_ptr<Character> character, std::string itemName) override{
        drugs.find(itemName)->use(this, std::move(character));
        drugs.deleteItem(itemName);
    }
    void spell (std::shared_ptr<Character> character, std::string itemName) override {
        swears.find(itemName)->use(this, std::move(character));
        swears.deleteItem(itemName);
    }
    Container<Weapon> getWeapons() override {
        return guns;
    }
    Container<Potion> getPotions() override {
        return drugs;
    }
    Container<Spell> getSpells() override {
        return swears;
    }
};

// Derived class for wizards
class Wizard : public Character {
public:
    Wizard(std::string name, int hp) : Character(std::move(name), hp, "wizard",
                                                 MAX_ALLOWED_GUNS, MAX_ALLOWED_POTIONS, MAX_ALLOWED_SPELLS) {};

    const static int MAX_ALLOWED_GUNS = 0;
    const static int MAX_ALLOWED_POTIONS = 10;
    const static int MAX_ALLOWED_SPELLS = 10;

    void assignItem(std::shared_ptr<PhysicalItem> item, std::string type) override {
        if (type == "weapon") {
            std::shared_ptr<Weapon> weaponItem = std::dynamic_pointer_cast<Weapon>(item);
            guns.addItem(weaponItem);
        }
        if (type == "potion") {
            std::shared_ptr<Potion> potionItem = std::dynamic_pointer_cast<Potion>(item);
            drugs.addItem(potionItem);
        }
        if (type == "spell") {
            std::shared_ptr<Spell> spellItem = std::dynamic_pointer_cast<Spell>(item);
            swears.addItem(spellItem);
        }
    }
    void assignItem(std::shared_ptr<PhysicalItem> item, std::map<std::string, std::shared_ptr<Character>> victims) override {
        std::shared_ptr<Spell> spell = std::dynamic_pointer_cast<Spell>(item);
        swears.addItem(spell);
    }
    void attack (std::shared_ptr<Character> character, std::string itemName) override {
        guns.find(itemName)->use(this, character);
    }
    void potion (std::shared_ptr<Character> character, std::string itemName) override{
        drugs.find(itemName)->use(this, std::move(character));
        drugs.deleteItem(itemName);
    }
    void spell (std::shared_ptr<Character> character, std::string itemName) override {
        swears.find(itemName)->use(this, std::move(character));
        swears.deleteItem(itemName);
    }
    Container<Weapon> getWeapons() override {
        return guns;
    }
    Container<Potion> getPotions() override {
        return drugs;
    }
    Container<Spell> getSpells() override {
        return swears;
    }
};


// Function to split a string based delimiter is whitespace
std::vector<std::string> split(const std::string& input) {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;

    while (std::getline(ss, token, ' ')) {
        tokens.push_back(token);
    }

    return tokens;
}

// Function to create a new character
void createCharacter(std::vector<std::string> &keywords, std::map<std::string, std::shared_ptr<Character>> &characters) {
    // Create character $[string]type $[string]name $[int]initHP - example of keywords
    std::string characterType = keywords[2];
    std::string characterName = keywords[3];
    int characterHp = std::stoi(keywords[4]);

    // HP check
    if (!(1 <= characterHp && characterHp <= 200)) {
        outputFile << "Error caught\n";
        return;
    }
    if (characterType == "fighter") {
        std::shared_ptr<Character> character = std::make_shared<Fighter>(characterName, characterHp);
        characters.insert(std::make_pair(characterName, character));

    }
    else if (characterType == "archer") {
        std::shared_ptr<Character> character = std::make_shared<Archer>(characterName, characterHp);
        characters.insert(std::make_pair(characterName, character));
    }
    else if (characterType == "wizard") {
        std::shared_ptr<Character> character = std::make_shared<Wizard>(characterName, characterHp);
        characters.insert(std::make_pair(characterName, character));
    }
    outputFile << "A new " << characterType << " came to town, " << characterName << ".\n";
}

// Function to create a new item (weapon, potion, spell)
void createItem(std::vector<std::string> &keywords, std::map<std::string, std::shared_ptr<Character>> &characters) {
    std::string itemType = keywords[2];
    std::string itemOwnerName = keywords[3];
    std::string itemName = keywords[4];
    std::shared_ptr<Character> owner;

    // Check owner of the item:
    auto it = characters.find(itemOwnerName);
    if (it != characters.end()) {
        owner = it->second;
    }
    else {
        outputFile << "Error caught\n";
        return;
    }

    switch (itemType[0]) {
        // Weapon creation
        case 'w': {
            // Check availability:
            if (owner->getWeapons().getRemainingSize() == 0) {
                outputFile << "Error caught\n";
                return;
            }
            int damageValue = std::stoi(keywords[5]);
            // Check damage:
            if (!(1 <= damageValue && damageValue <= 50)) {
                outputFile << "Error caught\n";
                return;
            }
            std::shared_ptr<PhysicalItem> physicalItem = std::make_shared<Weapon>(owner, itemName, damageValue);
            owner->assignItem(physicalItem, "weapon");
            break;
        }
        // Potion creation
        case 'p': {
            // Check availability:
            if (owner->getPotions().getRemainingSize() == 0) {
                outputFile << "Error caught\n";
                return;
            }
            int healValue = std::stoi(keywords[5]);
            // Check heal:
            if (!(1 <= healValue && healValue <= 50)) {
                outputFile << "Error caught\n";
                return;
            }
            std::shared_ptr<PhysicalItem> physicalItem = std::make_shared<Potion>(owner, itemName, healValue);
            owner->assignItem(physicalItem, "potion");
            break;
        }
        // Spell creation
        case 's': {
            // Check availability:
            if (owner->getSpells().getRemainingSize() == 0) {
                outputFile << "Error caught\n";
                return;
            }
            std::map<std::string, std::shared_ptr<Character>> victims;
            int len = std::stoi(keywords[5]);
            // Check len:
            if (!(0 <= len && len <= 50)) {
                outputFile << "Error caught\n";
                return;
            }
            for (int i = 0; i < len; i++) {
                auto chel = characters.find(keywords[i+6]);
                if (chel == characters.end()) {
                    outputFile << "Error caught\n";
                    return;
                }
                victims[chel->first] = chel->second;
            }
            std::shared_ptr<PhysicalItem> physicalItem = std::make_shared<Spell>(owner, itemName, victims);
            owner->assignItem(physicalItem, victims);
            break;
        }
        default:
            outputFile << "oshibka\n";
            break;
    }
    outputFile << itemOwnerName << " just obtained a new " << itemType << " called " << itemName << ".\n";
};


template <typename K, typename V>
void sortMapByKey(const std::map<K, V>& map) {
    // Create a custom comparator function to compare keys
    auto cmp = [](const K& a, const K& b) {
        return a < b;
    };

    // Create a set to store the sorted keys
    std::set<K, decltype(cmp)> sortedKeys(cmp);

    // Populate the set with keys from the map
    for (const auto& pair : map) {
        sortedKeys.insert(pair.first);
    }
}

// Function to display information about characters, items, or spells
void showSomething(std::vector<std::string> &keywords, std::map<std::string, std::shared_ptr<Character>> &characters) {
    std::string type = keywords[1];
    switch (type[0]) {
        case 'c': {
            sortMapByKey(characters);
            for (const auto& pair : characters) {
                outputFile << pair.first << ":" << pair.second->getType() << ":" << pair.second->getHP() << " ";
            }
            outputFile << "\n";
            break;
        }
        case 'w': {
            std::string name = keywords[2];
            auto it = characters.find(name);
            if (it != characters.end()) {
                auto wp = it->second->getWeapons().getContainer();
                if (it->second->getWeapons().getMaxSize() == 0) {
                    outputFile << "Error caught\n";
                    return;
                }
                sortMapByKey(wp);
                for (auto const& pair : wp) {
                    outputFile << pair.first << ":" << pair.second->getDamage() << " ";
                }
                outputFile << "\n";
            }
            else {
                outputFile << "Error caught\n";
                return;
            }
            break;
        }
        case 'p': {
            std::string name = keywords[2];
            auto it = characters.find(name);
            if (it != characters.end()) {
                auto wp = it->second->getPotions().getContainer();
                sortMapByKey(wp);
                if (it->second->getPotions().getMaxSize() == 0) {
                    outputFile << "Error caught\n";
                    return;
                }
                for (auto const& pair : wp) {
                    outputFile << pair.first << ":" << pair.second->getHeal() << " ";
                }
                outputFile << "\n";
            }
            else {
                outputFile << "Error caught\n";
                return;
            }
            break;
        }
        case 's': {
            std::string name = keywords[2];
            auto it = characters.find(name);
            if (it != characters.end()) {
                auto wp = it->second->getSpells().getContainer();
                sortMapByKey(wp);
                if (it->second->getSpells().getMaxSize() == 0) {
                    outputFile << "Error caught\n";
                    return;
                }
                for (auto const& pair : wp) {
                    outputFile << pair.first << ":" << pair.second->getVictims().size() << " ";
                }
                outputFile << "\n";
            }
            else {
                outputFile << "Error caught\n";
                return;
            }
            break;
        }
        default:
            break;
    }
}

// Function to execute actions (attack, cast spell, drink potion)
void doAction(std::vector<std::string> &keywords, std::map<std::string, std::shared_ptr<Character>> &characters) {
    std::string action = keywords[0];
    std::string user = keywords[1];
    std::string target = keywords[2];
    std::string objectName = keywords[3];

    // Names check:
    auto it1 = characters.find(user);
    auto it2 = characters.find(target);
    if (!(it1 != characters.end() && it2 != characters.end())) {
        outputFile << "Error caught\n";
        return;
    }

    switch (action[0]) {
        case 'A': {
            // Weapon name check:
            if (it1->second->getWeapons().find(objectName) == nullptr) {
                outputFile << "Error caught\n";
                return;
            }
            it1->second->attack(it2->second, objectName);
            outputFile << user << " attacks " << target << " with their " << objectName << "!\n";
            // Check for death
            if (it2->second->getHP() <= 0) {
                outputFile << it2->second->getName() << " has died...\n";
                characters.erase(it2->second->getName());
            }
            break;
        }
        case 'C': {
            // Weapon name check:
            auto harchok = it1->second->getSpells().find(objectName);
            if (harchok == nullptr) {
                outputFile << "Error caught\n";
                return;
            } else {
                auto chel = harchok->getVictims().find(target);
                if (chel == harchok->getVictims().end()) { // No chel
                    outputFile << "Error caught\n";
                    return;
                }
            }
            it1->second->spell(it2->second, objectName);
            outputFile << user << " casts " << objectName << " on " << target <<  "!\n";
            outputFile << target << " has died...\n";
            characters.erase(target);
            break;
        }
        case 'D': {
            // Weapon name check:
            if (it1->second->getPotions().find(objectName) == nullptr) {
                outputFile << "Error caught\n";
                return;
            }
            it1->second->potion(it2->second, objectName);
            outputFile << target << " drinks " << objectName << " from " << user <<  ".\n";
            break;
        }
        default:
            break;
    }
}

// Function to handle character dialogues
void doChat(std::vector<std::string> &keywords, std::map<std::string, std::shared_ptr<Character>> &characters) {
    std::string name = keywords[1];
    int len = std::stoi(keywords[2]);
    if (len <= 10 && len >= 1) {
        if (name == "Narrator") {
            outputFile << "Narrator: ";}
        else {
            auto it = characters.find(name);
            if (it == characters.end()) {
                outputFile << "Error caught\n";
                return;
            }
            outputFile << name << ": ";
        }
        for (int i = 0; i < len; i++) {
            outputFile << keywords[i + 3] << ' ';
        }
        outputFile << "\n";
    }
    else {
        outputFile << "Error caught\n";
        return;
    }
}

// Function to execute commands and simulate gameplay
void execute(std::string &input, std::map<std::string, std::shared_ptr<Character>> &characters, std::ofstream& outputFile) {
    // Split input into commands
    // Determine command type and execute corresponding function
    std::vector<std::string> commands = split(input);
    std::string command = commands[0];

    // Creation block
    if (command == "Create") {
        if (commands[1] == "character") {
            createCharacter(commands, characters);
        }
        else if (commands[1] == "item") {
            createItem(commands, characters);
        }
    }
    // Attacking block
    else if (command == "Attack") {
        doAction(commands, characters);
    }
    else if (command == "Cast") {
        doAction(commands, characters);
    }
    else if (command == "Drink") {
        doAction(commands, characters);
    }
    else if (command == "Dialogue") {
        doChat(commands, characters);
    }
    else if (command == "Show") {
        showSomething(commands, characters);
    }
    else {
        outputFile << "wrong command\n";
    }
}


int main () {
    // Initialize variables
    // Open input file
    // Read each line from the file and execute corresponding commands

    using namespace std;

    map<string, shared_ptr<Character>> characters;
    std::ifstream inputFile("input.txt");
    std::string line;

    if (inputFile.is_open()) { // Check if the file is open successfully
        std::getline(inputFile, line);
        while (std::getline(inputFile, line)) { // Read each line from the file
            execute(line, characters, outputFile);
        }
    }

    outputFile.close();

    return 0;
}

