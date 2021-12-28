# Walld

A wallpaper daemon. Walld automatically sets and changes the wallpaper. It sets the wallpaper through feh. It changes the wallpaper by default every 30 minutes. It also has a feature that grabs the 16 most prominent colors from the wallpaper that it has set. It then modifies the colors according to your preferences in the config file. After the colors have been modified, walld puts them in a file located at $HOME/.walld/colors. You can include this file in your .Xresources. Walld as a convenience then reloads your .Xresource with xrdb.

## Usage

Walld is meant to be started by your xinit or started in your i3 config. It can also be started in a terminal as well.
Values passed by arguments will take priority over values in the config.

`$ walld`

`Usage: walld [-c config] [-t time] [-s source]`

A custom config can be passed with `-c`

A custom time can be passed with `-t`

A custom source can be passed with `-s`
See Config section for more info

## Installation

It is available on the arch AUR.

find it under

`walld`

Walld can also be installed by makefile

### Dependencies:
```
feh
libconfig
imagemagick
```

### Optional Dependencies:
```
xorg-xrdb
```

#### Steps:

1. Download latest release from Github.

2. `$ make`

3. `$ make PREFIX="/install/Path" install`

## Config

Walld has a config located at $HOME/.walld/.walldrc

You can pass a custom config in with `-c`

If it does not exist walld will make a new one.

Walld uses libconfig for handling config files.

| Option | Description | Default |
| :----------- | ------- | ----------- |
| Colors | This tells walld whether or not to generate colors and put them in a file located at $HOME/.walld/colors | FALSE |
| Dark | When walld generates colors, it modifies them to make them darker or lighter. If Dark is true it make the colors better fit a dark color scheme. If it is false it makes the colors fit a lighter color scheme. | TRUE |
| Use-xrdb | Set this to true to have walld automatically reload your Xresources when it generates a new color file. | FALSE |
| Minutes | This tells walld how many minutes to wait before changing the wallpaper. | 30 |
| Source_to_use | Walld can use many different sources for images. This tells walld what source to use. | "default" |
| Default | This is the default source for walld to use. | "$HOME/.walld/images" |
| Feh-path | This is where the feh executable is located. | "/usr/bin/feh" |
| Bg-style | This is an option that walld passes to feh when it sets the wallpaper. | "--bg-scale" |
| XAUTHORITY | Walld usually gets this from environment variables. If you have something custom you can set it here. | not set |
| Display | Walld usually gets this from environment variables. If you have something custom you can set it here. It is used to tell feh what display to set the wallpaper on. | not set |
| Xrdb-path | This is where the xrdb executable is located. | "/usr/bin/xrdb" |
| Xrdb-args | These are the arguments passed to xrdb. If you have something custom you can set it here. | "--load" |
| Xresources | This is your .Xresources that xrdb will load. If you have something custom you can set it here. | "$HOME/.Xresources" |

## Signals

| Signal | Action |
| :---   | :----- |
| SIGHUP | Reload config |
| SIGUSR1| Advance the wallpapers by one |
| SIGUSR2| Reshuffle wallpapers |

## License
This project is released under the GNU Public License Version 3 or Later.

See COPYING for more details.
