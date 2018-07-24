

#include <stdio.h>
#include <string.h>

#include "defs.h"

int ThreeFoldRep(const S_BOARD *pos) {//function for the Three fold repetition of the moves.
	int i = 0, r = 0;
	for(i = 0; i < pos->hisPly; ++i) {
		if(pos->history[i].posKey == pos->posKey) {//function to put the value of poskey at history array.
			r++;
		}
	}

	return r;
}

int DrawMaterial(const S_BOARD *pos) {//function to draw the material on the board.
	if(pos->pceNum[wP] || pos->pceNum[bP]) return FALSE;
	if(pos->pceNum[wQ] || pos->pceNum[bQ] || pos->pceNum[wR] || pos->pceNum[bR]) return FALSE;
	if(pos->pceNum[wB] > 1 || pos->pceNum[bB] > 1) return FALSE;
	if(pos->pceNum[wN] > 1 || pos->pceNum[bN] > 1) return FALSE;
	if(pos->pceNum[wN] && pos->pceNum[wB]) return FALSE;
	if(pos->pceNum[bN] && pos->pceNum[bB]) return FALSE;

	return TRUE;
}

int checkresult(S_BOARD *pos) {//Function to check the result of the board or to handle certain special cases.
	S_MOVELIST list[1];
	int MoveNum = 0;
	int found = 0;
	int InCheck;

	if(pos->fiftyMove > 100) {//Fifty move non-mate draw.//Three fold repetition draw.
		printf("1/2-1/2 (fifty move rule (claimed by Vice))\n"); return TRUE;
	}

	if(ThreeFoldRep(pos)) {//Three fold repetition draw.
		printf("1/2-1/2 (3-fold repetition (claimed by Vice))\n"); return TRUE;
	}

	if(DrawMaterial(pos) == TRUE) {//Material position draw.
		printf("1/2-1/2 (insufficient material (claimed by Vice))\n"); return TRUE;
	}

	GenerateAllMoves(pos, list);//Generate all the moves.
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		if(!MakeMove(pos, list->moves[MoveNum].move)) {
			continue;
		}
		found++;//To finding counter
		TakeMove(pos);
		break;
	}

	if(found != 0) return FALSE;

	InCheck = SqAttacked(pos->KingSq[pos->side], pos->side ^ 1, pos);

	if(InCheck == TRUE) {
		if(pos->side == WHITE) {
			printf("0-1 (black mates (claimed by Axiom))\n"); return TRUE;
		} else {
			printf("0-1 (white mates (claimed by Axiom))\n"); return TRUE;
		}
	} else {
		printf("1/2-1/2 (stalemate (claimed by Axiom))\n"); return TRUE;
	}

	return FALSE;
}

void PrintOptions() {
	printf("feature ping=1 setboard=1 colors=0 usermove=1 memory=1\n");
	printf("feature done=1\n");
}

void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info) {//xboard function having position pointer and information pointer
	int depth = -1, movestogo[2] = {30,30 }, movetime = -1;//time of the move the depth of the move 
	int time = -1, inc = 0;//time						
	int engineSide = BOTH;//side of the engine 				
	int timeLeft;//time left
	int sec;//seconds
	int mps;//moves per seconds 
	int move = NOMOVE;//integer of move
	char inBuf[80], command[80];//buffer array and command array
	int MB;

	info->GAME_MODE = XBOARDMODE;//Game-mode pointer which points to the xboard pointer
	info->POST_THINKING = TRUE;//post-thinking = true
	setbuf(stdin, NULL);//set the standard input as NULL
	setbuf(stdout, NULL);//set the standard output as NULL
	PrintOptions(); // HACK

	engineSide = BLACK;//engine side variable is initialized to black
	ParseFen(START_FEN, pos);//Function to parse board pointer position
	depth = -1;//depth to be set at -1
	time = -1;//time is out of bound currently

	while(TRUE) {//Infinite loop

		fflush(stdout);//Flush the standard output

		if(pos->side == engineSide && checkresult(pos) == FALSE) {//Currently the engine side is the Black
			info->starttime = GetTimeMs();//Get the time in starttime variable by calling GetTimeMs()
			info->depth = depth;//set depth pointed by the pointer as depth

			if(time != -1) {//If time is not equal to null
				info->timeset = TRUE;//information in the timeset is to be set as true
				time /= movestogo[pos->side];//time is equal to time = time / movestopos
				time -= 50;//time = time - 50
				info->stoptime = info->starttime + time + inc;//stoptime is always to be incremented by in counter and time
			}//required for each move in particular 

			if(depth == -1 || depth > MAXDEPTH) { //If depth is out of bound then set the depth equals to maxdepth
				info->depth = MAXDEPTH;
			}

			printf("time:%d start:%d stop:%d depth:%d timeset:%d movestogo:%d mps:%d\n",//Print various variables.
				time, info->starttime, info->stoptime, info->depth, info->timeset, movestogo[pos->side], mps);
			SearchPosition(pos, info);

			if(mps != 0) {
				movestogo[pos->side ^ 1]--;
				if(movestogo[pos->side ^ 1] < 1) {
					movestogo[pos->side ^ 1] = mps;
				}
			}
		}

		fflush(stdout);

		memset(&inBuf[0], 0, sizeof(inBuf));
		fflush(stdout);
		if (!fgets(inBuf, 80, stdin))
		continue;

		sscanf(inBuf, "%s", command);
	
		printf("command seen:%s\n",inBuf);

		if(!strcmp(command, "quit")) {
			info->quit = TRUE;
			break;
		}
	
		if(!strcmp(command, "force")) {
			engineSide = BOTH;
			continue;
		}
	
		if(!strcmp(command, "protover")){
			PrintOptions();
			continue;
		}
	
		if(!strcmp(command, "sd")) {
			sscanf(inBuf, "sd %d", &depth);
			printf("DEBUG depth:%d\n",depth);
			continue;
		}
	
		if(!strcmp(command, "st")) {
			sscanf(inBuf, "st %d", &movetime);
			printf("DEBUG movetime:%d\n",movetime);
			continue;
		}
	
		if(!strcmp(command, "time")) {
			sscanf(inBuf, "time %d", &time);
			time *= 10;
			printf("DEBUG time:%d\n",time);
			continue;
		}

		if(!strcmp(command, "polykey")) {
			PrintBoard(pos);
			GetBookMove(pos);
			continue;
		}

		if(!strcmp(command, "memory")) {		
			sscanf(inBuf, "memory %d", &MB);	
			if(MB < 4) MB = 4;
			if(MB > MAX_HASH) MB = MAX_HASH;
			printf("Set Hash to %d MB\n",MB);
			InitHashTable(pos->HashTable, MB);
			continue;
		}

		if(!strcmp(command, "level")) {
			sec = 0;
			movetime = -1;
			if( sscanf(inBuf, "level %d %d %d", &mps, &timeLeft, &inc) != 3) {
				sscanf(inBuf, "level %d %d:%d %d", &mps, &timeLeft, &sec, &inc);
				printf("DEBUG level with :\n");
			}	else {
				printf("DEBUG level without :\n");
			}		
			timeLeft *= 60000;
			timeLeft += sec * 1000;
			movestogo[0] = movestogo[1] = 30;
			if(mps != 0) {
				movestogo[0] = movestogo[1] = mps;
			}
			time = -1;
			printf("DEBUG level timeLeft:%d movesToGo:%d inc:%d mps%d\n",timeLeft,movestogo[0],inc,mps);
			continue;
		}

		if(!strcmp(command, "ping")) {
			printf("pong%s\n", inBuf+4);
			continue;
		}
	
		if(!strcmp(command, "new")) {
			ClearHashTable(pos->HashTable);
			engineSide = BLACK;
			ParseFen(START_FEN, pos);
			depth = -1;
			time = -1;
			continue;
		}
	
		if(!strcmp(command, "setboard")){
			engineSide = BOTH;
			ParseFen(inBuf+9, pos);
			continue;
		} 	
	
		if(!strcmp(command, "go")) {
			engineSide = pos->side;
			continue;
		}	
	
		if(!strcmp(command, "usermove")){
			movestogo[pos->side]--;
			move = ParseMove(inBuf+9, pos);
			if(move == NOMOVE) continue;
			MakeMove(pos, move);
			pos->ply=0;
		}
	}
}

void Console_Loop(S_BOARD *pos, S_SEARCHINFO *info) {//Function for the console mode.
	int depth = MAXDEPTH, movetime = 3000;		
	int engineSide = BOTH;
	int move = NOMOVE;	
	char inBuf[80], command[80];

	printf("Welcome to Axiom In Console Mode!\n");
	printf("Type help for commands\n\n");

	info->GAME_MODE = CONSOLEMODE;
	info->POST_THINKING = TRUE;
	engineSide = BLACK;
	ParseFen(START_FEN, pos);

	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

	while(TRUE) {

		fflush(stdout);

		if(pos->side == engineSide && checkresult(pos) == FALSE) {
			info->starttime = GetTimeMs();
			info->depth = depth;
		
			if(movetime != 0) {
				info->timeset = TRUE;
				info->stoptime = info->starttime + movetime;
			}
		
			SearchPosition(pos, info);
		}
	
		printf("\nAxiom > ");

		fflush(stdout);

		memset(&inBuf[0], 0, sizeof(inBuf));
		fflush(stdout);
		if (!fgets(inBuf, 80, stdin))
		continue;

		sscanf(inBuf, "%s", command);
	
		if(!strcmp(command, "help")) {
			printf("Commands:\n");
			printf("quit - quit game\n");
			printf("force - computer will not think\n");
			printf("print - show board\n");
			printf("post - show thinking\n");
			printf("nopost - do not show thinking\n");
			printf("new - start new game\n");
			printf("go - set computer thinking\n");
			printf("depth x - set depth to x\n");
			printf("time x - set thinking time to x seconds (depth still applies if set)\n");
			printf("view - show current depth and movetime settings\n");
			printf("setboard x - set position to fen x\n");
			printf("** note ** - to reset time and depth, set to 0\n");
			printf("enter moves using b7b8q notation\n\n\n");
			continue;
		}

		if(!strcmp(command, "mirror")) {
			engineSide = BOTH;
			MirrorEvalTest(pos);
			continue;
		}

		if(!strcmp(command, "eval")) {
			PrintBoard(pos);
			printf("Eval:%d",EvalPosition(pos));
			MirrorBoard(pos);
			PrintBoard(pos);
			printf("Eval:%d",EvalPosition(pos));
			continue;
		}

		if(!strcmp(command, "setboard")){
			engineSide = BOTH;
			ParseFen(inBuf+9, pos);
			continue;
		}

		if(!strcmp(command, "quit")) {
			info->quit = TRUE;
			break;
		}

		if(!strcmp(command, "post")) {
			info->POST_THINKING = TRUE;
			continue;
		}

		if(!strcmp(command, "print")) {
			PrintBoard(pos);
			continue;
		}

		if(!strcmp(command, "nopost")) {
			info->POST_THINKING = FALSE;
			continue;
		}

		if(!strcmp(command, "force")) {
			engineSide = BOTH;
			continue;
		}

		if(!strcmp(command, "view")) {
			if(depth == MAXDEPTH) printf("depth not set ");
			else printf("depth %d",depth);

			if(movetime != 0) printf(" movetime %ds\n",movetime/1000);
			else printf(" movetime not set\n");

			continue;
		}

		if(!strcmp(command, "depth")) {
			sscanf(inBuf, "depth %d", &depth);
			if(depth==0) depth = MAXDEPTH;
			continue;
		}

		if(!strcmp(command, "time")) {
			sscanf(inBuf, "time %d", &movetime);
			movetime *= 1000;
			continue;
		}

		if(!strcmp(command, "new")) {
			ClearHashTable(pos->HashTable);
			engineSide = BLACK;
			ParseFen(START_FEN, pos);
			continue;
		}

		if(!strcmp(command, "go")) {
			engineSide = pos->side;
			continue;
		}

		move = ParseMove(inBuf, pos);
		if(move == NOMOVE) {
			printf("Command unknown:%s\n",inBuf);
			continue;
		}
		MakeMove(pos, move);
		pos->ply=0;
	}
}
