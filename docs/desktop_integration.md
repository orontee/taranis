# Desktop integration

## Application icon

To customize the application icon and group:

1. One must copy two icons in BMP format on the e-reader (eg under
   `/mnt/ext1/application/icons/`). One is used to represent the
   application in the "desktop" view, the other is displayed
   temporarily when the application is launched.
   
   Samples icons are provided: [icon_taranis.bmp](./icon_taranis.bmp) and
   [icon_taranis_f.bmp](./icon_taranis_f.bmp).
   
2. Then the `/mnt/ext1/system/config/desktop/view.json` file must be
   edited on the e-reader (path may vary depending on the use of
   profiles):
   
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
   +            "icon": "/mnt/ext1/applications/icons/icon_taranis.bmp",
   +            "focused_icon": "/mnt/ext1/applications/icons/icon_taranis_f.bmp"
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

⚠️ Note that comments describing the format and possible attributes
contain a typo: One must use the `params` (plural!) attribute to pass
parameters to an application.

## Shutdown logo

1. Start Taranis, open the `parameters` menu and enable generation of
   shutdown logo.
   
   From now on, each model change will update the image file located at
   `/mnt/ext1/system/logo/offlogo/taranis_weather_forecast.bmp`.

2. Open "Appearance / Logos" screen of the device configuration
   application. In the poweroff logo item select
   `taranis_weather_forecast.bmp`.
