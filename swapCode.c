
swapValue(row1, row2, col1, col2);

bool win;
 //Check one position
 win = checkIfValid(row1, col1);
 //Check the other position
 if(win == false){
     win = checkIfValid(row2, col2);
 }
 
 if(win == false){
     //Swap Back
     swapValue(row2, row1, col2, col1);
     printf("Wrong move!\n");
 }else{
     printf("Excellent!\n");
 }