# P2P chat dokumentáció

## Tartalom jegyzék

- [Specifikáció](@ref specs)

- [Felhasználói útmutató](@ref user-manual)

- [Fordítás](@ref compile)

## Fordítás {#compile}
A programkód lefordításához szükséges a [cmake](https://cmake.org/) minimum 3.10-es verziója.
Ha ez megvan akkor a programkód gyökérkönyvtárában a következő parancsokat kell lefuttatni:
```shell script
mkdir build
cd build
cmake ..
```
Ez létrehozza a megfelelő fordító környezetét.
### Linux,MinGW vagy CygWin
A program lefordításához csak a következő parancsot kell lefuttatni:
```shell script
make
```
### Visual Studio
Nyissuk meg a kapott `build` mappát a Visual Studio `Developer Command Prompt`-ban és futtassuk le az alábbi parancsot:
```shell script
MSBuild p2p.sln
```
