# Walld

A wallpaper daemon. Walld automatically sets and changes the wallpaper. It sets the wallpaper through feh. It changes the wallpaper by default every 30 minutes. It also has a feature that grabs the 16 most prominent colors from the wallpaper that it has set. It then modifies the colors according to your preferences in the config file. After the colors have been modified, walld puts them in a file located at $HOME/.walld/colors. You can include this file in your .Xresources. Walld as a convenience then reloads your .Xresource with xrdb.

## Usage

Walld is meant to be started by your xinit or started in your i3 config. It can also be started in a terminal as well.

`$ walld`

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

#### Step 1:

Download latest release from Github

#### Step 2:

`$ make`

#### Step 3:

`$ make PREFIX="/install/Path" install`

Step 3 may need to be ran as root

Walld will now be installed

## Config

Walld has a config located at $HOME/.walld/.walldrc

If it does not exist walld will make a new one.

Walld uses libconfig for handling config files.

The config can be used to set many things.

| Option | Description | Default |
| :----------- | :-------: | ----------- |
| Colors | This tells walld whether or not to generate colors and put them in a file located at $HOME/.walld/colors | FALSE |
| Dark | When walld generates colors, it modifies them to make them darker or lighter. If Dark is true it make the colors better fit a dark color scheme. If it is false it makes the colors fit a lighter color scheme. | TRUE |
| Use-xrdb | Set this to true to have walld automatically reload your Xresources when it generates a new color file. | FALSE |
| Minutes | This tells walld how many minutes to wait before changing the wallpaper. | 30 |
| Source_to_use | Walld can use many different sources for images. This tells walld what source to use. | "default" |
| Default | This is the default source for walld to use. | "$HOME/.walld/images" |
| Feh-path | This is where the feh executable is located. | "/usr/bin/feh" |
| Bg-style | This is an option that walld passes to feh when it sets the wallpaper. | "--bg-scale" |
| XAUTHORITY | Walld usually gets this from environment variables. If you have something custom you can set it here. | not set |
| Display | Walld usally gets this from environment variables. If you have something custom you can set it here. It is used to tell feh what display to set the wallpaper on. | not set |
| Xrdb-path | This is where the xrdb executable is located. | "/usr/bin/xrdb" |
| Xrdb-args | These are the arguments passed to xrdb. If you have something custom you can set it here. | "--load" |
| Xresources | This is your .Xresources that xrdb will load. If you have something custom you can set it here. | "$HOME/.Xresources" |

## Signals

Walld can respond to serveal diffrent signals

| Signal | Action |
| :---   | :----- |
| SIGHUP | Reload config |
| SIGUSR1| Advances the wallpapers by one |
| SIGUSR2| Reshuffles wallpapers |

## License
This project is released under the GNU Public License Version 3 or Later.

See COPYING for more details