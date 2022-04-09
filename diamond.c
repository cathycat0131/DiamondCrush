/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00
#define BACKGROUND 0
#define CLEAR -1

    
#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */
#define SIZE_BOARD 10
#define WIDTH 18
#define NUM_BOXES 8
#define SIZE_DIAMOND 12
#define SPEED 2

#define SHIFT_X 65
#define SHIFT_Y 30

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h> 
#include <time.h>


// Begin part3.c code for Lab 7

//Functions for graphics
void clear_screen();
void clear_screen_char();
void wait_for_vsync();
void plot_pixel(int x, int y, short int line_color);
void draw_line(int x0, int x1, int y0, int y1, short int colour);
void swap(int *a, int *b);
void move_line();
void clearBlock(int row, int col, int colour);
void drawCrushLine_V(int row1, int row2, int col1, int col2);
void drawCrushLine_H(int row1, int row2, int col1, int col2);

//Shape
void drawDiamond(int row, int col, short int colour);
void drawSqaure(int row, int col, short int colour);
void drawVerticalSqaure(int row, int col, short int colour);
void drawTraingle(int row, int col, short int colour);
void circle(int row, int col, int r, short int colour);
void drawCircle(int xc, int yc, int x, int y, short int colour);
int convertRow(int row);
int convertCol(int col);

//Text
void write_char(int x, int y, char c);
void move_text();
void draw_text(int x, int y, char* word);

//Function for the game 
bool checkIfValid(int row, int col);
int clearDiamond(int row1, int row2, int col1, int col2);
void printBoard();
void swapValue(int row1, int row2, int col1, int col2);
void initializeBoard();
bool checkDiamondAndClear();

//Rendering
void diamondSelection(int row, int col, int num);
void drawAllDiamonds();
void automaticPlay();
void diamondSelection_XY(int x, int y, int num);

//Move Rendering
void move_triangle(int row1,int row2, int col, int colour);
void move_VerticalSqaure(int row1,int row2, int col, int colour);
void move_sqaure(int row1,int row2, int col, int colour);
void diamondMove(int row1,int row2, int col, int num);
void diamondBlockMove(int row1,int row2, int col1, int col2, int numOfClear);
void move_triangle_pixel(int row, int row1,int row2, int col, int colour);
void move_sqaure_pixel(int row, int row1,int row2, int col, int colour);
void move_VerticalSqaure_pixel(int row, int row1,int row2, int col, int colour);

volatile int pixel_buffer_start; // global variable
volatile int * pixel_ctrl_ptr;
volatile int * HEX3_ptr;
volatile int * LEDR_BASE_PTR;
int diamonds[SIZE_BOARD][SIZE_BOARD];
int num;
int score = 0;
char seg7[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
0x7f, 0x67, 0x77};

int main(void){

	pixel_ctrl_ptr = (int *)0xFF203020;
	HEX3_ptr = (int*) HEX3_HEX0_BASE;
	LEDR_BASE_PTR = (int*)LEDR_BASE;
	
	//Initialize the diamonds array to store the value of the diamond

	/* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer
	//clear_screen_char();
	
	//Initialize the diamonds randomly
    int row1 = 0, row2 = 0, col1 = 0, col2 = 0;
    bool win = false;
    bool checking = true;
    srand(time(0));
    initializeBoard();

	drawAllDiamonds();
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
	pixel_buffer_start = *(pixel_ctrl_ptr + 1); 
	drawAllDiamonds();
	while(checking == true){
		checking = checkDiamondAndClear();
		int count = score;
		int count_2 = 0;
		while(count >= 10){
			count -= 10;
			count_2++;
		} 
	}
	while(1){
		drawAllDiamonds();
		wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		pixel_buffer_start = *(pixel_ctrl_ptr + 1); 
	}
    return 0;
    
}

void drawAllDiamonds(){
	int i = 0, j = 0;
	for( i = 0; i < SIZE_BOARD; i++){
		for(j = 0; j < SIZE_BOARD; j++){
            diamondSelection(i,j, diamonds[i][j]);
		}
	}
}

int convertRow(int row){
	row = SHIFT_Y+ WIDTH/2 + row * WIDTH;
	return row;
}

int convertCol(int col){
	col = SHIFT_X + WIDTH/2 + col * WIDTH;
	return col;
}

void diamondSelection(int row, int col, int num){
	row = convertRow(row);
	col = convertCol(col);
	if(num == 0){
		clearBlock(row, col, BACKGROUND);
		
		drawSqaure(row,col,RED);
	}else if(num == 1){
		clearBlock(row, col, BACKGROUND);
		drawSqaure(row,col,BLUE);
	}else if(num == 2){
		clearBlock(row, col, BACKGROUND);
		drawVerticalSqaure(row, col, YELLOW);
	}else if(num == 3){
		clearBlock(row, col, BACKGROUND);
		drawVerticalSqaure(row,col,CYAN);
	}else if(num == 4){
		clearBlock(row, col, BACKGROUND);
		drawTraingle(row,col, GREEN);
	}else if(num == 5){
		clearBlock(row, col, BACKGROUND);
		//circle(row, col, 6,ORANGE);
		drawTraingle(row,col, ORANGE);
	}
}

void diamondSelection_XY(int x, int y, int num){
	int row = x;
	int col = y;
	if(num == 0){
		clearBlock(row, col, BACKGROUND);
		drawSqaure(row,col,RED);
	}else if(num == 1){
		clearBlock(row, col, BACKGROUND);
		drawSqaure(row,col,BLUE);
	}else if(num == 2){
		clearBlock(row, col, BACKGROUND);
		drawVerticalSqaure(row, col, YELLOW);
	}else if(num == 3){
		clearBlock(row, col, BACKGROUND);
		drawVerticalSqaure(row,col,CYAN);
	}else if(num == 4){
		clearBlock(row, col, BACKGROUND);
		drawTraingle(row,col, GREEN);
	}else if(num == 5){
		clearBlock(row, col, BACKGROUND);
		//circle(row, col, 6,ORANGE);
		drawTraingle(row,col, ORANGE);
	}else if(num == CLEAR){
		clearBlock(row, col, BACKGROUND);
	}
}

void clearBlock(int row, int col,int colour){
	int y, x;
    	for (y = row-WIDTH/2;y <= row + WIDTH/2-1; y++)
                for (x = col-WIDTH/2; x <= col + WIDTH/2-1; x++)
                        plot_pixel (x,y,colour);
} 

void move_text(){
	char* word = "";
	while(1){
		int new_x = 0;
		int change = 1;
		write_char(new_x-1,5,*word);
		while(*word){
			word += change;
		}
		word = "Cathy";
		write_char(new_x,5,*word);
		while(*word){
			word += change;
		}
		word = "";
		while(*word){
			word += change;
		}
		word = "";
		if(new_x == 80){
			change = -1;
		}	
		if(new_x == 0){
			change = 1;
		}
		clear_screen_char();
		}	
}

void write_char(int x, int y, char c) {
  // VGA character buffer
  volatile char *character_buffer = (char *) (0xC9000000 + (y<<7) + x);
  *character_buffer = c;
}

void draw_text(int x, int y, char* word){
	while(*word){
			write_char(x,y,*word);
			word ++;
			x++;
		}
}



void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void move_line(){
	int new_y = 0;
	int change = 1;
	while(1){
		draw_line(100,200,new_y, new_y, WHITE);
		wait_for_vsync();
		draw_line(100,200,new_y, new_y, 0);
		new_y += change;
		if(new_y == 239){
			change = -1;
		}	
		if(new_y == 0){
			change = 1;
		}
	}	
}

// code for subroutines (not shown)
void wait_for_vsync(){
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    int status;
    
    //Launch the swap
    * pixel_ctrl_ptr = 1;
    
    //Poll for status bit
    status = * (pixel_ctrl_ptr + 3);
    while((status & 0x01) != 0){
        status = *(pixel_ctrl_ptr + 3);
    }

    //exit
}

void clear_screen()
{
        int y, x;
        for (x = 0; x < RESOLUTION_X; x++)
                for (y = 0; y < RESOLUTION_Y; y++)
                        plot_pixel (x, y, 0);
}


void plot_character(int x, int y, short int colour){
	 *(short int *)(FPGA_CHAR_BASE + (y << 10) + (x << 1)) = colour;

}
 
void draw_line(int x0, int x1, int y0, int y1, short int colour)
{
    bool is_steep = (abs(y1-y0) > abs(x1-x0));
    
    if(is_steep == true){
        swap(&x0,&y0);
        swap(&x1,&y1);
    }
    if(x0 > x1){
        swap(&x0,&x1);
        swap(&y0,&y1);
    }

    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = - (deltax / 2);
    int y = y0;
    int y_step;

    if(y0 < y1){
        y_step = 1;
    }else{
        y_step = -1;
    }

    for(int x = x0; x <= x1; ++x){
        if(is_steep == true){
            plot_pixel(y,x, colour);
        }else{
            plot_pixel(x,y, colour);
        }
        error = error + deltay;
        if(error >= 0){
            y = y + y_step;
            error = error - deltax;
        }
    }
    
}

void swap(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

void drawDiamond(int row, int col, short int colour)
{	
	draw_line(40,50, 50, 50, colour);//Top
	draw_line(30,40, 60, 50, colour);//Top left
	draw_line(50,60, 50, 60, colour);//Top right
	draw_line(30,45, 60, 80, colour);//Bottom left
	draw_line(60,45, 60, 80, colour);//Bottom right
}


void drawSqaure(int row, int col,short int colour){
	int size = SIZE_DIAMOND, i, j;
	draw_line(col-size/2,col+size/2, row-size/2, row-size/2, colour);//Top
	draw_line(col-size/2,col-size/2, row-size/2, row+size/2, colour);//Left
	draw_line(col+size/2,col+size/2, row-size/2, row+size/2, colour);//Right
	draw_line(col-size/2,col+size/2, row+size/2, row+size/2, colour);//Bottom
	//Fill the colour
	for(j = row-size/2; j < row+size/2; j++){
		draw_line(col-size/2,col+size/2, j, j, colour);
	}
}



void drawVerticalSqaure(int row, int col,short int colour){
	int height = SIZE_DIAMOND ;
	draw_line(col - height / 2,col, row, row-height/2, colour);//TOP LEFT
	draw_line(col,col + height / 2, row-height/2, row, colour);//TOP RIGHT
	draw_line(col - height / 2,col, row, row+height/2, colour);//BOTTOM LEFT
	draw_line(col,col + height / 2, row+height/2, row, colour);//BOTTOM RIGHT
	//Fill the colour
	int a = col,b = col,c,d;

	//Fill upper
	for(c = row-height/2; c < row; c++){
		if(a > col-height/2){
			draw_line(a,b,c,c,colour);
			a --; 
			b ++;
		}else{
			break;
		}
	}
	//Fill lower
	for(c = row; c < row+height/2; c++){
		if(a < col){
			draw_line(a,b,c,c,colour);
			a ++; 
			b --;
		}else{
			break;
		}
	}
}


void drawTraingle(int row, int col, short int colour){
	int height = SIZE_DIAMOND ;
	draw_line(col-height/2,col, row+height/2, row-height/2, colour);//LEFT
	draw_line(col,col+height/2, row-height/2,row+height/2, colour);//RIGHT
	draw_line(col-height/2,col+height/2, row+height/2, row+height/2, colour);//BOTTOM

	int i = col-height/2;
	int j = col+height/2+1;
	int k = row + height/2;
	//Fill in the colour
	while(i != j && k != row-height/2){
		draw_line(i,j, k, k, colour);
		i++;
		j--;
		k -= 2;
	}
	i = col-height/2;
	j = col+height/2;
	k = row + height/2 - 1; 
	while(i != j && k != row-height/2 - 1){
		draw_line(i,j, k, k, colour);
		i++;
		j--;
		k -= 2;
	}
}


void diamondMove(int row1,int row2, int col, int num){
	row1 = convertRow(row1);
	row2 = convertRow(row2);
	col = convertCol(col);
	if(num == 0){
		move_sqaure(row1,row2,col,RED);
	}else if(num == 1){
		move_sqaure(row1,row2,col,BLUE);
	}else if(num == 2){
		move_VerticalSqaure(row1,row2,col, YELLOW);
	}else if(num == 3){
		move_VerticalSqaure(row1,row2,col, CYAN);
	}else if(num == 4){
		move_triangle(row1,row2,col,GREEN);
	}else if(num == 5){
		move_triangle(row1,row2,col,ORANGE);
	}
}

void crush_diamonds(int row1,int row2, int col1, int col2,int numOfClear){
	int i,j;
	//Crush horizontally
	if(row1 == row2){
		//Earase the blocks
		for(i = col1; i <= col2; i++){
			//erase the diamonds
			clearBlock(row1,i,BACKGROUND);
		}
		//diamondSelection(row1,col1,diamonds[])


	}


	//Crush horizontally
	if(col1 == col2){
		for(i = row1; i <= row2; i++){
			clearBlock(i,col1,BACKGROUND);
		}
	}
}
// void diamondBlockMove(int row1,int row2, int col1, int col2, int numOfClear){
// 	int i = 0;
// 	int col = 0;
// 	int totalTravel;
// 	int count;
// 	int j,k;
// 	printf("c\n");

// 	if(row1 == row2){
// 		totalTravel = numOfClear * WIDTH;
// 		count = 0;
// 		while(count <= totalTravel){
// 			printf("k\n");
// 			for(i = 0; i <= row2-numOfClear; i++){
// 				printf("a\n");
// 				for(col = col1; col<=col2; col++){
// 					num = diamonds[i][col];
// 					j = convertRow(i);
// 					k = convertCol(col);
// 					if(num == 0){
// 						move_sqaure_pixel(j,j+count,row2,k,RED);
// 					}else if(num == 1){
// 						move_sqaure_pixel(j,j+count,row2,k,BLUE);
// 					}else if(num == 2){
// 						move_VerticalSqaure_pixel(j,j+count,row2,k, YELLOW);
// 					}else if(num == 3){
// 						move_VerticalSqaure_pixel(j,j+count,row2,k, CYAN);
// 					}else if(num == 4){
// 						move_triangle_pixel(j,j+count,row2,k,GREEN);
// 					}else if(num == 5){
// 						move_triangle_pixel(j,j+count,row2,k,ORANGE);
// 					}
// 				}
// 			}
// 			//clearBlock(j+count-2,k,BACKGROUND);
// 			wait_for_vsync(); // swap front and back buffers on VGA vertical sync
//     		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
// 			count ++;
// 		}
// 		//clearBlock(j+count-3,k-1,BACKGROUND);
		
// 	}


// 	if(col1 == col2){
// 		totalTravel = numOfClear * WIDTH;
// 		count = 0;
		
// 		while(count <= totalTravel){
// 			printf("kk\n");
// 			for(i = 0; i <= row2-numOfClear; i++){
// 				printf("nn\n");
// 				for(col = (col1); col<=(col2); col++){
// 					num = diamonds[i][col];
// 					j = convertRow(i);
// 					k = convertCol(col);
// 					if(num == 0){
// 						move_sqaure_pixel(j,j+count,row2,k,RED);
// 					}else if(num == 1){
// 						move_sqaure_pixel(j,j+count,row2,k,BLUE);
// 					}else if(num == 2){
// 						move_VerticalSqaure_pixel(j,j+count,row2,k, YELLOW);
// 					}else if(num == 3){
// 						move_VerticalSqaure_pixel(j,j+count,row2,k, CYAN);
// 					}else if(num == 4){
// 						move_triangle_pixel(j,j+count,row2,k,GREEN);
// 					}else if(num == 5){
// 						move_triangle_pixel(j,j+count,row2,k,ORANGE);
// 					}
// 				}
// 			}
// 			//clearBlock(j+count-2,k,BACKGROUND);
// 			wait_for_vsync(); // swap front and back buffers on VGA vertical sync
//     		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
// 			printf("%d",count);
// 			count ++;
// 		}
// 		//clearBlock(j+count-3,k-1,BACKGROUND);
// 	}
// }

void diamondBlockMove(int row1,int row2, int col1, int col2, int numOfClear){
	int i = 0,j = 0,a = 0,b = 0;
	int init = 0;
	if(row1 == row2 && row1!=0){
		for(i = 1; i < WIDTH;i+=SPEED){
			//Clear old drawing
			if(init >= 2){
				for(b = 0; b < row2;b++){
					for(a = col1; a <= col2; a++){
						diamondSelection_XY(convertRow(b)+i-2*SPEED,convertCol(a),CLEAR);
					}
				}
			}
			
			for(b = 0; b < row2;b++){
				for(a = col1; a <= col2; a++){
					diamondSelection_XY(convertRow(b)+i,convertCol(a),diamonds[b][a]);
				}
			}
			wait_for_vsync(); // swap front and back buffers on VGA vertical sync
      		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
			init ++;
		}
		//drawAllDiamonds();
	}
	
	init = 0;
	if(col1 == col2){
		for(i = 1; i < WIDTH*numOfClear;i+=SPEED){
			//Clear old drawing
			if(init >= 2){
				for(b = 0; b <= row2-numOfClear;b++){
					for(a = col1; a <= col2; a++){
						diamondSelection_XY(convertRow(b)+i-2*SPEED,convertCol(a),CLEAR);
					}
				}
			}
			
			for(b = 0; b <= row2-numOfClear;b++){
				for(a = col1; a <= col2; a++){
					diamondSelection_XY(convertRow(b)+i,convertCol(a),diamonds[b][a]);
				}
			}
			init ++;
			wait_for_vsync(); // swap front and back buffers on VGA vertical sync
      		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
		}
		}
}


void move_triangle_pixel(int row, int row1,int row2, int col, int colour){
	//clearBlock(row2,col,BACKGROUND);
	
	if(row1-2>=row){
	clearBlock(row1-2,col,BACKGROUND);
	}
	drawTraingle(row1,col,colour);
}

void move_sqaure_pixel(int row, int row1,int row2, int col, int colour){
	//clearBlock(row2,col,BACKGROUND);

	
	if(row1-2>=row){
	clearBlock(row1-2,col,BACKGROUND);
	}
	drawSqaure(row1,col,colour);
}

void move_VerticalSqaure_pixel(int row, int row1,int row2, int col, int colour){
	//clearBlock(row2,col,BACKGROUND);
	if(row1-2>=row){
	clearBlock(row1-2,col,BACKGROUND);
	}
	drawVerticalSqaure(row1,col,colour);
}


void move_triangle(int row1,int row2, int col, int colour){
	clearBlock(row2,col,BACKGROUND);
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	clearBlock(row2,col,BACKGROUND);
	int i = 0;
	for(i = row1; i <= row2; i+=1){
		if(i-2 >= row1){
			clearBlock(i-2,col,BACKGROUND);
		}
		drawTraingle(i,col,colour);
		wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	}
}

void move_sqaure(int row1,int row2, int col, int colour){
	clearBlock(row2,col,BACKGROUND);
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);
	clearBlock(row2,col,BACKGROUND);
	int i = 0;
	for(i = row1; i <= row2; i += 1){
		if(i-2 >= row1){
			clearBlock(i-2,col,BACKGROUND);
		}
		drawSqaure(i,col,colour);
		wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	}
}

void move_VerticalSqaure(int row1,int row2, int col, int colour){
	clearBlock(row2,col,BACKGROUND);
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);
	clearBlock(row2,col,BACKGROUND);
	int i = 0;
	for(i = row1; i <= row2; i+=1){
		if(i-2 >= row1){
			clearBlock(i-2,col,BACKGROUND);
		}
		drawVerticalSqaure(i,col,colour);
		wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	}
}



void drawCircle(int xc, int yc, int x, int y, short int colour)
{
    plot_pixel(xc+x, yc+y, colour);
    plot_pixel(xc-x, yc+y, colour);
    plot_pixel(xc+x, yc-y, colour);
    plot_pixel(xc-x, yc-y, colour);
    plot_pixel(xc+y, yc+x, colour);
    plot_pixel(xc-y, yc+x, colour);
    plot_pixel(xc+y, yc-x, colour);
    plot_pixel(xc-y, yc-x, colour);
}

void circle(int row, int col, int r, short int colour)
{
    while(r >= 0){
    	int x = 0, y = r;
    	int d = 3 - 2 * r;
	    drawCircle(col, row, x, y,colour);
	    while (y >= x)
	    {
	        // for each pixel we will
	        // draw all eight pixels
	        x++;
	        // check for decision parameter
	        // and correspondingly
	        // update d, x, y
	        if (d > 0)
	        {
	            y--;
	            d = d + 4 * (x - y) + 10;
	        }
	        else
	            d = d + 4 * x + 6;
	        drawCircle(col, row, x, y,colour);
	        //delay(50);
	    }
	    r --;
    }
}

void clear_screen_char()
{
        int y, x;
        for (x = 0; x < 80; x++)
                for (y = 0; y < 60; y++)
                    	write_char(x, y, "");
}

void initializeBoard(){
    int i = 0, j = 0;
    for( i = 0; i < SIZE_BOARD; i++){
		for(j = 0; j < SIZE_BOARD; j++){
			num = rand() % 6;
			diamonds[i][j] = num;
            //printf("%d",num);
            //printf(" ");
		}
        //printf("\n");  
	}
}
void printBoard(){
    int i = 0, j = 0;

	for( i = 0; i < SIZE_BOARD; i++){
		for(j = 0; j < SIZE_BOARD; j++){
            printf("%d",diamonds[i][j]);
            printf(" ");
		}
        printf("\n");
	}
}
void swapValue(int row1, int row2, int col1, int col2){
    printf("Swap value at row %d, col %d and row %d, col %d\n", row1,col1,row2,col2);
    int value1 = diamonds[row1][col1];
    int value2 = diamonds[row2][col2];
    printf("OK\n");
    diamonds[row1][col1] = value2;
    diamonds[row2][col2] = value1;
}

bool checkIfValid(int row, int col){
	int diamonds_colour = diamonds[row][col];
	//int currentColour = diamonds_colour;
	int count = 0;
	int i = 0;
	int first = 0;
	int row1 = 0,row2 = 0, col1 = 0, col2 = 0;
	//printf("3\n");
	//Check the neighbouring col colour (Horizontal)
	for(i = 0 ; i < SIZE_BOARD; i++){
		if(diamonds[row][i] == diamonds_colour){
			if(first == 0){
				col1 = i;
				first = 1;
			}
			count ++;
		}else{
            //Check if there are 3 or more 
            if(count >= 3){
                col2 = i - 1;
                break;
            }
			count = 0;
            first = 0;
		}
	}
    //printf("4\n");
    //Check if there are 3 or more
	if(count >= 3){
        col2 = i - 1;
		row1 = row;
		row2 = row;
        score += clearDiamond(row1, row2, col1, col2);
		return true;
	}
    //printf("5\n");
    //Check the neighbouring col colour (vertical)
	first = 0;
    count = 0;
	for(i = 0 ; i < SIZE_BOARD; i++){
		if(diamonds[i][col] == diamonds_colour){
			if(first == 0){
				row1 = i;
				first = 1;
			}
			count ++;
		}else{
            if(count >= 3){
                row2 = i - 1;
                break;
            }
			count = 0;
            first = 0;
		}
	}
//printf("6\n");
	if(count >= 3){
		col1 = col;
		col2 = col;
        row2 = i - 1;
		score += clearDiamond(row1, row2, col1, col2);
		return true;
	}

	return false;
}


int clearDiamond(int row1, int row2, int col1, int col2){
	int i = 0;
	int numOfClear = 0;
	int r1 = row1, r2 = row2,c1 = col1, c2 = col2;
	
	int a,b;
    //Clear vertical
	if(col1 == col2){
		numOfClear = row2 - row1 + 1;
		drawCrushLine_V(r1,r2, c1,c2);
		diamondBlockMove(r1,r2, c1,c2,numOfClear);
		 
		while(row2 >= 0){
			if(row2 - numOfClear >= 0){
				diamonds[row2][col1] = diamonds[row2 - numOfClear][col1];
				//diamondMove(row2 - numOfClear,row2, col1,diamonds[row2 - numOfClear][col1]);
				//diamondBlockMove(row1,row2, col1,col2,numOfClear);
				diamondSelection(row2, col1, diamonds[row2][col1]);
				wait_for_vsync(); // swap front and back buffers on VGA vertical sync
  		       	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
				diamondSelection(row2, col1, diamonds[row2][col1]);
			}else{
				diamonds[row2][col1] = rand() % 6;
				//diamondMove(0,row2,col1,diamonds[row2][col1]);
				//diamondBlockMove(row1,row2, col1,col2,numOfClear);
				diamondSelection(row2, col1, diamonds[row2][col1]);
				wait_for_vsync(); // swap front and back buffers on VGA vertical sync
  		       	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
				diamondSelection(row2, col1, diamonds[row2][col1]);
			}
			row2 --;
		}
	}

	//Clear horizontal
	if(row1 == row2){
		numOfClear = col2 - col1 + 1;
		drawCrushLine_H(r1,r2, c1,c2);
		diamondBlockMove(r1,r2, c1,c2,numOfClear);
		while(row1 >= 0){
			for(i = col1; i <= col2;i++){
				if(row1-1 >= 0){
					diamonds[row1][i] = diamonds[row1-1][i];
					//diamondMove(row1-1,row1,i,diamonds[row1-1][i]);
					diamondSelection(row1, i, diamonds[row1][i]);
					wait_for_vsync(); // swap front and back buffers on VGA vertical sync
  		       		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
					diamondSelection(row1, i, diamonds[row1][i]);
				}else{	
                    diamonds[row1][i] = rand() % 6;
                    //diamondMove(0,row1,i,diamonds[row1][i]);
                    diamondSelection(row1, i, diamonds[row1][i]);
					wait_for_vsync(); // swap front and back buffers on VGA vertical sync
  		       		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
					diamondSelection(row1, i, diamonds[row1][i]);
                }
			}
			row1 --;
		}
		
	}
    return numOfClear;
}


bool checkDiamondAndClear(){
	int i = 0, j = 0;
	bool win = false;

	for( i = 0; i < SIZE_BOARD; i++){
		for(j = 0; j < SIZE_BOARD; j++){
            win = checkIfValid(i, j);
            if(win == true){
            	printf("Happy days!");
            	printf("Score: ");
          		printf("%d", score);
         		printf("\n");
				int count = score;
				int count_2 = 0;
				 while(count >= 10){
					count -= 10;
					 count_2++;
				 }
				 *HEX3_ptr = seg7[count_2]<< 8 | seg7[count];
				*LEDR_BASE_PTR = score;
                return true;
            }
            win = false;
		}
	}
    return false;
}

void automaticPlay(){
	int i = 0, j = 0;
	bool win = false;
	for( i = (rand()%(SIZE_BOARD-1)); i < SIZE_BOARD-1; i++){
		for(j = (rand()%(SIZE_BOARD-1)) ; j < SIZE_BOARD-1; j++){
			swapValue(i,j,i+1,j+1);

			//Check one position
		     win = checkIfValid(i, j);
		     //Check the other position
		     if(win == false){
		         //Swap Back
		         swapValue(i+1, j+1, i, j);
		         printf("Wrong move!\n");
		     }else{
		         printf("Excellent!\n");
		     }

            if(win == true){
            	printf("HAHA!");
            	printf("Score: ");
          		printf("%d", score);
         		printf("\n");
				int count = score;
				int count_2 = 0;
				 while(count >= 10){
					count -= 10;
					 count_2++;
				 }
				 *HEX3_ptr = seg7[count_2]<< 8 | seg7[count];
				 *LEDR_BASE_PTR = score;
            }
			drawAllDiamonds();
			wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
		}
	}
}

void drawCrushLine_V(int row1, int row2, int col1, int col2){
	draw_line(convertCol(col1), convertCol(col2), convertRow(row1)-1, convertRow(row2)+1, WHITE);
	draw_line(convertCol(col1)-1, convertCol(col2)-1, convertRow(row1)-1, convertRow(row2)+1, WHITE);
	draw_line(convertCol(col1)+1, convertCol(col2)+1, convertRow(row1)-1, convertRow(row2)+1, WHITE);
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
  	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
  	draw_line(convertCol(col1), convertCol(col2), convertRow(row1)-1, convertRow(row2)+1, WHITE);
  	draw_line(convertCol(col1)-1, convertCol(col2)-1, convertRow(row1)-1, convertRow(row2)+1, WHITE);
	draw_line(convertCol(col1)+1, convertCol(col2)+1, convertRow(row1)-1, convertRow(row2)+1, WHITE);
}

void drawCrushLine_H(int row1, int row2, int col1, int col2){
	draw_line(convertCol(col1)-1, convertCol(col2)+1, convertRow(row1)-1, convertRow(row2)-1, WHITE);
	draw_line(convertCol(col1)-1, convertCol(col2)+1, convertRow(row1), convertRow(row2), WHITE);
	draw_line(convertCol(col1)-1, convertCol(col2)+1, convertRow(row1)+1, convertRow(row2)+1, WHITE);
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
  	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
  	draw_line(convertCol(col1)-1, convertCol(col2)+1, convertRow(row1)-1, convertRow(row2)-1, WHITE);
	draw_line(convertCol(col1)-1, convertCol(col2)+1, convertRow(row1), convertRow(row2), WHITE);
	draw_line(convertCol(col1)-1, convertCol(col2)+1, convertRow(row1)+1, convertRow(row2)+1, WHITE);
}








	for(i = 0;i < 36; i++){
		for(j = 0 ; j < 34; j++){
			plot_pixel(i,j,Red[i][j]);
		}
	}




const Red[36][34] = {
	{65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65503,65405,65178,64983,64983,65145,65405,65503,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535},
	{65535,65535,65535,65535,65535,65535,65535,65535,65502,65210,64982,62804,56433,54223,47885,45707,39303,41383,45609,47788,52111,54289,58645,65081,65243,65470,65535,65535,65535,65535,65535,65535,65535,65535},
	{65535,65535,65535,65535,65535,65535,65406,64888,54096,49771,51690,49512,51722,60239,62417,60336,64723,60465,56239,54062,56046,58062,55917,49610,45546,54127,64854,65340,65535,65535,65535,65535,65535,65535},
	{65535,65535,65535,65535,65276,60630,51984,47693,58224,60240,60109,64238,64465,64659,64725,64822,62578,60594,64723,64626,64465,64401,64272,64077,40963,32769,37061,47788,58548,65308,65535,65535,65535,65535},
	{65535,65535,65535,64953,47790,49805,53999,64628,60563,56401,58417,60529,62578,64756,64789,60530,60562,56368,56465,64820,64756,64432,57769,43009,36864,32768,32768,28672,34981,39401,58743,65535,65535,65535},
	{65535,65535,65471,49871,56112,64693,64823,64822,62870,62966,65014,62869,62707,64821,64919,64951,64886,64982,65014,64949,64625,60205,64171,59848,59880,57768,59914,55753,49512,37029,45741,65438,65535,65535},
	{65535,65535,64953,51853,64757,65048,65080,65177,62967,65047,65079,65047,64723,60303,62351,64692,64950,65112,64982,62738,58318,56010,64203,61928,61895,64009,61832,61898,61963,34818,37127,64953,65535,65535},
	{65535,65535,64563,60175,64951,65209,65274,65112,65080,65080,65112,64885,62416,62319,62254,53932,56239,60562,60561,64787,64624,60171,64235,64040,63975,63911,63944,63913,55688,34818,28674,56403,65535,65535},
	{65535,65438,55949,64433,64885,65111,65143,65111,64983,64983,64983,64691,58093,58060,62383,64691,64691,64691,64886,64820,64560,62251,64267,64104,63975,63943,63944,61993,43172,30721,22528,39563,65340,65535},
	{65535,64984,58029,64595,64820,62836,65013,60756,64853,64724,64691,64561,64561,64691,64658,64658,62513,64594,64528,64593,64820,64592,62186,55717,59717,63911,61895,55718,32768,24576,26625,33191,60952,65535},
	{65535,56434,56045,64691,64690,62544,62706,65047,65112,65113,64950,64723,64561,64561,60399,60431,62513,62448,64497,64496,64528,64657,64591,47460,45153,47168,47266,53767,39139,26624,26624,20481,48306,65535},
	{65405,49965,60335,64561,64495,64527,60302,64885,64982,64950,62610,62481,64528,64528,64625,64625,64593,64529,64497,64497,64463,62415,64527,60203,45185,40960,38912,60139,64527,47688,28672,24576,35402,65341},
	{64952,43464,62318,64463,64268,62090,53736,64528,64917,62707,60400,62448,64528,64528,62479,62479,62480,64561,64497,64464,64562,64529,64463,64561,57897,40961,53638,64431,64462,64560,62253,36996,33030,64855},
	{64724,41286,62285,60075,57767,59880,59977,55946,64690,60497,64594,64594,64561,64593,64527,64625,62448,64529,64562,64497,64464,64594,64561,64464,64464,45349,57994,64495,64560,64463,64561,62351,34981,62548},
	{64821,32996,45414,57896,61993,61928,61961,62123,64561,64561,62481,62480,64560,64560,62447,62415,64561,64529,64529,64562,64530,62449,60368,64594,62513,56206,64624,62511,60398,64624,64592,51915,35078,64693},
	{65178,35304,26624,41123,57832,61993,61961,64400,64432,64691,64561,64529,64561,64528,64592,64528,64528,64529,62449,64529,62417,64530,64594,62546,58449,62707,58448,60528,64625,60366,43462,28770,31047,65018},
	{65470,39693,20481,28673,36898,51591,62156,64367,64496,64821,64529,64497,64496,64528,64592,62511,64625,62480,62481,64530,64466,64498,64498,62514,58418,64821,64853,60497,58190,39108,32866,22529,39693,65405},
	{65535,50356,22628,24578,28672,34817,49479,64269,64529,64626,64659,64496,64529,64496,62479,64624,62511,64560,64529,64465,64465,64497,64497,60433,60563,65016,65016,54093,36930,34816,32834,24642,46161,65535},
	{65535,60888,28934,24577,26624,26624,40961,53575,64237,64399,64756,64561,64496,64528,64527,64559,64495,64560,64496,64496,62512,60496,60496,58481,64982,65177,65079,56238,38913,40961,32768,30883,54614,65535},
	{65535,65244,35305,24577,26624,30720,40960,45057,55590,62091,64495,64625,64495,64528,64560,64495,64527,64527,64463,64560,64592,60496,60528,60625,64949,62836,65241,65079,45317,38912,34817,37287,65146,65535},
	{65535,65503,47983,24576,26624,32768,45121,47104,49153,53476,53703,64495,64528,64495,64528,64527,64527,64494,64463,64528,62480,62512,64593,64626,58449,62804,65208,65176,56076,36897,28672,47884,65470,65535},
	{65535,65535,58581,26658,26624,34848,43041,47104,49152,47104,47202,55848,64463,64528,64593,64528,64526,64462,64463,64463,60399,62610,64659,64691,62805,62935,65306,65338,62609,37091,30849,54352,65535,65535},
	{65535,65535,62903,33029,30720,38977,43008,45056,47104,47104,47104,47234,62252,64560,64625,64592,64559,64494,64528,62513,60627,64918,64983,63000,63163,65502,65534,65501,65145,41480,30947,64983,65535,65535},
	{65535,65535,65372,39465,28672,38945,43073,43040,45088,47104,47104,47137,60075,62382,62512,64690,64624,64559,64625,64723,64950,65210,65242,65470,65503,65502,65502,65405,65308,52111,37449,65242,65535,65535},
	{65535,65535,65535,48079,28705,39042,38945,40992,43072,43008,47104,47137,53703,62382,64626,64690,64755,64722,64852,64918,65080,65176,65371,65500,65437,65341,65374,65212,65050,47789,48015,65503,65535,65535},
	{65535,65535,65535,65243,43691,24576,34817,38945,34816,43073,45056,43009,45219,64561,64723,64821,64723,62610,64852,64982,65209,65111,65403,65338,65145,64983,62547,58257,39305,43693,65050,65535,65535,65535},
	{65535,65535,65535,65535,65113,47949,30721,32768,34881,34816,36864,38945,38946,53866,64853,64723,64723,60465,56401,64755,64755,64754,64591,64365,60042,55752,49512,36996,47821,64985,65535,65535,65535,65535},
	{65535,65535,65535,65535,65535,65178,47917,32867,26624,32833,36930,34817,34817,34850,62480,64626,60465,60465,58416,62512,49542,57798,55555,53410,53379,47138,36865,51949,65048,65535,65535,65535,65535,65535},
	{65535,65535,65535,65535,65535,65535,65179,47951,30981,26658,30721,32801,30720,30720,45512,64594,62481,62513,64625,53931,40960,45056,53378,49185,47170,39010,50029,63064,65535,65535,65535,65535,65535,65535},
	{65535,65535,65535,65535,65535,65535,65535,65277,48048,30949,28673,28672,28672,28672,34915,62416,64561,64593,64528,43269,43042,47138,45057,45154,39010,54158,65242,65535,65535,65535,65535,65535,65535,65535},
	{65535,65535,65535,65535,65535,65535,65535,65535,65439,52274,39174,28672,26625,24576,28672,51883,62447,64592,62189,41027,40962,40995,32770,37126,56370,65308,65535,65535,65535,65535,65535,65535,65535,65535},
	{65535,65535,65535,65535,65535,65535,65535,65535,65535,65470,64855,47756,26788,22528,26625,34980,64561,64431,47399,40995,36867,32836,43628,60758,65470,65535,65535,65535,65535,65535,65535,65535,65535,65535},
	{65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65276,50355,33321,22627,24578,56014,62223,36868,32772,39305,56371,65244,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535},
	{65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65503,63066,43919,28902,34982,39046,39176,49967,65018,65502,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535},
	{65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65405,60758,43660,45773,56500,65373,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535},
	{65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65438,65405,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535},

};











