#include "allbottums.h"
#include <stack>
#include <cstring>
#include <cstdlib>
#include <stdio.h> // 为了 sprintf
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
    btnx=w-btnW-20;
    int baseY=h-(BtnH*4+20*3)-20;
    colrBtnY=baseY;
    gravBtnY=baseY+52;
    fricBtnY=baseY+52*2;
    spdBtnY=baseY+52*3;
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
    
    // 注意：这里只是为了清屏绘制背景，实际因为有 saveScr，背景会被覆盖
    // 我们需要在循环里持续绘制
    
    bool done=false;
    while(!done) {
        // 1. 每次循环重绘界面
        cleardevice(); 
        restoreScr(); // 先画背景（这可能会闪烁，但这是最简单的修复逻辑）
        // 实际上 saveScr保存了截图，我们直接在截图上画框即可
        // 简单做法：不清屏，直接覆盖画
        
        // 绘制面板背景
        setfillcolor(WHITE);
        solidrectangle(cx-10, cy-50, cx+160, cy+260);
        
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

        // 【关键修复】必须刷新缓冲区，否则看见的是卡死的画面！
        FlushBatchDraw(); 

        ExMessage m;
        // 这里需要持续获取消息
        if(peekmessage(&m,EM_MOUSE|EM_KEY)) {
            if(m.message==WM_KEYDOWN && m.vkcode==VK_ESCAPE) {
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
                    break;
                }
            }
        }
        Sleep(16); 
    }
    restoreScr();
    // 退出后刷新一下，恢复原样
    FlushBatchDraw(); 
}

void numInput(const char* t,float* val) {
    saveScr();
    char buf[40]="";
    int len=0;
    const int cx = 280;   
    const int cy = 260;
    
    bool finish=false;
    while(!finish) {
        // 绘制界面
        setfillcolor(WHITE);
        solidrectangle(cx,cy-60,cx+240,cy+120); // 背景板
        
        setbkmode(TRANSPARENT);
        settextcolor(BLACK);
        outtextxy(cx+10,cy-50,t);
        outtextxy(cx+10,cy-25,"(Enter=OK,ESC=Cancel)");
        
        setfillcolor(RGB(240,240,240));
        solidrectangle(cx+10,cy,cx+230,cy+60); // 输入框

        char tmp[40]; 
        sprintf(tmp,"%s",buf);
        outtextxy(cx+20,cy+20,tmp);

        setfillcolor(RGB(190,240,190));
        solidrectangle(cx+30,cy+70,cx+100,cy+110);
        outtextxy(cx+45,cy+82,"OK");

        setfillcolor(RGB(240,190,190));
        solidrectangle(cx+130,cy+70,cx+200,cy+110);
        outtextxy(cx+140,cy+82,"Cancel");

        // 【关键修复】刷新缓冲区！
        FlushBatchDraw();

        ExMessage m;
        if(peekmessage(&m,EM_MOUSE|EM_KEY)) {
            if(m.message==WM_KEYDOWN) {
                if(m.vkcode==VK_RETURN && len>0) {
                    *val=(float)atof(buf);
                    finish=true;
                }
                else if(m.vkcode==VK_ESCAPE) {
                    finish=true;
                }
                else if(m.vkcode==VK_BACK && len>0) {
                    len--;
                    buf[len]='\0';
                }
                else if((m.vkcode>='0' && m.vkcode<='9') || m.vkcode=='.') {
                    if(len<38){
                        buf[len]=m.vkcode;
                        len++;
                        buf[len]='\0';
                    }
                }
            }
            else if(m.message==WM_LBUTTONDOWN) {
                if(inside(m.x,m.y,cx+30,cy+70,70,40) && len>0) { // OK
                    *val=(float)atof(buf);
                    finish=true; 
                }
                else if(inside(m.x,m.y,cx+130,cy+70,70,40)) { // Cancel
                    finish=true;
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

// Getters
char* getColor() { return colorN; }
float getGrav() { return gravVal; }
float getFric() { return fricVal; }
float getSpeedVal() { return speedVal; }