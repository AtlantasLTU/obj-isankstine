# Programos naudojimas:
  ## Reikalavimai:
    C++ kompiliatorius su C++20 palaikymu.
    Unix OS (Linux arba MacOS) arba WSL (Windows Subsystem for Linux)
    git
    ICU 78 - International Components for Unicode
  ## Programos diegimas iš source:
    git clone https://github.com/AtlantasLTU/obj-isankstine.git
    cd ./obj-isankstine
  ### main :
    make main
    ./main
  ### clean (ištrina main programą):
    make clean
  ## Programos naudojimas:
  ### main:
    Funkcijos:
        Galima pasirinkti tekstinį failą, kurį norima apdoroti iš programos direktorijos.
        Suskaičiuoja žodžių pasikartojimų kiekį ir išveda rezultatus į zodziai.txt
        Sukuria cross-reference tipo lentelę ir išveda į cross_ref.txt
        Suranda URL adresus tekste ir išveda į urls.txt
