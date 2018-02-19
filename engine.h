#ifndef ENGINE_H
#define ENGINE_H

#include <windows.h>

#define PI 3.141592653

extern DEVMODE FS;
extern int WNDW,WNDH;

struct TPoint{
  int x,y;       
};

class TStream{
public:
  TStream operator<<(char* x){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD X;
    WriteConsole(hOut,x,strlen(x),&X,NULL);        
  }        
};

extern TStream cout;

struct SStr{
  char s[50];
};

bool IsLeap(int year);

struct TDate{
  short int d;
  short int m;
  int y;
  bool operator <(TDate Date2){
    if (y < Date2.y) return true;
    else if (y == Date2.y){
      if (m < Date2.m) return true;
      else if (m == Date2.m){
        if (d < Date2.d) return true;
        else return false;
      } else return false;
    } else return false;
  }
  bool operator >(TDate Date2){
    if (y > Date2.y) return true;
    else if (y == Date2.y){
      if (m > Date2.m) return true;
      else if (m == Date2.m){
        if (d > Date2.d) return true;
        else return false;
      } else return false;
    } else return false;     
  }
  bool operator ==(TDate Date2){
    return ((Date2.d == d) && (Date2.m == m) && (Date2.y == y));    
  }
  TDate operator --(){
    d--;
    if (!d){
      m--;
      switch (m){
        case 0:{
          m = 12;
          d = 31;
          y--;     
        } break;
        case 1: d = 31; break;
        case 2: d = 28+IsLeap(y); break;
        case 3: d = 31; break;
        case 4: d = 30; break;
        case 5: d = 31; break;
        case 6: d = 30; break;
        case 7: d = 31; break;
        case 8: d = 31; break;
        case 9: d = 30; break;
        case 10: d = 31; break;
        case 11: d = 30; break;       
      }        
    }      
  }
  TDate operator ++(){
    d++;
    switch (m){
      case 1:{
        if (d > 31){
          d = 1;
          m++;
        }   
      } break;
      case 2:{
        if (d > 28+IsLeap(y)){
          d = 1;
          m++;      
        }   
      } break;
      case 3:{
        if (d > 31){
          d = 1;
          m++;      
        }  
      } break;
      case 4:{
        if (d > 30){
          d = 1;
          m++;      
        }   
      } break;
      case 5:{
        if (d > 31){
          d = 1;
          m++;      
        }
      } break;
      case 6:{
        if (d > 30){
          d = 1;
          m++;      
        }   
      } break;
      case 7:{
        if (d > 31){
          d = 1;
          m++;      
        }   
      } break;
      case 8:{
        if (d > 31){
          d = 1;
          m++;      
        }
      } break;
      case 9:{
        if (d > 30){
          d = 1;
          m++;      
        }
      } break;
      case 10:{
        if (d > 31){
          d = 1;
          m++;      
        }
      } break;
      case 11:{
        if (d > 30){
          d = 1;
          m++;      
        }
      } break;
      case 12:{
        if (d > 31){
          d = 1;
          m = 1;
          y++;     
        }
      } break;       
    }      
  }
};

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);
HBITMAP Rotate(HDC hdc,HBITMAP Hbm,int AF,COLORREF Background,double a,TPoint p);
HBITMAP Rotate2(HDC hdc,HBITMAP Hbm,int AF,COLORREF Background,double a,TPoint p);
HBITMAP Rotate2Mask(HDC hdc,HBITMAP Hbm,int AF,COLORREF Background,double a,TPoint p);
HBITMAP Rotate3(HDC hdc,HBITMAP Hbm,int AF,COLORREF Background,double a,TPoint p);
HBITMAP Rotate4(HDC hdc,HBITMAP Hbm,int AF,COLORREF Background,double a,TPoint p);
HBITMAP Rotate5(HDC hdc,HBITMAP Hbm,int AF,COLORREF Background,double a,TPoint p,TPoint *outp);
double DegToRad(double Deg);
double RadToDeg(double Rad);
double Abs(double x);
bool CheckForColor(HDC hdc,HBITMAP Hbm,COLORREF Color);
TPoint RotateOnBitmap(int x,int y,int Width,int Height,double a);
TPoint RotateOnBitmap2(int Width,int Height,double a,TPoint p);
void ShowMessage(char* msg);
char* inttostr(int x);
int strtoint(char* x);
bool Exist(const char *file);
bool ValidDate(int y,int m,int d);
int DayInTheYear(TDate Date);
bool OnlyNumbers(char* x);
LPSTR GetWindowTextFast(HWND hwnd);  //GlobalFree after this
void WritelnInConsole(char* x);
void WriteInConsole(char* x);
int DaysAfter22(int y);
TDate Easter(int y);
TDate BozeCialo(int y);
int DayOfTheWeek(TDate Date);
TDate DayToDate(int year,int day);
void ClearRect(RECT &rc);
int Max(int x,int y);
int Min(int x,int y);
COLORREF Fade(COLORREF Color);
COLORREF Darken(COLORREF Color);
void ActivateFullscreen();
double sqr(double a);

#endif
