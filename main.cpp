#include <iostream>
#include <random>
#include <stdlib.h>
#include <chrono>
#include <thread>

enum class Cell : char {
  CROSS = 'X',
  ZERO = '0',
  EMPTY = '_'
};

enum class Progress : uint8_t {
  IN_PROGRESS,
  WON_AI,
  WON_HUM,
  DRAW
};

struct Coord {
  size_t y{ 0 };
  size_t x{ 0 };
};

#pragma pack(push, 1)
struct Game {
  Cell** ppField = nullptr;
  const size_t size = 3;
  size_t turn = 0;
  Cell human = Cell::CROSS;
  Cell ai = Cell::ZERO;
  std::string difLevel;
  Progress progress = Progress::IN_PROGRESS;
};
#pragma pack(pop)

inline void clearScr()
{
  system("cls");
}

std::ostream& operator << (std::ostream& out, const Cell& cell)
{
  switch (cell) 
  {
  case Cell::CROSS: return (out << 'X');
  case Cell::ZERO : return (out << '0');
  case Cell::EMPTY : return (out << '_');
  }

  return (out);
}

int32_t getRandomNumber(int32_t min, int32_t max)
{
  const static auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  static std::mt19937_64 generator(seed);
  std::uniform_int_distribution<int32_t> dis(min, max);
  return dis(generator);
}

void __fastcall initGame(Game& game) 
{
  do
  {
    std::cout << "Choose the difficulty level: normal or easy or hard: " << std::endl;
    std::cin >> game.difLevel;
  } while (game.difLevel != "normal" && game.difLevel != "eazy" && game.difLevel != "hard");

  game.ppField = new Cell*[game.size];
  for (size_t i = 0; i < game.size; i++)
  {
    game.ppField[i] = new Cell[game.size];
  }

  for (size_t y = 0; y < game.size; y++)
  {
    for (size_t x = 0; x < game.size; x++)
    {
      game.ppField[y][x] = Cell::EMPTY;
    }
  }

  if (getRandomNumber(0, 1000) > 500)
  {
    game.human = Cell::CROSS;
    game.ai = Cell::ZERO;
    game.turn = 0;
  }
  else 
  {
    game.human = Cell::ZERO;
    game.ai = Cell::CROSS;
    game.turn = 1;
  }
}

void __fastcall deinitGame(Game& game)
{
  for (size_t i = 0; i < game.size; i++)
  {
    delete[] game.ppField[i];
  }
  delete game.ppField; 
  game.ppField = nullptr;
}

void __fastcall drawGame(const Game& game)
{
  std::cout << "    ";
  for (size_t x = 0; x < game.size; x++)
  {
    std::cout << x + 1 << "   ";
  }
  std::cout << std::endl;

  for (size_t y = 0; y < game.size; y++)
  {
    std::cout << y + 1 << " | ";
    for (size_t x = 0; x < game.size; x++)
    {
      std::cout << game.ppField[y][x] << " | ";
    }
    std::cout << std::endl;
  }

  std::cout << std::endl << "Human: " << game.human << std::endl << "Computer: " << game.ai << std::endl;
}

Coord getHumanCoord(const Game& game) 
{
  Coord coord;
  do
  {
    std::cout << "Enter X: ";
    std::cin >> coord.x;

    std::cout << "Enter Y: ";
    std::cin >> coord.y;

  } while (coord.x == 0 || coord.x > 3 || coord.y == 0 || coord.y > 3 
      || game.ppField[coord.y-1][coord.x-1] != Cell::EMPTY);

  coord.x--;
  coord.y--;

  return coord;
}

Progress getWon(const Game& game)
{
  for (size_t y = 0; y < game.size; y++)
  {
    if (game.ppField[y][0] == game.ppField[y][1] && game.ppField[y][0] == game.ppField[y][2])
    {
      if (game.ppField[y][0] == game.human)
      {
        return Progress::WON_HUM;
      }
      if (game.ppField[y][0] == game.ai)
      {
        return Progress::WON_AI;
      }
    }
    for (size_t x = 0; x < game.size; x++)
    {
      if (game.ppField[0][x] == game.ppField[1][x] && game.ppField[0][x] == game.ppField[2][x])
      {
        if (game.ppField[0][x] == game.human)
        {
          return Progress::WON_HUM;
        }
        if (game.ppField[0][x] == game.ai)
        {
          return Progress::WON_AI;
        }
      }
    }
  }
  
  if (game.ppField[0][0] == game.ppField[1][1] && game.ppField[0][0] == game.ppField[2][2])
  {
    if (game.ppField[0][0] == game.human)
    {
      return Progress::WON_HUM;
    }
    if (game.ppField[0][0] == game.ai)
    {
      return Progress::WON_AI;
    }
  }

  if (game.ppField[0][2] == game.ppField[1][1] && game.ppField[2][0] == game.ppField[1][1])
  {
    if (game.ppField[1][1] == game.human)
    {
      return Progress::WON_HUM;
    }
    if (game.ppField[1][1] == game.ai)
    {
      return Progress::WON_AI;
    }
  }

  bool draw = true;
  for (size_t y = 0; y < game.size; y++)
  {
    for (size_t x = 0; x < game.size; x++)
    {
      if (game.ppField[y][x] == Cell::EMPTY)
      {
        draw = false;
        break;
      }
    }
    if (!draw)
    {
      break;
    }
  }

  if (draw) return Progress::DRAW;

  return Progress::IN_PROGRESS;
}

Coord getAICoord(Game& game)
{
  if (game.difLevel == "normal" && getRandomNumber(0,500) > 100 || game.difLevel == "hard")
  {
    for (size_t y = 0; y < game.size; y++)
    {
      for (size_t x = 0; x < game.size; x++)
      {
        if (game.ppField[y][x] == Cell::EMPTY)
        {
          game.ppField[y][x] = game.ai;
          if (getWon(game) == Progress::WON_AI)
          {
            game.ppField[y][x] = Cell::EMPTY;
            return { y, x };
          }
          game.ppField[y][x] = Cell::EMPTY;
        }
      }
    }

    for (size_t y = 0; y < game.size; y++)
    {
      for (size_t x = 0; x < game.size; x++)
      {
        if (game.ppField[y][x] == Cell::EMPTY)
        {
          game.ppField[y][x] = game.human;
          if (getWon(game) == Progress::WON_HUM)
          {
            game.ppField[y][x] = Cell::EMPTY;
            return { y, x };
          }
          game.ppField[y][x] = Cell::EMPTY;
        }
      }
    }
  }

  if (game.ppField[1][1] == Cell::EMPTY)
  {
    return { 1, 1 };
  }

  Coord arr[4];
  size_t num{ 0 };

  if (game.ppField[0][0] == Cell::EMPTY)
  {
    arr[num] = { 0, 0 };
    num++;
  }

  if (game.ppField[2][2] == Cell::EMPTY)
  {
    arr[num] = { 2, 2 };
    num++;
  }

  if (game.ppField[2][0] == Cell::EMPTY)
  {
    arr[num] = { 2, 0 };
    num++;
  }

  if (game.ppField[0][2] == Cell::EMPTY)
  {
    arr[num] = { 0, 2 };
    num++;
  }

  if (num > 0)
  {
    const size_t index = getRandomNumber(0, 1000) % num;
    return arr[index];
  }

  num = 0;
  if (game.ppField[0][1] == Cell::EMPTY)
  {
    arr[num] = { 0, 1 };
    num++;
  }

  if (game.ppField[2][1] == Cell::EMPTY)
  {
    arr[num] = { 2, 1 };
    num++;
  }

  if (game.ppField[1][0] == Cell::EMPTY)
  {
    arr[num] = { 1, 0 };
    num++;
  }

  if (game.ppField[1][2] == Cell::EMPTY)
  {
    arr[num] = { 1, 2 };
    num++;
  }

  if (num > 0)
  {
    const size_t index = getRandomNumber(0, 1000) % num;
    return arr[index];
  }

  return {};
}

void __fastcall theEnd(const Game& game) 
{
  if (game.progress == Progress::WON_HUM)
  {
    std::cout << "Human won!" << std::endl;
  }
  if (game.progress == Progress::WON_AI)
  {
    std::cout << "Computer won!" << std::endl;
  }
  if (game.progress == Progress::DRAW)
  {
    std::cout << "Draw!" << std::endl;
  }
}

int main() 
{
  Game game;
  initGame(game);

  clearScr();
  drawGame(game);

  do
  {
    if (game.turn % 2 == 0)
    {
      Coord coord = getHumanCoord(game);
      game.ppField[coord.y][coord.x] = game.human;
    }
    else
    {
      Coord coord = getAICoord(game);
      game.ppField[coord.y][coord.x] = game.ai;
      std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }

    clearScr();
    drawGame(game);
    game.progress = getWon(game);
    game.turn++;

  } while (game.progress == Progress::IN_PROGRESS);

  theEnd(game);

  deinitGame(game);
  return 0;
}
