# NyaSweeper - quick and dirty minesweeper clone

## Try it out

From [releases](todo)

## Building

1. Initialize submodules: `git submodule update --init --recursive`
2. Build with CMake: `cmake -B build . && cmake --build build`

## Todo

- [ ] Create web build
- [ ] Fix sounds - they sound bad
- [ ] Add configuration file instead of baking the settings into the code

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

Raylib is licensed under the zlib/libpng License - see the [Raylib LICENSE](https://github.com/raysan5/raylib/blob/d582becbc2a358202de65cac39bd68ed9148a731/LICENSE) file for details.

About resources:

- All Sounds, excluding [5.wav](assets/sfx/5.wav) that is considered part of the software, are taken from [this video](https://www.youtube.com/watch?v=9RE0yHwb2oM). It doesn't list orginal authors or license. I do not claim ownership of them.
- Sprites are a mix of those taken from google images and then modified by me:
    - [nyaa](assets/sprites/individual/nyaa.png)
    - [ough](assets/sprites/individual/ough.png)
    - [chaos](assets/sprites/individual/chaos.png)
    I do not claim ownership of them, all rights belong to their respective owners.

    and those that I made myself - the rest. They are considered part of the software and are licensed under the same MIT License.
