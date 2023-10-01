# Taranis icons

## Sources

Weather icons come from
https://openweathermap.org/weather-conditions#How-to-get-icon-URL.

Other icons come from 
https://fonts.google.com/icons.

## Format

Icons are expected to be in BMP format with ≤256 colors. 

```sh
$ identify icons/icon_menu.bmp
icons/icon_menu.bmp BMP 100x100 100x100+0+0 1-bit sRGB 2c 1746B 0.000u 0:00.000
```

PNG to BMP conversion can be done using [Gimp](https://www.gimp.org)
(use grayscale color mode, 8-bit precision and flatten image to get
rid of the alpha channel).

The icon file name must be a valid C variable name since `pbres` is
used to convert the icon to a char array named after the file. See
`icons_c` target in the parent [meson.build](../meson.build).
