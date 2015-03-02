# DaisyStats

Data Visualization as a stylized flora

![wildflowers](/media/wildflowers.png)

# What is Daisy Stats?

This is a simple command line utility that will display spreadsheet data in the form of flowers instead of more traditional pie charts, bar charts, cheesecake charts etc. It was created as a distraction and is kind of fun to play with so it has been wrapped up and published.

# Usage

The simple way to use Daisy Stats is to expert a .csv file from a spreadsheet program and drag it to the program icon. This is works great on PC but not on mac. The workaround is to open a terminal window (click the magnifying glass in the upper right corner, type terminal and hit enter) then drag the mac executable (samples/daisystats_mac) into the termina window followed by the .csv file and then hitting enter on the keyboard.

The program will run for a little while and generate quite a large image in the same folder as the .csv file. Fast performance is not a priority for this project. There are a variety of csv file samples to review in the samples folder along with pre-built executables.

# Compiling

This project is a single source file (daisystats.cpp) and has a dependency on Sean Barrets std header file implementations of image loading, image saving and truetype rendering (https://github.com/nothings/stb)

# Command Line Arguments:

daisystats [a=<num>] [s=<num>] [f=<shape>] [o=<condition>] [p=<csv file>] [r=<num>] [d=<csv file>] [b=<color>] [input.csv] output.png

All command line arguments can also be rows in the top of input.csv file
If the only command line argument is a csv file a variety of guesses for command line arguments will be made based on the contents of that file.

* a/aspect=<num> : aspect ratio for round or rect, divide width by height
* b/background=<color>/<image file> : set background color or image
* d/data=<csv file> : Load in a csv file that has values to represent
* f/font=<font>: The font for legend, font should have a .ttf extension.
* l/legend=<height> : add a legend of the data names at the bottom
* c/color=<color> : set text color
* m/make=<shape> : Make a shape. Shape is one of: round, rect, first, last
* o/order=<condition> : Packing order. (orig, large, small, shuffle, name)
* p/preset=<csv file> : Load in a csv file of preset flowers
* r/random=<num> : Create <num> random sized flowers, uses presets
* s/size=<num> : Make the result fit within this size (max width or height) with given line height
* t/title=<size>:<name> : Add a title on the top of the page.

 Note about colors:
   The color parameter can be 0-15 for c64 palette, 000-fff for amiga, 0000-ffff amiga+alpha,
   000000-ffffff for web or 00000000-ffffffff for web+alpha or color name. Valid color
   names include old classics such as transparent, black, white, orange, blue, azure,
   teal, jade, green, cyan, turquoise, taupe, indigo, taupe, emerald, coffee, chocolate,
   aqua, maroon, burgundy, purple, olive, gray, brown, lavender, lime, lilac,
   periwinkle, bronze, pear, tan, orchid, crimson, cerise, ruby, violet, beige, coral,
   red, scarlet, pink, yellow, white.



# CSV (excel, pages, google sheets, etc.) format:

 any of the following strings in the top row will define that column to define a range of that property:
* value: the value (circle area) of data/Users/Carl-Henrik/Documents/flower_presets.csv
* name: optional value name
* center: proportion of diameter of flower that is the center of the flower in percent (~15-50)
* angle: angle in degrees
* petal: width of petals as percent, 100 is widest, 0 is thinnest
* color1: color of petals
* color2: color of center
* linear1: linearity of petal color randomization in %, 0 means box space 100 means straight line
* linear2: linearity of center color randomization in %
* flat: petal ends rounded at 0.0 and flat at 1.0
* type: type of flower, can be either 8 or 4 (petal count)

 To export excel, pages or any other spreadsheet software's proprietary data format to CSV, look in the file menu for an option to "Export" "Publish" or "Download As" to locate the CSV option. All spreadsheet software can open CSV files.

 Any value can be defined as a range and flowers will be created randomly within that range. By default the full range is used but can be overridden by any of the columns named above. To declare the range just add 'to' within the cell, for example: 'white to black' means all colors or '15 to 50' means any number between 15 and 50.

 For colors in spreadsheets, see command line arguments

Latest Version / More Info: https://github.com/sakrac/daisystats

![random preset last shuffle](/media/random_rgb_preset_last.png)
