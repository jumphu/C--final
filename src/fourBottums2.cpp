#include "allbottums.h"
#include <stack>
#include <cstring>
#include <cstdlib>
using std::stack;

//栈：用来临时保存和恢复屏幕图像
stack<IMAGE> scrS2;

int btnW2=140, BtnH2=40;
int col1x2, col2x2, baseY2;
int radius1BtnY2, mass1BtnY2, speed1BtnY2, color1BtnY2;
int radius2BtnY2, mass2BtnY2, speed2BtnY2, color2BtnY2;

//默认值
float radius1Val=10.0f, mass1Val=1.0f, speed1Val=1.0f;
char color1N[20]="Red";

float radius2Val=20.0f, mass2Val=2.0f, speed2Val=1.0f;
char color2N[20]="Blue";


int inside2(int mx,int my,int x,int y,int w,int h) {
    if(mx>=x && mx<=x+w && my>=y && my<=y+h) {
        return 1;
    }
    return 0;
}

//把屏幕保存到栈里面
void saveScr2() {
    IMAGE img;
    getimage(&img,0,0,getwidth(),getheight());
    scrS2.push(img);
}

//从栈里面恢复屏幕
void restoreScr2() {
    if(scrS2.empty()) {
        return;
    }
    putimage(0,0,&scrS2.top());
    scrS2.pop();
}

//初始化
void initBtns2(int w,int h) {
    baseY2=h-290;
    col1x2=w-btnW2*2-30; 
    col2x2=w-btnW2-15;   

    radius1BtnY2=baseY2;
    mass1BtnY2=baseY2+55;
    speed1BtnY2=baseY2+110;
    color1BtnY2=baseY2+170;

    radius2BtnY2=baseY2;
    mass2BtnY2=baseY2+55;
    speed2BtnY2=baseY2+110;
    color2BtnY2=baseY2+170;
}

//把按钮画出来
void drawBtns2() {
    setfillcolor(RGB(210,210,210));

    solidrectangle(col1x2,radius1BtnY2,col1x2+btnW2,radius1BtnY2+BtnH2);
    solidrectangle(col1x2,mass1BtnY2,col1x2+btnW2,mass1BtnY2+BtnH2);
    solidrectangle(col1x2,speed1BtnY2,col1x2+btnW2,speed1BtnY2+BtnH2);
    solidrectangle(col1x2,color1BtnY2,col1x2+btnW2,color1BtnY2+BtnH2);

    outtextxy(col1x2+12,radius1BtnY2+12,"Radius1");
    outtextxy(col1x2+12,mass1BtnY2+12,"Mass1");
    outtextxy(col1x2+12,speed1BtnY2+12,"Speed1");
    outtextxy(col1x2+12,color1BtnY2+12,"Color1");

    solidrectangle(col2x2,radius2BtnY2,col2x2+btnW2,radius2BtnY2+BtnH2);
    solidrectangle(col2x2,mass2BtnY2,col2x2+btnW2,mass2BtnY2+BtnH2);
    solidrectangle(col2x2,speed2BtnY2,col2x2+btnW2,speed2BtnY2+BtnH2);
    solidrectangle(col2x2,color2BtnY2,col2x2+btnW2,color2BtnY2+BtnH2);

    outtextxy(col2x2+12,radius2BtnY2+12,"Radius2");
    outtextxy(col2x2+12,mass2BtnY2+12,"Mass2");
    outtextxy(col2x2+12,speed2BtnY2+12,"Speed2");
    outtextxy(col2x2+12,color2BtnY2+12,"Color2");
}

//用来选择颜色
void colorSelect2(int idx) {
    saveScr2();
    const char* colors[4]={"Red","Blue","Yellow","Orange"};
    int cx=getwidth()/2-80;
    int cy=140;
    cleardevice();
    outtextxy(cx,cy-40,"Pick Color (ESC cancel)");

    //本来是一个个单独绘制，重复了，后来优化成循环绘制
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
                for(int i=0;i<4;i++){
                    if(inside2(m.x,m.y,cx,cy+i*48,150,42)) {
                        if(idx==1) strcpy(color1N,colors[i]);
                        else {
                            strcpy(color2N,colors[i]);
                        }
                        done=true;
                        break;
                    }
                }
                if(inside2(m.x,m.y,cx,cy+210,150,40)) {
                    break;
                }
            }
        }
        Sleep(16);
    }
    restoreScr2();
}

//用来输入数字
void numInput2(const char* t,float* val) {
    saveScr2();
    char buf[40]="";
    //加一个缓冲区，储存
    int len=0;
    int cx = 280;
    int cy = 260;
    
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
                    if(len<39) {
                        buf[len]=m.vkcode;
                        len++;
                        buf[len]='\0';
                    }
                }
            } 
            else if(m.message==WM_LBUTTONDOWN) {
                if(inside2(m.x,m.y,cx,cy+70,100,40) && len>0) {
                    *val=atof(buf);
                    finish=true;
                }
                else if(inside2(m.x,m.y,cx+130,cy+70,100,40)) {
                    finish=true;
                }
            }
        }
    }
    restoreScr2();
    drawBtns2();
}

//更新
void updBtns2() {
    ExMessage m;
    while(peekmessage(&m,EM_MOUSE)) {
        if(m.message==WM_LBUTTONDOWN) {
    
            if(inside2(m.x,m.y,col1x2,radius1BtnY2,btnW2,BtnH2)) {
                numInput2("Radius1",&radius1Val);
            }
            else if(inside2(m.x,m.y,col1x2,mass1BtnY2,btnW2,BtnH2)) {
                numInput2("Mass1",&mass1Val);
            }
            else if(inside2(m.x,m.y,col1x2,speed1BtnY2,btnW2,BtnH2)) {
                numInput2("Speed1",&speed1Val);
            }
            else if(inside2(m.x,m.y,col1x2,color1BtnY2,btnW2,BtnH2)) {
                colorSelect2(1);
            }
            else if(inside2(m.x,m.y,col2x2,radius2BtnY2,btnW2,BtnH2)) {
                numInput2("Radius2",&radius2Val);
            }
            else if(inside2(m.x,m.y,col2x2,mass2BtnY2,btnW2,BtnH2)) {
                numInput2("Mass2",&mass2Val);
            }
            else if(inside2(m.x,m.y,col2x2,speed2BtnY2,btnW2,BtnH2)) {
                numInput2("Speed2",&speed2Val);
            }
            else if(inside2(m.x,m.y,col2x2,color2BtnY2,btnW2,BtnH2)) {
                colorSelect2(2);
            }
        }
    }
}

//返回输入的值
char* getColor1() { 
    return color1N; 
}

float getRadius1() { 
    return radius1Val; 
}

float getMass1() { 
    return mass1Val; 
}

float getSpeed1() { 
    return speed1Val; 
}

char* getColor2() {
    return color2N; 
}

float getRadius2() { 
    return radius2Val; 
}

float getMass2() {
    return mass2Val; 
}

float getSpeed2() { 
    return speed2Val; 
}
