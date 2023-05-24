# tunnel-survivors

## Play in browser:

[https://szmergiell.github.io/tunnel-survivors/](https://szmergiell.github.io/tunnel-survivors/)

Click the image to watch the gameplay:

[![Watch the video](https://img.youtube.com/vi/gDEtkuTBouU/default.jpg)](https://youtu.be/gDEtkuTBouU)

Help detective Rama break through the old tunnel to the surface of Gyala Delve to put an end to the demons of the depths.

## Controls

- Press `ENTER` to start a game.
- Press `ESC` to exit to main menu or exit the game.
- Use `WASD`, arrow keys or `hjkl` to control character.
- Use mouse to aim and left mouse button to attack with Dragon Slash.

## Dependencies

- SDL2
- SDL2_image
- SDL2_ttf

## Build

Remember to copy assets to output folder.

### Linux

```
make release
```

### Windows

`windows` branch contains a Visual Studio project ready to be built.

- Beyond installing SDL2 libraries with `vcpkg` I also had instal SDL2-Image with specific feature to support JPG `vcpkg install sdl2-image[libjpeg-turbo]:x64-windows --recurse`.

## Motivation and goals

This is an MVP game in "Vampire Survivors"-like genre. It is based on Guild Wars 2 meme.

Unfortunatelly neither the meme, nor the game is fully realized. There are not enough references to the meme for the game to be funny. And the game itself is pretty bare bones. It lacks certain features characteristic to survivor-like genre, mainly progression, random power-ups and auto-attacking. _(In my game player aims and attacks with mouse, otherwise the game would be even less engaging)_.

Still, in some sense I would consider it a success. At the beggining of May 2023 Arena Net announced their newest End of Dragons content "What Lies Within" to be released on 23 of May, 2023. Around that time I wanted to try a low level language (C), so I thought it would be a great idea to challenge myself to write a simple core game loop, (mostly) from scratch, before that 23rd May dead-line, so that the tunnel meme would still be relevant.

Although the game and code quality is low, because of the limited time, I've still learned a lot. It was interesting to see that when I'm "investing" my own time I am more willing to let the code quality slide and focus just on delivering the "product" / "value". Which stands in contrast to our day jobs, where we sometimes spent too much time caring about code quality.
