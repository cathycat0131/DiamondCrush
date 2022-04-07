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
    //Check the neighbouring col colour (Horizontal)
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
    //printf("7\n");
	if(col1 == col2){
		numOfClear = row2 - row1 + 1;
		for(i = row1; i <= row2; i++){
                if(i-numOfClear >= 0){
                    diamonds[i][col1] = diamonds[i-numOfClear][col1];
                    //diamonds[i-numOfClear][col1] = rand() % 6;
                    diamonds[i-numOfClear][col1] = 8;
                }else{
                    diamonds[i][col1] = 9;
                }
				
			}
	}
//printf("8\n");
	if(row1 == row2){
		numOfClear = col2 - col1 + 1;
		for(i = col1; i <= col2; i++){
            if(i-numOfClear >= 0){
				diamonds[row1][i] = diamonds[row1][i-numOfClear];
				//diamonds[row1][i-numOfClear] = rand() % 6;
                diamonds[i-numOfClear][col1] = 8;
			}else{
                    diamonds[row1][i] = 9;
                }
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
                return true;
            }
            win = false;
		}
	}
    return false;
}




























