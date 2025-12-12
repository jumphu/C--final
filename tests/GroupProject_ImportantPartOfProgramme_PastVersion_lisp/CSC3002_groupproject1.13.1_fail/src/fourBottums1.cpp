#include "allbottums.h"
#include <stack>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
using std::stack;

stack<IMAGE> scrS; 

int btnx, btnW, BtnH;
int colrBtnY, gravBtnY, fricBtnY, spdBtnY;

char colorN[20]="Red";
float gravVal=9.8f;
float fricVal=0.1f;
float speedVal=1.0f;

static int inside(int mx,int my,int x,int y,int w,int h) {
    if(mx>=x && mx<=x+w && my>=y && my<=y+h) return 1;
    return 0;
}

void saveScr() {
    IMAGE img;
    getimage(&img,0,0,getwidth(),getheight());
    scrS.push(img);
}

void restoreScr() {
    if(scrS.empty()) return;
    putimage(0,0,&scrS.top());
    scrS.pop();
}

void initBtns(int w,int h) {
    btnW=120; BtnH=40;
    btnx = w - btnW - 30;
    int topY = 160;
    
    colrBtnY = topY;
    gravBtnY = topY + 52;
    fricBtnY = topY + 52*2;
    spdBtnY  = topY + 52*3;
}

void drawBtns() {
    setfillcolor(RGB(210,210,210));
    solidrectangle(btnx,colrBtnY,btnx+btnW,colrBtnY+BtnH);
    solidrectangle(btnx,gravBtnY,btnx+btnW,gravBtnY+BtnH);
    solidrectangle(btnx,fricBtnY,btnx+btnW,fricBtnY+BtnH);
    solidrectangle(btnx,spdBtnY,btnx+btnW,spdBtnY+BtnH);

    setbkmode(TRANSPARENT);
    settextcolor(BLACK);
    settextstyle(16, 0, "Consolas");
    outtextxy(btnx+12,colrBtnY+12,"Color");
    outtextxy(btnx+12,gravBtnY+12,"Gravity");
    outtextxy(btnx+12,fricBtnY+12,"Friction");
    outtextxy(btnx+12,spdBtnY+12,"Speed");
}

void colorSelect() {
    saveScr();
    const char* colors[4]={"Red","Blue","Yellow","Orange"};
    int cx=getwidth()/2-80;
    int cy=140;
    
    bool done=false;
    while(!done) {
        setfillcolor(WHITE);
        solidrectangle(cx-10, cy-50, cx+160, cy+260);
        
        setbkmode(TRANSPARENT);
        settextcolor(BLACK);
        outtextxy(cx,cy-40,"Pick Color (ESC)");
    
        for(int i=0;i<4;i++) {
            if(strcmp(colors[i],"Red")==0) setfillcolor(RED);
            else if(strcmp(colors[i],"Blue")==0) setfillcolor(BLUE);
            else if(strcmp(colors[i],"Yellow")==0) setfillcolor(YELLOW);
            else setfillcolor(RGB(255,165,0));

            solidrectangle(cx,cy+i*48,cx+150,cy+i*48+42);
            setbkmode(TRANSPARENT);
            outtextxy(cx+10,cy+i*48+10,colors[i]);
        }
    
        setfillcolor(RGB(230,230,230));
        solidrectangle(cx,cy+210,cx+150,cy+250);
        outtextxy(cx+40,cy+222,"Cancel");

        FlushBatchDraw(); 

        ExMessage m;
        // 【修正】去掉参数，接收所有消息（包括字符）
        while(peekmessage(&m)) {
            if(m.message==WM_KEYDOWN && m.vkcode==VK_ESCAPE) {
                done=true; 
                break;
            }
            if(m.message==WM_LBUTTONDOWN) {
                for(int i=0;i<4;i++) {
                    if(inside(m.x,m.y,cx,cy+i*48,150,42)) {
                        strcpy(colorN,colors[i]);
                        done=true;
                        break;
                    }
                }
                if(inside(m.x,m.y,cx,cy+210,150,40)) {
                    done=true;
                    break;
                }
            }
        }
        Sleep(16); 
    }
    restoreScr();
    FlushBatchDraw(); 
}

void numInput(const char* t, float* val) {
    saveScr();
    char buf[40]="";
    int len=0;
    const int cx = getwidth() / 2 - 120;
    const int cy = getheight() / 2 - 60;
    
    bool finish=false;
    
    ExMessage junk;
    while(peekmessage(&junk)); 

    while(!finish) {
        setfillcolor(WHITE);
        solidrectangle(cx,cy-60,cx+240,cy+120); 
        
        setbkmode(TRANSPARENT);
        settextcolor(BLACK);
        outtextxy(cx+10,cy-50,t);
        outtextxy(cx+10,cy-25,"(Enter=OK, ESC=Cancel)");
        
        setfillcolor(RGB(240,240,240));
        solidrectangle(cx+10,cy,cx+230,cy+60);

        char tmp[50]; 
        sprintf(tmp, "%s_", buf);
        outtextxy(cx+20,cy+20,tmp);

        setfillcolor(RGB(190,240,190));
        solidrectangle(cx+30,cy+70,cx+100,cy+110);
        outtextxy(cx+45,cy+82,"OK");

        setfillcolor(RGB(240,190,190));
        solidrectangle(cx+130,cy+70,cx+200,cy+110);
        outtextxy(cx+140,cy+82,"Cancel");

        FlushBatchDraw();

        ExMessage m;
        // 【关键修正】去掉参数，接收所有消息！
        if(peekmessage(&m)) {
            if(m.message == WM_KEYDOWN) {
                if(m.vkcode == VK_RETURN) {
                    if(len > 0) *val = (float)atof(buf);
                    finish = true;
                }
                else if(m.vkcode == VK_ESCAPE) {
                    finish = true;
                }
            }
            // 现在的过滤器能接收 WM_CHAR 了
            else if(m.message == WM_CHAR) {
                if(m.ch == 8) { // Backspace
                    if(len > 0) {
                        len--;
                        buf[len] = '\0';
                    }
                }
                // 允许数字、小数点、负号
                else if((m.ch >= '0' && m.ch <= '9') || m.ch == '.' || m.ch == '-') {
                    if(len < 38) {
                        buf[len] = m.ch;
                        len++;
                        buf[len] = '\0';
                    }
                }
            }
            else if(m.message == WM_LBUTTONDOWN) {
                if(inside(m.x, m.y, cx+30, cy+70, 70, 40)) {
                    if(len > 0) *val = (float)atof(buf);
                    finish = true; 
                }
                else if(inside(m.x, m.y, cx+130, cy+70, 70, 40)) {
                    finish = true;
                }
            }
        }
        Sleep(16);
    }
    restoreScr();
    FlushBatchDraw();
}

void updBtns(int mx, int my, bool isDown) {
    if (!isDown) return;

    if(inside(mx,my,btnx,colrBtnY,btnW,BtnH)) {
        colorSelect();
    }
    else if(inside(mx,my,btnx,gravBtnY,btnW,BtnH)) {
        numInput("Gravity",&gravVal);
    }
    else if(inside(mx,my,btnx,fricBtnY,btnW,BtnH)) {
        numInput("Friction",&fricVal);
    }
    else if(inside(mx,my,btnx,spdBtnY,btnW,BtnH)) {
        numInput("Speed",&speedVal);
    }
}

char* getColor() { return colorN; }
float getGrav() { return gravVal; }
float getFric() { return fricVal; }
float getSpeedVal() { return speedVal; }