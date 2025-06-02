# Wedding Seating Cards - For 68k Macintosh Computers

## Introduction
This program shows a UI with a alphabetical list of wedding guest names, allows users to filter, and select their name, see their table assignment, and print a receipt with more information.

This project was an intense struggle. The hardware used is very different in age and overall capabilities. Writing software for computers with limited memory, little or no available technical information, and with deprecated hardware is not easy. 

This project successfully bridged 1980s Mac hardware with modern ESC/POS printing, navigating physical signaling, firmware limitations, and software quirks to produce crisp, stylized prints. A perfect blend of vintage computing and modern convenience!


## Requirements

- Macintosh Plus with mouse
- BlueSCSI or similar SCSI interface
- Blank `hda` disk image
- Mini DIN 8 to DB25 serial cable (custom pinout below)
- Receipt Printer with DB25 interface (RS-232)
- Retro68 compiler

## Build Steps

Run the build script to execute all build steps.
The deploy part doesnt work because of permissions issues. Copy and paste the destination `.hda` file manually to the BlueSCSI SD card.
```sh
./build-and-deploy.sh
```

## Bitmap Preparation

The printer does not support QR code printing via `ESC/POS` commands. The following steps convert a QR code image, created elsewhere, to a bitmap image raw data placed in a `.h` file.

### Make the QR code BMP

Use this website to resize the image to `200x200px` and convert it to `.bmp` format. Select monochrome colors.

https://online-converting.com/image/convert2bmp/ 

Run the following Python script to convert the bitmap to raw data.
```sh
python3 convert-bmp.py wedding-snapshare-code-2.bmp qr.bin 
```

Run the following command to prepare a `.h` file to call from our C program.
```sh
xxd -i qr.bin > qr_data.h 
```

In our C program...
```c
// include the qr data
#include "qr_data.h"
...
// use the qr data array
qr_bin
```

## Serial Cable Pinout

Very few of the available pins are required for this to work. The resulting cable is likely not useful for generic serial IO as this cable is designed for Tx only to a device that does not require DSR/DTR signals.

```
Din-8 | DB-25
  7 ----- 8
  3 ----- 3
  8 ----- 7
  2 (1k to GND)
```

All unlisted pins on either side are not connected.
