
#include<ncurses.h>
#include<curses.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<limits.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>

typedef struct node {
	int ch;
	struct node *next;
	struct node *prev;
}node;

node *startdata;
node *enddata;						//global pointer to the start and end of the file
node *copy;
int totallen = 0, inlen = 0, totalscr = 0, currscr = 0, right = 0, totalscrflag = 0;



void traverse(int y, int x) {
	node *temp = startdata;
	int i = 0, tempx = 0;
	
	while(temp != NULL) {
		printw("%c", temp->ch);
		temp = temp->next;
		i++;
	}
	printw("%d, %d, %d, %c", totalscr, totallen, COLS * y + currscr * LINES * COLS + x, enddata->ch);
	return;
}



void getyandx() {
	int y, x;
	getyx(stdscr, y, x);
	printw("%d==%d", y, x);
	return;
}



void addend(int c) {								//adds to the end of the file or linked list
	node *new = (node*)malloc(sizeof(node));
	if(!new){
		printw("Memory not allocated");
		return;
	}
	
	new->ch = c;
	new->next = NULL;
	if(!enddata && !startdata) {
		
		startdata = new;

		enddata = new;
		new->prev = NULL;

		return;
	}

	new->prev = enddata;
	(enddata)->next = new;
	enddata = new;
	
	
	return;
}



void insert(int c) {								//adds to the current cursor position
	int y, x, i = 0, tempx, count = 0, isnextnewline = 0;
	getyx(stdscr, y, x);
	
	if(COLS * y + x + totalscr * LINES * COLS == totallen) {					//addend if the cursor is at the end

		addend(c);

			printw("%c", c);

		
		if(y != 0 && x != 0 && ((COLS * y + x + 1) + (COLS) * (LINES) * (currscr + 1) ) % ((LINES) * (COLS)) == 0) {	//last element on a screen
			
			mvprintw(1, 1, "%d", c);
			clear();

			move(0, 0);
			
			currscr++;
			//totalscr++;
		}
		else if(c == '\n' && y == LINES - 1) {

			clear();
			move(0, 0);
			currscr++;
			//totalscr++;

		}
		return;
	}
	
	node *temp = startdata;
	node *new = (node*)malloc(sizeof(node));
	if(!new){
		printf("Memory not allocated");
		return;
	}
	new->ch = c;
	if(COLS * y + totalscr * LINES * COLS + x == 0) {
		new->next = temp;
		new->prev = NULL;
		temp->prev = new;
		temp = new;
		startdata = temp;
		
		
		while(temp != NULL && ((currscr + 1) * COLS * LINES) > i) {		//traverse to the cursor and print
			if(temp->ch == '\n') {
				
				tempx = i % COLS;
				i = i + COLS - tempx;
				printw("%c", temp->ch);
				temp = temp->next;
				//printw("%d ", temp->ch);
				continue;
			}
				printw("%c", temp->ch);
				temp = temp->next;

				i++;
			}	
		
		
		if(c == '\n') { 
			move(1, 0);
			

			//right++;
			return;
		}
		move(0, 1);
		
		//right++;
		return;
	}
	
	while(COLS * y + x +  currscr * (LINES) * (COLS) > i + 1) {		//traverse to 1 element before the cursor
		if(temp->ch == '\n') {
			
			tempx = i % COLS;
			i = i + COLS - tempx;
			temp = temp->next;

			count++;
			continue;
		}
		
		temp = temp->next;

		i++;
	}
	if(x == 0) {
		temp = temp->prev;
	}

	temp->next->prev = new;
	new->next = temp->next;
	new->prev = temp;
	temp->next = new;
	temp = new;
	
	while(temp != NULL && ((currscr + 1) * COLS * LINES) > i) {		//traverse to the cursor and print
		if(temp->ch == '\n') {
			if((i - (currscr) * COLS * LINES) / COLS == y)				//to check if theres a new line at the end of the current cursor line
			isnextnewline = 1;
			tempx = i % COLS;
			i = i + COLS - tempx;
			printw("%c", temp->ch);
			temp = temp->next;

			continue;
		}
		printw("%c", temp->ch);
		temp = temp->next;

		i++;
	}	
		

	//return;
		
		
	if(c == '\n') {
		if(y == LINES - 1) {
			clear();
			move(0, 0);
			currscr++;
			while(temp != NULL && ((currscr + 1) * COLS * LINES) > i) {		//traverse to the cursor and print
				if(temp->ch == '\n') {
					
					tempx = i % COLS;
					i = i + COLS - tempx;
					printw("%c", temp->ch);
					temp = temp->next;

					continue;
				}
				printw("%c", temp->ch);
				temp = temp->next;

				i++;
			}
			move(0, 0);	
		}
		else
			move(y + 1, 0);
		//right++;
		if((totallen - totalscr * COLS * LINES) / COLS == y && c == '\n') {
					//printw("%d, %d, %c"	, (totallen - totalscr * COLS * LINES) / COLS, y, c);
				totallen = totallen - tempx - 1;
				return;
			}
		return;
	}	
	if(x == COLS - 1) {
		if(y == LINES - 1) {
			clear();
			move(0, 0);
			currscr++;
		}
		else{
			move(y + 1, 0);
			if(!isnextnewline && (totallen - totalscr * COLS * LINES) / COLS == y)	
				return;
			/*else if((totallen - totalscr * COLS * LINES) / COLS == y && c == '\n') {
				totallen = totallen - tempx;
				return;
			}*/
	}	
		right++;
		return;
	}
	move(y, x + 1);
	
	if(!isnextnewline && (totallen - totalscr * COLS * LINES) / COLS == y)	
		return;
	
	right++;
	return;
}



int listlength(node* starttemp) {
	node *temp = starttemp;
	int i = 0;
	if(temp == NULL)
		return 0;
	while(temp != NULL) {
		i++;
		temp = temp->next;
	}
	return i;
}



void savefile(int fd, char argv[]) {
	node* temp = startdata;
	char c;
	close(fd);
	fd = 0;
	fd = open(argv, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	
	while(temp != NULL) {
		c = (char)temp->ch; 

		write(fd, &c, 1);
		temp = temp->next;
	}
	return;
}	



void pastetext(void) {
	node * temp = copy;
	int y, x;
	while(temp != NULL) {
	
		right = 0;
		
		if(temp->ch == '\n') {
				getyx(stdscr, y, x);
				if(y * COLS + x + currscr * LINES * COLS != totallen)
					x = 0;
				insert('\n');
				
				
				while(x < COLS) { 
					
					x++;
					totallen++;
					refresh();
				}
				if(totallen >= LINES * COLS && totallen / (LINES * COLS) != totalscrflag ) {
					totalscr++;
					totalscrflag++;
				}
				temp = temp->next;
				continue;
			}
			totallen = totallen + 1 - right;
		insert(temp->ch);
		
		if(totallen >= LINES * COLS && totallen / (LINES * COLS) != totalscrflag ) {
			totalscr++;
			totalscrflag++;
		}
		
		temp = temp->next;
		//totallen++;
	}
	return;
}
	


node * copytext(int starty, int startx, int endy, int endx) {
	int  i = 0, tempx, tempy;
	node * temp = startdata;
	node * copiedstart = NULL;
	node * copiedend = NULL;
	node *new;
	if(starty * COLS + startx > endy * COLS + endx) {	//after swapping:  from start points selection begins and continues 
														//till one element before end
		tempx = startx;
		startx = endx;
		endx = tempx;
		tempy = starty;
		starty = endy;
		endy = tempy;
	}
			

	while(starty * COLS + startx + currscr * COLS * LINES > i) {

							if(temp->ch == '\n') {
							tempy = i / COLS;
							tempx = i % COLS;
							i = i + COLS - tempx;
							temp = temp->next;

							continue;
						}
							temp = temp->next;

							i++;
						}
	
	node *test = temp;
	while(endy * COLS + endx + currscr * COLS * LINES > i) {		//temp will point to point where selection began ie at start
		
		new = (node*)malloc(sizeof(node));
		if(!new){
			printf("Memory not allocated");
			exit(0);
		}	
		new->ch = test->ch;	
		new->next = NULL;	
		if(copiedend == NULL) {
			copiedstart = new;
			new->prev = NULL;
			copiedend = new;
		}		
		else {
			new->prev = copiedend;
			copiedend->next = new;
			copiedend = new;
		}
		new = NULL;
		
		if(test->ch == '\n') {				//test will point to one elment after the point where selection end ie before end
		tempy = i / COLS;
		tempx = i % COLS;
		i = i + COLS - tempx;
		test = test->next;

		continue;
	}
		test = test->next;

		i++;
	}
	return copiedstart;
}



void backspace() {
	
	int y, x, i = 0, j = 0, flag = 0, tempx, tempx1, newline = 0, point = 0, scrflag = 0, isnextnewline = 0;
	node *temp = startdata;
	node *t;
	getyx(stdscr, y, x);
	if(COLS * y + currscr * LINES + x == 0) {				//return if the cursor is at the beginning as there is nothin to backspace
		return;
	}
	
	
	while(COLS * y + currscr * LINES * COLS + x > i + 1) {		//traverse to 1 element before the cursor

		if(temp->next->ch == '\n') {			//at the end of the loop temp will point to 

			tempx = i % COLS;
			i = i + COLS - tempx;
			temp = temp->next;
			
			point++;
			continue;
		}
		
		temp = temp->next;
		
		i++;

	}
	
	
	if(point > 1)
		tempx = tempx - 1;
	
	if(x == 0 && i == COLS * y + currscr * LINES * COLS + x && temp->ch == '\n') {			//flag if the backspaced element is 
																	//newline	and at the end of the loop temp 
		newline = 1;													//will point to the element to be removed
	}
	
	
	if(COLS * y + currscr * LINES * COLS + x == 1) {					//if the backspace is used for the first element	
		t = temp;
		if(totallen == 1) {
			temp = NULL;
		}
		else {
			temp = temp->next;
			t->next->prev = NULL;
		}
		t->next = NULL;
		startdata = temp;
		enddata = temp;
		free(t);
		startdata = temp;
		
	}
	
	 else if(COLS * y + currscr * LINES * COLS + x == totallen) {
		
		
		if(point && !newline)
			temp = temp->next;
		t = temp;
		enddata = temp->prev;
		temp = NULL;
		t->prev->next = NULL;
		t->prev = NULL;
		free(t);
		/*
		if(newline) {
			if(COLS * y + x == 0 && currscr) {
				//printw("*");
				//return;
				move(LINES - 1, tempx + 1);
				scrflag = 1;
				currscr--;
			} 
			else
				move(y - 1, tempx + 1);
		}
		else {
			if(COLS * y + x == 0 && currscr) {
				move(LINES - 1, COLS - 1);
				scrflag = 1;
				currscr--;
			}
			else	
				move(y, x - 1);
			clrtobot();
			move(y, x);
		}*/
	}
	
	 else if(newline) {
		t = temp;
		temp = temp->next;
		t->next = NULL;
		t->prev->next = temp;
		temp->prev = t->prev;
		t->next = NULL;
		t->prev = NULL;
		free(t);
	}
	else {
		if(point)
			temp = temp->next;
		t = temp;
		temp = temp->next;
		t->next->prev = t->prev;
		t->prev->next = t->next;
		t->next = NULL;
		t->prev = NULL;
		free(t);
	}
	//printw("%d", tempx);

	if(newline) {
		if(COLS * y + x == 0 && currscr) {
			move(LINES - 1, tempx + 1);
			scrflag = 1;
			currscr--;
		} 
		else
			move(y - 1, tempx + 1);
	}
	else if(x == 0) {							//if the cursor was currently on the x=0 ie first element of a line
		if(COLS * y + x == 0 && currscr) {
				move(LINES - 1, COLS - 1);
				scrflag = 1;
				currscr--;
			}
		else
			move(y - 1, COLS - 1);		
		flag = 1;
	}
	else
		move(y, x - 1);
	
	//printw("%d, %d, %d", y, x, scrflag);
	if(scrflag && y == 0 && x == 0) {
		temp = startdata;
		i = 0;
					while((currscr) * COLS * LINES > i + 1) {		//traverse to the start of the page to print it till the end
							if(temp->ch == '\n') {

							tempx1 = i % COLS;
							i = i + COLS - tempx1;
							temp = temp->next;
							//printw("%d ", temp->ch);
							continue;
						}
							temp = temp->next;

							i++;
						}

		move(0, 0);
		//printw("%d, %c", tempx1, temp->ch)	;
		//return;
		while(temp != NULL && (currscr + 1) * COLS * LINES > i + 1) {		//traverse to the cursor and print
		if(temp->ch == '\n') {
		
		tempx1 = i % COLS;
		i = i + COLS - tempx1;
		printw("%c", temp->ch);
		temp = temp->next;

		continue;
		}
		printw("%c", temp->ch);
		temp = temp->next;
		i++;
	}
	}	
	
if(!scrflag) {
	while(temp != NULL && (currscr + 1) * COLS * LINES > i + 1) {		//traverse to the cursor and print
		if(temp->ch == '\n') {
		
		if((i - (currscr) * COLS * LINES) / COLS == y)				//to check if theres a new line at the end of the current cursor line
			isnextnewline = 1;
			
		tempx1 = i % COLS;
		i = i + COLS - tempx1;
		printw("%c", temp->ch);
		temp = temp->next;

		continue;
		}
		printw("%c", temp->ch);
		temp = temp->next;
		i++;
	}
}
	
	clrtobot();
	
	if(newline) {
		/*if(scrflag) {
			printw("-%d, %d, %d-", tempx, tempx1, totallen);
		}*/
		// {
			if(totallen / COLS == totalscr * LINES + y) {		
				totallen = totallen - (COLS - tempx - 1);
			}
			else
				totallen = totallen - COLS;
			move(y - 1, tempx + 1);
		//}
					//printw("-%d, %d, %d, %d-", tempx, tempx1, totallen, currscr);
		if(totallen >= LINES * COLS - COLS - tempx && (totallen) / (LINES * COLS) != totalscrflag ) {
			//printw("-%d, %d, %d, %d, %d-", tempx, tempx1, totallen, currscr, totalscr);
			if(scrflag)
				move(LINES - 1, tempx + 1);
			totalscr--;
			totalscrflag--;
			//return;
		}
		else if(scrflag) { 
			//totallen = totallen - tempx - 1;
		//printw("-%d, %d, %d, %d-", tempx, tempx1, totallen, currscr);
			move(LINES - 1, tempx + 1);
		}
		return;
	}
	else if(flag) {
		if(COLS * y + x == 0 && currscr) 
			move(LINES - 1, COLS - 1);
		else{
			move(y - 1, COLS - 1);
			totallen--;				//reduced totallen by one inside the function itself due to cursor at beginning condition 
		} 
		
	}
	else {
		move(y, x - 1);
		if(!isnextnewline && (totallen - totalscr * COLS * LINES) / COLS == y)	
			totallen--;				//reduced totallen by one inside the function itself due to cursor at beginning condition 
	}	

	if(totallen >= LINES * COLS && (totallen) / (LINES * COLS) != totalscrflag ) {

			totalscr--;
			totalscrflag--;
		}
	return;
}
	
	
	

void delete(int starty, int startx, int endy, int endx, int startscr, int endscr) {

	int tempx, tempy, isnextnewline = 0;
	int i = 0, zerocase = 0;
	if(startscr == endscr) {
		if(starty * COLS + startx > endy * COLS + endx) {	//after swapping:  from start points selection begins and continues 
														//till one element before end
			tempx = startx;
			startx = endx;
			endx = tempx;
			tempy = starty;
			starty = endy;
			endy = tempy;
		}
	}
	else if(startscr > endscr) {
		tempx = startx;
		startx = endx;
		endx = tempx;
		tempy = starty;
		starty = endy;
		endy = tempy;
		
		tempy = startscr;
		startscr = endscr;
		endscr = tempy;
		//printw("==%d, %d, %d, %d, %d, %d-", startscr, endscr, starty, startx, endy, endx);
	//return;
	}
	//startx++;
	//endx++;
	//printw("%d, %d, %d, %d, %d-", endy *COLS + endx - (starty * COLS + startx), starty, startx, endy, endx);
	//return;
	move(endy, endx);
	while(endscr * COLS * LINES + endy * COLS + endx - (starty * COLS + startx + startscr * COLS * LINES) !=0){
	//	printw("%c");
		backspace();
		getyx(stdscr, endy, endx);
		if(endy == 0 & endx == 0)
			endscr--;
	}
	//printw("%d", endy *COLS + endx - starty * COLS + startx);
	//move(starty, startx);
	return;
	
	
	
	node *temp = startdata;

	if(startx == 0) {

		zerocase = 1;
	}		
				
	while(starty * COLS + startx + currscr * COLS * LINES > i) {

							if(temp->ch == '\n') {
							tempy = i / COLS;
							tempx = i % COLS;
							i = i + COLS - tempx;
							temp = temp->next;
							//printw("%d ", temp->ch);
							continue;
						}
							temp = temp->next;
							//printw("%d ", temp->ch);
							i++;
						}						
	//printw("-%c*", temp->ch);
	node *test = temp;
	node *t;
	
	while(endy * COLS + endx + currscr * COLS * LINES > i) {
							if(test->ch == '\n') {
							tempy = i / COLS;
							tempx = i % COLS;
							i = i + COLS - tempx;
							test = test->next;
							//printw("%d ", temp->ch);
							continue;
						}
							test = test->next;

							i++;
						}
	if(zerocase) {
		t = temp;
		temp = test;
		test->prev->next = NULL;
		test->prev = NULL;
		startdata = temp;
	}
	else {
		
		t = temp;
		
		temp->prev->next = test;
		temp->prev = NULL;


		test->prev->next = NULL;
		test->prev = temp->prev;
		
		temp = test;	
	}
	test = NULL;
	free(t);
	
	totallen = totallen - (endy * COLS + endx - (starty * COLS + startx));
	
	move(starty, startx);
	
	while(temp != NULL && ((currscr + 1) * (COLS * LINES)) != 0) {
		printw("%c", temp->ch);
		temp = temp->next;
	}
	
	clrtobot();
	
		move(starty, startx);
	
	return;
}



void selecttext(int starty, int startx) {
	int i = 0, j = 0, tempy = starty, tempx = startx, y, x, newline = 0, xflag, prevud = 0, multiple = 0, atstart = 0;
	int startscr = currscr;
	node *temp = startdata, *temp1;
	getyx(stdscr, y, x);
	
	
	int c;
	attron(A_REVERSE);
	do {											//here initial temp->ch points to one element before cursor and tempx and tempy now point to 																temp->ch
		getyx(stdscr, y, x);
		
		i = 0;
		temp = startdata;
		while((currscr) * COLS * LINES + y * COLS + x > i + 1) {		//here initail temp->ch points to one element before cursor and tempx and 																			tempy now point to temp->ch
							
							if(temp->ch == '\n') {
							tempy = i / COLS;
							tempx = i % COLS;
							i = i + COLS - tempx;
							temp = temp->next;

							continue;
						}
							temp = temp->next;

							i++;
						}
	if(!atstart && temp->prev->ch == '\n') {
		temp = temp->prev;
		
	}
	
		c = wgetch(stdscr);
		getyx(stdscr, y, x);
	
		
		switch(c) {	
			
			case KEY_UP:		//here temp->ch points to one element before cursor and tempx and tempy also point to 												temp->ch and selection continues till the position of cursor after using arrow keys 
			
				if(prevud == 2)
					break;
				
				prevud = 1;
				if(!currscr && x == 0 && y == 0) {
						break;
					}
				j = 0;
				getyx(stdscr, y, xflag);
				
				if(temp->ch == '\n' || xflag == 0) 
					multiple = COLS;
				else 
					multiple = 0;
					
				while(j <  multiple + xflag - 1) {
					getyx(stdscr, y, x);
					if(!currscr && x == 0 && y == 0) {
						break;
					}
					
					if(!currscr && x == 1 && y == 0) {
						
						printw("%c", temp->ch);
						getyx(stdscr, y, x);
						move(0, 0);
						break;
					}
						
						
						
				if(x == 0 && temp->ch == '\n') {
					
					
						if(currscr && y == 0 && x == 0) {		//at the start of another page and prev element is \n
								temp1 = startdata;
					while((currscr) * COLS * LINES > i + 1) {		//traverse to the cursor
							if(temp1->ch == '\n') {
							tempy = i / COLS;
							tempx = i % COLS;
							i = i + COLS - tempx;
							temp1 = temp1->next;

							continue;
						}
							temp1 = temp1->next;

							i++;
						}
		
							move(0, 0);
			
	
					while(temp1 != NULL && (currscr + 1) * COLS * LINES > i + 1) {		//traverse to the cursor and print
						attroff(A_REVERSE);
						if(temp1->ch == '\n') {
						tempy = i / COLS;
						tempx = i % COLS;
						i = i + COLS - tempx;
						printw("%c", temp1->ch);
						temp1 = temp1->next;

						continue;
					}
						printw("%c", temp1->ch);
						temp1 = temp1->next;

						i++;
					}
					currscr--;
					attron(A_REVERSE);
					move(LINES - 1, tempx);
					printw("%c", temp->ch);			
					move(LINES - 1, tempx - 1);		
					j = j + COLS - tempx;		
					newline = 1;
					getyx(stdscr, y, x);
					temp = temp->prev;
					
					continue;
					}				
							
						
						move(tempy - currscr * LINES, tempx);
						printw("%c", temp->ch);

						move(tempy - currscr * LINES, --tempx);
						temp = temp->prev;
						
						j = j + COLS - tempx - 1;
						newline = 1;
						continue;
					}
					else if(!newline && x == 0 && temp->ch != '\n') {
					
					
					
						if(currscr && y == 0 && x == 0) {
								temp1 = startdata;
					while((currscr) * COLS * LINES > i + 1) {		//traverse to the cursor
							if(temp1->ch == '\n') {
							tempy = i / COLS;
							tempx = i % COLS;
							i = i + COLS - tempx;
							temp1 = temp1->next;

							continue;
						}
							temp1 = temp1->next;

							i++;
						}
		
							move(0, 0);
			
	
					while(temp1 != NULL && (currscr + 1) * COLS * LINES > i + 1) {		//traverse to the cursor and print
						attroff(A_REVERSE);
						if(temp1->ch == '\n') {
						tempy = i / COLS;
						tempx = i % COLS;
						i = i + COLS - tempx;
						printw("%c", temp1->ch);
						temp1 = temp1->next;

						continue;
					}
						printw("%c", temp1->ch);
						temp1 = temp1->next;

						i++;
					}
					
					currscr--;
					attron(A_REVERSE);
					move(LINES - 1, COLS - 1);
					printw("%c", temp->ch);			
					move(LINES - 1, COLS - 2);		
					j++;		
					newline = 1;
					getyx(stdscr, y, x);
					temp = temp->prev;
					
					continue;
					}
				
						move(--y, COLS - 1);
						printw("%c", temp->ch);

						
						move(y, COLS - 2);
						
						temp = temp->prev;

						j++;
						newline = 1;
						continue;
					}
					getyx(stdscr, y, x);

					if(!newline) {
						move(y, x - 1);
					}
					
					newline = 1;
					printw("%c", temp->ch);
					getyx(stdscr, y, x);
					move(y, x - 2);
					temp = temp->prev;
					
					j++;
				}
				
				getyx(stdscr, y, x);
				if(atstart)
					break;
				
				if(!currscr && x == 0 && y == 0)
					atstart++;	
				printw("%c", temp->ch);
				getyx(stdscr, y, x);
				move(y, --x);
				
				break;
				
				
			case KEY_DOWN:			//temp should point to the cursor. will select till end of that line and the cursor will be moved to next line
			
				if(prevud == 1)
					break;
					
				temp = temp->next;			//temp should point to the cursor
				
				prevud = 2;
				j = 0;
				getyx(stdscr, y, x);
				//if(totallen <= COLS * (y + 1) + x + totalscr * COLS * LINES) 
					//break;
				
				//if(currscr * LINES + y == totalscr * LINES - 1)
				//	break;
				
				while(j < COLS - x) {		
					if(temp->ch == '\n') {
						if(y == LINES - 1) {
							
							temp = temp->next;
							clear();
							move(0, 0);
							i = 0;
							temp1 = temp;
							while(temp1 != NULL &&  COLS * LINES > i + 1) {		//traverse to the cursor and print
								attroff(A_REVERSE);
								if(temp1->ch == '\n') {
								tempy = i / COLS;
								tempx = i % COLS;
								i = i + COLS - tempx;
								printw("%c", temp1->ch);
								temp1 = temp1->next;

								continue;
								}								
								printw("%c", temp1->ch);
								temp1 = temp1->next;

								i++;
							}
							currscr++;
							move(0, 0);
							attron(A_REVERSE);
							break;
						}
					
					
					 
						printw("%c", temp->ch);
						temp = temp->next;
						break;
					}	
					

					printw("%c", temp->ch);
					temp = temp->next;
					j++;
				}
				break;
			
			case KEY_LEFT:					//here temp->ch points to one element before cursor and tempx and tempy also point to 												temp->ch
				
				getyx(stdscr, y, x);
				if(x == 0 && y == 0) {
						break;
				}
				if(prevud == 2)
					break;
				prevud = 1;
				tempy = tempy - currscr * LINES;
				if(!currscr && x == 1 && y == 0) {
						
						move(0, 0);
						printw("%c", temp->ch);
						getyx(stdscr, y, x);
						move(0, 0);
					
					}
				if(x == 1){
					move(y, x - 1);
					printw("%c", temp->next->ch);
					move(y, x - 1);
					break;
				}
				if(x == 0) {
					if(temp->ch == '\n') {
						move(tempy, tempx);
						printw("\n", temp->ch);
						move(tempy, tempx);
						temp = temp->prev;
						break;
					}

					move(y - 1, COLS - 1);
					printw("%c", temp->ch);
					move(y - 1, COLS - 1);
					temp = temp->prev;
					break;
				}
				
					
				move(y, x - 1);
				printw("%c", temp->ch);
				move(y, x - 1);

				
				getyx(stdscr, y, x);
				if(atstart)
					break;
				
				if(!currscr && x == 1 && y == 0)
					atstart++;	
				break;
				
				
			case KEY_RIGHT:					//temp should point to the cursor. will select till next line's starting 
			
				if(prevud == 1)
					break;
				prevud = 2;
				
				temp = temp->next;			//temp should point to the cursor
				
				getyx(stdscr, y, x);
				if(totallen <= currscr * COLS * LINES + COLS * y + x + 1)
					break;
					
				if(x == 1){

					printw("%c", temp->next->ch);

					break;
				}
					
				if((x == COLS - 1 && y == LINES - 1) || (y == LINES - 1 && temp->ch == '\n')) {
						
					break;
					}	
												
				
				printw("%c", temp->ch);

				temp = temp->next;
				break;	
				
			case KEY_F(3):
				getyx(stdscr, y, x);
				attroff(A_REVERSE);

				delete(starty, startx, y, x, startscr, currscr);
				if((totallen) / (LINES * COLS) != totalscrflag ) {
					totalscr--;
					totalscrflag--;
				}
				for(i = 0; i < LINES; i++) 
					mvchgat(i, 0, -1, A_NORMAL, 0, NULL);
					move(y, x);
				return;
				
			case KEY_F(6):
				getyx(stdscr, y, x);
				attroff(A_REVERSE);
				copy = copytext(starty, startx, y, x);
				for(i = 0; i < LINES; i++) 
					mvchgat(i, 0, -1, A_NORMAL, 0, NULL);
					move(y, x);
				return;
				
			default:
				refresh();
				break;
		}
		
	}while(c != KEY_F(4));
	attroff(A_REVERSE);

	for(i = 0; i < LINES; i++) 
		mvchgat(i, 0, -1, A_NORMAL, 0, NULL);
		move(y, x);
	return;
}




int checknewlineleft(int y, int x) {
	node *temp = startdata;
	int len = 0, tempx, end = 0;

	if(y * COLS + x == 0)
		return -1;
	while(y * COLS + x > len) {		//traverse 1 element prior to the cursor
		if(temp->ch == '\n') {
			
			tempx = len % COLS;
			len = len + COLS - tempx;
			if(len == totallen) {
				end = 1;
				break;
			}
			temp = temp->next;
			continue;
		}
		
		temp = temp->next;
		
		len++;
	}
	

	if(end)
		return tempx;
	if(temp->prev->ch == '\n') {
		
		return tempx;
	}
	return -1;
}




int checknewlineright(int y, int x) {
	node *temp = startdata;
	int len = 0, tempx;

	if(y * COLS + x == totallen)
		return 0;
	while(y * COLS + x > len) {		//traverse to the cursor
		if(temp->ch == '\n') {
			tempx = len % COLS;
			len = len + COLS - tempx;
			temp = temp->next;

			continue;
		}
		
		temp = temp->next;

		len++;
	}

	if(temp->ch == '\n') {
		
		return tempx;
	}
	return -1;
}




int main(int argc, char *argv[]) {
	
	node * temp = startdata;
	int fd;
	int c;
	char ch;
	int flag = 0;
	int newline;
	int i = 0, y, x, tempx, scrflag = 0, scrx;			
	if(argc < 2){
		perror("Invalid Arguments");
		return EINVAL;
	}
	
	fd = open(argv[1],  O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		printf("Unable to open the file");
		return 1;
	}

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	
	while(1) {	
		flag = 0;
		while(read(fd, &ch, 1)) {

			c = ch;
			
			if(c == 10) {
				getyx(stdscr, y, x);
				insert('\n');

				while(x < COLS) { 
					
					x++;
					totallen++;
					refresh();
				}
				if(totallen >= LINES * COLS && totallen / (LINES * COLS) != totalscrflag ) {
					totalscr++;
					totalscrflag++;
				}
				continue;
			}
			
			insert(c);
			//addend(c);
			//printw("%c", c);
			refresh();
			totallen++;
			if(totallen >= LINES * COLS && totallen / (LINES * COLS) != totalscrflag ) {
					totalscr++;
					totalscrflag++;
				}
			inlen++;
		}

		
		c = wgetch(stdscr);
		right = 0;
		
		if(c == '\n') {
				getyx(stdscr, y, x);
				if(y * COLS + x + currscr * LINES * COLS != totallen)
					x = 0;
				insert('\n');
				
				
				while(x < COLS) { 
					
					x++;
					totallen++;
					refresh();
				}
				if(totallen >= LINES * COLS && totallen / (LINES * COLS) != totalscrflag ) {
					totalscr++;
					totalscrflag++;
				}
				
				continue;
			}
			
		if(c == '\t') {

				x = 0;
				while(x < 4) { 

					insert(' ');
					
					x++;
					totallen++;
					refresh();
					if(totallen >= LINES * COLS && totallen / (LINES * COLS) != totalscrflag ) {
					totalscr++;
					totalscrflag++;
					}
				}
				
				continue;
			}
		
		getyx(stdscr, y, x);

		
		switch(c) {
			
			case KEY_UP :
				flag = 1;
				i = 0;	
				getyx(stdscr, y, x);
				if(y == 0) {
					if(currscr) {
						temp = startdata;
						
						while((currscr - 1) * COLS * LINES > i + 1) {		//traverse to the cursor
							if(temp->ch == '\n') {
							tempx = i % COLS;
							i = i + COLS - tempx;
					     		temp = temp->next;

							continue;
						}
							temp = temp->next;

							i++;
						}
						
						while((currscr) * COLS * LINES > i + 1) {		//traverse to the cursor
							if(temp->ch == '\n') {
							tempx = i % COLS;
							i = i + COLS - tempx;
							printw("%c", temp->ch);
							temp = temp->next;

							continue;
						}
							printw("%c", temp->ch);
							temp = temp->next;

							i++;
						}
						
						move(LINES - 1, 0);
						currscr--;
					}
					break;
				}
				
					move(y - 1, 0);
					break;
				
				
			case KEY_DOWN :
				flag = 1;
				i = 0;
				scrflag = 0;
				getyx(stdscr, y, x);
				
				if(y == LINES - 1) {
					if(currscr != totalscr) {
						temp = startdata;
						
						
						while((currscr + 1) * COLS * LINES > i) {		//traverse to the end of screen
							if(temp->ch == '\n') {
							tempx = i % COLS;
							i = i + COLS - tempx;
							temp = temp->next;

							continue;
						}
							temp = temp->next;

							i++;
						}
						
						clear();
						move(0, 0);
						while(temp != NULL && ((currscr + 2) * COLS * LINES + COLS) > i) {		//traverse to the end of screen or till end of temp
							if(temp->ch == '\n') {

							tempx = i % COLS;
							i = i + COLS - tempx;
							printw("%c", temp->ch);
							temp = temp->next;

							continue;
						}
							printw("%c", temp->ch);
							temp = temp->next;

							i++;
						}
						
						move(0, 0);
						currscr++;
					}
					break;
				}
				
				if(totallen < (currscr) * COLS * LINES + COLS * (y + 1) + x)
					break;
				if(currscr == totalscr && y == LINES - 1)
					break;
				move(y + 1, 0);
				break;	
				
			case KEY_LEFT :
				flag = 1;
				i = 0;
				scrflag = 0;
				getyx(stdscr, y, x);
				temp = startdata;
				if(currscr && y == 0 && x == 0) {
					while((currscr - 1) * COLS * LINES > i + 1) {		//traverse to the cursor
							if(temp->ch == '\n') {

							tempx = i % COLS;
							i = i + COLS - tempx;
							temp = temp->next;

							continue;
						}
							temp = temp->next;

							i++;
						}
					
					while((currscr) * COLS * LINES > i + 1) {		//traverse to the cursor and print
							if(temp->ch == '\n') {

							tempx = i % COLS;
							i = i + COLS - tempx;
							printw("%c", temp->ch);
							temp = temp->next;

							continue;
						}
							printw("%c", temp->ch);
							temp = temp->next;

							i++;
						}
					y = (LINES - 1) * currscr;	
					scrx = x;
					x = tempx + 1;
					scrflag = 1;
					currscr--;
				}


				if(!scrflag) 
					y = (LINES) * currscr + y;
					
				if(scrflag && scrx == 0 && (newline = checknewlineleft(y, x)) != -1) {

					getyx(stdscr, y, x);
					move(y, newline);
					break;
				}
				
				if(x == 0 && (newline = checknewlineleft(y, x)) != -1) {

					getyx(stdscr, y, x);
					move(y - 1, newline);
					break;
				}
				
				getyx(stdscr, y, x);
				if(x == 0) {
					getyx(stdscr, y, x);
					move(y - 1, COLS - 1);
					break;
				}
				move(y, x - 1);
				break;
				
			case KEY_RIGHT :
				flag = 1;
				i = 0;
				temp = startdata;
				getyx(stdscr, y, x);
				
				if(totallen <= currscr * COLS * LINES + COLS * y + x)
					break;
				
				while((currscr) * COLS * LINES + y * COLS + x > i) {		//traverse to the cursor
					if(temp->ch == '\n') {
					tempx = i % COLS;
					i = i + COLS - tempx;
					temp = temp->next;

					continue;
					}
					temp = temp->next;

					i++;
				}
				
				if( y == LINES - 1 && temp->ch == '\n') {

					temp = temp->next;
					clear();
					move(0, 0);
					while(temp != NULL && ((currscr + 2) * COLS * LINES + COLS) > i) {		//traverse to the end of screen or till end of temp
						if(temp->ch == '\n') {

						tempx = i % COLS;
						i = i + COLS - tempx;
						printw("%c", temp->ch);
						temp = temp->next;

						continue;
					}
						printw("%c", temp->ch);
						temp = temp->next;

							i++;
					}
						
					move(0, 0);
					currscr++;
					break;
				}
				
				if((newline = checknewlineright(y + currscr * LINES, x)) != -1) {

					getyx(stdscr, y, x);
					move(y + 1, 0);
					break;
				}	
				
				if(x == COLS - 1)  {
					getyx(stdscr, y, x);
					move(y +1, 0);
					break;
				}
				move(y, x + 1);
				break;
		}
		
		if(flag) 				//for arrow keys
			continue;
	
		if(c == 'q'){
			getyx(stdscr, y, x);

			traverse(y, x);
			break;
		}
		
		
		if(c == KEY_F(1)) {
			clear();

			printw("F1 key pressed. Saving file\nPress any key to exit:");
			savefile(fd, argv[1]);
			
			
			break;
		}
		
		if(c == KEY_F(2)) {
			clear();

			printw("\nF2 key pressed. Press any key to quit without saving");

			break;
		}
		
		if(c == KEY_F(4)) {
			getyx(stdscr, y, x);
			selecttext(y, x);
			continue;
		}
		
		if(c == KEY_BACKSPACE) {
			backspace();
			continue;
		}
		
		if(c == KEY_F(5)) {
			pastetext();
			continue;
		}
			
		insert(c);
		
		totallen = totallen + 1 - right;
		
		
		if(totallen >= LINES * COLS && totallen / (LINES * COLS) != totalscrflag ) {
			totalscr++;
			totalscrflag++;
		}

	}
		
	c = getch();
	endwin();
	close(fd);
	return 0;
}

	
	
	
	

	
	
	
	

