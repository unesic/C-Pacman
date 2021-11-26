# C++ Pacman

Simplified version of pacman made in C++ using TXT files as backend for levels and game logic. Built entirely using VS Code as text editor and MinGW as the compiler.

## Getting Started

To get started using the program all you have to do is clone it with git or download it directly. If you decide to go with the git option use:

```
git clone https://github.com/unesic/C-Pacman.git [folder]
```

### Prerequisites

There were some issues with the game running in Visual Studio so until I have that fixed I'd recomend using it the same way as it was built (MinGW compiler) using the following command:

```
cd src
g++ new-main.cpp -std=c++11 -lncurses -I . -o pacman
```

## Built With

* [VS Code](https://code.visualstudio.com/) - Text editor
* [MinGW](http://www.mingw.org/) - Compiler

## Authors

* **Uroš Nešić** - *Developer* - [unesic](https://unesic.io)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
