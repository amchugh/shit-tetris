
# Shit Tetris!
This project started off by a recommendation by my friend Josh Moskoff. The idea is that "it's Tetris, but only the vertical line piece". My friend Alma Zimberoff then gave me the idea to poorly recreate the Tetris music using my poor sequencing abilities. 

## Game
The game is very simple. The arrow keys move the piece left and right while the up arrow drops the piece immediately. With every completed line the "pieces" (line piece) drops faster. At a certain point the pieces drops so fast it's impossible to continue. 
There's no rotation. This made coding the game quite trivial and is why I was able to make it in just a day. The music is what took me the most time.

## Running the game
The game should be quite trivial to build through the use of cmake. This project is built using [raylib](https://github.com/raysan5/raylib). 
The CMakeLists includes instructions to automatically fetch, build, and link raylib into the shittetris project. No extra work should be needed.

To build, try running the following commands:
```
mkdir build
cd build
cmake ..
cmake --build .
```

At this point a `shittetris` executible file should be created. You should be fine to move this file, but make sure the `rsc` directory is in the same location as the executible at runtime or else it will fail to grab resources. 
This project was made very quickly. I don't know what will happen if it fails to find the resource files lol.

If these commands fail, check out the raylib github page. The most likely error is that your machine does not have the appropriate dependencies for raylib installed.
