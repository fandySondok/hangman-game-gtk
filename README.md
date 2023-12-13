# Hangman Game with GTK3 and C

![Menu Layout](/docs/img/menu.png?raw=true 'Menu Layout')
![Main Layout](/docs/img/main.png?raw=true 'Main Layout')
![Pop UP Layout](/docs/img/pop-up.png?raw=true 'Pop Up Layout')

## Table of Contents

- [Description](#description)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)

## Description

This Hangman game is developed in C language using GTK3 for the graphical user interface. The game follows the classic rules of Hangman, where players guess letters to complete a hidden word before running out of attempts.

## Features

- User-friendly GTK3 GUI.
- Randomly selected words for gameplay.
- Visual representation of the hangman as incorrect guesses accumulate.
- Win and lose conditions with appropriate messages.

## Prerequisites

Before running the Hangman game, ensure you have the following prerequisites installed:

- `gcc` (C Compiler)
- `gtk+3.0 library` (for testing purpose)
- `glib2.0 library` (for testing purpose)

```bash
$ sudo apt-get update
$ sudo apt-get install build-essential
$ sudo apt-get install libgtk-3-dev
$ sudo apt-get install libglib2.0-dev
```

## Installation

1. **Clone the repository:**

```bash
$ git clone https://github.com/fandySondok/hangman-game-gtk.git
```

2. **Navigate to the project directory**

```bash
$ cd hangman-game-gtk
```

3. **Build the executable**

```bash
$ make
```

## Usage

```bash
$ ./hangman
>>>>>>> fc61ac4 (initial)
```
