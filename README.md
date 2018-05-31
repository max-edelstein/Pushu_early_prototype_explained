# Pushu_early_prototype_explained
This example is chock-full with source code from my video game "Pushu" that I documented.
In my code is shown how a sprite can be manipulated to display
sprites for 8 directions, even though only four directions of movement
are possible. Even though if only four directions are set, it is still 
possible to move in eight directions. So if anything this example is a more
visual one. I discovered this by experimenting randomly.
I found it funny because I eventually had to slow down the diagonal 
directions because they went faster than the normal directions. Meaning
left, right, up, and down. Note how the other sprites don't scroll unless 
one moves the other sprites in a way that it makes sense with reality.
Note how scrolling in vram the data set or the map which is 20 x 18 is displayed,
while you can move to the areas where the tile data has been filled with the first tile
by default. This example is here because I believe I have well documented it even though
some things look odd. Such as how the "A" block sprite moves with you as you scroll.
This example was the one I wanted to share because I believe it was the easiest to understand.
I hope this example can help one learn. It was made using c language for GBDK only. I wanted to give credit
to the creator of Burly Bear Sebastian Mihai and his website is "http://sebastianmihai.com/main.php?t=55". 
I examined his code of how he used scrolling and I implemented it in c using the scroll_bkg() function instead.
Most of the other stuff in this source I made by randomly experimenting to see what would work and what wouldn't.
