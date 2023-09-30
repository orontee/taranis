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

## Desktop integration

To customize the application icon and group:

1. One must copy two icons in BMP format on the e-reader (eg under
   `/mnt/ext1/application/icons/`). One is used to represent the
   application in the "desktop" view, the other is displayed
   temporarily when the application is launched.
   
   Samples icons are provided: [taranis.bmp](./taranis.bmp) and
   [taranis_f.bmp](./taranis_f.bmp).
   
2. Then the `/mnt/ext1/system/config/desktop/view.json` file must be
   edited on the e-reader (the file path may vary depending on the
   use of profiles):
   
   ```diff
   --- "/mnt/ext1/system/profiles/matthias/config/desktop/view.orig.json"	2023-09-30 20:20:34.454795306 +0200
   +++ "/mnt/ext1/system/profiles/matthias/config/desktop/view.json"	2023-09-30 20:21:06.283101211 +0200
   @@ -27,7 +27,13 @@
                "     \"focused_icon\": { \"path\": \"applications/icons/my_calc_icon_f.bmp\" }\n",
                "   }\n",
                "end."
   -        ]
   +        ],
   +        "U_Taranis": {
   +            "path": "/mnt/ext1/applications/taranis.app",
   +            "title": "Taranis",
   +            "icon": "/mnt/ext1/applications/icons/taranis.bmp",
   +            "focused_icon": "/mnt/ext1/applications/icons/taranis_f.bmp"
   +        }
        },
        "view": {
            "_comment": [
   @@ -57,7 +63,8 @@
                        "PB_Onleihe",
                        "PB_Player",
                        "PB_RSSNews",
   -                    "PB_Settings"
   +                    "PB_Settings",
   +                    "U_Taranis"
                    ]
                },
                {
   ```
