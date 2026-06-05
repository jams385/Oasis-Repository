# Oasis

by Jamil Palma, Cj Genosa, Chenille Villademos

## Gameplay Overview

**Goal**: defend cornucopias from waves of desert enemies using towers, soldiers, and water mines; restore all 5 oases to win

**Win condition:** restore all 5 cornucopias  
**Lose condition:** cornucopias destroyed

**Resources:** Water points (spend to place towers / restore cornucopias)  
**Tower types:** Water Tower, Sun Beam Tower, Tree Tower, Water Well  
**Enemy types:** Dust Mummy, Spore Puff, Shadow Crow, Rust Golem (+ boss)

**Mechanics:** day/night cycle, wave system, soldier patrols, water mine harvesting

## How to Build and Run

**Must Have:** CMake 3.28, C++17 compiler, no need to download SFML

1. Clone repository  
2. Build using Cmake commands:   
   

```shell
cmake -B build
cmake --build build
```

   

3. Run the program by using on the terminal:  
   

for mac:

```shell
	build/bin/Oasis
```

for windows:

```shell
	build/bin/Oasis.exe
```

## Controls

**Camera zoom**: scroll wheel  
**Camera pan:** drag and hold

**Tower placement:** left click to select a tower \-\> hover and left click to place  
**Sell Tower:** right click on a tower to open pop up  
**Cornucopia/water mine interaction:** left click on the tower