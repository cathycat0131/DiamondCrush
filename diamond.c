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

    
#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */
#define SIZE_BOARD 10
#define WIDTH 18
#define NUM_BOXES 8
#define SIZE_DIAMOND 12

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
     // while(score <= 20){
     //     printBoard();
     //     printf("Score: ");
     //     printf("%d", score);
     //     printf("\n");
     //     scanf(" %d%d%d%d",&row1, &col1, &row2, &col2);
         
         // swapValue(row1, row2, col1, col2);

         // //Check one position
         // win = checkIfValid(row1, col1);
         // //Check the other position
         // if(win == false){
         //     win = checkIfValid(row2, col2);
         // }
         
         // if(win == false){
         //     //Swap Back
         //     swapValue(row2, row1, col2, col1);
         //     printf("Wrong move!\n");
         // }else{
         //     printf("Excellent!\n");
         // }
     // }
	//draw_text(20, 80, "Diamond!");
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
		drawAllDiamonds();
    	move_triangle(30,200, 50, RED);
    	move_VerticalSqaure(30,200, 50, RED);
    	move_sqaure(30,200, 50, RED);
		drawAllDiamonds();
         //automaticPlay();
		checking = true;
         int count = score;
		int count_2 = 0;
         while(count >= 10){
         	count -= 10;
			 count_2++;
         }
         *HEX3_ptr = seg7[count_2]<< 8 | seg7[count];
		*LEDR_BASE_PTR = score;
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


void diamondBlockMove(int row1,int row2, int col1, int col2, int numOfClear){
	int i = 0;
	int col = 0;
	int totalTravel;
	int count;
	int j,k;
	printf("c\n");

	if(row1 == row2){
		totalTravel = numOfClear * WIDTH;
		count = 0;
		while(count <= totalTravel){
			printf("k\n");
			for(i = 0; i <= row2-numOfClear; i++){
				printf("a\n");
				for(col = col1; col<=col2; col++){
					num = diamonds[i][col];
					j = convertRow(i);
					k = convertCol(col);
					if(num == 0){
						move_sqaure_pixel(j,j+count,row2,k,RED);
					}else if(num == 1){
						move_sqaure_pixel(j,j+count,row2,k,BLUE);
					}else if(num == 2){
						move_VerticalSqaure_pixel(j,j+count,row2,k, YELLOW);
					}else if(num == 3){
						move_VerticalSqaure_pixel(j,j+count,row2,k, CYAN);
					}else if(num == 4){
						move_triangle_pixel(j,j+count,row2,k,GREEN);
					}else if(num == 5){
						move_triangle_pixel(j,j+count,row2,k,ORANGE);
					}
				}
			}
			//clearBlock(j+count-2,k,BACKGROUND);
			wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
			count ++;
		}
		//clearBlock(j+count-3,k-1,BACKGROUND);
		
	}


	if(col1 == col2){
		totalTravel = numOfClear * WIDTH;
		count = 0;
		
		while(count <= totalTravel){
			printf("kk\n");
			for(i = 0; i <= row2-numOfClear; i++){
				printf("nn\n");
				for(col = (col1); col<=(col2); col++){
					num = diamonds[i][col];
					j = convertRow(i);
					k = convertCol(col);
					if(num == 0){
						move_sqaure_pixel(j,j+count,row2,k,RED);
					}else if(num == 1){
						move_sqaure_pixel(j,j+count,row2,k,BLUE);
					}else if(num == 2){
						move_VerticalSqaure_pixel(j,j+count,row2,k, YELLOW);
					}else if(num == 3){
						move_VerticalSqaure_pixel(j,j+count,row2,k, CYAN);
					}else if(num == 4){
						move_triangle_pixel(j,j+count,row2,k,GREEN);
					}else if(num == 5){
						move_triangle_pixel(j,j+count,row2,k,ORANGE);
					}
				}
			}
			//clearBlock(j+count-2,k,BACKGROUND);
			wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
			printf("%d",count);
			count ++;
		}
		//clearBlock(j+count-3,k-1,BACKGROUND);
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

    //Clear vertical
	if(col1 == col2){
		numOfClear = row2 - row1 + 1;
		while(row2 >= 0){
			if(row2 - numOfClear >= 0){
				diamonds[row2][col1] = diamonds[row2 - numOfClear][col1];
				diamondMove(row2 - numOfClear,row2, col1,diamonds[row2 - numOfClear][col1]);
				//diamondBlockMove(row1,row2, col1,col2,numOfClear);
				diamondSelection(row2, col1, diamonds[row2][col1]);
			}else{
				diamonds[row2][col1] = rand() % 6;
				diamondMove(0,row2,col1,diamonds[row2][col1]);
				//diamondBlockMove(row1,row2, col1,col2,numOfClear);
				diamondSelection(row2, col1, diamonds[row2][col1]);
			}
			row2 --;
		}
		//diamondBlockMove(r1,r2, c1,c2,numOfClear);
	}

	//Clear horizontal
	if(row1 == row2){
		numOfClear = col2 - col1 + 1;
		while(row1 >= 0){
			for(i = col1; i <= col2;i++){
				if(row1-1 >= 0){
					diamonds[row1][i] = diamonds[row1-1][i];
					diamondMove(row1-1,row1,i,diamonds[row1-1][i]);
					diamondSelection(row1, i, diamonds[row1][i]);
				}else{	
                    diamonds[row1][i] = rand() % 6;
                    diamondMove(0,row1,i,diamonds[row1][i]);
                    diamondSelection(row1, i, diamonds[row1][i]);
                }
			}
			row1 --;
		}
		//diamondBlockMove(r1,r2, c1,c2,numOfClear);
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
























