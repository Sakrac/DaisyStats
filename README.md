# DaisyStats

Data Visualization as a stylized flora

![wildflowers](/media/wildflowers.png)

# What is Daisy Stats?

This is a simple command line utility that will display spreadsheet data in the form of flowers instead of more traditional pie charts, bar charts, cheesecake charts etc. It was created as a distraction and is kind of fun to play with so it has been wrapped up and published.

# Usage

The simple way to use DaisyStats is to export a .csv file from a spreadsheet program and drag it to the program icon. This is works great on PC but not on mac. The workaround is to open a terminal window (click the magnifying glass in the upper right corner, type terminal and hit enter) then drag the mac executable (samples/daisystats_mac) into the termina window followed by the .csv file and then hitting enter on the keyboard.  

The program will run for a little while and generate quite a large image in the same folder as the .csv file. Fast performance is not a priority for this project. There are a variety of csv file samples to review in the samples folder along with pre-built executables.

# Compiling

This project is a single source file (daisystats.cpp) and has a dependency on Sean Barrets std header file implementations of image loading, image saving and truetype rendering (https://github.com/nothings/stb)

# Command Line Arguments:

daisystats [a=&lt;num&gt;] [s=&lt;num&gt;] [f=&lt;shape&gt;] [o=&lt;condition&gt;] [p=&lt;csv file&gt;] [r=&lt;num&gt;] [d=&lt;csv file&gt;] [b=&lt;color&gt;] [input.csv] output.png  

All command line arguments can also be rows in the top of input.csv file.  
If the only command line argument is a csv file a variety of guesses for command line arguments will be made based on the contents of that file.  

* a/aspect=&lt;num&gt; : aspect ratio for round or rect, divide width by height
* b/background=&lt;color&gt;/&lt;image file&gt; : set background color or image
* c/color=&lt;color&gt; : set text color
* d/data=&lt;csv file&gt; : Load in a csv file that has values to represent
* f/font=&lt;font&gt;: The font for legend, font should have a .ttf extension.
* l/legend=&lt;height&gt; : add a legend of the data names at the bottom
* l/legend=inside : show the legend inside the flowers instead of the bottom
* n/name_color=&lt;color&gt; : Use a separate color for names than the title
* m/make=&lt;shape&gt; : Make a shape. Shape is one of: round, rect, first, last
* o/order=&lt;condition&gt; : Packing order. (orig, large, small, shuffle, name)
* p/preset=&lt;csv file&gt; : Load in a csv file of preset flowers
* r/random=&lt;num&gt; : Create &lt;num&gt; random sized flowers, uses presets
* s/size=&lt;num&gt; : Make the result fit within this size (max width or height)
* t/title=&lt;size&gt;:&lt;name&gt; : Add a title on the top of the page.

### Note about colors
The color parameter can be 0-15 for c64 palette, 000-fff for amiga, 0000-ffff amiga+alpha,
000000-ffffff for web or 00000000-ffffffff for web+alpha or color name.  
Valid color
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

# Example Spreadsheet

This table generates the wildflowers image above.

name|value|petal|color1|color2|center|flat|type
----|-----|-----|------|------|------|----|----
Orange Coneflower|28 to 36 cm²|5|#FED600 to #FEA000|#5C3436|15|0|8
Black-Eyed Susan|28 to 36 cm²|10|#FCDB07 to #FCF007|#723B2F|25|0|8
Purple Coneflower|40 to 50 cm²|10|#F389D7 to #F3B0F0|#5C3400|15|0|8
Woodland Sunflower|40 to 50 cm²|20|#F9E400 to #F9C000|#9C5B0C to #D0A010|25|0|8
Chicory|4 to 8 cm²|10|#B8C3F2 to #A0B0FF|#ECDBE1|10|0.5|8
Calico Aster|1 to 1.5 cm²|5|#D0D0FF to #F0F0FF|#F9FA06 to #F98006|20|0.1|8
Prickly Lettuce|8 to 10 cm²|85|#EFEFa0|#d0d060|30|1|8
Indian Blanket|18 to 22 cm²|90|#F54F53|#824D3F to #c0703f|25|0.2|8
Sneezeweed|10 to 13 cm²|75|#d4D405|#999903 to c0c003|35|0.75|8
Oxeye Daisy|10 to 13 cm²|30|#E7F2F8|#e0f309 to #f3e009|25|0|8
Chickweed|0.25 to 0.5 cm²|5|#FEF8FC|#CAE66F|15|0|8
Field Mustard|0.75 to 1.25 cm²|15|#FFEF0F|#FFEF0F|10|0|4
Bluets|0.75 to 1.25 cm²|10|#BAB7EC|#DDD954 to #b0b060|15|0|4
Flowering Dogwood|40 to 50 cm²|3.5|#F5F6F0 to #f58080|#c5c94B|10|0.25|4
Honesty|6 to 10 cm²|5|#FE95F8 to #e040ff|#B8B278|5|0|4
Western Anemone|8 to 12 cm²|40|#FDE5BA|#E68E49 to #e0f049|30|0|8
Bunchberry|8 to 12 cm²|15|#E9d7e0|#d7aD66|25|0|4
Dame's Rocket|1.5 to 2.5 cm²|28|#CF9EED to #DFB0FD|#AF3F7A|5|0|4


Latest Version / More Info: https://github.com/sakrac/daisystats

![random preset last shuffle](/media/random_rgb_preset_last.png)
