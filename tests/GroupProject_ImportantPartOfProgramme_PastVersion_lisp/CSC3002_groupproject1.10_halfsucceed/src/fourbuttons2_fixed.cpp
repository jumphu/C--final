#include "allbottums.h"
#include <stack>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
using std::stack;

stack<IMAGE> scrS2;

int btnW2 = 120, BtnH2 = 40;
int col1x2, col2x2, baseY2;
int radius1BtnY2, mass1BtnY2, speed1BtnY2, color1BtnY2;
int radius2BtnY2, mass2BtnY2, speed2BtnY2, color2BtnY2;

float radius1Val = 1.0f, mass1Val = 1.0f, speed1Val = 1.0f;
char color1N[20] = "Red";

float radius2Val = 1.5f, mass2Val = 2.0f, speed2Val = 1.0f;
char color2N[20] = "Blue";

static int inside2(int mx, int my, int x, int y, int w, int h) {
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
}

void saveScr2() {
    IMAGE img;
    getimage(&img, 0, 0, getwidth(), getheight());
    scrS2.push(img);
}

void restoreScr2() {
    if (scrS2.empty()) return;
    putimage(0, 0, &scrS2.top());
    scrS2.pop();
}

void initBtns2(int w, int h) {
    // 布局已根据要求调整到右上
    col2x2 = w - btnW2 - 30;        // 最右列
    col1x2 = w - btnW2 * 2 - 50;    // 次右列
    
    int topY = 160;

    radius1BtnY2 = topY;
    mass1BtnY2   = radius1BtnY2 + 52;
    speed1BtnY2  = mass1BtnY2 + 52;
    color1BtnY2  = speed1BtnY2 + 52;

    radius2BtnY2 = topY;
    mass2BtnY2   = radius2BtnY2 + 52;
    speed2BtnY2  = mass2BtnY2 + 52;
    color2BtnY2  = speed2BtnY2 + 52;
}

void drawBtns2() {
    setfillcolor(RGB(210, 210, 210));
    setbkmode(TRANSPARENT);
    settextcolor(BLACK);
    settextstyle(16, 0, "Consolas");

    // Col 1
    solidrectangle(col1x2, radius1BtnY2, col1x2 + btnW2, radius1BtnY2 + BtnH2);
    outtextxy(col1x2 + 12, radius1BtnY2 + 12, "Radius1");
    
    solidrectangle(col1x2, mass1BtnY2, col1x2 + btnW2, mass1BtnY2 + BtnH2);
    outtextxy(col1x2 + 12, mass1BtnY2 + 12, "Mass1");
    
    solidrectangle(col1x2, speed1BtnY2, col1x2 + btnW2, speed1BtnY2 + BtnH2);
    outtextxy(col1x2 + 12, speed1BtnY2 + 12, "Speed1");
    
    solidrectangle(col1x2, color1BtnY2, col1x2 + btnW2, color1BtnY2 + BtnH2);
    outtextxy(col1x2 + 12, color1BtnY2 + 12, "Color1");

    // Col 2
    solidrectangle(col2x2, radius2BtnY2, col2x2 + btnW2, radius2BtnY2 + BtnH2);
    outtextxy(col2x2 + 12, radius2BtnY2 + 12, "Radius2");
    
    solidrectangle(col2x2, mass2BtnY2, col2x2 + btnW2, mass2BtnY2 + BtnH2);
    outtextxy(col2x2 + 12, mass2BtnY2 + 12, "Mass2");
    
    solidrectangle(col2x2, speed2BtnY2, col2x2 + btnW2, speed2BtnY2 + BtnH2);
    outtextxy(col2x2 + 12, speed2BtnY2 + 12, "Speed2");
    
    solidrectangle(col2x2, color2BtnY2, col2x2 + btnW2, color2BtnY2 + BtnH2);
    outtextxy(col2x2 + 12, color2BtnY2 + 12, "Color2");
}

void colorSelect2(int idx) {
    saveScr2();
    const char* colors[4] = { "Red","Blue","Yellow","Orange" };
    int cx = getwidth() / 2 - 80;
    int cy = 140;

    bool done = false;
    while (!done) {
        setfillcolor(WHITE);
        solidrectangle(cx-10, cy-50, cx+160, cy+260);
        
        setbkmode(TRANSPARENT);
        settextcolor(BLACK);
        outtextxy(cx, cy - 40, "Pick Color (ESC)");

        for (int i = 0; i < 4; i++) {
            if (strcmp(colors[i], "Red") == 0) setfillcolor(RED);
            else if (strcmp(colors[i], "Blue") == 0) setfillcolor(BLUE);
            else if (strcmp(colors[i], "Yellow") == 0) setfillcolor(YELLOW);
            else setfillcolor(RGB(255, 165, 0));
            
            solidrectangle(cx, cy + i * 48, cx + 150, cy + i * 48 + 42);
            outtextxy(cx + 10, cy + i * 48 + 10, colors[i]);
        }

        setfillcolor(RGB(230, 230, 230));
        solidrectangle(cx, cy + 210, cx + 150, cy + 250);
        outtextxy(cx + 40, cy + 222, "Cancel");

        FlushBatchDraw();

        ExMessage m;
        while(peekmessage(&m, EM_MOUSE | EM_KEY)) {
            if (m.message == WM_KEYDOWN && m.vkcode == VK_ESCAPE) {
                done = true;
                break;
            }
            if (m.message == WM_LBUTTONDOWN) {
                for (int i = 0; i < 4; i++) {
                    if (inside2(m.x, m.y, cx, cy + i * 48, 150, 42)) {
                        if (idx == 1) strcpy(color1N, colors[i]);
                        else strcpy(color2N, colors[i]);
                        done = true;
                        break;
                    }
                }
                if (inside2(m.x, m.y, cx, cy + 210, 150, 40)) {
                    done = true;
                    break;
                }
            }
        }
        Sleep(16);
    }
    restoreScr2();
    FlushBatchDraw();
}

void numInput2(const char* t, float* val) {
    saveScr2();
    char buf[40] = "";
    int len = 0;
    // 弹窗位置居中
    int cx = getwidth() / 2 - 120;
    int cy = getheight() / 2 - 60;

    bool finish = false;
    
    ExMessage junk;
    while(peekmessage(&junk)); 

    while (!finish) {
        setfillcolor(WHITE);
        solidrectangle(cx,cy-60,cx+240,cy+120); 
        
        setbkmode(TRANSPARENT);
        settextcolor(BLACK);
        outtextxy(cx+10, cy - 50, t);
        outtextxy(cx+10, cy - 25, "(Enter=OK, ESC=Cancel)");
        
        setfillcolor(RGB(240,240,240));
        solidrectangle(cx+10, cy, cx+230, cy+60);

        char tmp[50];
        sprintf(tmp, "%s_", buf);
        outtextxy(cx+20, cy + 20, tmp);

        setfillcolor(RGB(190, 240, 190));
        solidrectangle(cx+30, cy + 70, cx + 100, cy + 110);
        outtextxy(cx + 45, cy + 82, "OK");

        setfillcolor(RGB(240, 190, 190));
        solidrectangle(cx + 130, cy + 70, cx + 200, cy + 110);
        outtextxy(cx + 140, cy + 82, "Cancel");

        FlushBatchDraw();

        ExMessage m;
        if(peekmessage(&m, EM_MOUSE | EM_KEY)) {
            // 1. 控制键
            if (m.message == WM_KEYDOWN) {
                if (m.vkcode == VK_RETURN) {
                    if (len > 0) *val = (float)atof(buf);
                    finish = true;
                }
                else if (m.vkcode == VK_ESCAPE) {
                    finish = true;
                }
            }
            // 2. 字符输入 (关键修正)
            else if (m.message == WM_CHAR) {
                if(m.ch == 8) { // Backspace
                    if (len > 0) { len--; buf[len] = '\0'; }
                }
                else if ((m.ch >= '0' && m.ch <= '9') || m.ch == '.' || m.ch == '-') {
                    if (len < 38) {
                        buf[len] = m.ch;
                        len++;
                        buf[len] = '\0';
                    }
                }
            }
            // 3. 鼠标
            else if (m.message == WM_LBUTTONDOWN) {
                if (inside2(m.x, m.y, cx+30, cy + 70, 70, 40)) {
                    if (len > 0) *val = (float)atof(buf);
                    finish = true;
                }
                else if (inside2(m.x, m.y, cx + 130, cy + 70, 70, 40)) {
                    finish = true;
                }
            }
        }
        Sleep(16);
    }
    restoreScr2();
    FlushBatchDraw();
}

void updBtns2(int mx, int my, bool isDown) {
    if (!isDown) return;

    if (inside2(mx, my, col1x2, radius1BtnY2, btnW2, BtnH2)) {
        numInput2("Radius1", &radius1Val);
    }
    else if (inside2(mx, my, col1x2, mass1BtnY2, btnW2, BtnH2)) {
        numInput2("Mass1", &mass1Val);
    }
    else if (inside2(mx, my, col1x2, speed1BtnY2, btnW2, BtnH2)) {
        numInput2("Speed1", &speed1Val);
    }
    else if (inside2(mx, my, col1x2, color1BtnY2, btnW2, BtnH2)) {
        colorSelect2(1);
    }
    else if (inside2(mx, my, col2x2, radius2BtnY2, btnW2, BtnH2)) {
        numInput2("Radius2", &radius2Val);
    }
    else if (inside2(mx, my, col2x2, mass2BtnY2, btnW2, BtnH2)) {
        numInput2("Mass2", &mass2Val);
    }
    else if (inside2(mx, my, col2x2, speed2BtnY2, btnW2, BtnH2)) {
        numInput2("Speed2", &speed2Val);
    }
    else if (inside2(mx, my, col2x2, color2BtnY2, btnW2, BtnH2)) {
        colorSelect2(2);
    }
}

// Getters
char* getColor1() { return color1N; }
float getRadius1() { return radius1Val; }
float getMass1() { return mass1Val; }
float getSpeed1() { return speed1Val; }

char* getColor2() { return color2N; }
float getRadius2() { return radius2Val; }
float getMass2() { return mass2Val; }
float getSpeed2() { return speed2Val; }