# Fly Airport Game

## Build and run

### Clone
Clone this repository
```
git clone https://github.com/dangarcar/fly
cd fly
```

### Install SDL
#### Windows (MinGW)
- Download https://github.com/libsdl-org/SDL/releases/tag/release-2.30.8 MinGW release and unzip it in `lib\`.
- Download https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.2 MinGW release and unzip it in `lib\`.
- Download https://github.com/libsdl-org/SDL_ttf/releases/tag/release-2.22.0 MinGW release and unzip it in `lib\`.

Now SDL2's `README.txt` should be in `lib\SDL2\README.txt`

#### Linux (Ubuntu)
Run the following commands:
```
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
```

### Compile
Run this command:
```
python compile.py
```

### Run
Run the executable `Fly` on linux or `Fly.exe` on Windows
> [!WARNING] 
> The executable can only be run on the `fly` folder, it can't be moved

