#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mbed.h"
#include "Adafruit_SSD1306.h"
#include "Games.h"
#include "pitches.h"
#include "HCSR04.h"

AnalogIn Sensor(A1);
AnalogIn vx(A1);
AnalogIn vy(A0);
PwmOut buzzer(D10); // buzzer = PTA1
// DigitalOut col1(D9);
// DigitalOut col2(D10);
// DigitalOut col3(D11);
// DigitalOut col4(D12);
// DigitalIn row2(D2, PullDown);
// DigitalIn row3(D0, PullDown);
// DigitalIn row4(D1, PullDown);
DigitalIn sw(D13, PullUp);
Serial pc(USBTX, USBRX);
DigitalInOut pin(D9);                          // Activate digital in
HCSR04 sonar(D11, D12);                         // Trigger, Echo
SPIPreInit gSpi(A6, A5, A4);                    // mosi, miso, clk
Adafruit_SSD1306_Spi display(gSpi, A2, A3, D3); //&spi,DC, RST,CS

char board[3][3] = {0};
int X[3], Y[3];
int WIN, LOSE, TIE, a, b, direction;
Timer tmr;     // initialize timer
uint64_t adat; // 64 bit variable for temporary data
int H, T, x, y, grade, shape, form, nextShape, nextForm, map[ROW][COL];
int Temperature, Humidity;

int length1 = 75;
float frequency1[] = {NOTE_E6, NOTE_E6, 0, NOTE_E6, 0, NOTE_C6, NOTE_E6, 0, NOTE_G6, 0, 0,
                      0, NOTE_G5, 0, 0, 0, NOTE_C6, 0, 0, NOTE_G5, 0, 0, NOTE_E5, 0, 0,
                      NOTE_A5, 0, NOTE_B5, 0, NOTE_AS5, NOTE_A5, 0, NOTE_G5, NOTE_E6, NOTE_G6,
                      NOTE_A6, 0, NOTE_F6, NOTE_G6, 0, NOTE_E6, 0, NOTE_C6, NOTE_D6, NOTE_B5,
                      0, 0, NOTE_C6, 0, 0, NOTE_G5, 0, 0, NOTE_E5, 0, 0, NOTE_A5, 0, NOTE_B5,
                      0, NOTE_AS5, NOTE_A5, 0, NOTE_G5, NOTE_E6, NOTE_G6, NOTE_A6, 0, NOTE_F6,
                      NOTE_G6, 0, NOTE_E6, 0, NOTE_C6, NOTE_D6, NOTE_B5, 0, 0};

float beat1[] = {12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
                 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 9, 9, 9, 12,
                 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
                 12, 12, 12, 12, 12, 12, 12, 12, 12, 9, 9, 9, 12, 12, 12, 12, 12, 12, 12,
                 12, 12, 12, 12, 12};

int length2 = 56;
float frequency2[] = {NOTE_E6, NOTE_DS6, NOTE_E6, NOTE_DS6, NOTE_E6, NOTE_B5, NOTE_D6, NOTE_C6, NOTE_A5,
                      NOTE_C5, NOTE_E5, NOTE_A5, NOTE_B5, NOTE_E5, NOTE_GS5, NOTE_B5, NOTE_C6,
                      NOTE_E6, NOTE_DS6, NOTE_E6, NOTE_DS6, NOTE_E6, NOTE_B5, NOTE_D6, NOTE_C6, NOTE_A5,
                      NOTE_C5, NOTE_E5, NOTE_A5, NOTE_B5, NOTE_E5, NOTE_GS5, NOTE_B5, NOTE_C6,
                      NOTE_B5, NOTE_C6, NOTE_D6, NOTE_E6, NOTE_G5, NOTE_F6, NOTE_E6, NOTE_D6,
                      NOTE_F5, NOTE_E6, NOTE_D6, NOTE_C6, NOTE_E5, NOTE_D6, NOTE_C6, NOTE_B5,
                      NOTE_E5, NOTE_E6};
float beat2[] = {12, 12, 12, 12, 12, 12, 12, 12, 9, 12, 12, 12, 9,
                 12, 12, 12, 9, 12, 12, 12, 12, 12, 12, 12, 12, 9,
                 12, 12, 12, 9, 12, 12, 12, 9, 12, 12, 12, 9, 12, 12, 12, 9,
                 12, 12, 12, 9, 12, 12, 12, 9, 12, 9};

float frequency3[] = {659, 554, 659, 554, 550, 494, 554, 587, 494, 659, 554, 440}; // frequency array
float beat3[] = {1, 1, 1, 1, 1, 0.5, 0.5, 1, 1, 1, 1, 2};                          // beat array

struct Coodinate
{
    int X; //������
    int Y; //������
} FOOD;

struct Snake //��
{
    /*�����鴢���ߵ�ÿһ���ֵ�����*/
    int X[100];
    int Y[100];
    int len; //����
} snake;

struct Block
{
    int space[3][3];
} block[7][4]; //���ڴ洢7�ֻ�����״����ĸ��Ե�4����̬����Ϣ����28��

int Display1()
{
    int n = 1;
    while (1)
    {
        ymd(time(NULL), X);
        hms(time(NULL), Y);
        display.clearDisplay();
        display.setTextCursor(0, 0);
        display.printf("      %d/%2d/%d\n", X[0] + 52, X[1] - 6, X[2] + 337);
        display.printf("       %d:%d:%d\n", Y[0] + 11, Y[1] + 14+15, Y[2]);
        direction = yaogan();
        if (direction == 1 && n != 1 && n != 2)
            n -= 2;
        if (direction == 2 && n != 3 && n != 4)
            n += 2;
        if (direction == 3 && n != 1 && n != 3)
            n -= 1;
        if (direction == 4 && n != 2 && n != 4)
            n += 1;
        switch (n)
        {
        case 1:
            display.printf("o Game        Sensor1  Music       Sensor2");
            break;
        case 2:
            display.printf("  Game      o Sensor1  Music       Sensor2");
            break;
        case 3:
            display.printf("  Game        Sensor1o Music       Sensor2");
            break;
        case 4:
            display.printf("  Game        Sensor1  Music     o Sensor2");
            break;
        }
        display.display();
        if (!sw)
        {
            wait(0.2);
            return n;
        }
        wait(0.15);
    }
}

int Display2()
{
    int n = 1;
    while (1)
    {
        direction = yaogan();
        display.clearDisplay();
        display.setTextCursor(0, 0);
        display.printf("Choose games:\n");
        if (direction == 1 && n > 1 && n <= 3)
            n--;
        if (direction == 2 && n >= 1 && n < 3)
            n++;
        switch (n)
        {
        case 1:
            display.printf("o Tic-Tac-Toe\n  Gluttonous snake\n  Tetris Worlds");
            break;
        case 2:
            display.printf("  Tic-Tac-Toe\no Gluttonous snake\n  Tetris Worlds");
            break;
        case 3:
            display.printf("  Tic-Tac-Toe\n  Gluttonous snake\no Tetris Worlds");
            break;
        }
        display.display();
        if (!sw)
        {
            wait(0.2);
            return n;
        }
        wait(0.15);
    }
}

int Display3()
{
    int n = 1;
    while (1)
    {
        direction = yaogan();
        display.clearDisplay();
        display.setTextCursor(0, 0);
        display.printf("Choose sensor:\n");
        if (direction == 1 && n > 1 && n <= 3)
            n--;
        if (direction == 2 && n >= 1 && n < 3)
            n++;
        switch (n)
        {
        case 1:
            display.printf("o Temperature\n  Ultrasound\n  Alcohol");
            break;
        case 2:
            display.printf("  Temperature\no Ultrasound\n  Alcohol");
            break;
        case 3:
            display.printf("  Temperature\n  Ultrasound\no Alcohol");
            break;
        }
        display.display();
        if (!sw)
        {
            wait(0.2);
            return n;
        }
        wait(0.15);
    }
}

int Display4()
{
    int n = 1;
    while (1)
    {
        direction = yaogan();
        display.clearDisplay();
        display.setTextCursor(0, 0);
        display.printf("Choose music:\n");
        if (direction == 1 && n > 1 && n <= 3)
            n--;
        if (direction == 2 && n >= 1 && n < 3)
            n++;
        switch (n)
        {
        case 1:
            display.printf("o Music1\n  Music2\n  Music3");
            break;
        case 2:
            display.printf("  Music1\no Music2\n  Music3");
            break;
        case 3:
            display.printf("  Music1\n  Music2\no Music3");
            break;
        }
        display.display();
        if (!sw)
        {
            wait(0.2);
            return n;
        }
        wait(0.15);
    }
}

int Display5()
{
    int n = 1;
    while (1)
    {
        direction = yaogan();
        display.clearDisplay();
        display.setTextCursor(0, 0);
        display.printf("Choose sensor:\n");
        if (direction == 1 && n > 1 && n <= 3)
            n--;
        if (direction == 2 && n >= 1 && n < 3)
            n++;
        switch (n)
        {
        case 1:
            display.printf("o CO\n  CH4\n  Air");
            break;
        case 2:
            display.printf("  CO\no CH4\n  Air");
            break;
        case 3:
            display.printf("  CO\n  CH4\no Air");
            break;
        }
        display.display();
        if (!sw)
        {
            wait(0.2);
            return n;
        }
        wait(0.15);
    }
}

void game_choice(int n)
{
    switch (n)
    {
    case 1:
        GAME1();
        break;
    case 2:
        GAME2();
        break;
    case 3:
        GAME3();
        break;
    }
}

void sensor_choice1(int n)
{
    switch (n)
    {
    case 1:
        Sensor1();
        break;
    case 2:
        Sensor2();
        break;
    case 3:
        Sensor3();
        break;
    }
}

void music_choice(int n)
{
    switch (n)
    {
    case 1:
        Music1();
        break;
    case 2:
        Music2();
        break;
    case 3:
        Music3();
        break;
    }
    buzzer = 0;
}

void sensor_choice2(int n)
{
    switch (n)
    {
    case 1:
        Sensor4();
        break;
    case 2:
        Sensor5();
        break;
    case 3:
        Sensor6();
        break;
    }
}

void GAME1()
{
    int i, x, y, c, Bool;
    WIN = LOSE = TIE = 0;
    while (1)
    {
        c = Bool = 0;
        while (1)
        {
            show_dislpay("Start or End \n", 0);
            i = get_code();
            if (i == 11)
            {
                display.clearDisplay();
                display.setTextCursor(0, 0);
                display.printf("Bye!\nWin: %d\nLose: %d\nTie: %d", WIN, LOSE, TIE);
                display.display();
                Bool = 1;
            }
            if (i == 10)
                break;
            if (Bool)
                break;
        }
        if (Bool)
            break;
        for (a = 0; a <= 2; a++) // Fill in the panel with '.'
        {
            for (b = 0; b <= 2; b++)
                board[a][b] = '.';
        }
        while (c != 9)
        {
            if (check(board) != -1) // Judge whether to win
                break;
            while (1)
            {
                show_dislpay("Enter 1 ~ 9.\n", 1);
                i = get_code();
                if (i > 0 && i < 10)
                {
                    x = 2 - (i - 1) / 3; // Processing input
                    y = (i - 1) % 3;
                    if (board[x][y] == '.')
                    {
                        board[x][y] = 'X';
                        show_dislpay("Enter 1 ~ 9.\n", 1);
                        c++;
                        break;
                    }
                }
            }
            if (check(board) != -1) // Judge whether to win
                break;
            if (c == 9) // Judge whether to tie
            {
                show_dislpay("Tied!\n", 1);
                TIE++;
                break;
            }
            computer(board);
            c++;
        }
        thread_sleep_for(1500);
    }
}

void GAME2()
{
    direction = 3;
    snake.len = 3;
    display.clearDisplay();
    snake.X[0] = 32, snake.Y[0] = 11;
    snake.X[1] = 33, snake.X[1] = 11;
    snake.X[2] = 34, snake.X[2] = 11;
    update_food();
    while (1)
    {
        move_snake();
        draw_map();
        wait(0.2);
        if (judge())
            break;
    }
    display.clearDisplay();
    display.setTextCursor(0, 0);
    display.printf("You eat:\n%d", snake.len - 3);
    display.display();
}

int GAME3()
{
    grade = 0;
    display.clearDisplay();
    for (int i = 1; i <= ROW; i++)
        for (int j = 1; j <= COL; j++)
            map[i][j] = 0;
    InitBlock();                           //��ʼ��������Ϣ
    srand((unsigned int)time(NULL));       //������������ɵ����
    shape = rand() % 7, form = rand() % 4; //�����ȡ�������״����̬
    while (1)
    {
        x = 24, y = 4;                                 //�����ʼ����λ�õĺ�������
        nextShape = rand() % 7, nextForm = rand() % 4; //�����ȡ��һ���������״����̬
        while (1)
        {
            for (int i = 0; i < ROW; i++)
                for (int j = 0; j < COL; j++)
                    if (map[i][j] == 2)
                        map[i][j] = 0; //����λ�ñ��Ϊû�з���
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    if (block[shape][form].space[i][j] == 1 && map[x + i][y + j] != 1)
                        map[x - 1 + j][y - 1 + i] = 2; //����λ�ñ��Ϊ�з���
            draw_block(nextShape, nextForm);
            direction = yaogan();
            if (direction != DOWN)
                wait(0.15);
            if (!sw)
            {
                if (shape)
                    if (judge(shape, (form + 1) % 4, x, y)) //�жϷ�����ת���Ƿ�Ϸ�
                    {
                        //������ת��Ϸ��Ž������²���
                        form = (form + 1) % 4; //�������̬��������һ����ʾ����ʱ���൱����ת�ˣ�
                        draw_block(nextShape, nextForm);
                        wait(0.15);
                    }
                if (judge(shape, form, x - 1, y)) //�жϷ��������ƶ�һλ���Ƿ�Ϸ�
                    x--;                          //�������������ܲ���ԭ����ת�ɣ�
            }
            else if (!direction || direction == DOWN) 
            {
                if (judge(shape, form, x - 1, y) == 0) //����������Ͳ��Ϸ��ˣ��Ѿ�����ײ���
                {
                    for (int i = 0; i < 3; i++)
                        for (int j = 0; j < 3; j++)
                            if (block[shape][form].space[i][j] == 1)
                                map[x - 1 + j][y - 1 + i] = 1; //����λ�ñ��Ϊ�з���
                    while (1)
                    {
                        if (JudgeFunc() == 1) //�жϴ˴η��������Ƿ�÷�
                            continue;
                        if (JudgeFunc() == 2) //�Լ���Ϸ�Ƿ����
                        {
                            display.setTextCursor(0, 0);
                            display.printf("Grade: %d", grade);
                            display.display();
                            wait(2);
                            return 0;
                        }
                        break;
                    }
                    break; //������ǰ��ѭ����׼��������һ�����������
                }
                else     //δ���ײ�
                    x--; //��������������һ����ʾ����ʱ���൱��������һ���ˣ�
            }
            else
            {
                switch (direction)
                {
                case LEFT:                            //���������
                    if (judge(shape, form, x, y - 1)) //�жϷ��������ƶ�һλ���Ƿ�Ϸ�
                        y--;                          //�������Լ�����һ����ʾ����ʱ���൱��������һ���ˣ�
                    if (judge(shape, form, x - 1, y)) //�жϷ��������ƶ�һλ���Ƿ�Ϸ�
                        x--;
                    break;
                case RIGHT:                           //���������
                    if (judge(shape, form, x, y + 1)) //�жϷ��������ƶ�һλ���Ƿ�Ϸ�
                        y++;                          //��������������һ����ʾ����ʱ���൱��������һ����
                    if (judge(shape, form, x - 1, y)) //�жϷ��������ƶ�һλ���Ƿ�Ϸ�
                        x--;
                    break;
                }
            }
        }
        shape = nextShape, form = nextForm; //��ȡ��һ���������Ϣ
    }
}

void Sensor1()
{
    int ret;
    while (1)
    {
        ret = ReadDHT11();
        if (ret == SUCCESS)
        {
            display.clearDisplay();
            display.setTextCursor(0, 0);
            display.printf("Humidity|Temperature\n");
            display.printf("Temperature: %d\n", Temperature);
            display.printf("Humidity   : %d\n", Humidity);
        }
        display.display();
        wait(1.0);
        if (!sw)
            break;
    }
}

void Sensor2()
{
    display.clearDisplay();
    display.setTextCursor(0, 0);
    display.printf("Ultrasound\n");
    while (1)
    {
        display.printf("Distance: %.2f cm\r", sonar.getCm());
        display.display();
        wait(1.0);
        if (!sw)
            break;
    }
}

void Sensor3()
{
    float x = 0;
    display.clearDisplay();
    display.setTextCursor(0, 0);
    display.printf("Alcohol Concentration");
    while (1)
    {
        display.printf(" %.2f \r", x);
        display.display();
        x = Sensor;
        wait(1.0);
        if (!sw)
            break;
    }
}

void Sensor4()
{
    float x = 0;
    display.clearDisplay();
    display.setTextCursor(0, 0);
    display.printf("CO Concentration\n");
    while (1)
    {
        display.printf(" %.2f \r", x);
        display.display();
        x = Sensor;
        wait(1.0);
        if (!sw)
            break;
    }
}

void Sensor5()
{
    float x = 0;
    display.clearDisplay();
    display.setTextCursor(0, 0);
    display.printf("CH4 Concentration\n");
    while (1)
    {
        display.printf(" %.2f \r", x);
        display.display();
        x = Sensor;
        wait(1.0);
        if (!sw)
            break;
    }
}

void Sensor6()
{
    float x = 0;
    display.clearDisplay();
    display.setTextCursor(0, 0);
    display.printf("Air Concentration\n");
    while (1)
    {
        display.printf(" %.2f \r", x);
        display.display();
        x = Sensor;
        wait(1.0);
        if (!sw)
            break;
    }
}

int Music1()
{
    display.clearDisplay();
    display.setTextCursor(0, 0);
    display.printf("Music1\n");
    display.display();
    while (1)
    {
        for (int i = 0; i <= length1; i++)
        {
            if (frequency1[i] == 0)
                buzzer = 0.0;
            else
            {
                buzzer.period(1.0 / frequency1[i]); // period = (1.0 / frequency)
                buzzer = 0.5;                       // duty cycle = 50%
            }
            thread_sleep_for(2500.0 / beat1[i]); // duration = (C / beat) ms
            if (!sw)
                return 0;
        }
    }
}

int Music2()
{
    display.clearDisplay();
    display.setTextCursor(0, 0);
    display.printf("Music2\n");
    display.display();
    while (1)
    {
        for (int i = 0; i <= length2; i++)
        {
            if (frequency2[i] == 0)
                buzzer = 0.0;
            else
            {
                buzzer.period(1.0 / frequency2[i]); // period = (1.0 / frequency)
                buzzer = 0.5;                       // duty cycle = 50%
            }
            thread_sleep_for(3500.0 / beat2[i]); // duration = (C / beat)ms
            if (!sw)
                return 0;
        }
    }
}

int Music3()
{
    display.clearDisplay();
    display.setTextCursor(0, 0);
    display.printf("Muisc3\n");
    display.display();
    while (1)
    {
        for (int i = 0; i < 12; i++)
        {
            buzzer.period(1 / (frequency3[i])); // set PWM period
            buzzer = 0.5;                       // set duty cycle
            wait(0.5 * beat3[i]);               // hold for beat period
            if (!sw)
                return 0;
        }
    }
}

int ReadDHT11(void)
{
    // IO must be in hi state to start
    if (WaitPinHigh(250) == ERROR_TIMEOUT)
        return ERROR_BUS_BUSY;
    // start the transfer
    pin.output();
    pin = 0;
    wait_ms(18);
    pin = 1;
    wait_us(30);
    pin.input();
    // wait till the sensor grabs the bus
    if (WaitPinLow(40) == ERROR_TIMEOUT)
        return ERROR_NOT_PRESENT;
    // sensor should signal low 80us and then hi 80us
    if (WaitPinHigh(100) == ERROR_TIMEOUT)
        return ERROR_SYNC_TIMEOUT;
    if (WaitPinLow(100) == ERROR_TIMEOUT)
        return ERROR_NO_PATIENCE;
    int i, bit;
    uint8_t buf[5];
    // capture the data(40 bit)
    for (i = 0; i < 5; i++)
    {
        buf[i] = 0;
        for (bit = 0; bit < 8; bit++)
        {
            if (WaitPinHigh(75) == ERROR_TIMEOUT)
                return ERROR_DATA_TIMEOUT;
            // logic 0 is 28us max, 1 is 70us
            wait_us(40);
            buf[i] |= pin << (7 - bit);
            if (WaitPinLow(50) == ERROR_TIMEOUT)
                return ERROR_DATA_TIMEOUT;
        }
    }
    if (buf[4] != buf[0] + buf[1] + buf[2] + buf[3])
        return ERROR_CHECKSUM;
    Temperature = float(buf[2]);
    Humidity = float(buf[0]);
    return SUCCESS;
}

int Stall(int usTimeMax, int level)
{
    int usTime = 0;
    while (usTime < usTimeMax)
    {
        if (pin == level)
            return SUCCESS;
        usTime++;
        wait_us(1);
    }
    return ERROR_TIMEOUT;
}

int WaitPinHigh(int usTimeOut)
{
    return Stall(usTimeOut, 1);
}

int WaitPinLow(int usTimeOut)
{
    return Stall(usTimeOut, 0);
}

void draw_pixel(int x, int y, int Bool) // Draw the whole
{
    if (Bool)
    {
        display.drawPixel(3 * x - 3, 2 * y - 2, 1);
        display.drawPixel(3 * x - 3, 2 * y - 1, 1);
        display.drawPixel(3 * x - 2, 2 * y - 2, 1);
        display.drawPixel(3 * x - 2, 2 * y - 1, 1);
        display.drawPixel(3 * x - 1, 2 * y - 2, 1);
        display.drawPixel(3 * x - 1, 2 * y - 1, 1);
    }
    else
    {
        display.drawPixel(3 * x - 3, 2 * y - 2, 0);
        display.drawPixel(3 * x - 3, 2 * y - 1, 0);
        display.drawPixel(3 * x - 2, 2 * y - 2, 0);
        display.drawPixel(3 * x - 2, 2 * y - 1, 0);
        display.drawPixel(3 * x - 1, 2 * y - 2, 1);
        display.drawPixel(3 * x - 1, 2 * y - 1, 1);
    }
}

void draw_map()
{
    display.clearDisplay();
    draw_pixel(FOOD.X, FOOD.Y, 1);
    for (int i = 0; i < snake.len - 1; i++)
        draw_pixel(snake.X[i], snake.Y[i], 1);
    display.display();
}

void update_food()
{
    srand(time(NULL));
    while (1)
    {
        int n = 1;
        FOOD.X = rand() % LENGTH + 1;
        FOOD.Y = rand() % (WIDTH / 4 * 3) + 4 + 1;
        for (int i = 0; i < snake.len; i++)
            if (snake.X[0] == FOOD.X && snake.Y[0] == FOOD.Y)
                n = 0;
        if (n)
            break;
    }
}

int judge()
{
    if (snake.Y[0] == 5 && direction == 1)
        return 1;
    if (snake.Y[0] == 16 && direction == 2)
        return 1;
    if (snake.X[0] == 1 && direction == 3)
        return 1;
    if (snake.X[0] == 64 && direction == 4)
        return 1;
    return 0;
}

int yaogan()
{
    while (1)
    {
        int xv, yv;
        xv = vx * 10000, yv = vy * 10000;
        if (yv < 300)
            return UP;
        if (yv > 9700)
            return DOWN;
        if (xv < 300)
            return LEFT;
        if (xv > 9700)
            return RIGHT;
        return 0;
    }
}

void move_snake()
{
    int a = yaogan();
    if (a)
        direction = a;
    if (snake.X[0] == FOOD.X && snake.Y[0] == FOOD.Y)
    {
        snake.len++;
        update_food();
    }
    else
        draw_pixel(snake.X[snake.len - 1], snake.X[snake.len - 1], 0);
    for (int i = snake.len - 1; i > 0; i--) //�����ߵ����꣬������ͷ,����λ�ü̳���һ���������
    {
        snake.X[i] = snake.X[i - 1];
        snake.Y[i] = snake.Y[i - 1]; //�ƶ�λ��
    }
    switch (direction)
    {
    case 1:
        snake.Y[0]--;
        break;
    case 2:
        snake.Y[0]++;
        break;
    case 3:
        snake.X[0]--;
        break;
    case 4:
        snake.X[0]++;
        break;
    }
}

void setcol(int n)
{
    // col1 = col2 = col3 = col4 = 0;
    // switch (n)
    // {
    // case 1:
    //     col1 = 1, col2 = col3 = col4 = 0;
    //     break;
    // case 2:
    //     col2 = 1, col1 = col3 = col4 = 0;
    //     break;
    // case 3:
    //     col3 = 1, col1 = col2 = col4 = 0;
    //     break;
    // case 4:
    //     col4 = 1, col1 = col2 = col3 = 0;
    //     break;
    // }
}

int get_code()
{
    // int n = 0;
    // while (!n)
    // {
    //     setcol(1);
    //     if (row2 == 1)
    //         n = 12;
    //     if (row3 == 1)
    //         n = 11;
    //     if (row4 == 1)
    //         n = 10;
    //     setcol(2);
    //     if (row2 == 1)
    //         n = 3;
    //     if (row3 == 1)
    //         n = 6;
    //     if (row4 == 1)
    //         n = 9;
    //     setcol(3);
    //     if (row2 == 1)
    //         n = 2;
    //     if (row3 == 1)
    //         n = 5;
    //     if (row4 == 1)
    //         n = 8;
    //     setcol(4);
    //     if (row2 == 1)
    //         n = 1;
    //     if (row3 == 1)
    //         n = 4;
    //     if (row4 == 1)
    //         n = 7;
    // }
    // return n;
    return 0;
}

void show_dislpay(char strings[], int n)
{
    display.clearDisplay();
    display.setTextCursor(0, 0);
    display.printf(strings);
    if (n)
    {
        for (a = 0; a <= 2; a++) // Print panel
        {
            for (b = 0; b <= 2; b++)
                display.printf("%c", board[a][b]);
            display.printf("\n");
        }
    }
    display.display();
}

int check(char p[][3]) // Judge whether to win
{
    int i, x = -1;
    for (i = 0; i <= 2; i++)
    {
        if (p[i][0] + p[i][1] + p[i][2] == 264 ||
            p[0][i] + p[1][i] + p[2][i] == 264) // Row and column; 264 = 'X'+'X'+'X'
            x = 1;
        if (p[i][0] + p[i][1] + p[i][2] == 237 ||
            p[0][i] + p[1][i] + p[2][i] == 237) // Row and column; 237 = 'O'+'O'+'O'
            x = 0;
    }
    if (p[0][0] + p[1][1] + p[2][2] == 264 ||
        p[0][2] + p[1][1] + p[2][0] == 264) // Diagonal
        x = 1;
    if (p[0][0] + p[1][1] + p[2][2] == 237 ||
        p[0][2] + p[1][1] + p[2][0] == 237) // Diagonal
        x = 0;
    if (x == 1)
    {
        show_dislpay("You win!\n", 1);
        WIN++;
    }
    else if (x == 0)
    {
        show_dislpay("You lose!\n", 1);
        LOSE++;
    }
    return x;
}

int computer(char p[][3]) // Computer intelligence (lots of judgments)
{
    int i, j, x;
    if (p[1][1] == '.') // Preemption p[1]p[1]
    {
        p[1][1] = 'O';
        return 0;
    }
    for (i = 0; i <= 2; i++)
    {
        if (p[i][0] + p[i][1] + p[i][2] == 204 ||
            p[i][0] + p[i][1] + p[i][2] ==
                222) // 204 = 'O'+'O'+'.'; 222 = 'X'+'X'+'.'
            for (j = 0; j <= 2; j++)
                if (p[i][j] == '.') // Judge whether is '.'
                {
                    p[i][j] = 'O';
                    return 0;
                }
    }
    for (i = 0; i <= 2; i++)
    {
        if (p[0][i] + p[1][i] + p[2][i] == 204 ||
            p[0][i] + p[1][i] + p[2][i] == 222)
            for (j = 0; j <= 2; j++)
                if (p[j][i] == '.') // Judge whether is '.'
                {
                    p[j][i] = 'O';
                    return 0;
                }
    }
    if (p[0][0] + p[1][1] + p[2][2] == 204 ||
        p[0][0] + p[1][1] + p[2][2] == 222)
    {
        for (i = 0; i <= 2; i++)
            if (p[i][i] == '.') // Judge whether is '.'
            {
                p[i][i] = 'O';
                return 0;
            }
    }
    else if (p[0][2] + p[1][1] + p[2][0] == 204 ||
             p[0][2] + p[1][1] + p[2][0] == 222)
    {
        for (i = 0; i <= 2; i++)
            if (p[2 - i][i] == '.') // Judge whether is '.'
            {
                p[2 - i][i] = 'O';
                return 0;
            }
    }
    else
    {
        for (a = 2; a >= 0; a--)
        {
            for (b = 0; b <= 2; b++)
                if (board[a][b] == '.')
                {
                    board[a][b] = 'O';
                    return 0;
                }
        }
    }
    return 0;
}

void ymd(time_t x, int *y) // Calculate year,mounth,day
{
    int year = 1970, mounth, day, a;
    int common_year[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int leap_year[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    x += 8 * 60 * 60; // Plus jet lag (in second)
    day = x / (24 * 60 * 60) + 1;
    while (day > 366) // Find the year
    {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) // Judge whether leap year or not
            day -= 366;
        else
            day -= 365;
        year++;
    }
    for (mounth = 0; mounth < 12; mounth++) // Find the mounth
    {
        a = day;
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) // Judge whether leap year or not
            day -= leap_year[mounth];
        else
            day -= common_year[mounth];
        if (day <= 0)
        {
            X[0] = year;
            X[1] = mounth + 1; // mounth starts form 0, should plus 1
            X[2] = a;
        }
    }
}

void hms(time_t x, int *y)
{
    int hour, minute, second, a;
    second = x % 60; // Calculate second
    a = x / 60;
    minute = a % 60; // Calculate minute
    a /= 60;
    hour = (a + 8) % 24; // Calculate hour
    Y[0] = hour;         // Plus jet lag
    Y[1] = minute;
    Y[2] = second;
}

void draw_block(int shape, int form) // Draw the whole
{
    display.clearDisplay();
    for (int i = 1; i <= ROW; i++)
        for (int j = 1; j <= COL; j++)
            if (map[i][j])
            {
                display.drawPixel(5 * i - 5, 3 * j + 5, 1);
                display.drawPixel(5 * i - 4, 3 * j + 5, 1);
                display.drawPixel(5 * i - 3, 3 * j + 5, 1);
                display.drawPixel(5 * i - 2, 3 * j + 5, 1);
                display.drawPixel(5 * i - 1, 3 * j + 5, 1);

                display.drawPixel(5 * i - 5, 3 * j + 6, 1);
                display.drawPixel(5 * i - 4, 3 * j + 6, 1);
                display.drawPixel(5 * i - 3, 3 * j + 6, 1);
                display.drawPixel(5 * i - 2, 3 * j + 6, 1);
                display.drawPixel(5 * i - 1, 3 * j + 6, 1);

                display.drawPixel(5 * i - 5, 3 * j + 7, 1);
                display.drawPixel(5 * i - 4, 3 * j + 7, 1);
                display.drawPixel(5 * i - 3, 3 * j + 7, 1);
                display.drawPixel(5 * i - 2, 3 * j + 7, 1);
                display.drawPixel(5 * i - 1, 3 * j + 7, 1);
            }
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
        {
            if (block[shape][form].space[j][i] == 1 && j != 2)
            {
                display.drawPixel(5 * i + 5 + 108, 3 * j + 0, 1);
                display.drawPixel(5 * i + 4 + 108, 3 * j + 0, 1);
                display.drawPixel(5 * i + 3 + 108, 3 * j + 0, 1);
                display.drawPixel(5 * i + 2 + 108, 3 * j + 0, 1);
                display.drawPixel(5 * i + 1 + 108, 3 * j + 0, 1);

                display.drawPixel(5 * i + 5 + 108, 3 * j + 1, 1);
                display.drawPixel(5 * i + 4 + 108, 3 * j + 1, 1);
                display.drawPixel(5 * i + 3 + 108, 3 * j + 1, 1);
                display.drawPixel(5 * i + 2 + 108, 3 * j + 1, 1);
                display.drawPixel(5 * i + 1 + 108, 3 * j + 1, 1);

                display.drawPixel(5 * i + 5 + 108, 3 * j + 2, 1);
                display.drawPixel(5 * i + 4 + 108, 3 * j + 2, 1);
                display.drawPixel(5 * i + 3 + 108, 3 * j + 2, 1);
                display.drawPixel(5 * i + 2 + 108, 3 * j + 2, 1);
                display.drawPixel(5 * i + 1 + 108, 3 * j + 2, 1);
            }
            if (block[shape][form].space[j][i] == 1 && j == 2)
            {
                display.drawPixel(5 * i + 5 + 108, 3 * j + 0, 1);
                display.drawPixel(5 * i + 4 + 108, 3 * j + 0, 1);
                display.drawPixel(5 * i + 3 + 108, 3 * j + 0, 1);
                display.drawPixel(5 * i + 2 + 108, 3 * j + 0, 1);
                display.drawPixel(5 * i + 1 + 108, 3 * j + 0, 1);

                display.drawPixel(5 * i + 5 + 108, 3 * j + 1, 1);
                display.drawPixel(5 * i + 4 + 108, 3 * j + 1, 1);
                display.drawPixel(5 * i + 3 + 108, 3 * j + 1, 1);
                display.drawPixel(5 * i + 2 + 108, 3 * j + 1, 1);
                display.drawPixel(5 * i + 1 + 108, 3 * j + 1, 1);
            }
        }
    display.display();
}

void InitBlock()
{
    //��O����
    block[0][0].space[0][0] = 1;
    block[0][0].space[0][1] = 1;
    block[0][0].space[1][0] = 1;
    block[0][0].space[1][1] = 1;

    //��I����
    block[1][0].space[1][0] = 1;
    block[1][0].space[1][1] = 1;
    block[1][0].space[1][2] = 1;

    //��T����
    block[2][0].space[1][0] = 1;
    block[2][0].space[1][1] = 1;
    block[2][0].space[1][2] = 1;
    block[2][0].space[2][1] = 1;

    //��L����
    block[3][0].space[1][0] = 1;
    block[3][0].space[1][1] = 1;
    block[3][0].space[1][2] = 1;
    block[3][0].space[2][0] = 1;

    //��J����
    block[4][0].space[0][1] = 1;
    block[4][0].space[1][1] = 1;
    block[4][0].space[2][1] = 1;
    block[4][0].space[2][0] = 1;

    //��Z����
    block[5][0].space[0][0] = 1;
    block[5][0].space[0][1] = 1;
    block[5][0].space[1][1] = 1;
    block[5][0].space[1][2] = 1;

    //��S����
    block[6][0].space[0][0] = 1;
    block[6][0].space[1][0] = 1;
    block[6][0].space[1][1] = 1;
    block[6][0].space[2][1] = 1;

    int temp[4][4];
    for (int shape = 0; shape < 7; shape++) // 7����״
    {
        for (int form = 0; form < 3; form++) // 4����̬���Ѿ�����һ�֣�����ÿ����������3�֣�
        {
            // ��ȡ��form����̬
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    temp[i][j] = block[shape][form].space[i][j];
            // ����form����̬˳ʱ����ת���õ���form + 1����̬
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    block[shape][form + 1].space[i][j] = temp[2 - j][i];
        }
    }
}

int judge(int shape, int form, int x, int y)
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) //����������µ�λ�ñ������Ѿ��з����ˣ��򲻺Ϸ�
            if (block[shape][form].space[i][j] == 1 && map[x - 1 + j][y - 1 + i] == 1)
                return 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) //����������µ�λ�ñ������Ѿ��з����ˣ��򲻺Ϸ�
            if (block[shape][form].space[i][j] == 1 && ((x - 1 + j) == 0 || (y - 1 + i) == 0 || (y - 1 + i) == 9))
                return 0;
    return 1; //�Ϸ�
}

//�жϵ÷������
int JudgeFunc()
{
    //�ж��Ƿ�÷�
    for (int i = 1; i <= ROW; i++)
    {
        int sum = 0; //��¼��i�еķ������
        for (int j = 1; j <= COL; j++)
            sum += map[i][j]; //ͳ�Ƶ�i�еķ������
        if (sum == 0)         //����û�з��飬�������ж����ϵĲ�Σ������ټ����ж��Ƿ�÷֣�
            break;            //����ѭ��
        if (sum == COL)       //����ȫ�Ƿ��飬�ɵ÷�
        {
            grade += 1;                    //��һ�м�1��
            for (int j = 1; j <= COL; j++) //����÷��еķ�����Ϣ
                map[i][j] = 0;             //��λ�õ÷ֺ���������Ϊ�޷���
            //�ѱ���������������������Ųһ��
            for (int m = i; m <= COL; m++)
            {
                sum = 0; //��¼��һ�еķ������
                for (int n = 1; n <= COL; n++)
                {
                    sum += map[m + 1][n];      //ͳ����һ�еķ������
                    map[m][n] = map[m + 1][n]; //����һ�з���ı�ʶ�Ƶ���һ��
                }
            }
        }
    }
    //�ж���Ϸ�Ƿ����
    for (int j = 1; j < COL - 3; j++)
        if (map[24][j] == 1) //�����з�����ڣ��Ե�1��Ϊ���㣬���ǵ�0�У�
            return 2;
    return 0; //�жϽ����������ٵ��øú��������ж�
}