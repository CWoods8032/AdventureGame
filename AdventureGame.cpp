#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <exception>

using namespace std;

class Character {
protected:
	string name; // Protected (Encapsulation): Access is restricted to derived classes
	int health;

public:
	// Constructor to initialize character attributes
	Character(string name, int health) : name(name), health(health) {}

	// Pure virtual function (Abstraction): Must be implemented by derived classes
	virtual void displayStats() const = 0;

	// Pure virtual function for attacks (Abstraction)
	virtual void attack(Character& target) = 0;

	// Method to apply damage to a character (Encapsulation: controlled access to health)
	void takeDamage(int damage) {
		health -= damage;
		if (health < 0) health = 0; // Ensure health doesn't go negative
	}

	// Getter for health (Encapsulation: controlled read access)
	int getHealth() const { return health; }

	// Getter for name (Encapsulation: controlled read access)
	string getName() const { return name; }

	// Virtual destructor ensures proper cleanup for derived classes
	virtual ~Character() = default;
};

class Player : public Character {
private:
	int treasuresCollected; // Private (Encapsulation): Only accessible within Player Class

public:
	// Constructor: Initializes the player with a name and default health
	Player(string name) : Character(name, 100), treasuresCollected(0) {}

	// Overriding displayStats (Polymorphism : method overriding)
	void displayStats() const override {
		cout << "Player: " << name << ", Health: " << health
			<< ", Treasures: " << treasuresCollected << "\n";
	}

	// Overriding attack method (Polymorphism: method overriding)
	void attack(Character& target) override {
		cout << name << " attacks the enemy!\n";
		target.takeDamage(20); // Deal 20 damage
	}

	// Method to collect treasures (Unique to Player class)
	void collectTreasure() {
		treasuresCollected++;
		cout << "Collected a treasure! Total: " << treasuresCollected << "\n";
	}
};

class Enemy : public Character {
public:
	// Constructor: Initializes the enemy with a name and default health
	Enemy(string name) : Character(name, 50) {}

	// Overriding displayStats (Polymorphism: method overriding)
	void displayStats() const override {
		cout << "Enemy: " << name << ", Health: " << health << "\n";
	}

	// Overriding attack method (Polymorphism: method overriding)
	void attack(Character& target) override {
		cout << name << " attacks the player!\n";
		target.takeDamage(15); // Deal 15 damage
	}
};

class Game {
private:
	Player* player; // Pointer to Player object (allocated dynamically)

public:
	// Constructor initializes the game with no player
	Game() : player(nullptr) {}

	// Displays the opening screen with game instructions
	void displayOpeningScreen() {
		cout << "=====================\n";
		cout << "  Welcome to Mystic Quest!\n";
		cout << "=====================\n\n";
		cout << "Instructions:\n";
		cout << "1. Navigate through the forest.\n";
		cout << "2. Solve puzzles, battle enemies, and collect treasures.\n";
		cout << "3. Escape the forest to win.\n\n";
	}

	// Displays the main menu and handles user input
	void displayMenu() {
		int choice = 0;

		while (choice != 3) { // Loop until the user chooses to exit
			cout << "1. Start Game\n2. Load Game\n3. Exit\n";
			cout << "Choose an option: ";
			cin >> choice;

			if (choice == 1) {
				startGame();
			}
			else if (choice == 2) {
				loadGame();
			}
			else if (choice == 3) {
				cout << "Thank you for playing Mystic Quest!\n";
			}
			else {
				cerr << "Invalid choice. Please try again.\n"; // Error message for invalid input
			}
		}
	}

	// Starts a new game by creating a Player object
	void startGame() {
		string playerName;
		cout << "Enter your name: ";
		cin >> playerName;

		player = new Player(playerName); // Dynamic allocation of player object

		cout << "Starting new game...\n";
		play();
	}

	// Loads the game state from a file (Feature: File Input)
	void loadGame() {
		try {
			ifstream file("game_state.txt"); // Open file for reading
			if (!file) {
				throw ios_base::failure("Failed to open file for loading.");
			}

			string playerName;
			int playerHealth;

			file >> playerName >> playerHealth; // Read player data
			player = new Player(playerName);
			player->takeDamage(100 - playerHealth); // Adjust health based on saved state

			file.close();
			cout << "Game state loaded successfully.\n";
			play();
		}
		catch (exception& e) {
			cerr << "Error loading game: " << e.what() << '\n'; // Exception handling for file errors
		}
	}

	// Saves the current game state to a file (Feature: File Output)
	void saveGame() {
		try {
			ofstream file("game_state.txt"); // Open file for writing
			if (!file) {
				throw ios_base::failure("Failed to open file for saving.");
			}

			file << player->getName() << '\n';
			file << player->getHealth() << '\n';

			file.close();
			cout << "Game state saved successfully.\n";
		}
		catch (exception& e) {
			cerr << "Error saving game: " << e.what() << '\n'; // Exception handling for file errors
		}
	}

	// Main gameplay loop
	void play() {
		Enemy enemy("Goblin"); // Create an enemy object

		while (player->getHealth() > 0 && enemy.getHealth() > 0) {
			player->displayStats();
			enemy.displayStats();

			cout << "\nChoose an action:\n1. Attack\n2. Collect Treasure\n3. Save and Exit\n";
			int action = 0;
			cin >> action;

			if (action == 1) {
				player->attack(enemy); // Player attacks enemy
				if (enemy.getHealth() > 0) enemy.attack(*player); // Enemy retaliates if still alive
			}
			else if (action == 2) {
				player->collectTreasure(); // Player collects treasure
			}
			else if (action == 3) {
				saveGame(); // Save game and exit gameplay loop
				delete player;
				player = nullptr;
				return;
			}
			else {
				cerr << "Invalid action. Try again.\n"; // Handle invalid input
			}
		}

		if (player->getHealth() <= 0) {
			cout << "You have been defeated. Game over.\n";
		}
		else if (enemy.getHealth() <= 0) {
			cout << "You defeated the enemy! Victory!\n";
		}

		delete player; // Free dynamically allocated memory
		player = nullptr;
	}
};

// Function to simulate background music (Feature: Multi-threading)
void playBackgroundMusic() {
	cout << "Playing background music...\n";
	this_thread::sleep_for(chrono::seconds(5)); // Simulate 5 seconds of music
	cout << "Music ended.\n";
}

// Main function to start the game
int main() {
	Game game; // Create game object
	game.displayOpeningScreen();

	thread musicThread(playBackgroundMusic); // Start background music in a separate thread
	game.displayMenu();

	if (musicThread.joinable()) musicThread.join(); // Wait for the music thread to finish
	return 0;
}