#include <gb/gb.h>
#include <gb/cgb.h>
#include "sprite_test_twentyfr.c"   // the sprite data
#include "bkg_tiles_eleven.c"    // the background tile data was nine
#include "background_test_twelve.c"   // the created map was nine



 /* THE GAME BOY IS READ ONLY MEMORY, "ROM", THEREFORE EVERYTHING IS LOST WHEN YOU TURN OFF THE GB */


// this is a temp change to 'ani_tempo' variable for looping animations
 UINT8 ani = 16;
    
UINT8 can_be_damaged;   // if 1 then the player can be damaged, false if 0

unsigned int i_timer;  // is used to hold the invincibility timer

UINT8 play_walk_sound;  // flag to play walking sound only if walking currently

// this is the sprites colors I made in game boy tile designer
UWORD spritePalette[] = 
{
    RGB(18,18,18), RGB(30,27,20), RGB(31,31,31), RGB(0,0,0),
    RGB( 18,18,18), RGB(3,6,31), RGB(30,31,8), RGB(0,0,0),
    RGB( 31,31,31), RGB(31,31,31), RGB(31,31,31), RGB(31,31,31)  // this is to get the blink
};



UWORD backgroundPalette[] = 
{
    RGB(5,31,5), RGB(0,0,0), RGB(0,0,0), RGB(0,0,0),   // the color green with black
    RGB( 15,15,15), RGB(15,15,15), RGB(15,15,15), RGB(15,15,15),   // the color green was 5, 31, 5
    RGB(1,13,1), RGB(0,22,13), RGB(24,30,22), RGB(31,15,8),
    RGB(8,24,31), RGB(0,10,26), RGB(16,23,30), RGB(7,0,20),
    RGB(31,0,0), RGB(31,31,0), RGB(30,17,28), RGB(25,3,31)
   
};


 // dx represents the change in the x coordinate, dy represents the  change in the y coordinates
// ultimately the variable represents player movement.
// essentially this is the player, invisibly. Or the magic behind the code of video games
// think of it as an invisible box that is the players/ sprites soul
UINT8 player_dxdy[2];  // this is to be used for the sprites coordinates

// if higher setting cover more ground
UINT8 player_distance;  // this holds the current distance covered setting of the player on the dxdy movement

// if higher faster movement
UINT8 player_speed[2];  // this holds the pace at which the player moves, ex - faster moving dxdy, not more ground covered in moving

UINT8 block_dxdy[2];  // this is used to hold the coordinates of a new sprite



// this is a set of flags to hold the players current direction
UINT8 dir[8];  // declared an array of 4, for down, up, left, right


UINT8 damaged;  // if is = 1, player loses a health point, else player can't loose a health point
UINT8 player_health;  // is the players health currently either 3 or 6


 
// Function Prototypes
// void scroll();
void set_initialize_game();  // initializes stuff for the video game
void set_check_input();  // checks constantly for player input and coll D
void set_check_switches(); // required for game, must be set last
void set_tapu_attack();  // this is to set the push direction
void set_faced_sprite_direction();  // this is to keep the sprite in the faced direction when no direction key is being pressed
UINT8 collision_detection(UINT8 x1, UINT8 y1, UINT8 w1, UINT8 h1, UINT8 x2, UINT8 y2, UINT8 w2, UINT8 h2 );   // checks for sprite collision
void set_tim(); // this is for an interrupt test
void set_resolve_collision();   // determines direction and resolves collision if colliding
void set_play_death();  // checks to see if the player has been deaded
void set_temp_invincibility();  // keeps the player from receiving damage if they just took damage

void main()     // the start of the main method, in this case returns nothing
{

 


   // set the players position initially
   player_dxdy[0] = 80;
   player_dxdy[1] = 64;

   player_health = 3;  // the players health initially
   damaged = 0;  // the player is currently not taking any damage
   can_be_damaged = 1;  // player can take damage
   i_timer = 0;  // invincibility timer should not be on

   play_walk_sound = 0;  // when player starts they are not walking

   player_distance = 1;  // current distance covered (dxdy) is one pixel at a time movement, this may vary
 
   // this will be placed in the delay(player_speed[#] function
  // two of them because moving diagonal is faster than normal directions
   player_speed[0] = 14;  // covers how fast the player can move when arrow key is pressed
   player_speed[1] = 12; // this is for normal controls cause was too slow

  // this is to set the 'A' blocks position initially
   block_dxdy[0] = 50;       // was 64
   block_dxdy[1] = 50;   // was 128
   
 


   // required settings to start the game, has to do with gb/gbc hardware   
    set_initialize_game();

    
    
   while(1) // this is the video game loop, a necessary infinite loop
   {
    
       
       set_check_input(); // checks for player input, Ex- moving
       set_resolve_collision();  // resolves collision detection
       set_temp_invincibility();  // sets the invinciblity frames 
       set_play_death();  // resolves if the player has died
       set_tapu_attack();  // lets one use the push attack
       set_check_switches(); // must be last to show obj and bkg
       wait_vbl_done(); // done to not corrupt visual memory
   }  // end of video game while loop


} // end of the main method



// checks for player input and handles animation depending
// on the direction and or buttons pressed
void set_check_input()
{

   // this code plays the normal walking sound, which is just a bunch of sound registers playing a sine wave 
if (play_walk_sound )
 { // NR10_REG = 0x50;
 //  NR11_REG = 0x83;
 //  NR12_REG = 0x64;
 //  NR13_REG = 0x6C;
 // NR14_REG = 0x60;
   NR21_REG = 0x41;   // was 0x41
   NR22_REG = 0x41;   // was 0x84
   NR23_REG = 0xBB;   // was 0xD7
   NR24_REG = 0x86;   // was 0x86
   NR30_REG = 0x00;
 //  NR31_REG = 0x01;
 //  NR32_REG = 0x20;
   NR33_REG = 0xAD;
   NR34_REG = 0x86;
 //  NR41_REG = 0x3A;
 //  NR42_REG = 0xA2;
 //  NR43_REG = 0x31;
 //  NR44_REG = 0x80;
  // NR50_REG  = 0x77; 
   NR51_REG = 0xFF;    // WAS |= 0x11
   NR52_REG = 0x80;  // required to turn on
  }   // end of if (play_walk_sound )



       // this is to move up and right, this function does work
         // this requires fixing cause the sprite head is odd
        if( (joypad() & J_UP) && (joypad() & J_RIGHT) || (joypad() & J_RIGHT) && (joypad() & J_UP)  ) // added J_UP
       {
   
        
          // the below will scroll everything around the sprite, since the engine I want will make the sprite stationary but use animation
         // to give the illusion of movement
        scroll_bkg(player_distance, -player_distance);   // first param is x dir, second param is ydir        
      
             

          // this is to hold the current players direction
      dir[0] = 0;  // 1 down is true, 0 down is false
      dir[1] = 0;  // 1 up is true, 0 up is false
      dir[2] = 0;  // 1 left is true, 0 left is false
      dir[3] = 0;  // 1 right is true, 0 right is false
      dir[4] = 0; // 1 up and left is true, 0 up left is false
      dir[5] = 1; // 1 up and right is true, 0 up right is false
      dir[6] = 0; // 1 down and left is true, 0 down left is false
      dir[7] = 0; // 1 down and right is true, 0 down right is false


       // when needed, this will display the backward head
       set_sprite_tile(0, 0);
       set_sprite_tile(1,2);
       set_sprite_prop(0,0);
       set_sprite_prop(1,0);
       move_sprite(0, player_dxdy[0], player_dxdy[1] );
       move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );

       
      
           // this is for the tech on body
      set_sprite_prop(2,1);
      set_sprite_prop(3,1);
       set_sprite_tile(2,124);
       set_sprite_tile(3,126);
       move_sprite(2, player_dxdy[0], player_dxdy[1] + 14);
       move_sprite(3, player_dxdy[0] + 8, player_dxdy[1] + 14);
      move_sprite(3, 200, 200);

       // this is for a up right body 
      // 14 so head is closer to body
       set_sprite_tile(4,44);
       set_sprite_tile(5,46);
       set_sprite_prop(4,0); // to set to skin color
       set_sprite_prop(5,0); // to set to skin color
       move_sprite(4,player_dxdy[0], player_dxdy[1] + 14);
       move_sprite(5,player_dxdy[0] + 8, player_dxdy[1] + 14);

           
      //    scroll_sprite(0,1,-1);
      //    scroll_sprite(1,1,-1);
      //    scroll_sprite(2,1,-1);
      //    scroll_sprite(3,1,-1);
       //   scroll_sprite(4,1,-1);
       //   scroll_sprite(5,1,-1);

            
    
            play_walk_sound = 1;   // the play is indeed walking

            delay(player_speed[1]);      // had to slow it down, because diagonal movement is faster than normal straight movement
           

        } // end of moving right
       else if( (joypad() & J_UP) && (joypad() & J_LEFT) || (joypad() & J_LEFT) && (joypad() & J_UP)  ) 
        {
  
           // to scroll the player up and left, well player is stationary but background moves accordingly
           scroll_bkg(-player_distance, -player_distance);        


           // this is to hold the current players direction
        dir[0] = 0;  // 1 down is true, 0 down is false
        dir[1] = 0;  // 1 up is true, 0 up is false
        dir[2] = 0;  // 1 left is true, 0 left is false
        dir[3] = 0;  // 1 right is true, 0 right is false
        dir[4] = 1; // 1 up and left is true, 0 up left is false
        dir[5] = 0; // 1 up and right is true, 0 up right is false
        dir[6] = 0; // 1 down and left is true, 0 down left is false
        dir[7] = 0; // 1 down and right is true, 0 down right is false
                
       // when needed, this will display the backward head
       set_sprite_tile(0, 0);
       set_sprite_tile(1,2);
       set_sprite_prop(0,0);
       set_sprite_prop(1,0);
        move_sprite(0, player_dxdy[0], player_dxdy[1] );
        move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );

       // this is for the tech
       set_sprite_tile(2,126);
      set_sprite_prop(2,1);
      move_sprite(2, player_dxdy[0] + 8, player_dxdy[1] + 13 );
      move_sprite(3,200,  200); // off the screen cause not needed currently

     // this is for the up left diagonal body
     set_sprite_tile(4,48);
     set_sprite_prop(4,0);
     move_sprite(4, player_dxdy[0] , player_dxdy[1] + 14);
     set_sprite_tile(5,50);
     set_sprite_prop(5,0);
     move_sprite(5, player_dxdy[0] + 8, player_dxdy[1] + 14); 
   

            play_walk_sound = 1;     

            delay( player_speed[1] );       // had to slow it down, because diagonal movement is faster than normal straight movement


        } // end of moving up and left
       else if(   (joypad() & J_DOWN) && (joypad() & J_LEFT) || (joypad() & J_LEFT) && (joypad() & J_DOWN)   )    
       {
    

          // to scroll the stationary sprite in the direction to give the illusion of movement
         scroll_bkg(-player_distance, player_distance );


           // this is to hold the current players direction
         dir[0] = 0;  // 1 down is true, 0 down is false     
         dir[1] = 0;  // 1 up is true, 0 up is false
         dir[2] = 0;  // 1 left is true, 0 left is false
         dir[3] = 0;  // 1 right is true, 0 right is false
         dir[4] = 0; // 1 up and left is true, 0 up left is false
         dir[5] = 0; // 1 up and right is true, 0 up right is false
         dir[6] = 1; // 1 down and left is true, 0 down left is false
         dir[7] = 0; // 1 down and right is true, 0 down right is false

        // when needed, this will display the down left head
        set_sprite_tile(0, 72);
        set_sprite_prop(0,0);
        set_sprite_tile(1,74);
        set_sprite_prop(1,0);
        move_sprite(0, player_dxdy[0], player_dxdy[1] );
        move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );

           // this is for the tech on body
      set_sprite_tile(2,108);
      set_sprite_tile(3,110);
      set_sprite_prop(2,0);
      set_sprite_prop(3,0);
       move_sprite(2, player_dxdy[0], player_dxdy[1] + 13);
       move_sprite(3, player_dxdy[0] + 8, player_dxdy[1] + 13);
     
       set_sprite_tile(4,126);
       set_sprite_prop(4,1);
       move_sprite(4,player_dxdy[0] + 12, player_dxdy[1] + 12 );
      
      move_sprite(5,200,200);  // to move this not needed sprite out of the way

            play_walk_sound = 1;
             
            delay( player_speed[1] );     // had to slow it down, because diagonal movement is faster than normal straight movement

    
        } // end of moving down and left
        else if(  (joypad() & J_DOWN) && (joypad() & J_RIGHT) || (joypad() & J_RIGHT) && (joypad() & J_DOWN)   )  
       {
  
        
          // to scroll everything but the player to give the illusion of movement
         scroll_bkg(player_distance, player_distance );


          // this is to hold the current players direction
         dir[0] = 0;  // 1 down is true, 0 down is false
         dir[1] = 0;  // 1 up is true, 0 up is false
         dir[2] = 0;  // 1 left is true, 0 left is false
         dir[3] = 0;  // 1 right is true, 0 right is false
         dir[4] = 0; // 1 up and left is true, 0 up left is false
         dir[5] = 0; // 1 up and right is true, 0 up right is false
         dir[6] = 0; // 1 down and left is true, 0 down left is false
         dir[7] = 1; // 1 down and right is true, 0 down right is false


            // when needed, this will display the forward head
        set_sprite_tile(0, 76);
       set_sprite_tile(1,78);
       set_sprite_prop(0,0);
       set_sprite_prop(1,0);
       move_sprite(0, player_dxdy[0], player_dxdy[1] );
       move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );

           // this is for the down right body
       set_sprite_tile(2,92);
       set_sprite_tile(3,94);
       set_sprite_prop(2,0);
       set_sprite_prop(3,0);
       move_sprite(2, player_dxdy[0] , player_dxdy[1] + 13);
       move_sprite(3, player_dxdy[0] + 8, player_dxdy[1] + 13);
     
       // this is to show the tech when down right
     set_sprite_tile(4,124);
     set_sprite_prop(4,1);
     move_sprite(4,player_dxdy[0] - 2 , player_dxdy[1] + 13 ); 
      
     
      // this is to hide the unneeded sprite
      move_sprite(5,200,200);
      
            play_walk_sound = 1;
                   
            delay( player_speed[1] );    // had to slow it down, because diagonal movement is faster than normal straight movement
                   
            
        
        } // end of moving down and right
        else if ( joypad() & J_UP ) // just straight up, was seperated from codeblock and was just if
        {
   
 
        // to scroll everything but the player
      // 0 is here because the x direction is not changing
        scroll_bkg(0, -player_distance );



            // this is to hold the current players direction
         dir[0] = 0;  // 1 down is true, 0 down is false
         dir[1] = 1;  // 1 up is true, 0 up is false
         dir[2] = 0;  // 1 left is true, 0 left is false
         dir[3] = 0;  // 1 right is true, 0 right is false
         dir[4] = 0; // 1 up and left is true, 0 up left is false
         dir[5] = 0; // 1 up and right is true, 0 up right is false
         dir[6] = 0; // 1 down and left is true, 0 down left is false
         dir[7] = 0; // 1 down and right is true, 0 down right is false          

         


        // when needed, this will display the backward head
        set_sprite_tile(0, 0);
        set_sprite_tile(1,2);
        set_sprite_prop(0,0);
        set_sprite_prop(1,0);
        move_sprite(0, player_dxdy[0], player_dxdy[1] );
        move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );
         
        // this is for the tech on body
       set_sprite_tile(2,28);
       set_sprite_tile(3,30); 
       set_sprite_prop(2,1);
       set_sprite_prop(3,1);
       move_sprite(2, player_dxdy[0], player_dxdy[1] + 14);
       move_sprite(3, player_dxdy[0] + 8, player_dxdy[1] + 14);

       // JUST ADDED THIS MAX
       move_sprite(2, 200, 200);
       move_sprite(3, 200, 200);

       // this is for a backwards body 
      // 13 so head is closer to body
       set_sprite_tile(4,32);
       set_sprite_tile(5,34);
       set_sprite_prop(4,1); 
       set_sprite_prop(5,1); 
       move_sprite(4,player_dxdy[0], player_dxdy[1] + 13);
       move_sprite(5,player_dxdy[0] + 8, player_dxdy[1] + 13);
       
            play_walk_sound = 1;

            delay( player_speed[1] );   // normal straight movement

        }  // end of move player just up 
        else if (  joypad() & J_DOWN  )   
        {
            // BY MOVING THE BACKGROUND INSTEAD OF THE PLAYER BY THE BELOW,
           // WE HAVE A SCROLLING ENGINE THAT WORKS, I LEARNED THIS FROM BURLY BEAR GAME CREATOR.
          // I USED HIS EXAMPLE, BUT FIGURED OUT HOW TO USE THE scoll_bkg(param1,param2) function
       

           // here we just need to increment the y direction to move downward
          scroll_bkg(0, player_distance );




             // this is to hold the current players direction
             dir[0] = 1;  // 1 down is true, 0 down is false
             dir[1] = 0;  // 1 up is true, 0 up is false
             dir[2] = 0;  // 1 left is true, 0 left is false
             dir[3] = 0;  // 1 right is true, 0 right is false
             dir[4] = 0; // 1 up and left is true, 0 up left is false
             dir[5] = 0; // 1 up and right is true, 0 up right is false
             dir[6] = 0; // 1 down and left is true, 0 down left is false
             dir[7] = 0; // 1 down and right is true, 0 down right is false 


         // when needed, this will display the backward head
        set_sprite_tile(0,5);
        set_sprite_tile(1,7);
        set_sprite_prop(0,0);
        set_sprite_prop(1,0);
        move_sprite(0, player_dxdy[0], player_dxdy[1] );
        move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );

     
          // this is for the tech on body back
        set_sprite_tile(2,16);
        set_sprite_tile(3,18);
        set_sprite_prop(2,1);
        set_sprite_prop(3,1);
        move_sprite(2, player_dxdy[0], player_dxdy[1] + 14);
        move_sprite(3, player_dxdy[0] + 8, player_dxdy[1] + 14);
        
    
       // this is for forwards body 
      // 13 so head is closer to body
      set_sprite_tile(4,28);
      set_sprite_tile(5,30);
      set_sprite_prop(4,1);
      set_sprite_prop(5,1);
       move_sprite(4,player_dxdy[0], player_dxdy[1] + 13);
       move_sprite(5,player_dxdy[0] + 8, player_dxdy[1] + 13);

       // JUST ADDED THIS TODAY ON APRIL 21 2018
       move_sprite(4, 200,200);
       move_sprite(5, 200,200);

     


  
   // this is the down animation of the body
   // oh god, the calls to the stack. I have no idea how I even pulled this off.
  // this example shows how one can make sprite animations. 
  // but this animation only occurs when the player is moving down.
  if( ani == 24) // where I want the animation to loop
  {
    ani = 16; // In the VRAM this will cause a animation
  }
    ani += 4;
    set_sprite_tile(2,ani );
    set_sprite_tile(3,ani + 2);

       
      

 
           
           play_walk_sound = 1;       // will play a sound if it is true that the player is walking
 
           delay(player_speed[1] );    // normal straight movement
       

        }  // end of move player moved just down
       else if (  joypad() & J_RIGHT   ) // just moving right  
       {
       
            
            // just scroll the player to the right, by moving everything but the player
            scroll_bkg(player_distance, 0);






                 // this is to hold the current players direction
    dir[0] = 0;  // 1 down is true, 0 down is false  // just added this
    dir[1] = 0;  // 1 up is true, 0 up is false
    dir[2] = 0;  // 1 left is true, 0 left is false
    dir[3] = 1;  // 1 right is true, 0 right is false
    dir[4] = 0; // 1 up and left is true, 0 up left is false
    dir[5] = 0; // 1 up and right is true, 0 up right is false
    dir[6] = 0; // 1 down and left is true, 0 down left is false
    dir[7] = 0; // 1 down and right is true, 0 down right is false       

   


           // when needed, this will display the right head
        set_sprite_tile(0, 8);
        set_sprite_tile(1, 10);
        set_sprite_prop(0,0);
        set_sprite_prop(1,0);
        move_sprite(0, player_dxdy[0], player_dxdy[1] );
        move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );
 

        // this is for the tech on body
       set_sprite_tile(2,124);
       set_sprite_prop(2,1);
       move_sprite(2, player_dxdy[0], player_dxdy[1] + 13); // modify this max
  

       // this is for  right moving body 
      // 13 so head is closer to body, and + 8 to center the body
       set_sprite_tile(4,61);
       set_sprite_prop(4,0); // to set to skin color
       move_sprite(4,player_dxdy[0] + 8, player_dxdy[1] + 13);
    
      move_sprite(3, 200,200); // will this get rid of the body sprite
      move_sprite(5,200,200);     // to hide the not needed sprite      

            play_walk_sound = 1;




            delay( player_speed[1] );   // normal straight movement
  
        } // end of moving right
       else if (  joypad() & J_LEFT )  // just moving left
       {


          // scroll just to the left
         scroll_bkg(-player_distance, 0);
 



           
  

             // this is to hold the current players direction
          dir[0] = 0;  // 1 down is true, 0 down is false
          dir[1] = 0;  // 1 up is true, 0 up is false
          dir[2] = 1;  // 1 left is true, 0 left is false
          dir[3] = 0;  // 1 right is true, 0 right is false
          dir[4] = 0; // 1 up and left is true, 0 up left is false
          dir[5] = 0; // 1 up and right is true, 0 up right is false
          dir[6] = 0; // 1 down and left is true, 0 down left is false
          dir[7] = 0; // 1 down and right is true, 0 down right is false           


       // when needed, this will display the left head
       set_sprite_tile(0, 12);
       set_sprite_tile(1,14);
       set_sprite_prop(0,0);
       set_sprite_prop(1,0);
        move_sprite(0, player_dxdy[0], player_dxdy[1] );
        move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );

      // this is for the tech on body
       set_sprite_tile(2,126);
       set_sprite_prop(2,1);
       move_sprite(2, player_dxdy[0] + 8, player_dxdy[1] + 13); // modify this max
  

       // this is for the left moving body 
      // 13 so head is closer to body, and + 8 to center the body
       set_sprite_tile(4,63);
       set_sprite_prop(4,0); 
       move_sprite(4,player_dxdy[0] , player_dxdy[1] + 13);
    
      move_sprite(3, 200,200); // will this get rid of the body sprite
      move_sprite(5,200,200);  // to move not needed sprite off screen             


            play_walk_sound = 1;

            delay( player_speed[1] );   // normal straight movement

     
        } // end of just move player left               
        else
        {
           // code to do, to give illusion of multiple things occuring at once
          // this can be used to do multiple checks or move others players or AI
         // this may also hold normal animations and what not

           play_walk_sound = 0;   // the player is currently not walking

          scroll_bkg(0,0);  //   don't scroll the engine

        }    // end of the grand input loop
  





          
}   // end of set_check_input() function



// handles starting game information
void set_initialize_game()
{

    // I MAY NOT USE THIS, ITS HARD TO WORK WITH INTERRUPTS IN GBDK
    // below I hacked my own way to do so.
    disable_interrupts();
    add_TIM(set_tim);  
    enable_interrupts();
    set_interrupts( TIM_IFLAG | VBL_IFLAG );
    
    DISPLAY_ON;  // to turn the game screen on
    SPRITES_8x16;   // to say we are using tiles that are 8 width and 16 pixels in height
 

     // you can use four colors for the background, but everything else must be set individually
   // so you can have a grass plain but set everything else using move_bkg etc
    set_bkg_data(0, 6, bkg_tiles_eleven );   // bkg_tiles_three is the array in the for_background_map.c file
    
     // sets colors for the current image
     set_bkg_palette(0,5, &backgroundPalette[0] );  // this should load the green color
   
    // switch to vram
     VBK_REG = 1;
    set_bkg_tiles(0, 0, 20, 18, background_test_twelve);   // should set the map which works.

   // switch out of vram  
    VBK_REG = 0;

  // set_bkg_tiles(0, 0, 20, 18, bkg_tiles_nine);

   // set colors palettes
    set_sprite_palette(0,3, spritePalette);

   
 
   // the stuff above here is going to the the sprite head load point
    set_sprite_data(0, 206, sprite_test_twentyfr);
 

 
   // the information below is to start and load the player, only required on start up and not important to rest of game
   // initialize the sprite
        // when needed, this will display the forward head
       set_sprite_tile(0,5);
       set_sprite_tile(1,7);
       set_sprite_prop(0,0);
       set_sprite_prop(1,0);
   //     move_sprite(0, player_dxdy[0], player_dxdy[1] );   
    //    move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );  
        // this is for the body on body back
       set_sprite_tile(2,16);
       set_sprite_tile(3,18);
       set_sprite_prop(2,1);
       set_sprite_prop(3,1);
    //   move_sprite(2, player_dxdy[0], player_dxdy[1] + 14);
    //   move_sprite(3, player_dxdy[0] + 8, player_dxdy[1] + 14);
       // this is for the tech
      // 13 so head is closer to body
      set_sprite_tile(4,28);
      set_sprite_tile(5,30);
      set_sprite_prop(4,1);
      set_sprite_prop(5,1);
    //  move_sprite(4,player_dxdy[0], player_dxdy[1] + 13);
    //  move_sprite(5,player_dxdy[0] + 8, player_dxdy[1] + 13);
   //   move_sprite(4, 200, 200);
   //   move_sprite(5, 200, 200);


     // This is to load a different sprite called block for collision and game testing
     set_sprite_tile(10, 204);  // this is to set a block with the letter 'A' on it
     set_sprite_prop(10,1);
     //  move_sprite(10, block_dxdy[0], block_dxdy[1] );     
 

  
  
   // these are required registers to play sound on gbc
 //   NR52_REG = 0x80;    turn on the sound, try 0x80
  //  NR51_REG = 0x11;
  //  NR50_REG = 0x77;

 
     
       // all of this is to set the player sprite on the game screen, is only done once on initialization
       move_sprite(0, player_dxdy[0], player_dxdy[1] );    // this sets the sprite initially
        move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );  // this sets the sprite initially
        move_sprite(2, player_dxdy[0], player_dxdy[1] + 14);
       move_sprite(3, player_dxdy[0] + 8, player_dxdy[1] + 14);
        move_sprite(4,player_dxdy[0], player_dxdy[1] + 13);
      move_sprite(5,player_dxdy[0] + 8, player_dxdy[1] + 13);
      move_sprite(4, 200, 200);
      move_sprite(5, 200, 200);
       move_sprite(10, block_dxdy[0], block_dxdy[1] );    // this sets the block on the screen



}    // end of set_initialize_game() function



// These are required to actually show the obj and bkg and win
void set_check_switches()
{
  HIDE_WIN;
  SHOW_SPRITES;
  SHOW_BKG;

} // end of function set_check_switches()



// this function is to handle the attack tapu direction
// delay(128), 128 is to limit the time that the attack can be used at one time
void set_tapu_attack()
{
    
       if ( joypad() & J_A  )  // pressed A button and direction is down, then 
       {
             
   // play a game sound here for tapu attack           
 // below is just the code to play a tapu attack sound
 { // NR10_REG = 0x10;
   // NR11_REG = 0x81;
  // NR12_REG = 0x43;
  // NR13_REG = 0x73;
  // NR14_REG = 0x86;
   NR21_REG = 0x41;
   NR22_REG = 0x84;
   NR23_REG = 0xD7;
   NR24_REG = 0x86;
   NR30_REG = 0x00;
   NR31_REG = 0x01;
   NR32_REG = 0x20;
   NR33_REG = 0xAD;
   NR34_REG = 0x86;
   NR41_REG = 0x3A;
   NR42_REG = 0xA2;
   NR43_REG = 0x31;
   NR44_REG = 0x80;
   NR50_REG  =0x77; 
   NR51_REG = 0xFF;    // WAS |= 0x11
   NR52_REG = 0x80;  // required to turn on
  }

         // These flags below represent the players current direction
          //   dir[0] = 1;     1 down is true, 0 down is false
        //     dir[1] = 0;   1 up is true, 0 up is false
        //     dir[2] = 0;   1 left is true, 0 left is false
       //      dir[3] = 0;   1 right is true, 0 right is false
      //       dir[4] = 0; // 1 up and left is true, 0 up left is false
      //       dir[5] = 1; // 1 up and right is true, 0 up right is false
      //       dir[6] = 0; // 1 down and left is true, 0 down left is false
      //       dir[7] = 0; // 1 down and right is true, 0 down right is false


        if ( dir[0] == 1 )   // player facing down and used tapu attack
        {
           set_sprite_tile(6,116); // sets the left hand pushu
           set_sprite_prop(6,1);
           move_sprite(6, player_dxdy[0] - 16 , player_dxdy[1] + 6 );
           set_sprite_tile(7,118); // sets the extended arm pushu
           set_sprite_prop(7,1);
           move_sprite(7, player_dxdy[0] - 8 , player_dxdy[1] + 6 ); 


          set_sprite_tile(8, 123); // sets the right hand pushu 
          set_sprite_prop(8,1);
          move_sprite(8, player_dxdy[0] + 24, player_dxdy[1] + 6 );
         set_sprite_tile(9,121);  // sets the right arm pushu
         set_sprite_prop(9,1);
         move_sprite(9, player_dxdy[0] + 16, player_dxdy[1] + 6 ); 
            
          delay(128); // this is to limit times an push attack can be used
       
          // this is to hide the tech after the used attack
         move_sprite(6, 200,200);
         move_sprite(7, 200, 200);
         move_sprite(8, 200, 200);
         move_sprite(9, 200, 200);
       
         }  // end of pushu if player is facing down



       
          if ( dir[1] == 1 )   // player facing up is true
        {
           set_sprite_tile(6,128); // sets the left hand pushu facing up
           set_sprite_prop(6,1);
           move_sprite(6, player_dxdy[0] - 16 , player_dxdy[1] + 12 );
           set_sprite_tile(7,130); // sets the extended arm pushu facing up
           set_sprite_prop(7,1);
           move_sprite(7, player_dxdy[0] - 8 , player_dxdy[1] + 12 ); 


          set_sprite_tile(8, 134); // sets the right hand pushu facing up
          set_sprite_prop(8,1);
          move_sprite(8, player_dxdy[0] + 24, player_dxdy[1] + 12 );
         set_sprite_tile(9,132);  // sets the right arm pushu facing up
         set_sprite_prop(9,1);
         move_sprite(9, player_dxdy[0] + 16, player_dxdy[1] + 12 ); 
            
          delay(128); // this is to limit times an push attack can be used
 
        // this is to hide the tech after the used attack
         move_sprite(6, 200,200);
         move_sprite(7, 200, 200);
         move_sprite(8, 200, 200);
         move_sprite(9, 200, 200);       


         }  // end of if player is facing up
      
      
           if ( dir[2] == 1 )   // player facing left is true
        {
           // this handles the facing up left pushu hand
           set_sprite_tile(6,150); // sets the left hand pushu facing up
           set_sprite_prop(6,1);
           move_sprite(6, player_dxdy[0] + 23 , player_dxdy[1] - 8   ); // was 8
 
          // this handles the facing down left pushu hand
           set_sprite_tile(7,148); // sets the extended arm pushu facing left
           set_sprite_prop(7,1);
           move_sprite(7, player_dxdy[0] + 15 , player_dxdy[1] - 8 ); 


          set_sprite_tile(8, 144); // sets the left hand pushu facing up    
          set_sprite_prop(8,1);
          move_sprite(8, player_dxdy[0] + 16, player_dxdy[1] + 20 );
         set_sprite_tile(9,146);  // sets the left arm pushu facing up
         set_sprite_prop(9,1);
         move_sprite(9, player_dxdy[0] + 24, player_dxdy[1] + 20 ); 

       
            
          delay(128); // this is to limit times an push attack can be used
       
        // this is to hide the tech after the used attack
         move_sprite(6, 200,200);
         move_sprite(7, 200, 200);
         move_sprite(8, 200, 200);
         move_sprite(9, 200, 200); 



         }  // end of if player is facing left so use left pushu hand
         



       
       if ( dir[3] == 1 )   // player facing right is true
        {
           // this handles the facing up left pushu hand
           set_sprite_tile(6,140); // sets the left hand pushu facing up
           set_sprite_prop(6,1);
           move_sprite(6, player_dxdy[0] - 15  , player_dxdy[1] - 8   ); // was -16 x
 
          // this handles the facing down left pushu hand
           set_sprite_tile(7,142); // sets the extended arm pushu facing left
           set_sprite_prop(7,1);
           move_sprite(7, player_dxdy[0] - 7 , player_dxdy[1] - 8 );   // was - 8 x


          set_sprite_tile(8, 138); // sets the left hand pushu facing up    
         set_sprite_prop(8,1);
          move_sprite(8, player_dxdy[0] - 6, player_dxdy[1] + 20 );  // was - 7 x
         set_sprite_tile(9,136);  // sets the left arm pushu facing up
         set_sprite_prop(9,1);
         move_sprite(9, player_dxdy[0] - 14 , player_dxdy[1] + 20 );  // was - 15 x

       
 
          delay(128); // this is to limit times an push attack can be used
       
          // this is to hide the tech after the used attack
         move_sprite(6, 200,200);
         move_sprite(7, 200, 200);
         move_sprite(8, 200, 200);
         move_sprite(9, 200, 200); 


         }  // end of if player is facing right so use right pushu hand
         
      if (dir[4] == 1 )   // player is moving both up and left and used tapu attack
      {
        
             // this handles the facing up left pushu hand
           set_sprite_tile(6,161); // sets the left hand pushu facing up  ON LEFT SIDE
           set_sprite_prop(6,1);
           move_sprite(6, player_dxdy[0] - 6  , player_dxdy[1] + 22   );         
 
          // this handles the facing down left pushu hand
           set_sprite_tile(7,163); // sets the extended arm pushu facing left  ON RIGHT SIDE
           set_sprite_prop(7,1);
           move_sprite(7, player_dxdy[0] + 20 , player_dxdy[1]  - 1 );   

      // use 8 and 9 for the 172 sprite extended arm
          
           set_sprite_tile(8, 172);
           set_sprite_prop(8,1);
           move_sprite(8, player_dxdy[0] + 2 , player_dxdy[1] + 18 );    // ON LEFT SIDE
  
           set_sprite_tile(9, 172);
           set_sprite_prop(9,1);
           move_sprite(9, player_dxdy[0] + 15, player_dxdy[1] + 8);  // ON RIGHT SIDE was 13 and 10


          delay(128); // this is to limit times an push attack can be used

           // this is to hide the tech after the used attack
         move_sprite(6, 200,200);
         move_sprite(7, 200, 200);
         move_sprite(8, 200, 200);
         move_sprite(9, 200, 200); 


      }   // end of player is facing up left function
         
      
       if (dir[5] == 1 )   // player is moving both up and right and used the tapu attack
      {
        
             // this handles the facing up left pushu hand
           set_sprite_tile(6,153); // sets the left hand pushu facing up
          set_sprite_prop(6,1);
           move_sprite(6, player_dxdy[0] - 12  , player_dxdy[1] - 1     ); // was 13 and - 2 
 
          // this handles the facing down left pushu hand
           set_sprite_tile(7,155); // sets the extended arm pushu facing left
          set_sprite_prop(7,1);
           move_sprite(7, player_dxdy[0] + 16 , player_dxdy[1] + 26 );   

         set_sprite_tile(8, 170);  // this is used for the tapu tech extended
         set_sprite_prop(8,1);
         move_sprite(8, player_dxdy[0] - 7, player_dxdy[1] + 8 );

         set_sprite_tile(9, 170);  // this is used for the tapu tech extended
         set_sprite_prop(9,1);
         move_sprite(9, player_dxdy[0] + 8, player_dxdy[1] + 22 );

 
          delay(128); // this is to limit times an push attack can be used

          // this is to hide the tech after the used attack
         move_sprite(6, 200,200);
         move_sprite(7, 200, 200);
         move_sprite(8, 200, 200);
         move_sprite(9, 200, 200); 

      }   // end of player is facing up and right

 
    if (dir[6] == 1 )  // player used tapu attack while down and left
    {
           // this handles the facing up left pushu hand
           set_sprite_tile(6,152); // sets the left hand pushu facing up
           set_sprite_prop(6,1);
           move_sprite(6, player_dxdy[0] - 10  , player_dxdy[1] - 3    );   // was - 13 and - 2       LEFT SIDE
 
          // this handles the facing down left pushu hand
           set_sprite_tile(7,155); // sets the extended arm pushu facing left
           set_sprite_prop(7,1);
           move_sprite(7, player_dxdy[0] + 25 , player_dxdy[1] + 20 );   // was 16 and 26   RIGHT SIDE hand

         set_sprite_tile(8, 170);  // this is used for the tapu tech extended  LEFT SIDE
         set_sprite_prop(8,1);
         move_sprite(8, player_dxdy[0] - 5, player_dxdy[1] + 6 );

         set_sprite_tile(9, 170);  // this is used for the tapu tech extended RIGHT SIDE
         set_sprite_prop(9,1);
         move_sprite(9, player_dxdy[0] + 18, player_dxdy[1] + 17 );      

 
          delay(128); // this is to limit times an push attack can be used

           // this is to hide the tech after the used attack
         move_sprite(6, 200,200);
         move_sprite(7, 200, 200);
         move_sprite(8, 200, 200);
         move_sprite(9, 200, 200); 

    }   // end of facing down and left tapu attack
       

   if (dir [7] == 1 )
   {
       // this handles the facing up left pushu hand
           set_sprite_tile(6,161); // sets the left hand pushu facing up  ON LEFT SIDE
          set_sprite_prop(6,1);
           move_sprite(6, player_dxdy[0] - 16  , player_dxdy[1] + 22   ); // was - 6 and 22         
 
          // this handles the facing down left pushu hand
           set_sprite_tile(7,163); // sets the extended arm pushu facing left  ON RIGHT SIDE
           set_sprite_prop(7,1);
           move_sprite(7, player_dxdy[0] + 20 , player_dxdy[1]  - 1 );   

      // use 8 and 9 for the 172 sprite extended arm
          
           set_sprite_tile(8, 172);
           set_sprite_prop(8,1);
           move_sprite(8, player_dxdy[0] - 8 , player_dxdy[1] + 18 );    // ON LEFT SIDE was - 2
  
           set_sprite_tile(9, 172); 
           set_sprite_prop(9,1);
           move_sprite(9, player_dxdy[0] + 15, player_dxdy[1] + 8);  // ON RIGHT SIDE was 13 and 10


          delay(128); // this is to limit times an push attack can be used

         // this is to hide the tech after the used attack
         move_sprite(6, 200,200);
         move_sprite(7, 200, 200);
         move_sprite(8, 200, 200);
         move_sprite(9, 200, 200); 

   }



  }   // end of function if (joypad() & J_A ) which handles the tapu attack



} // end of function set_tapu_attack() 




// this function is for resetting animations when the player is no longer pressing any keys
// is also used to set a frame for when animations stop
void set_faced_sprite_direction()
{ 
      // These flags below represent the players current direction
          //   dir[0] = 0;     1 down is true, 0 down is false
        //     dir[1] = 0;   1 up is true, 0 up is false
        //     dir[2] = 0;   1 left is true, 0 left is false
       //      dir[3] = 0;   1 right is true, 0 right is false
      //       dir[4] = 0; // 1 up and left is true, 0 up left is false
      //       dir[5] = 0; // 1 up and right is true, 0 up right is false
      //       dir[6] = 0; // 1 down and left is true, 0 down left is false
      //       dir[7] = 0; // 1 down and right is true, 0 down right is false
      

   if (dir[0] == 1)  // faced down is true
   {
 // when needed, this will display the backward head
        set_sprite_tile(0,5);
        set_sprite_tile(1,7);
        set_sprite_prop(0,0);
        set_sprite_prop(1,0);
        move_sprite(0, player_dxdy[0], player_dxdy[1] );
        move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );

     
          // this is for the tech on body back
        set_sprite_tile(2,16);
        set_sprite_tile(3,18);
        set_sprite_prop(2,1);
        set_sprite_prop(3,1);
        move_sprite(2, player_dxdy[0], player_dxdy[1] + 14);
        move_sprite(3, player_dxdy[0] + 8, player_dxdy[1] + 14);
        move_sprite(2, 200, 200);
        move_sprite(3, 200, 200);
    
       // this is for forwards body 
      // 13 so head is closer to body
      set_sprite_tile(4,28);
      set_sprite_tile(5,30);
      set_sprite_prop(4,1);
      set_sprite_prop(5,1);
       move_sprite(4,player_dxdy[0], player_dxdy[1] + 13);
       move_sprite(5,player_dxdy[0] + 8, player_dxdy[1] + 13);

       move_sprite(4, 200,200);
       move_sprite(5, 200,200);

     
     

   }  // end of last key pressed was down key

   
   if ( dir[1] == 1)  // faced up is true
   {
         // when needed, this will display the backward head
        set_sprite_tile(0, 0);
        set_sprite_tile(1,2);
        set_sprite_prop(0,0);
        set_sprite_prop(1,0);
        move_sprite(0, player_dxdy[0], player_dxdy[1] );
        move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );
         
        // this is for the tech on body
       set_sprite_tile(2,28);
       set_sprite_tile(3,30); 
       set_sprite_prop(2,1);
       set_sprite_prop(3,1);
       move_sprite(2, player_dxdy[0], player_dxdy[1] + 14);
       move_sprite(3, player_dxdy[0] + 8, player_dxdy[1] + 14);

       // JUST ADDED THIS MAX
       move_sprite(2, 200, 200);
       move_sprite(3, 200, 200);

       // this is for a backwards body 
      // 13 so head is closer to body
       set_sprite_tile(4,32);
       set_sprite_tile(5,34);
       set_sprite_prop(4,1); 
       set_sprite_prop(5,1); 
       move_sprite(4,player_dxdy[0], player_dxdy[1] + 13);
       move_sprite(5,player_dxdy[0] + 8, player_dxdy[1] + 13);
       
   }


   if ( dir[2] == 1 ) // faced left is true
   {
      // when needed, this will display the left head
       set_sprite_tile(0, 12);
       set_sprite_tile(1,14); 
       set_sprite_prop(0,0);
       set_sprite_prop(1,0);
        move_sprite(0, player_dxdy[0], player_dxdy[1] );
        move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );

      // this is for the tech on body
       set_sprite_tile(2,126);
       set_sprite_prop(2,1);
       move_sprite(2, player_dxdy[0] + 8, player_dxdy[1] + 13); // modify this max
  

       // this is for the left moving body 
      // 13 so head is closer to body, and + 8 to center the body
       set_sprite_tile(4,63);
       set_sprite_prop(4,0); 
       move_sprite(4,player_dxdy[0] , player_dxdy[1] + 13);
    
      move_sprite(3, 200,200); // will this get rid of the body sprite
      move_sprite(5,200,200);  // to move not needed sprite off screen          
   
   }


   if ( dir[3] == 1 )  // faced right is true
   {
     // when needed, this will display the right head
        set_sprite_tile(0, 8);
        set_sprite_tile(1, 10);
        set_sprite_prop(0,0);
        set_sprite_prop(1,0);
        move_sprite(0, player_dxdy[0], player_dxdy[1] );
        move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );
 

           // this is for the tech on body
       set_sprite_tile(2,124);
       set_sprite_prop(2,1);
       move_sprite(2, player_dxdy[0], player_dxdy[1] + 13); 
  

       // this is for  right moving body 
      // 13 so head is closer to body, and + 8 to center the body
       set_sprite_tile(4,61);
       set_sprite_prop(4,0); 
       move_sprite(4,player_dxdy[0] + 8, player_dxdy[1] + 13);
    
      move_sprite(3, 200,200); // will this get rid of the body sprite
      move_sprite(5,200,200);    // hide this sprite         
  
   }


   if ( dir[4] == 1 ) // up and left is true
   {
        // when needed, this will display the backward head
       set_sprite_tile(0, 0);
       set_sprite_tile(1,2);
       set_sprite_prop(0,0);
       set_sprite_prop(1,0);
        move_sprite(0, player_dxdy[0], player_dxdy[1] );
        move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );

       // this is for the tech
       set_sprite_tile(2,126);
      set_sprite_prop(2,1);
      move_sprite(2, player_dxdy[0] + 8, player_dxdy[1] + 13 );
      move_sprite(3,200,200); // off the screen

     // this is for the up left diagonal body
     set_sprite_tile(4,48);
     set_sprite_prop(4,0);
     move_sprite(4, player_dxdy[0] , player_dxdy[1] + 14);
     set_sprite_tile(5,50);
     set_sprite_prop(5,0);
     move_sprite(5, player_dxdy[0] + 8, player_dxdy[1] + 14); 
   
   }


    if ( dir[5] == 1 )  // up and right was pressed last
   {
       // when needed, this will display the backward head
       set_sprite_tile(0, 0);
       set_sprite_tile(1,2);
       set_sprite_prop(0,0);
       set_sprite_prop(1,0);
       move_sprite(0, player_dxdy[0], player_dxdy[1] );
       move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );

      
           // this is for the tech on body
       set_sprite_tile(2,124);
       set_sprite_tile(3,126);
       set_sprite_prop(2,1);
      set_sprite_prop(3,1);
       move_sprite(2, player_dxdy[0], player_dxdy[1] + 14);
       move_sprite(3, player_dxdy[0] + 8, player_dxdy[1] + 14);
      move_sprite(3, 200, 200);

       // this is for a up right body 
      // 14 so head is closer to body
       set_sprite_tile(4,44);
       set_sprite_tile(5,46);
       set_sprite_prop(4,0); 
       set_sprite_prop(5,0); 
       move_sprite(4,player_dxdy[0], player_dxdy[1] + 14);
       move_sprite(5,player_dxdy[0] + 8, player_dxdy[1] + 14);

   }  // end of up and right was pressed last


  if ( dir[6] == 1 )  // down and left is true
  {
      // when needed, this will display the down left head
        set_sprite_tile(0, 72);
        set_sprite_tile(1,74);
        set_sprite_prop(0,0);
       set_sprite_prop(1,0);
        move_sprite(0, player_dxdy[0], player_dxdy[1] );
        move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );

           // this is for the tech on body
      set_sprite_tile(2,108);
      set_sprite_tile(3,110);
      set_sprite_prop(2,0);
      set_sprite_prop(3,0);
       move_sprite(2, player_dxdy[0], player_dxdy[1] + 13);
       move_sprite(3, player_dxdy[0] + 8, player_dxdy[1] + 13);
     
       set_sprite_tile(4,126);
       set_sprite_prop(4,1);
       move_sprite(4,player_dxdy[0] + 12, player_dxdy[1] + 12 );
      
      move_sprite(5,200,200);

  }


  if ( dir[7] == 1 )  // down and right is true
  {
       // when needed, this will display the forward head
        set_sprite_tile(0, 76);
       set_sprite_tile(1,78);
       set_sprite_prop(0,0);
       set_sprite_prop(1,0);
       move_sprite(0, player_dxdy[0], player_dxdy[1] );
       move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );

           // this is for the down right body
       set_sprite_tile(2,92);
       set_sprite_tile(3,94);
       set_sprite_prop(2,0);
       set_sprite_prop(3,0);
       move_sprite(2, player_dxdy[0] , player_dxdy[1] + 13);
       move_sprite(3, player_dxdy[0] + 8, player_dxdy[1] + 13);
     
       // this is to show the tech when down right
     set_sprite_tile(4,124);
       set_sprite_prop(4,1);
       move_sprite(4,player_dxdy[0] - 2 , player_dxdy[1] + 13 );
      
     
      // this is to hide the unneeded sprite
      move_sprite(5,200,200);

  }



}  // end of set_faced_sprite_direction()



// I learned this collision detection from a series on a website called videlais. The person had great insight
// this function checks for collisions and returns either 1 for true a collison occured, or 0 for false no collision occured
// first four parameters are the players x- coordinate, y - coordinate, width, height, the last four are of the other sprite/background enemy
UINT8 collision_detection(UINT8 x1, UINT8 y1, UINT8 w1, UINT8 h1, UINT8 x2, UINT8 y2, UINT8 w2, UINT8 h2 )
{
         
     
   if (   (x1 < ( x2 + w2) )  && ( x2 < (x1 + w1) ) && ( y1 < (h2 + y2) ) && (  y2 < (y1 + h1) )      )
   {
       return 1;  // collision has occured
    }
    else   if (   (x1 <= ( x2 + w2) )  && ( x2 <= (x1 + w1) ) && ( y1 <= (h2 + y2) ) && (  y2 <= (y1 + h1) )      )  
    {                                                                                      
      return 1;
    }
    else     // is required to check constantly
    {
      return 0;   // no collision has occured
    }


}    // end of function collision_detection, returns a UINT8 or an unsigned integer of 8 bits



// MAX YOU MAY WANT TO GET RID OF THE INTERRUPT ALTOGETHER EVENTUALLY
// This is an interrupt which can be called at any time
// this can occur at any time but can't return  anything
// maybe use this code to run background animations or play music 
void set_tim()
{

  // timer interrupt code should go here
  

} // end of tim function for interrupt




// determines if collisoin has occured and depending on direction handles the collision appropriately
void set_resolve_collision()
{

    // this is used to enable the invincibility frames
    if ( can_be_damaged )  // is true then do the below
    {

           // These flags below represent the players current direction
          //   dir[0] = 0;     1 down is true, 0 down is false
        //     dir[1] = 0;   1 up is true, 0 up is false
        //     dir[2] = 0;   1 left is true, 0 left is false
       //      dir[3] = 0;   1 right is true, 0 right is false
      //       dir[4] = 0; // 1 up and left is true, 0 up left is false
      //       dir[5] = 0; // 1 up and right is true, 0 up right is false
      //       dir[6] = 0; // 1 down and left is true, 0 down left is false
      //       dir[7] = 0; // 1 down and right is true, 0 down right is false


    //  MAX YOU SHOULD STRUCTURE THIS SO THAT THE THROWBACK MAKES SENSE
   // make note how the below conditional codeblock was structured similiar to the set_check_player_input loop
  // collision has occured and player has last pressed up and right key
    if ( (collision_detection(player_dxdy[0], player_dxdy[1], 16, 32, block_dxdy[0], block_dxdy[1], 8, 16 ) && ( dir[5] == 1 )  ) )
     {
       // move_sprite(10, player_dxdy[0] + 16, player_dxdy[1] );
      
 
      // the player has taken damage
      damaged = 1;

     }      // collision has occured and player is facing up and left key
     else  if ( (collision_detection(player_dxdy[0], player_dxdy[1], 16, 32, block_dxdy[0], block_dxdy[1], 8, 16 ) && ( dir[4] == 1 )  ) )
     {
       // move_sprite(10, player_dxdy[0] + 16, player_dxdy[1] );
      
 
      // the player has taken damage
      damaged = 1;

     }     // down and left is true and collision has occured
     else  if ( (collision_detection(player_dxdy[0], player_dxdy[1], 16, 32, block_dxdy[0], block_dxdy[1], 8, 16 ) && ( dir[6] == 1 )  ) )
     {
       // move_sprite(10, player_dxdy[0] + 16, player_dxdy[1] );
      
 
      // the player has taken damage
      damaged = 1;

     }        //  down and right is true and collision has occured
     else  if ( (collision_detection(player_dxdy[0], player_dxdy[1], 16, 32, block_dxdy[0], block_dxdy[1], 8, 16 ) && ( dir[7] == 1 )  ) )
     {
       // move_sprite(10, player_dxdy[0] + 16, player_dxdy[1] );
      
 
      // the player has taken damage
      damaged = 1;

     }    // up  is true and collison has occured
     else  if ( (collision_detection(player_dxdy[0], player_dxdy[1], 16, 32, block_dxdy[0], block_dxdy[1], 8, 16 ) && ( dir[1] == 1 )  ) )
     {
       // move_sprite(10, player_dxdy[0] + 16, player_dxdy[1] );
      
 
      // the player has taken damage
      damaged = 1;

     }     // down is true and collison has occured
     else  if ( (collision_detection(player_dxdy[0], player_dxdy[1], 16, 32, block_dxdy[0], block_dxdy[1], 8, 16 ) && ( dir[0] == 1 )  ) )
     {
       // move_sprite(10, player_dxdy[0] + 16, player_dxdy[1] );
      
 
      // the player has taken damage
      damaged = 1;

     }    // right is true and collision has occured
     else  if ( (collision_detection(player_dxdy[0], player_dxdy[1], 16, 32, block_dxdy[0], block_dxdy[1], 8, 16 ) && ( dir[3] == 1 )  ) )
     {
       // move_sprite(10, player_dxdy[0] + 16, player_dxdy[1] );
      
 
      // the player has taken damage
      damaged = 1;

     }      // left is true and collision has occured
    else   // no collison has occured, so no damage my dude
     {
        // the player is out of harms way
       damaged = 0;  
     }


      // checks to see whether the player is damaged and if so act on it
   if (damaged == 1)
  {
     player_health -= 1;  // lose one point of health

    // this code moves the hidden player back and forth always to cause damage even when not moving if on harmful tile
    // doesn't work currently because of the scrolling.
     player_dxdy[0] +=1;
     player_dxdy[1] +=1;
     player_dxdy[0] -= 1;
     player_dxdy[1] -= 1;  
    
  }

  // not sure if I should place the above here again
   
 }  // end of if ( can_be_damaged ) conditional



}  // end of set_function resolve_collision



 // does this when the players health reaches 0/6 or 0/3
void set_play_death()
{

   if ( player_health == 0 )
    {

      
      // ALL OF THIS STUFF IS TEMPORARY AND WILL BE REPLACED WITH A DEATH ANIMATION
       player_dxdy[0] = 128;
       player_dxdy[1] = 128;

     // hide the current sprite template
       set_sprite_prop(0,2);
       set_sprite_prop(1,2);
       set_sprite_prop(2,2);
       set_sprite_prop(3,2);
       set_sprite_prop(4,2);
       set_sprite_prop(5,2);
       set_sprite_prop(6,2);
       set_sprite_prop(7,2);
       set_sprite_prop(8,2);
       set_sprite_prop(9,2); 

      // now move the invisible sprite off screen
      move_sprite(0, player_dxdy[0], player_dxdy[1] );
      move_sprite(1, player_dxdy[0] + 8, player_dxdy[1] );
     move_sprite(2, player_dxdy[0], player_dxdy[1] + 14);
     move_sprite(3, player_dxdy[0] + 8, player_dxdy[1] + 14);
     move_sprite(3, 200, 200);
     move_sprite(4,player_dxdy[0], player_dxdy[1] + 14);
     move_sprite(5,player_dxdy[0] + 8, player_dxdy[1] + 14);

    }    // end of conditional if ( player_health == 0 )



}  // end of function set_play_death()



// this function will start if it is true the player is damaged and still alive and if so then will give them some time so 
// the player won't take damage through the use of i_timer
// i_timer is used to keep the player invincible for a short time
void set_temp_invincibility()
{
    
     // MAX YOU MAY HAVE TO USE THE ABOVE CODE OR PLACE THIS WITHIN THE dir[#] FOR HANDLING THE DIRECTION
     // I took advantage of the 8-bit cpu here, eventually the counter i_timer reaches 1974, which is around 2 seconds,
    // well I faked it and made it.
   if ( (damaged == 1) && (player_health != 0) )
   {

        // I decided to give the player a lot of invincibility time after taking damage
      if ( i_timer < 1974)   // here 2000 should mean 2 seconds but its  really 1974
       {
          can_be_damaged = 0;  // player can't take damage now
          i_timer += 13;    // this is to run out the invinciblity timer eventually
       }
       else  // timer has ran out, meaning over 2000 or two seconds, well not really
       {
          can_be_damaged = 1;  // the player can now take damage once again
          i_timer = 0;  // resets the invincibility timer and won't be incremented by 13
       }

   }  


}   // end of function set_temp_invincibility


