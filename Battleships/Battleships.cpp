#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>

using namespace std;

//---------DECLERATIONS----------//

enum //Global stuff
{
	AIRCRAFTCARRIER_SIZE = 5,
	BATTLESHIP_SIZE = 4,
	GUNBOAT_SIZE = 3,
	MINESWEEPER_SIZE = 2,

	BOARD_SIZE = 10,
	NUM_SHIPS = 4,
	PLAYER_NAME_SIZE = 12,
	MAX_SHIP_SIZE = AIRCRAFTCARRIER_SIZE
};

enum ShipType
{
	TYPE_NONE = 0,
	TYPE_AIRCRAFTCARRIER,
	TYPE_BATTLESHIP,
	TYPE_GUNBOAT,
	TYPE_MINESWEEPER
};

enum ShipOrientation
{
	ORI_HORIZONTAL = 0,
	ORI_VERTICAL = 1
};

struct ShipPosition
{
	int row;
	int col;
};

struct Ship
{
	ShipType shipType;
	int shipSize;
	ShipOrientation orientation;
	ShipPosition position;
};

enum GuessType
{
	GUESS_NONE = 0,
	GUESS_MISSED,
	GUESS_HIT
};

struct ShipPartType
{
	ShipType shipType;
	bool isHit;
	bool isMiss;
};

enum PlayerType
{
	PLAYER_HUMAN = 0,
	PLAYER_COMPUTER
};

struct Player
{
	PlayerType playerType;
	char playerName[PLAYER_NAME_SIZE];
	Ship ships[NUM_SHIPS];
	GuessType guessBoard[BOARD_SIZE][BOARD_SIZE];
	ShipPartType shipBoard[BOARD_SIZE][BOARD_SIZE];
};

//----------FUNCTIONS------------//

PlayerType GetPlayer2Type();

void GeneratePlayer(Player& player, const char* playerName);

void GenerateShip(Ship& ship, int shipSize, ShipType shipType);

void PlayGame(Player& player1, Player& player2);

void SetupBoards(Player& player);

void ClearBoards(Player& player);

void SetupComputerBoards(Player& player);

ShipPosition GetRandomPosition();

bool IsValidPlacement(const Player& player, const Ship& currentShip, const ShipPosition& shipPosition, ShipOrientation orientation);

void PlaceShip(Player& player, Ship& currentShip, const ShipPosition& shipPosition, ShipOrientation orientation);

void  DrawBoards(const Player& player);

void DrawColumnsRow();

void SeparatorLine();

void DrawShipBoardRow(const Player& player, int row);

char GetShipRepresentation(const Player& player, int row, int col);

void DrawGuessBoardRow(const Player& player, int row);

char GetGuessRepresentation(const Player& player, int row, int col);

const char* GetShipName(ShipType shipType);

ShipPosition GetBoardPosition();

ShipPosition MapBoardPosition(int rowInput, int colInput);

ShipOrientation GetShipOrientation();

ShipPosition GetComputerGuess(const Player& ComputerPlayer);

ShipType UpdateBoards(ShipPosition guess, Player& currentPlayer, Player& otherPlayer);

void SwitchPlayers(Player** currentPlayer, Player** otherPlayer);

bool IsGameOver(const Player& player1, const Player& player2);

bool AreAllShipsSunk(const Player& player);

bool IsSunk(const Player& player, const Ship& ship);

void DisplayWinner(const Player& player1, const Player& player2);

bool PlayAgain();

//------------START------------//

int main()
{
	Player player1;
	Player player2;
	
	player1.playerType = PLAYER_HUMAN;
	player2.playerType = GetPlayer2Type();

	cout << "Player 1 please enter your name: ";
	cin >> player1.playerName;
	cout << endl;

	GeneratePlayer(player1, player1.playerName);

	if (player2.playerType == PLAYER_HUMAN) 
	{
		cout << "Player 2 please enter your name: ";
		cin >> player2.playerName;
		cout << endl;

		GeneratePlayer(player2, player2.playerName);
	}
	else 
	{
		GeneratePlayer(player2, "Computer");
	}
	

	do
	{
		PlayGame(player1, player2);

	} while (PlayAgain());

	return 0;
}

PlayerType GetPlayer2Type()
{
	int input;

	cout << "HUMAN (1) or COMPUTER (2)?: ";

	while (!(cin >> input) || (input > 2) || (input < 0)) //ERROR CHECK
	{
		cout << "ERROR: Enter 1 or 2: ";

		cin.clear();

		cin.ignore(123, '\n');
	}

	if (input == 1)
	{
		return PLAYER_HUMAN;
	}
	else if (input == 2)
	{
		return PLAYER_COMPUTER;
	}
	else
	{
		return GetPlayer2Type();
	}
}

void GeneratePlayer(Player& player, const char* playerName)
{
	if (playerName != nullptr && strlen(playerName) > 0)
	{
		strcpy_s(player.playerName, playerName);
	}

	GenerateShip(player.ships[0], AIRCRAFTCARRIER_SIZE, TYPE_AIRCRAFTCARRIER);

	GenerateShip(player.ships[1], BATTLESHIP_SIZE, TYPE_BATTLESHIP);

	GenerateShip(player.ships[2], GUNBOAT_SIZE, TYPE_GUNBOAT);

	GenerateShip(player.ships[3], MINESWEEPER_SIZE, TYPE_MINESWEEPER);
}

void GenerateShip(Ship& ship, int shipSize, ShipType shipType)
{
	ship.shipType = shipType;
	ship.shipSize = shipSize;
	ship.position.row = 0;
	ship.position.col = 0;
	ship.orientation = ORI_HORIZONTAL;
}

void PlayGame(Player& player1, Player& player2)
{
	system("CLS");
	
	SetupBoards(player1);
	SetupBoards(player2);

	Player* currentPlayer = &player1;
	Player* otherPlayer = &player2;

	ShipPosition guess;

	do
	{
		if (currentPlayer->playerType == PLAYER_HUMAN)
		{
			DrawBoards(*currentPlayer);
		}

		bool isValidGuess;

		do
		{
			if (currentPlayer->playerType == PLAYER_HUMAN)
			{
				cout << currentPlayer->playerName << " what is your guess? " << endl;

				guess = GetBoardPosition();
			}
			else
			{
				guess = GetComputerGuess(*currentPlayer);
			}

			isValidGuess = currentPlayer->guessBoard[guess.row][guess.col] == GUESS_NONE;

			if (!isValidGuess && currentPlayer->playerType == PLAYER_HUMAN)
			{
				cout << "That was not a valid guess! Please try again." << endl;
			}


		} while (!isValidGuess);

		ShipType type = UpdateBoards(guess, *currentPlayer, *otherPlayer);

		if (currentPlayer->playerType == PLAYER_COMPUTER)
		{
			DrawBoards(*otherPlayer);
			cout << currentPlayer->playerName << " chose row " << guess.row << " and column " << guess.col << endl;
		}
		else
		{
			DrawBoards(*currentPlayer);
		}

		if (type != TYPE_NONE && IsSunk(*otherPlayer, otherPlayer->ships[type - 1]))
		{
			if (currentPlayer->playerType == PLAYER_COMPUTER)
			{
				cout << currentPlayer->playerName << " sunk your " << GetShipName(type) << "!" << endl;
			}
			else
			{
				cout << "You sunk " << otherPlayer->playerName << "'s " << GetShipName(type) << "!" << endl;

			}
		}

		system("pause");

		SwitchPlayers(&currentPlayer, &otherPlayer);


	} while (!IsGameOver(player1, player2));

	DisplayWinner(player1, player2);
}

void SetupBoards(Player& player)
{
	ClearBoards(player);

	if (player.playerType == PLAYER_COMPUTER)
	{
		SetupComputerBoards(player);
		return;
	}

	for (int i = 0; i < NUM_SHIPS; i++)
	{

		DrawBoards(player);

		Ship& currentShip = player.ships[i];

		ShipPosition shipPosition;
		ShipOrientation orientation;

		bool isValidPlacement = false;

		do
		{
			cout << player.playerName << " please set the position and orientation for your " << GetShipName(currentShip.shipType) << endl;

			shipPosition = GetBoardPosition();
			orientation = GetShipOrientation();

			isValidPlacement = IsValidPlacement(player, currentShip, shipPosition, orientation);

			if (!isValidPlacement)
			{
				cout << "That was not a valid placement. Please try again." << endl;
				system("pause");
			}

		} while (!isValidPlacement);


		PlaceShip(player, currentShip, shipPosition, orientation);

	}

	DrawBoards(player);
	system("pause");
}

void ClearBoards(Player& player) //Makes everything in the 10x10 grid 'blank'
{
	for (int r = 0; r < BOARD_SIZE; r++)
	{
		for (int c = 0; c < BOARD_SIZE; c++)
		{
			player.guessBoard[r][c] = GUESS_NONE;
			player.shipBoard[r][c].shipType = TYPE_NONE;
			player.shipBoard[r][c].isHit = false;
			player.shipBoard[r][c].isMiss = false;
		}
	}
}

void SetupComputerBoards(Player& player)
{
	ShipPosition pos;
	ShipOrientation orientation;
	srand(time(NULL));

	for (int i = 0; i < NUM_SHIPS; i++)
	{
		Ship& currentShip = player.ships[i];

		do
		{
			pos = GetRandomPosition();
			orientation = ShipOrientation(rand() % 2);

		} while (!IsValidPlacement(player, currentShip, pos, orientation));

		PlaceShip(player, currentShip, pos, orientation);
	}
}

ShipPosition GetRandomPosition()
{
	ShipPosition guess;
	guess.row = rand() % BOARD_SIZE;
	guess.col = rand() % BOARD_SIZE;

	return guess;
}

bool IsValidPlacement(const Player& player, const Ship& currentShip, const ShipPosition& shipPosition, ShipOrientation orientation)
{
	if (orientation == ORI_HORIZONTAL)
	{
		for (int c = shipPosition.col; c < (shipPosition.col + currentShip.shipSize); c++)
		{
			if (player.shipBoard[shipPosition.row][c].shipType != TYPE_NONE || c >= BOARD_SIZE)
			{

				return false;
			}
		}
	}
	else
	{
		for (int r = shipPosition.row; r < (shipPosition.row + currentShip.shipSize); r++)
		{
			if (player.shipBoard[r][shipPosition.col].shipType != TYPE_NONE || r >= BOARD_SIZE)
			{
				return false;
			}
		}
	}
	return true;
}

void PlaceShip(Player& player, Ship& currentShip, const ShipPosition& shipPosition, ShipOrientation orientation)
{
	currentShip.position = shipPosition;
	currentShip.orientation = orientation;

	if (orientation == ORI_HORIZONTAL)
	{
		for (int c = shipPosition.col; c < (shipPosition.col + currentShip.shipSize); c++)
		{
			player.shipBoard[shipPosition.row][c].shipType = currentShip.shipType;
			player.shipBoard[shipPosition.row][c].isHit = false;
		}
	}
	else
	{
		for (int r = shipPosition.row; r < (shipPosition.row + currentShip.shipSize); r++)
		{
			player.shipBoard[r][shipPosition.col].shipType = currentShip.shipType;

			player.shipBoard[r][shipPosition.col].isHit = false;
		}
	}
}

void DrawBoards(const Player& player)
{
	system("CLS");

	DrawColumnsRow();

	DrawColumnsRow();

	cout << endl;

	for (int r = 0; r < BOARD_SIZE; r++)
	{
		SeparatorLine();

		cout << " ";

		SeparatorLine();

		cout << endl;

		DrawShipBoardRow(player, r);

		cout << " ";

		DrawGuessBoardRow(player, r);

		cout << endl;
	}

	SeparatorLine();

	cout << " ";

	SeparatorLine();

	cout << endl;
}

void DrawColumnsRow()
{
	cout << "  ";
	for (int c = 0; c < BOARD_SIZE; c++)
	{
		int columnName = c;

		cout << " " << columnName << "  ";
	}
	cout << " ";
}

void SeparatorLine()
{
	cout << " ";

	for (int c = 0; c < BOARD_SIZE; c++)
	{
		cout << "----";
	}

	cout << "-";
}

void DrawShipBoardRow(const Player& player, int row)
{
	int rowName = row + 0;
	cout << rowName << "|";

	for (int c = 0; c < BOARD_SIZE; c++)
	{
		cout << " " << GetShipRepresentation(player, row, c) << " |";
	}
}

char GetShipRepresentation(const Player& player, int row, int col)
{
	if (player.shipBoard[row][col].isHit)
	{
		return '*'; //represents hit
	}

	if (player.shipBoard[row][col].shipType == TYPE_AIRCRAFTCARRIER)
	{
		return 'A';
	}
	else if (player.shipBoard[row][col].shipType == TYPE_BATTLESHIP)
	{
		return 'B';
	}
	else if (player.shipBoard[row][col].shipType == TYPE_GUNBOAT)
	{
		return 'C';
	}
	else if (player.shipBoard[row][col].shipType == TYPE_MINESWEEPER)
	{
		return 'D';
	}
	else
	{
		return ' ';
	}
}

void DrawGuessBoardRow(const Player& player, int row)
{
	int rowName = row + 0;
	cout << rowName << "|";

	for (int c = 0; c < BOARD_SIZE; c++)
	{
		cout << " " << GetGuessRepresentation(player, row, c) << " |";
	}
}

char GetGuessRepresentation(const Player& player, int row, int col)
{
	if (player.guessBoard[row][col] == GUESS_HIT)
	{
		return '*';
	}
	else if (player.guessBoard[row][col] == GUESS_MISSED)
	{
		return 'o';
	}
	else
	{
		return ' ';
	}
}

const char* GetShipName(ShipType shipType)
{
	if (shipType == TYPE_AIRCRAFTCARRIER)
	{
		return "Aircraft Carrier";
	}
	else if (shipType == TYPE_BATTLESHIP)
	{
		return "Battleship";
	}
	else if (shipType == TYPE_GUNBOAT)
	{
		return "Gunboat";
	}
	else if (shipType == TYPE_MINESWEEPER)
	{
		return "Minesweeper";
	}
	return "None";
}

ShipPosition GetBoardPosition()
{
	int rowInput;
	int colInput;

	cout << "ROW: ";
	while (!(cin >> rowInput) || (rowInput < 0) || (rowInput > 9)) //ERROR CHECK
	{
		cout << "ERROR: Enter a number from 0-9: ";

		cin.clear();

		cin.ignore(123, '\n');
	}

	cout << "COLUMN: ";
	while (!(cin >> colInput) || (colInput < 0) || (colInput > 9)) //ERROR CHECK
	{
		cout << "ERROR: Enter a number from 0-9: ";

		cin.clear();

		cin.ignore(123, '\n');
	}

	return MapBoardPosition(rowInput, colInput);
}

ShipPosition MapBoardPosition(int rowInput, int colInput)
{
	int realRow = rowInput;
	int realCol = colInput;

	ShipPosition boardPosition;

	boardPosition.row = realRow;
	boardPosition.col = realCol;

	return boardPosition;
}

ShipOrientation GetShipOrientation()
{
	int input;

	cout << "Horizontal (1) or Vertical (2)?: " << endl;

	while (!(cin >> input) || (input > 2) || (input < 0)) //ERROR CHECK
	{
		cout << "ERROR: Enter 1 or 2: ";

		cin.clear();

		cin.ignore(123, '\n');
	}

	if (input == 1)
	{
		return ORI_HORIZONTAL;
	}
	else
	{
		return ORI_VERTICAL;
	}
}

ShipPosition GetComputerGuess(const Player& ComputerPlayer)
{
	return GetRandomPosition();
}

ShipType UpdateBoards(ShipPosition guess, Player& currentPlayer, Player& otherPlayer)
{
	if (otherPlayer.shipBoard[guess.row][guess.col].shipType != TYPE_NONE)
	{
		currentPlayer.guessBoard[guess.row][guess.col] = GUESS_HIT;
		otherPlayer.shipBoard[guess.row][guess.col].isHit = true;
	}
	else
	{
		currentPlayer.guessBoard[guess.row][guess.col] = GUESS_MISSED;
		otherPlayer.shipBoard[guess.row][guess.col].isMiss = true;
	}

	return otherPlayer.shipBoard[guess.row][guess.col].shipType;
}

void SwitchPlayers(Player** currentPlayer, Player** otherPlayer)
{
	Player* temp = *currentPlayer;
	*currentPlayer = *otherPlayer;
	*otherPlayer = temp;
}

bool IsGameOver(const Player& player1, const Player& player2)
{
	return AreAllShipsSunk(player1) || AreAllShipsSunk(player2);
}

bool AreAllShipsSunk(const Player& player)
{
	for (int i = 0; i < NUM_SHIPS; i++)
	{
		if (!IsSunk(player, player.ships[i]))
		{
			return false;
		}
	}
	return true;
}

bool IsSunk(const Player& player, const Ship& ship)
{
	if (ship.orientation == ORI_HORIZONTAL)
	{
		for (int col = ship.position.col; col < (ship.position.col + ship.shipSize); col++)
		{
			if (!player.shipBoard[ship.position.row][col].isHit)
			{
				return false;
			}
		}
	}
	else
	{
		for (int row = ship.position.row; row < (ship.position.row + ship.shipSize); row++)
		{
			if (!player.shipBoard[row][ship.position.col].isHit)
			{
				return false;
			}
		}
	}
	return true;
}

void DisplayWinner(const Player& player1, const Player& player2)
{
	if (AreAllShipsSunk(player1))
	{
		cout << "Congratulations " << player2.playerName << "! You won!" << endl;
	}
	else
	{
		cout << "Congratulations " << player1.playerName << "! You won!" << endl;
	}
}

bool PlayAgain()
{
	char input = 'a';

	cout << "Play again? (y/n) ";

	while (!(cin >> input) || (input != 'y') && (input != 'n')) //ERROR CHECK
	{
		cout << "ERROR: Enter y or n: ";

		cin.clear();

		cin.ignore(123, '\n');
	}

	return input == 'y';
}
