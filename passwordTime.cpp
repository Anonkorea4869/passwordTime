#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <termios.h>
//#include <term.h>
#include <unistd.h>
/*

#include <windows.h>
#include <io.h>
#include <direct.h>
*/


#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

#define ID_MAX 21//Actullay ID BUFFER SIZE
#define PW_MAX 21//Actullay PW BUFFER SIZE
#define LOG_COUNTER 3
#define allowTimeRange 150//ms(The fewer, the more accurate)
#define ALGORITHMN pw_time_median
//#define HOW_SHOW printf("*");
//#define HOW_SHOW2 printf("*");
#define HOW_SHOW printf("%c", pwCheck[i]);
#define HOW_SHOW2 printf("%c", pwCheck[j]);

void makeRootDirectory();/*Make root directory for save id*/
int menu();/*Menu for move pointer*/
int weightNumber(int set);/*Weight for control menu*/
void login_check();/*Login function*/
void login(char* id);//login menu
void pw_time_average(char* path, double* pwTimeAverage);/*password time logging for Accuracy*/
void pw_time_median(char* path, double* pwTimeAverage);/*password time logging for Accuracy*/
void find_median(double* pwTimeAverage, int** arr, int height, int width);/*Find median(Selection sort)*/
void setRegister();/*Register function*/
int pwTimeLogging(char* id, int count);/*password time logging for Accuracy*/
void insertTime(char* path, double* time);/*Record a new time after backup*/
void time_interval(double* time, int pwSize);/*Time between word and word*/
char* find_pw(char* path);/*Find password using path(any path)*/

int _getch(void)  
{  
  int ch;  
  struct termios buf;  
  struct termios save;  
  
   tcgetattr(0, &save);  
   buf = save;  
   buf.c_lflag &= ~(ICANON|ECHO);  
   buf.c_cc[VMIN] = 1;  
   buf.c_cc[VTIME] = 0;  
   tcsetattr(0, TCSAFLUSH, &buf);  
   ch = getchar();  
   tcsetattr(0, TCSAFLUSH, &save);  
   return ch;  
} 

char* strlwr(const char *str)
{
	char *arr = (char*)str;

    while(*arr) {
		if(*arr >= 97 && *arr <= 132) {
			*arr -= 32;
		}
	}
	return arr;
}

int main(void)
{
	int choose;

	system("cls");
	makeRootDirectory();//Create root directory

	while (1) {
		switch (menu()) { //Choose option
		case 1:
			login_check();
			break;

		case 2:
			setRegister();
			break;

		case 3:
			return 0;
			break;

		}
	}
	return 0;
}

/*Make root directory for save id*/
void makeRootDirectory() {

	//if (_mkdir("root") == 0);
	
	//printf("Root directry succesfully create\n");

	//else;
	//printf("Root directory is already created\n");

	if (access("./root/", 0)) {//Not exist root file
		system("mkdir root");
	}
}

/*Menu for move pointer*/
/*OUTPUT : menu number*/
int menu() {

	int setNumber = 1;
	int pastNumber = 1;//If user not input pointer

	while (1) {
		system("clear");

		printf("1. login ");
		if (setNumber == 1) printf("<-");

		printf("\n2. Register ");
		if (setNumber == 2) printf("<-");

		printf("\n3. exit ");
		if (setNumber == 3) printf("<-");

		printf("\n\nIf you want back plz insert \"back\" Anywhere\n");
		pastNumber = setNumber;
		setNumber = weightNumber(setNumber);

		if (setNumber == -1)
			return pastNumber;
	}
}

/*Weight for control menu*/
/*OUTPUT : Menu pointer 1 ~ 3*/
int weightNumber(int set) {//This function help viewing pointer

	char ch;
	int weight = 0;

	ch = _getch();
	if (ch == 13)//choose interrupt
		return -1;

	else if (ch == 224)//wrong insert flush
		ch = _getch();


	if (ch == 72 || ch == 75) //up and left
		weight--;

	else if (ch == 80 || ch == 77) //down and right
		weight++;

	//else
	//	printf("Wrong interrupt\n");

	if (1 <= set + weight && set + weight <= 3)//collect
		return set + weight;

	else
		return set;
}

/*Login check function*/
void login_check() {

	int i, check;
	double time[PW_MAX], pwTimeAverage[PW_MAX];
	char id[ID_MAX], pw[PW_MAX], pwCheck[PW_MAX];
	char path[ID_MAX + 20];

	while (1) {//Check id
		system("clear");

		printf("ID : ");
		scanf("%s", id);

		if (!strcmp(strlwr(id), "back"))//back
			return;

		path[0] = 0;
		strcat(path, "root//");
		strcat(path, id);

		if (access(path, 0) != 0) {//Not exist file
			printf("Unvalid ID <Press any key>\n");
			_getch();
		}

		else//Exist file
			break;
	}

	pw[0] = NULL;
	strcat(pw, find_pw(path));//Set pw

	//pw_time_average(path, pwTimeAverage);//I don't give argument pw so maybe it slow
	ALGORITHMN(path, pwTimeAverage);//I don't give argument pw so maybe it slow

	while (1) {//Check pw

		system("cls");

		printf("ID : %s\n", id);
		printf("PW : ");

		for (i = 0; i < PW_MAX - 1; i++) {//-1 is secure

			pwCheck[i] = _getch();//Virtual insert

			if (pwCheck[i] == 13)//Enter
				break;

			//printf("*");
			HOW_SHOW

				time[i] = clock();//time check
		}
		pwCheck[i] = 0;//NULL

		if (!strcmp(strlwr(pwCheck), "back"))//Back and remove information
			return;

		if (!strcmp(pw, pwCheck)) {//is Same

			time_interval(time, strlen(pw));//Change origin to interval format

			check = 0;
			for (i = 1; i < strlen(pw); i++)//Pass check of the first word
				if ((pwTimeAverage[i] - allowTimeRange) < time[i] && time[i] < (pwTimeAverage[i] + allowTimeRange))//If intermediate value
					check++;

			if (check == strlen(pw) - 1) {//Success
				printf("\nLogin success\n");
				_getch();
				break;
			}

			else {
				printf("\nTime fail <Press any key>\n");
				_getch();
				continue;
			}
		}

		else {//is not same
			printf("\nWrong password press any key\n");
			_getch();
			continue;
		}
	}

	//Login success
	login(id);
}

void login(char* id) {

	char pw[PW_MAX];
	int count;

	system("cls");

	pw[0] = 0;
	strcat(pw, find_pw(id));

	printf("Increased accuracy\n");//Just one menu
	printf("How much logging? : ");
	scanf("%d", &count);

	pwTimeLogging(id, count);

}

/*Analyze the data and return the appropriate time.*/
void pw_time_average(char* path, double* pwTimeAverage) {

	int i, h, w, dataNum, num;
	char pw[PW_MAX];
	char timePath[ID_MAX + 20];
	int** arr;
	FILE* fp;

	pw[0] = NULL;//Flush
	strcat(pw, find_pw(path));

	timePath[0] = NULL;
	strcat(timePath, path);
	strcat(timePath, "//summary.txt");

	fp = fopen(timePath, "r");
	fscanf(fp, "%d", &dataNum);

	for (i = 0; i < strlen(pw); i++)//Clear
		pwTimeAverage[i] = 0;

	for (h = 0; h < dataNum; h++) {//Insert
		for (w = 0; w < strlen(pw); w++) {
			fscanf(fp, "%d ", &num);
			pwTimeAverage[w] += num;
		}
	}

	for (i = 0; i < strlen(pw); i++)//Clear
		pwTimeAverage[i] /= dataNum;

	fclose(fp);

}

/*Analyze the data and return the appropriate time.*/
void pw_time_median(char* path, double* pwTimeAverage) {

	int i, h, w, dataNum, num;
	char pw[PW_MAX];
	char timePath[ID_MAX + 20];
	int** arr;
	FILE* fp;

	pw[0] = NULL;//Flush
	strcat(pw, find_pw(path));

	timePath[0] = NULL;
	strcat(timePath, path);
	strcat(timePath, "//summary.txt");

	fp = fopen(timePath, "r");
	fscanf(fp, "%d", &dataNum);

	arr = (int**)malloc(sizeof(int*) * dataNum);//Dynamic allocation h

	for (i = 0; i < dataNum; i++)
		arr[i] = (int*)malloc(sizeof(int) * strlen(pw));//dynamic allocation w(password string)

	for (h = 0; h < dataNum; h++) {//Read original data 
		for (w = 0; w < strlen(pw); w++) {
			fscanf(fp, "%d ", &arr[h][w]);
		}
	}

	find_median(pwTimeAverage, arr, h, w);//Call by reference

	for (i = 0; i < dataNum; i++)
		free(arr[i]);//Free w
	free(arr);//Free h

	fclose(fp);

}

/*Find median(Selection sort)*/
void find_median(double* pwTimeAverage, int** arr, int height, int width) {

	int h, w, i;
	int min, minIndex, temp;
	/*
	for (h = 0; h < height; h++) {
		for (w = 0; w < width; w++)
			printf("%5d ", arr[h][w]);

		printf("\n");
	}
	*/

	for (w = 1; w < width; w++) {
		for (h = 0; h < height - 1; h++) {

			min = arr[h][w];
			minIndex = h;

			for (i = h; i < height; i++) {
				if (min > arr[i][w]) {
					min = arr[i][w];
					minIndex = i;
				}
			}

			if (h != minIndex) {
				temp = arr[h][w];
				arr[h][w] = arr[minIndex][w];
				arr[minIndex][w] = temp;
			}
		}
	}


	for (i = 0; i < width; i++) {
		pwTimeAverage[i] = arr[(int)(height / 2)][i];//Insert midian
		//printf("%d ", (int)pwTimeAverage[i]);
	}

}

/*Register function*/
/*NOT EXCEPTION CONTROL : MAKE ID, PW BUT NOT PWTIMELOGGING SUCESSFULLY*/
void setRegister() {

	char id[ID_MAX], pw[PW_MAX], path[ID_MAX + PW_MAX + 20];
	FILE* fp;

	while (1) {
		system("cls");

		printf("CREATE ID = ");
		scanf("%s", id, sizeof(id));

		if (!strcmp(strlwr(id), "back"))//Back
			return;

		path[0] = 0;//May be it error
		strncat(path, "root//", sizeof("root//"));
		strncat(path, id, sizeof(id));

		if (access(path, 0) == 0)//sucessfully create
			break;

		else {//Alreday exist or error
			printf("Exist ID <Press any key>\n");
			_getch();
		}
	}

	printf("CREATE PW = ");
	scanf("%s", pw, sizeof(pw));

	if (!strcmp(strlwr(pw), "back")) {//Back
		//_rmdir(path);//Remove id directory
		system(strcat("system ", path));
		return;
	}

	strncat(path, "//password.txt", sizeof(pw));

	fp = fopen(path, "w");//Open file Stream
	fputs(pw, fp);//Insert password
	fclose(fp);//Close file Stream

	if (pwTimeLogging(id, 3) != 0) {//fail end//Somethign wrong
		path[0] = 0;//Flush

		strcat(path, "root//");
		strcat(path, id);

		//remove directory
		system(strcat("system ", path));
		if (access(strcat("system ", path),0) == -1)
			printf("\nRemove directry success\n");

		else
			printf("\nRemove directry fail\n");
	}
}

/*password time logging for Accuracy*/
/*OUTPUT : Sccess = 0, Fail = -1*/
int pwTimeLogging(char* id, int count) {//password time loging

	int i, j;
	double time[PW_MAX];
	char pw[PW_MAX], pwCheck[PW_MAX];
	char path[ID_MAX + PW_MAX + 20];
	clock_t start, finish;
	FILE* fp;

	path[0] = NULL;
	strcat(path, "root//");
	strcat(path, id);
	strcat(path, "//summary.txt");

	pw[0] = NULL;
	strcat(pw, find_pw(id));

	if (access(path, 0) != 0) {//Not exist file
		fp = fopen(path, "a");
		fputc('0', fp);
		fclose(fp);
	}

	for (i = 0; i < count; i++) {//increase accuracy

		system("cls");
		printf("Insert password for accuracy (%d/%d)\n", i + 1, count);
		printf("password : ");

		for (j = 0; j < PW_MAX - 1; j++) {//-1 is secure

			pwCheck[j] = _getch();//Virtual insert

			if (pwCheck[j] == 13)//Enter
				break;

			//printf("*");
			HOW_SHOW2
				time[j] = clock();//time check
		}
		pwCheck[j] = 0;//NULL

		if (!strcmp(strlwr(pwCheck), "back"))//Back and remove information
			return -1;

		if (!strcmp(pw, pwCheck))//is Same
			insertTime(path, time);

		else {//is not same
			printf("\nWrong password press any key\n");
			_getch();
			i--;
			continue;
		}
	}

	return 0;//Success
}

/*Record a new time after backup*/
void insertTime(char* path, double* time) {

	int dataNum, i, h, w;
	FILE* fp;
	int** arr;
	char id[ID_MAX], pw[PW_MAX];

	//���ڿ��� ����� �˾ƺ��� -> root//���� /ã�� �� ����
	pw[0] = NULL;//Flush
	strcat(pw, find_pw(path));

	fp = fopen(path, "r");//Append file stream open
	fscanf(fp, "%d", &dataNum);//Read data number

	//integer is also okay(under te floating point flush)
	arr = (int**)malloc(sizeof(int*) * dataNum);//Dynamic allocation h

	for (i = 0; i < dataNum; i++)
		arr[i] = (int*)malloc(sizeof(int) * strlen(pw));//dynamic allocation w(password string)

	for (h = 0; h < dataNum; h++) {//Read original data 
		for (w = 0; w < strlen(pw); w++) {
			fscanf(fp, "%d ", &arr[h][w]);
		}
	}
	fclose(fp);

	//ó������ �ٽ� ����
	fp = fopen(path, "w");//Open file stream for write new data
	fprintf(fp, "%d\n", dataNum + 1);//Write number

	for (h = 0; h < dataNum; h++) {//Write again
		for (w = 0; w < strlen(pw); w++) {
			fprintf(fp, "%d ", arr[h][w]);
		}

		fprintf(fp, "\n");
	}

	time_interval(time, strlen(pw));

	for (i = 0; i < strlen(pw); i++)//Write new data
		fprintf(fp, "%0.lf ", time[i]);

	for (i = 0; i < dataNum; i++)
		free(arr[i]);//Free w
	free(arr);//Free h

	fclose(fp);//Close file stream

}

/*Time between word and word*/
/*First word default is 0*/
void time_interval(double* time, int pwSize) {

	for (int i = pwSize; i > 0; i--)//Move big to low
		time[i] -= time[i - 1];

	time[0] = 0;
}

/*Find password using path(any path)*/
/*OUTPUT : password*/
char* find_pw(char* path) {//Return value actually char*

	int i;
	char id[ID_MAX], pw[PW_MAX];
	char newPath[ID_MAX + 20];
	FILE* fp;


	if (strstr(path, "//") != NULL) {//Not just id
		for (i = 0; i < strlen(path) - strlen("root//"); i++) {//When only need root //id
			if (path[strlen("root//") + i] == '/')
				break;

			id[i] = path[strlen("root//") + i];//Insert id
		}
		id[i] = NULL;
	}

	else {//Just id
		id[0] = NULL;
		strcat(id, path);
	}

	newPath[0] = NULL;//Flush
	strcat(newPath, "root//");
	strcat(newPath, id);
	strcat(newPath, "//password.txt");

	fp = fopen(newPath, "r");
	fscanf(fp, "%s", pw);//Read password

	return pw;

}