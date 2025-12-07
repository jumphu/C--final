#include "allbuttons.h"
#include <stack>
#include <cstring>
#include <cstdlib>
using std::stack;

//栈：用来临时保存和恢复屏幕图像
stack<IMAGE> scrS; 

int btnx, btnW, BtnH;
int colrBtnY, gravBtnY, fricBtnY, spdBtnY;

//默认值
char colorN[20]="Red";
float gravVal=9.8f;
float fricVal=0.1f;
float speedVal=1.0f;


int inside(int mx,int my,int x,int y,int w,int h) {
    if(mx>=x && mx<=x+w && my>=y && my<=y+h) return 1;
    return 0;
}

//把屏幕保存到栈里面
void saveScr() {
    IMAGE img;
    getimage(&img,0,0,getwidth(),getheight());
    scrS.push(img);
}

//从栈里面恢复屏幕
void restoreScr() {
    if(scrS.empty()) return;
    putimage(0,0,&scrS.top());
    scrS.pop();
}

//初始化
void initBtns(int w,int h) {
    btnW=120; BtnH=40;
    btnx=w-btnW-20;
    int baseY=h-(BtnH*4+20*3)-20;
    colrBtnY=baseY;
    gravBtnY=baseY+52;
    fricBtnY=baseY+52*2;
    spdBtnY=baseY+52*3;
}   //4个按钮竖直排列在页面右下角，已将按钮大小修改为适合的大小

//把按钮画出来
void drawBtns() {
    setfillcolor(RGB(210,210,210));
    solidrectangle(btnx,colrBtnY,btnx+btnW,colrBtnY+BtnH);
    solidrectangle(btnx,gravBtnY,btnx+btnW,gravBtnY+BtnH);
    solidrectangle(btnx,fricBtnY,btnx+btnW,fricBtnY+BtnH);
    solidrectangle(btnx,spdBtnY,btnx+btnW,spdBtnY+BtnH);

    outtextxy(btnx+12,colrBtnY+12,"Color");
    outtextxy(btnx+12,gravBtnY+12,"Gravity");
    outtextxy(btnx+12,fricBtnY+12,"Friction");
    outtextxy(btnx+12,spdBtnY+12,"Speed");
}

//用来选择颜色
void colorSelect() {
    saveScr();
    const char* colors[4]={"Red","Blue","Yellow","Orange"};
    int cx=getwidth()/2-80;
    int cy=140;
    cleardevice();
    outtextxy(cx,cy-40,"Pick Color (ESC cancel)");
    
    //本来是一个个单独绘制，重复了，后来优化成循环来绘制
    for(int i=0;i<4;i++) {
        if(strcmp(colors[i],"Red")==0) {
            setfillcolor(RED);
        }
        else if(strcmp(colors[i],"Blue")==0) {
            setfillcolor(BLUE);
        }
        else if(strcmp(colors[i],"Yellow")==0) {
            setfillcolor(YELLOW);
        }
        else {
            setfillcolor(RGB(255,165,0));
        }

        solidrectangle(cx,cy+i*48,cx+150,cy+i*48+42);
        setcolor(BLACK);
        outtextxy(cx+10,cy+i*48+10,colors[i]);
    }
    
    //绘制取消按钮，放在循环外面，避免重复绘制
    setfillcolor(RGB(230,230,230));
    solidrectangle(cx,cy+210,cx+150,cy+250);
    outtextxy(cx+40,cy+222,"Cancel");

    bool done=false;
    while(!done) {
        ExMessage m;
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
}

//用来输入数字
void numInput(const char* t,float* val) {
    saveScr();
    char buf[40]="";
    //加一个缓冲区，储存
    int len=0;  //输入长度计数，初始化为0
    const int cx = 280;   
    const int cy = 260;
    
    bool finish=false;
    while(!finish) {
        cleardevice();
        outtextxy(cx,cy-50,t);
        outtextxy(cx,cy-25,"(Enter=OK,ESC=Cancel)");
        setfillcolor(WHITE);
        solidrectangle(cx,cy,cx+240,cy+60);

        char tmp[40]; 
        sprintf(tmp,"%s",buf);
        //把 buf 中的字符串复制到 tmp 中
        outtextxy(cx+10,cy+20,tmp);

        setfillcolor(RGB(190,240,190));
        solidrectangle(cx,cy+70,cx+100,cy+110);
        outtextxy(cx+30,cy+82,"OK");

        setfillcolor(RGB(240,190,190));
        solidrectangle(cx+130,cy+70,cx+230,cy+110);
        outtextxy(cx+150,cy+82,"Cancel");

        ExMessage m;
        if(peekmessage(&m,EM_MOUSE|EM_KEY)) {
            if(m.message==WM_KEYDOWN) {
                if(m.vkcode==VK_RETURN && len>0) {
                    *val=atof(buf);
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
                    if(len<39){
                        buf[len]=m.vkcode;
                        len++;
                        buf[len]='\0';
                    }
                }
            }
            else if(m.message==WM_LBUTTONDOWN) {
                if(inside(m.x,m.y,cx,cy+70,100,40) && len>0) {
                    *val=atof(buf);
                    finish=true;  //点击了OK键
                }
                else if(inside(m.x,m.y,cx+130,cy+70,100,40)) {
                    finish=true;
                }
            }
        }
    }
    restoreScr();
    drawBtns();
}

//更新
void updBtns() {
    ExMessage m;
    while(peekmessage(&m,EM_MOUSE)) {
        if(m.message==WM_LBUTTONDOWN) {
            if(inside(m.x,m.y,btnx,colrBtnY,btnW,BtnH)) {
                colorSelect();
            }
            else if(inside(m.x,m.y,btnx,gravBtnY,btnW,BtnH)) {
                numInput("Gravity",&gravVal);
            }
            else if(inside(m.x,m.y,btnx,fricBtnY,btnW,BtnH)) {
                numInput("Friction",&fricVal);
            }
            else if(inside(m.x,m.y,btnx,spdBtnY,btnW,BtnH)) {
                numInput("Speed",&speedVal);
            }
        }
    }
}

//返回输入的值
char* getColor() {
    return colorN;
}

float getGrav() {
    return gravVal;
}

float getFric() {
    return fricVal;
}

float getSpeedVal() {
    return speedVal;
}
