
#include "shell.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define printf(...) shellPrint(shellPushbox, ##__VA_ARGS__)
#define getchar()   shellGetChar(shellPushbox)

Shell *shellPushbox = NULL;

static char shellGetChar(Shell *shell) {
    char data;
    if (shell->read && shell->read(&data, 1) == 1) {
        return data;
    } else {
        return -1;
    }
}


//地图7行8列 行[0-6] 列[0-7]
//0 路
//1 墙
//2 箱子
//3 终点
//4 小老鼠
//7 小老鼠站在终止上
//5 箱子到达终点上

int g_boards[7][8] = {
    { 0, 1, 1, 1, 1, 1, 1, 0 },
    { 0, 1, 0, 0, 0, 0, 1, 1 },
    { 1, 3, 0, 1, 1, 2, 0, 1 },
    { 1, 0, 3, 3, 2, 0, 0, 1 },
    { 1, 0, 0, 1, 2, 0, 0, 1 },
    { 1, 0, 0, 4, 0, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 0, 0, 0 }
},
    boards[7][8] = {};

//记录小老鼠在移动中的位置
int row = 0;
int col = 0;
int cnt = 0; //箱子个数，用来判断游戏是否结束。

//初始化地图
void pushbox_init()
{
    for (int i = 0; i < 7 * 8; i++) { //这里是为了当你按下enter键时，能够重置游戏，重置游戏时需要重新初始化地图
        boards[i / 8][i % 8] = g_boards[i / 8][i % 8];
        if (4 == boards[i / 8][i % 8]) //这里还需要记录老鼠的位置
        {
            row = i / 8;
            col = i % 8;
        }
    }
}

//打印地图
void pushbox_print_map()
{
    printf( "\r\n  LetterShell PushBox!\r\n");
    for (int i = 0; i < 7; i++) {
        printf( "  ");
        for (int j = 0; j < 8; j++) {
            switch (boards[i][j]) {
            case 0:
                printf( " ");
                break; //空格代表路径
            case 1:
                printf( "■");
                break; //打印墙
            case 2:
            case 5:
                printf( "●");
                break; //打印箱子
            case 3:
                printf( "★");
                break; //打印终点
            case 4:
            case 7:
                printf( "♀");
                break; //打印小老鼠
            }
        }
        printf( "\r\n");
    }
	printf("w,a,s,d or q\n");
}

//推箱子
int pushbox_print_move(int nrow, int ncol, int nnrow, int nncol)
{
    printf( "\033[H");
    if (0 == boards[nrow][ncol] || 3 == boards[nrow][ncol]) //如果小老鼠前面是路或者终点，说明可以移动
    {
        boards[nrow][ncol] += 4; //小老鼠进入这个位置了
        boards[row][col] -= 4; //小老鼠离开这个位置了
        //移动过后小老鼠的位置也改变了
        row = nrow;
        col = ncol;
    } else if (2 == boards[nrow][ncol] || 5 == boards[nrow][ncol]) //如果小老鼠前面是箱子
    {
        if (0 == boards[nnrow][nncol] || 3 == boards[nnrow][nncol]) //如果箱子前面是路或者终点则可以移动
        {
            boards[nnrow][nncol] += 2;
            boards[nrow][ncol] -= 2 - 4;
            //这里其实是boards[nrow][ncol]=boards[nrow][ncol]-2+4
            //减去2是因为箱子移走了，加上4是因为小老鼠进入了
            boards[row][col] -= 4;
            row = nrow;
            col = ncol;
        }
    }
    for (int i = 0; i < 7; i++) //统计游戏是否结束，当箱子都在终点上时就结束了
    {
        for (int j = 0; j < 8; j++) {
            if (5 == boards[i][j]) {
                cnt++;
            }
            if (3 == cnt) {
                printf( "通关！\r\n");
                return 1; //通关退出游戏
            }
        }
    }
    cnt = 0; //如果游戏没有结束，下次还是需要从0统计
    pushbox_print_map();
    return 0;
}


int main_pushbox(int argc, char* argv[])
{
    char c;
    int res = 0;

    shellPushbox = shellGetCurrent();
    if (!shellPushbox) {
        return -1;
    }
		
    printf( "\033[?25l\033[2J");
    printf( "\033[H");
    pushbox_init();//游戏初始化
    pushbox_print_map();
    while (1) {
        c = getchar();;
        if (c < 0) {
			printf("\nError! Cannot read keyboard input!");
            break;
        }
        switch (c) //每次都需要判断小老鼠前面和前面的前面的坐标的状态，这样可以在move()
        {          //函数中统一各个方向的写法
        case 'w':
            res = pushbox_print_move(row - 1, col, row - 2, col);
            break;
        case 's':
            res = pushbox_print_move(row + 1, col, row + 2, col);
            break;
        case 'a':
            res = pushbox_print_move(row, col - 1, row, col - 2);
            break;
        case 'd':
            res = pushbox_print_move(row, col + 1, row, col + 2);
            break;
        case 'r':
            pushbox_init();
            break; //按enter则从新开始游戏
        case 'q':
            res = 1; //如果按q则退出游戏
        }
        if(res){
            break;
        }
    }
    printf( "\033[?25l\033[2J");
	return EXIT_SUCCESS;
}
