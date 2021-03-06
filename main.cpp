#include <math.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string.h>
#include<fstream>
#include<vector>
#include<sstream>
#include<unistd.h>
#include "sdglib.h"
using namespace std;
using namespace SDGLibF;
char str[] = " - 2D Graphics (61711356 Yuki Takayama) - ";
double a,vx1,vx2,vy,f=0;  //a:加速度、vx1:右向き速度、vx2:左向き速度、f:ジャンプ時の初速設定
double x=-100; //x座標
double x_position=-100;  //右スクロール時、画面中央に来たときにx座標をこっちに切り替えて画面中央より右に行かないようにした
double k=0.8;   //空気抵抗の比例定数
double y=-145;  //y座標の設定
double m=200;   //playerの質量設定
double p=0;  //横スクロールのために使用
double level=-145;  //足場の立ち位置設定
#define g 9.8;
int t=0;  //開始時間からカウント
double dt=0;  //何か条件が起きたときにカウント(ex. jump or fall)
double jump_position=-145;  //ジャンプ時のy座標を記録（これがないとlevelに左右されて穴を通り越せない）
double ar=0;  //着地しているときにはar=0
double r=1;   //落下の判定
double s=1;   //ゴールの判定
double end_time;  //終了時刻の表示


void timer(){
    usleep(100);
    
    //落下の設定
    a=-g-k/m*vy;
    if((level==-250&&ar==0)){   //穴場のポジションにあり、着地していたら落下
        dt+=0.5;
        vy=a*dt;
    }
    if(y<-230) r=0;      //穴に落ちたらGame Overコメントを表示
    vy=a*dt;              //ジャンプ時の速度設定
    y=jump_position+vy*dt;    //最初jump_positionの代わりにlevelを使っていたが、levelが異なる位置に跳ぶとエラーが生じた
    
    //timer
    t++;
    stringstream sb;
    string strb = "time=  ";     //時間を表示(ゴールでendtimeに切り替え)
    sb << strb << t;
    SetColor(0.5765+0.4235*s, 0.8275*(1-s), 0.9412*(1-s));
    DrawString(60, 140, sb.str());
    if(s==0){
        end_time=t;                     //ゴール時の時間を記録
        t--;
    }
    SetColor(0.5765+0.4235*(1-s), 0.8275*s, 0.9412*s);
    stringstream sa;
    string stra = "Time=  ";
    sa << stra << end_time;
    DrawString(60, 120, sa.str());
    ReDraw();
}

//player製作
class making_character{
public:
    void move_to_right();
    void move_to_left();
    void jump();
    void friction();
    void making_player();
};
making_character you;
void making_character::move_to_right(){
    vx1++;
    if(vx1>=5)vx1=5;
}
void making_character::move_to_left(){
    vx2++;
    x-=vx2;
    if(vx2>=5)vx2=5;
}
void making_character::jump(){
    if(ar==1){       //空中にいるとき
        dt+=0.05;  //ジャンプ始めてからの時間経過
        usleep(10000);
        f=20000;          //上むきの外力f
        //a=-g-k/m*vy;
        vy=f/m+a*dt;
        y+=vy*dt;
        cout<<"(x,y)="<<x<<","<<y<<endl;
        cout<<"level="<<level<<endl;
        cout<<"jump_position="<<jump_position<<endl;
        ReDraw();
        if(y<=level){       //ジャンプ後、足場に着地したら
            y=level;         //y座標をlevelに設定
            ar=0;           //地上にいる状態に戻す
            dt=0;          //経過時間を0にする
            if(jump_position!=level)jump_position=level;
            //cout<<"(x,y)="<<x<<","<<y<<endl;
            IdleFunc(NULL);   //ジャンプを終了
        }
    }
}
void jump2(){
    you.jump();      //IdleFunc内に入れる関数としてクラス内のjumpを別の関数に置き換えた
}
void making_character::friction(){          //横移動入力をやめたら徐々に減速するようにする
    if(vx1<0.1)vx1=0.1;
    vx1-=0.1;
    x+=vx1;
    if(vx2<0.1)vx2=0.1;
    vx2-=0.1;
    x-=vx2;
    
    //横スクロールの設定
    if(x>0){
        x_position=0;          //xの位置が画面中央より右に位置したら、これに座標を移行する
        p=x;                   //その後の横スクロールに使ったpにxを代入
    }
    else{
        x_position=x;
    }
    if(x<-200)x=-195;          //画面左端にはいけないように条件式を導入した
}
void making_character::making_player(){    //キャラクターの作画
    for(int i=0;i<10;i++){
        SetColor(0,8,10);
        DrawCircle(1, x_position,y, i);
        SetColor(0, 0, 0);
        if(vx1>=vx2)DrawLine(1,x_position+2,y+3,x_position+8,y+3);
        if(vx1<vx2)DrawLine(1,x_position-2,y+3,x_position-8,y+3);
    }
}
void up(){         //上移動足場での動き
    y=level+3;
    jump_position=level+3;
}

//ステージ設定
class making_stage{
public:
    void making_floor();
    void making_sky();
    void message();
};
making_stage stage;
void making_stage::making_floor(){
    for(int k=0;k<10;k++){
        for(int j=-200;j<-150;j+=10){
            for(int i=-200;i<200;i+=30){
                SetColor(0.8,0.5,0);       //ブロックの作図
                DrawLine(1,i-p,j,i+3*k-p,j);
                DrawLine(1,i+3*k-p,j,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j+1*k,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j,i-p,j+1*k);
            }
        }
        for(int j=-200;j<-150;j+=10){
            for(int i=280;i<350;i+=30){
                DrawLine(1,i-p,j,i+3*k-p,j);
                DrawLine(1,i+3*k-p,j,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j+1*k,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j,i-p,j+1*k);
            }
        }
        for(int j=-200;j<-100;j+=10){
            for(int i=450;i<600;i+=30){
                DrawLine(1,i-p,j,i+3*k-p,j);
                DrawLine(1,i+3*k-p,j,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j+1*k,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j,i-p,j+1*k);
            }
        }
        for(int j=-200;j<-150;j+=10){
            for(int i=700;i<750;i+=30){
                DrawLine(1,i-p,j,i+3*k-p,j);
                DrawLine(1,i+3*k-p,j,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j+1*k,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j,i-p,j+1*k);
            }
        }
        for(int j=-200;j<-100;j+=10){
            for(int i=800;i<950;i+=30){
                DrawLine(1,i-p,j,i+3*k-p,j);
                DrawLine(1,i+3*k-p,j,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j+1*k,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j,i-p,j+1*k);
            }
        }
        for(int j=-200;j<-150;j+=10){
            for(int i=1050;i<1200;i+=30){
                DrawLine(1,i-p,j,i+3*k-p,j);
                DrawLine(1,i+3*k-p,j,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j+1*k,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j,i-p,j+1*k);
            }
        }
        for(int l=0;l<15;l++){
            for(int j=-200;j<-150+10*l;j+=10){
                for(int i=1200;i<1231;i+=30){
                    DrawLine(1,i-p+30*l,j,i+3*k-p+30*l,j);
                    DrawLine(1,i+3*k-p+30*l,j,i+3*k-p+30*l,j+1*k);
                    DrawLine(1,i-p+30*l,j+1*k,i+3*k-p+30*l,j+1*k);
                    DrawLine(1,i-p+30*l,j,i-p+30*l,j+1*k);
                }
            }
        }
        for(int j=-200;j<-150;j+=10){
            for(int i=2000;i<2200;i+=30){
                DrawLine(1,i-p,j,i+3*k-p,j);
                DrawLine(1,i+3*k-p,j,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j+1*k,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j,i-p,j+1*k);
            }
        }
        double dt2;
        dt2=t%400;
        for(int j=-200+dt2;j<-200+dt2+11;j+=10){
            for(int i=1800;i<1870;i+=30){
                DrawLine(1,i-p,j,i+3*k-p,j);
                DrawLine(1,i+3*k-p,j,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j+1*k,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j,i-p,j+1*k);
            }
        }
        for(int j=-160;j<-150;j+=10){
            for(int i=2360;i<2400;i+=30){
                DrawLine(1,i-p,j,i+3*k-p,j);
                DrawLine(1,i+3*k-p,j,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j+1*k,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j,i-p,j+1*k);
            }
        }
        double dt3;
        dt3=t%500;
        for(int j=-660+dt3;j<-660+dt3+10;j+=10){
            for(int i=2500;i<2600;i+=30){
                DrawLine(1,i-p,j,i+3*k-p,j);
                DrawLine(1,i+3*k-p,j,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j+1*k,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j,i-p,j+1*k);
            }
        }
        for(int j=-160;j<-150;j+=10){
            for(int i=2500+dt3;i<2600+dt3;i+=30){
                DrawLine(1,i-p,j,i+3*k-p,j);
                DrawLine(1,i+3*k-p,j,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j+1*k,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j,i-p,j+1*k);
            }
        }
        for(int j=-160-dt3;j<-160-dt3+10;j+=10){
            for(int i=3000;i<3100;i+=30){
                DrawLine(1,i-p,j,i+3*k-p,j);
                DrawLine(1,i+3*k-p,j,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j+1*k,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j,i-p,j+1*k);
            }
        }
        for(int j=-160;j<-150;j+=10){
            for(int i=3200;i<3260;i+=30){
                DrawLine(1,i-p,j,i+3*k-p,j);
                DrawLine(1,i+3*k-p,j,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j+1*k,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j,i-p,j+1*k);
            }
        }
        for(int j=-160;j<-150;j+=10){
            for(int i=3400;i<3550;i+=30){
                SetColor(1, 0, 0);
                DrawLine(1,i-p,j,i+3*k-p,j);
                DrawLine(1,i+3*k-p,j,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j+1*k,i+3*k-p,j+1*k);
                DrawLine(1,i-p,j,i-p,j+1*k);
            }
        }
        //playerの立ち位置（落下）の設定
        if(x<=220) level=-145;
        //if(x>220&&y<=-150) x>220;
        if((x>220&&x<280)) level=-250;   //穴場をlevel=-250で設定した
        if((x>=280&&x<=365)) level=-145;
        if((x>365&&x<450)) level=-250;
        if((x>=450&&x<=600)) level=-95;
        if((x>600&&x<700)) level=-250;
        if((x>=700&&x<=760)) level=-145;
        if((x>760&&x<800)) level=-250;
        if((x>=800&&x<=950)) level=-95;
        if((x>950&&x<1050)) level=-250;
        if((x>=1050&&x<=1220)) level=-145;
        for(int l=0;l<14;l++){
            if((x>=1220+30*l&&x<1220+30*l+30)) level=-135+10*l;
        }
        for(int l=0;l<14;l++){
            if((x>=1250+30*l&&x<=1641)&&y<=-135+10*l) x=1245+30*l;
        }
        if((x>1680&&x<1800)) level=-250;
        if((x>=1800&&x<=1889)&&y>=-200+dt2+25) {
            level=-200+dt2+25;
            if(y>=level&&y<=level+4) IdleFunc(up);
        }
        if((x>=1800&&x<=1889)&&(y<-200+dt2)) {
            level=-250;
        }
        if((x>1889&&x<2000)) level=-250;
        if((x>=2000&&x<=2200)) level=-145;
        if((x>2205&&x<2355)) level=-250;
        if((x>=2355&&x<=2415)) level=-145;
        if((x>2415&&x<2500+dt3)) level=-250;
        if((x>=2500&&x<=2620)&&y>=-660+dt3+10) {
            level=-660+dt3+10;
            if(y>=level&&y<=level+4) IdleFunc(up);
        }
        if((x>=2500+dt3&&x<=2620+dt3)&&(y>=-150&&y<=-142)){
            level=-145;
            x+=0.1;
        }
        /*if((x>=3015&&x<=3135)&&y>=-170-dt3+25) {
            level=-145;
        }*/
        if((x>=3015&&x<=3135)&&(y>=-146)&&(y<=-140)) level=-145;
        if((x>2620+dt3&&x<3200)) level=-250;
        if((x>=3200&&x<=3260)) level=-145;
        if((x>3260&&x<3400)) level=-250;
        if((x>=3400&&x<=3550)&&(y<-130)) {
            level=-145;
            s=0;    //ゴールしたらs=0でgoalを表現
        }
    }
}
void making_stage::making_sky(){
    SetColor(0.5765, 0.8275, 0.9412);    //背景の作画
    for(int i=0;i<400;i++){
        DrawLine(10,-200,-200+i,5000,-200+i);
    }
    /*for(int i=0;i<3000;i+=1000){      //雲の作成（重くなる)
        for(int j=0;j<20;j++){
            SetColor(1,1,1);
            DrawCircle(1, -200+i,150, j);
            DrawCircle(1, -170+i,150, j);
            DrawCircle(1, -140+i,150, j);
        }
    }*/
}

void making_stage::message(){
    SetColor(0.5765+0.4235*(1-r), 0.8275*r, 0.9412*r);
    stringstream gameover;
    string str1 = "Game Over";
    gameover<< str1;
    DrawString(0,170, gameover.str());
    
    SetColor(0.5765+0.4235*(1-s), 0.8275*s, 0.9412*s);
    stringstream goal;
    string str2 = "GOOOooooooooooooooooOOOL!!!!";
    goal<< str2;
    DrawString(-150,150, goal.str());
}

void displayfunc(){
    using namespace SDGLibF;
    Before();    // Set drawing plane
    stage.making_sky();    //最初にこれを作画することで背景とした
    stage.message();
    you.making_player();
    stage.making_floor();
    timer();
    you.friction();
    After();
}

void keyboardfunc(unsigned char k, int x, int y){
    switch (k) {
        case 27:    /* Escape */
            exit(0);
            break;
        case 'i':
            IdleFunc(NULL);
            jump_position=level;
            ar=1;
            IdleFunc(jump2);
            break;
        case 'j':
            you.move_to_left();
            break;
        case 'l':
            you.move_to_right();
            break;
    }
}

int main(void)
{
    SDGLib mygraphic(800, 400, "- 2D Graphics - (Yuki Takayama)", -200.0, 600.0, -200.0, 200.0);
    mygraphic.SetCursor(GLUT_CURSOR_WAIT);
    mygraphic.Display(displayfunc);
    mygraphic.Keyboard(keyboardfunc);
    mygraphic.Start();
    return 0;
}
