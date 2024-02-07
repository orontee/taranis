# Taranis icons

## Sources

Weather icons come from
[OpenWeather](https://openweathermap.org/weather-conditions#How-to-get-icon-URL).

Other icons come from [Google
Fonts](https://fonts.google.com/icons). Use a 200 weight for 24px
optical size, then export to SVG.

Icons meant to be used in configuration editor are rendered on a 75x75
canvas that is then enlarged to 100x100.

Conversion from SVG to BMP can be done using
[Gimp](https://www.gimp.org) (use grayscale color mode, 8-bit
precision and flatten image to get rid of the alpha channel).

## Format

Icons are expected to be 100x100 in BMP format with ≤256 colors.

```sh
$ identify icons/icon_menu.bmp
icons/icon_menu.bmp BMP 100x100 100x100+0+0 8-bit sRGB 256c 11162B 0.000u 0:00.000
```

The icon file name must be a valid C variable name since `pbres` is
used to convert the icon to a char array named after the file. See
`icons_c` target in the parent [meson.build](../meson.build).
