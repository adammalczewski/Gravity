#include <windows.h>
#include <cmath>
#include "engine.h"
#include "resources.h"
#include "Heap.h"

#define PANELWIDTH 200
#define ANTIALIASING 1

#define ID_BTNSELECT 0
#define ID_BTNWALL 1
#define ID_BTNSTARCRAFT 2
#define ID_BTNBUBBLE1 3
#define ID_BTNBUBBLE2 4
#define ID_BTNBUBBLE3 5
#define ID_BTNBUBBLE4 6
#define ID_BTNBUBBLE5 7
#define ID_BTNBUBBLE6 8

#define MAXBALLS 10000
#define MAXWALLS 1000

#define SPEED 1
                                         
#define HITSIDEENERGYRED 0.3
#define HITENERGYRED 0.7

#define G 1000

struct TPointR{
  double x,y;       
};

struct TRectR{
  double left,right;
  double top,bottom;       
};

struct TBall{
  TRectR rc;
  int size;
  double vx,vy;
  bool real;
  bool col;
  int n;     
};

struct TBorder{
  double a,b;
  int side;       
};

struct TWall{
  bool type; //0-wall,1-starcraft
  TRectR rc;
  TPointR lt,lb,rt,rb;
  TBorder Bs[4];
  int a;
  bool real;
  bool col;
  bool alert;
  int n;     
};

Heap CH;

bool FULLSCREEN;
DEVMODE FS;
int WNDH,WNDW;

bool switching;

HINSTANCE HInstance;
HWND hwnd;

TStream cout;

LARGE_INTEGER Freq;

HBITMAP BufferHbm;
HBITMAP ColorHbm;

HBITMAP WallHbms[360],WallMaskHbms[360];
HBITMAP StarcraftHbms[360],StarcraftMaskHbms[360];
HBITMAP BallHbms[6],BallMaskHbms[6];

TPoint WallMids[360];
TPoint StarcraftMids[360];

TBall* Balls[MAXBALLS];
TWall* Walls[MAXWALLS];
int bn,wn;

int creating;
void* citem;

int mx,my;

int BallSizes[6];                

void MakeGraphics();

void OffsetRectR(TRectR* rc,double dx,double dy);

void UpdateGame(double dt,RECT rc);
void CheckCollisions();
void Draw(HDC hdc,RECT rc);

TBall* AddBall(int x,int y,int size,double vx,double vy,bool real);
TWall* AddWall(int x,int y,bool type,int a,bool real);

bool DeleteBall(int x);
bool DeleteWall(int x);
bool DeleteBall(TBall* x);
bool DeleteWall(TWall* x);

bool PrepareWall(TWall* x); 

void GetPoints(int w,int h,int x,int y,int a,POINT p,POINT p2,TPointR &lt,TPointR &rt,TPointR &lb,TPointR &rb);

double GetTime();

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain (HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszArgument,int nFunsterStil){
     
    FULLSCREEN = true;
    WNDW = 800;
    WNDH = 650;
    switching = false;                
                   
    HInstance = hInstance;
                   
    WNDCLASSEX Window; 

    Window.hInstance = hInstance;
    Window.lpszClassName = "MyWindow";
    Window.lpfnWndProc = WndProc; 
    Window.style = 0;
    Window.cbSize = sizeof (WNDCLASSEX);
    Window.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    Window.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    Window.hCursor = LoadCursor (NULL, IDC_ARROW);
    Window.lpszMenuName = NULL; 
    Window.cbClsExtra = 0;    
    Window.cbWndExtra = 0;      
    Window.hbrBackground = CreateSolidBrush(RGB(236,233,216));


    if (!RegisterClassEx (&Window)) return 0;
    
    if (FULLSCREEN){
      ActivateFullscreen();
      hwnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_TOPMOST,"MyWindow","Title",WS_POPUP,0,0,WNDW,WNDH,NULL,NULL,HInstance,NULL);
    } else hwnd = CreateWindowEx(0,"MyWindow","Title",WS_SYSMENU|WS_MINIMIZEBOX,100,100,WNDW,WNDH,NULL,NULL,hInstance,NULL);
    
    HWND HBtnSelect = CreateWindow("BUTTON","Select",BS_RADIOBUTTON|WS_VISIBLE|WS_CHILD,WNDW-PANELWIDTH+10,20,100,16,hwnd,(HMENU)ID_BTNSELECT,hInstance,NULL);
    HWND HBtnWall = CreateWindow("BUTTON","Wall",BS_RADIOBUTTON|WS_VISIBLE|WS_CHILD,WNDW-PANELWIDTH+10,45,100,16,hwnd,(HMENU)ID_BTNWALL,hInstance,NULL);
    HWND HBtnStarcraft = CreateWindow("BUTTON","Starcraft",BS_RADIOBUTTON|WS_VISIBLE|WS_CHILD,WNDW-PANELWIDTH+10,70,100,16,hwnd,(HMENU)ID_BTNSTARCRAFT,hInstance,NULL);
    HWND HBtnBubble1 = CreateWindow("BUTTON","Bubble 1",BS_RADIOBUTTON|WS_VISIBLE|WS_CHILD,WNDW-PANELWIDTH+10,95,100,16,hwnd,(HMENU)ID_BTNBUBBLE1,hInstance,NULL);
    HWND HBtnBubble2 = CreateWindow("BUTTON","Bubble 2",BS_RADIOBUTTON|WS_VISIBLE|WS_CHILD,WNDW-PANELWIDTH+10,120,100,16,hwnd,(HMENU)ID_BTNBUBBLE2,hInstance,NULL);
    HWND HBtnBubble3 = CreateWindow("BUTTON","Bubble 3",BS_RADIOBUTTON|WS_VISIBLE|WS_CHILD,WNDW-PANELWIDTH+10,145,100,16,hwnd,(HMENU)ID_BTNBUBBLE3,hInstance,NULL);
    HWND HBtnBubble4 = CreateWindow("BUTTON","Bubble 4",BS_RADIOBUTTON|WS_VISIBLE|WS_CHILD,WNDW-PANELWIDTH+10,170,100,16,hwnd,(HMENU)ID_BTNBUBBLE4,hInstance,NULL);
    HWND HBtnBubble5 = CreateWindow("BUTTON","Bubble 5",BS_RADIOBUTTON|WS_VISIBLE|WS_CHILD,WNDW-PANELWIDTH+10,195,100,16,hwnd,(HMENU)ID_BTNBUBBLE5,hInstance,NULL);
    HWND HBtnBubble6 = CreateWindow("BUTTON","Bubble 6",BS_RADIOBUTTON|WS_VISIBLE|WS_CHILD,WNDW-PANELWIDTH+10,220,100,16,hwnd,(HMENU)ID_BTNBUBBLE6,hInstance,NULL);
    
    MakeGraphics();
    
    CheckRadioButton(hwnd,ID_BTNSELECT,ID_BTNBUBBLE6,ID_BTNBUBBLE3);
    creating = ID_BTNBUBBLE3;
    
    citem = AddBall(WNDW,0,2,0,0,false);
    mx = WNDW;
    my = 0; 
    
    RECT WndRect;
    GetWindowRect(hwnd,&WndRect);
    WndRect.right -= PANELWIDTH;
    
    ShowWindow (hwnd, nFunsterStil);
    
    if (!QueryPerformanceFrequency(&Freq)){
      return 0;                                       
    };                                                                                      
    
    MSG msg;
    
    double ts,lt,t,dt;
    
    ts = GetTime();
    lt = ts;

    while (msg.message != WM_QUIT){  
    
      if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
        TranslateMessage (&msg);
        DispatchMessage (&msg);
      } else {
        t = GetTime();
        dt = t-lt;
        lt = t;
        HDC hdc = GetDC(hwnd);
        UpdateGame(dt,WndRect);
        CheckCollisions();
        Draw(hdc,WndRect);
        ReleaseDC(hwnd,hdc);
      }
      
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
  switch (message){
    /*case WM_PAINT:{
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd,&ps);
      
      EndPaint(hwnd,&ps);    
    } break;*/
    case WM_DESTROY:{
      PostQuitMessage(0);   
    } break;
    case WM_COMMAND:{
      if ((wParam >= ID_BTNSELECT) && (wParam <= ID_BTNBUBBLE6) && (wParam != creating)){
        SetFocus(hwnd);
        CheckRadioButton(hwnd,ID_BTNSELECT,ID_BTNBUBBLE6,wParam);
        if ((creating == ID_BTNWALL) || (creating == ID_BTNSTARCRAFT)){
          DeleteWall((TWall*)citem);              
        } else if (creating > ID_BTNSTARCRAFT){
          DeleteBall((TBall*)citem);     
        }
        if ((wParam == ID_BTNWALL) || (wParam == ID_BTNSTARCRAFT)){                           
          citem = AddWall(WNDW,0,wParam-ID_BTNWALL,63,false);
          mx = WNDW;
          my = 0;             
        } else if (wParam > ID_BTNSTARCRAFT){
          citem = AddBall(WNDW,0,wParam-ID_BTNBUBBLE1,0,0,false);
          mx = WNDW;
          my = 0;       
        }
        creating = wParam;            
      }
    } break;
    case WM_MOUSEMOVE:{
      if (creating){
        int MX = LOWORD(lParam);
        int MY = HIWORD(lParam);
        if ((creating == ID_BTNWALL) || (creating == ID_BTNSTARCRAFT)){                           
          OffsetRectR(&(((TWall*)citem)->rc),MX-mx,MY-my);            
        } else if (creating > ID_BTNSTARCRAFT){
          OffsetRectR(&(((TBall*)citem)->rc),MX-mx,MY-my);      
        }  
        mx = MX;
        my = MY;
      }   
    } break;
    case WM_LBUTTONDOWN:{
      int MX = LOWORD(lParam);
      int MY = HIWORD(lParam);
      if ((creating == ID_BTNWALL) || (creating == ID_BTNSTARCRAFT)){
        ((TWall*)citem)->real = true;
        PrepareWall((TWall*)citem);
        citem = AddWall(MX,MY,creating-ID_BTNWALL,((TWall*)citem)->a,false);
          mx = MX;
          my = MY;         
      } else if (creating > ID_BTNSTARCRAFT){
        ((TBall*)citem)->real = true;
        citem = AddBall(MX,MY,creating-ID_BTNBUBBLE1,0,0,false);
        mx = MX;
        my = MY;        
      }   
    } break;
    case WM_KEYDOWN:{
      switch (wParam){
        case VK_LEFT:{
          if ((creating == ID_BTNWALL) || (creating == ID_BTNSTARCRAFT)){
            int z = ((TWall*)citem)->a;
            ((TWall*)citem)->a = (z+361)%360;
            if (creating == ID_BTNWALL){
              OffsetRectR(&((TWall*)citem)->rc,WallMids[z].x-WallMids[((TWall*)citem)->a].x,WallMids[z].y-WallMids[((TWall*)citem)->a].y);             
            } else OffsetRectR(&((TWall*)citem)->rc,StarcraftMids[z].x-StarcraftMids[((TWall*)citem)->a].x,StarcraftMids[z].y-StarcraftMids[((TWall*)citem)->a].y);
          
          }   
        } break;
        case VK_RIGHT:{
          int z = ((TWall*)citem)->a;
          ((TWall*)citem)->a = (z+359)%360;
          if (creating == ID_BTNWALL){
              OffsetRectR(&((TWall*)citem)->rc,WallMids[z].x-WallMids[((TWall*)citem)->a].x,WallMids[z].y-WallMids[((TWall*)citem)->a].y);             
            } else OffsetRectR(&((TWall*)citem)->rc,StarcraftMids[z].x-StarcraftMids[((TWall*)citem)->a].x,StarcraftMids[z].y-StarcraftMids[((TWall*)citem)->a].y);   
        } break;       
      }   
    } break;
    case WM_KEYUP:{  
      switch (wParam){
        case VK_LEFT:{
             
        } break;
        case VK_RIGHT:{
             
        } break;       
      }      
    } break;
    case WM_ACTIVATE:{
      if (!FULLSCREEN) break;
      if (switching) break;
      if ((LOWORD(wParam) == WA_ACTIVE) || (LOWORD(wParam) == WA_CLICKACTIVE)){
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, WNDW, WNDH, SWP_SHOWWINDOW);                    
        if (!IsIconic(hwnd)){
          ChangeDisplaySettings(&FS, CDS_FULLSCREEN);                     
        }
      } else {
        ChangeDisplaySettings(NULL, CDS_RESET);
        ShowWindow(hwnd,SW_MINIMIZE);     
      }   
    } break;
    default : return DefWindowProc(hwnd, message, wParam, lParam);       
  }
  
  return 0;     
}

void MakeGraphics(){
  
  HDC hdc = GetDC(hwnd);
  
  BITMAP bm;
  
  WallHbms[0] = (HBITMAP)LoadImage(HInstance,MAKEINTRESOURCE(BM_WALL),IMAGE_BITMAP,0,0,LR_SHARED);
  GetObject(WallHbms[0],sizeof(BITMAP),&bm);
  WallMids[0].x = bm.bmWidth/2;
  WallMids[0].y = bm.bmHeight/2;
  
  WallMaskHbms[0] = CreateBitmapMask(WallHbms[0],0xFF0000);
  for (int i = 1;i < 360;i++){
    if (i % 90){
      WallMaskHbms[i] = Rotate5(hdc,WallMaskHbms[0],ANTIALIASING,0xFFFFFF,DegToRad(i),WallMids[0],NULL);
    }
    WallHbms[i] = Rotate5(hdc,WallHbms[0],ANTIALIASING,0x000000,DegToRad(i),WallMids[0],&WallMids[i]);   
  }
  
  StarcraftHbms[0] = (HBITMAP)LoadImage(HInstance,MAKEINTRESOURCE(BM_STARCRAFT),IMAGE_BITMAP,0,0,LR_SHARED);
  GetObject(StarcraftHbms[0],sizeof(BITMAP),&bm);
  StarcraftMids[0].x = bm.bmWidth/2;
  StarcraftMids[0].y = bm.bmHeight/2;
  
  StarcraftMaskHbms[0] = CreateBitmapMask(StarcraftHbms[0],0x0000FF);
  for (int i = 1;i < 360;i++){
    if (i % 90){
      StarcraftMaskHbms[i] = Rotate5(hdc,StarcraftMaskHbms[0],ANTIALIASING,0xFFFFFF,DegToRad(i),StarcraftMids[0],NULL);
    }
    StarcraftHbms[i] = Rotate5(hdc,StarcraftHbms[0],ANTIALIASING,0x000000,DegToRad(i),StarcraftMids[0],&StarcraftMids[i]);   
  }   
  
  for (int i = 0;i < 6;i++){
    BallHbms[i] = (HBITMAP)LoadImage(HInstance,MAKEINTRESOURCE(BM_BUBBLE1+i),IMAGE_BITMAP,0,0,LR_SHARED);
    GetObject(BallHbms[i],sizeof(BITMAP),&bm);
    BallSizes[i] = bm.bmWidth;
    BallMaskHbms[i] = CreateBitmapMask(BallHbms[i],RGB(255,255,255));
  }
  
  if (FULLSCREEN){
    BufferHbm = CreateCompatibleBitmap(hdc,FS.dmPelsWidth,FS.dmPelsHeight);
    ColorHbm = CreateCompatibleBitmap(hdc,FS.dmPelsWidth,FS.dmPelsHeight);
  } else {
    BufferHbm = CreateCompatibleBitmap(hdc,800,650);
    ColorHbm = CreateCompatibleBitmap(hdc,800,650);
  }
  
  ReleaseDC(hwnd,hdc);
     
}

void OffsetRectR(TRectR* rc,double dx,double dy){
  rc->top += dy;
  rc->bottom += dy;
  rc->left += dx;
  rc->right += dx;     
}
 double time;
void UpdateGame(double dt,RECT rc){
    
     /*time += dt;
     
     if (time > 15){
     time -= 15;
     
    
     for (int i = 0;i < 100;i++) {
         int x = rand()%700;
         AddBall(x,100,3,2.5,10,true);
     }
     }*/
     
     /*time += dt;
     if (time > 0.1){
     time -= 0.1;
     for (int i = 0;i < wc;i++){
     if (Walls[i].type){
      int oldwc = wc;
      wc = i;
      AddWall(Walls[wc].rc.left+StarcraftMids[Walls[wc].a].x,Walls[wc].rc.top+StarcraftMids[Walls[wc].a].y,1,(Walls[wc].a+1)%360,true);
      wc = oldwc;
    } 
}
}  */
               
  for (int i = 0;i < bn;i++){
    Balls[i]->col = false;
    if (!Balls[i]->real){
      if ((Balls[i]->rc.bottom > rc.bottom) || (Balls[i]->rc.top < rc.top)
        || (Balls[i]->rc.left < rc.left) || (Balls[i]->rc.right > rc.right)) Balls[i]->col = true;
      continue;  
    }  
    Balls[i]->vy = Balls[i]->vy + SPEED*G*dt;
    
    OffsetRectR(&Balls[i]->rc,Balls[i]->vx*SPEED*dt,(Balls[i]->vy-SPEED*G*dt/2)*SPEED*dt);  
    
    if (Balls[i]->rc.bottom >= rc.bottom){
      if (Balls[i]->vy > 0) Balls[i]->vy = -Balls[i]->vy*(1-HITENERGYRED);
      OffsetRectR(&Balls[i]->rc,-Balls[i]->vx*(HITSIDEENERGYRED)*SPEED*dt,(rc.bottom-Balls[i]->rc.bottom)*(2.0-HITENERGYRED));                     
      if (Balls[i]->vy > 0) Balls[i]->vx = Balls[i]->vx*(1-HITSIDEENERGYRED);
    } else if (Balls[i]->rc.top <= rc.top){
      if (Balls[i]->vy < 0) Balls[i]->vy = -Balls[i]->vy*(1-HITENERGYRED);
      OffsetRectR(&Balls[i]->rc,0,(rc.top-Balls[i]->rc.top)*(2.0-HITENERGYRED));
      if (Balls[i]->vy < 0) Balls[i]->vx = Balls[i]->vx*(1-HITSIDEENERGYRED);       
    }
    if (Balls[i]->rc.left <= rc.left){
      if (Balls[i]->vx < 0) Balls[i]->vx = -Balls[i]->vx*(1-HITENERGYRED);
      OffsetRectR(&Balls[i]->rc,(rc.left-Balls[i]->rc.left)*(2.0-HITENERGYRED),-Balls[i]->vy*(HITSIDEENERGYRED)*SPEED*dt);
      if (Balls[i]->vx < 0) Balls[i]->vy = Balls[i]->vy*(1-HITSIDEENERGYRED);                    
    } else if (Balls[i]->rc.right >= rc.right){
      if (Balls[i]->vx > 0) Balls[i]->vx = -Balls[i]->vx*(1-HITENERGYRED);
      OffsetRectR(&Balls[i]->rc,(rc.right-Balls[i]->rc.right)*(2.0-HITENERGYRED),-Balls[i]->vy*(HITSIDEENERGYRED)*SPEED*dt);
      if (Balls[i]->vx > 0) Balls[i]->vy = Balls[i]->vy*(1-HITSIDEENERGYRED);     
    }
  }
     
} 

void CheckCollisions(){
     
  /*for (int j = 0;j < wn;j++){ 
    Walls[j]->alert = false;
    Walls[j]->col = false;
  }*/
  for (int i = 0;i < bn;i++){
    if (!Balls[i]->real) continue;
    
    TPointR o;
    double r;
    o.x = (Balls[i]->rc.right+Balls[i]->rc.left)/2.0;
    o.y = (Balls[i]->rc.top+Balls[i]->rc.bottom)/2.0;
    r = BallSizes[Balls[i]->size]/2.0;
    
    /*if (Balls[i]->rc.bottom >= rc.bottom){
      Col c;
      c.type1 = 0;
      c.type2 = -1;
      c.n1 = i;
      c.n2 = 3;
      c.t = (rc.bottom - Balls[i]->rc.bottom)/Balls[i].vy;                      
    } else if (Balls[i]->rc.top <= rc.top){
      Col c;
      c.type1 = 0;
      c.type2 = -1;
      c.n1 = i;
      c.n2 = 2;
      c.t = (rc.top - Balls[i]->rc.top)/Balls[i].vy;       
    }
    
    if (Balls[i]->rc.right >= rc.right){
      Col c;
      c.type1 = 0;
      c.type2 = -1;
      c.n1 = i;
      c.n2 = 1;
      c.t = (rc.right - Balls[i]->rc.right)/Balls[i].vx;                     
    } else if (Balls[i]->rc.left <= rc.left){
      Col c;
      c.type1 = 0;
      c.type2 = -1;
      c.n1 = i;
      c.n2 = 0;
      c.t = (rc.left - Balls[i]->rc.left)/Balls[i].vx;     
    } */
    
    for (int j = i+1;j < bn;j++){
      if (!Balls[j]->real) continue;
      double r2 = BallSizes[Balls[j]->size]/2.0;
      TPointR o2; 
      o2.x = (Balls[j]->rc.right+Balls[j]->rc.left)/2.0;
      o2.y = (Balls[j]->rc.top+Balls[j]->rc.bottom)/2.0;
      if ((r+r2)*(r+r2) > sqr(o.x-o2.x)+sqr(o.y-o2.y)){
        
        double pit = sqrt(sqr(o.x-o2.x)+sqr(o.y-o2.y));
        TPointR Vector,Vector2;
        Vector.x = (o.x-o2.x)/pit;
        Vector.y = (o.y-o2.y)/pit;
        Vector2.x = Vector.y;
        Vector2.y = -Vector.x;
        double dot1 = Vector.x*Balls[i]->vx + Vector.y*Balls[i]->vy;
        double dot2 = Vector.x*Balls[j]->vx + Vector.y*Balls[j]->vy;
        
        double dotp1 = Vector2.x*Balls[i]->vx + Vector2.y*Balls[i]->vy;
        double dotp2 = Vector2.x*Balls[j]->vx + Vector2.y*Balls[j]->vy;
        if (dot2 - dot1 > 0.000001){
          Balls[i]->vx = dot2*Vector.x + dotp1*Vector2.x;
          Balls[i]->vy = dot2*Vector.y + dotp1*Vector2.y;
          Balls[j]->vx = dot1*Vector.x + dotp2*Vector2.x;
          Balls[j]->vy = dot1*Vector.y + dotp2*Vector2.y;
          OffsetRectR(&Balls[i]->rc,(r+r2-pit)/(dot1-dot2)*(dot2-dot1)*Vector.x,(r+r2-pit)/(dot1-dot2)*(dot2-dot1)*Vector.y);
          OffsetRectR(&Balls[j]->rc,(r+r2-pit)/(dot2-dot1)*(dot1-dot2)*Vector.x,(r+r2-pit)/(dot2-dot1)*(dot1-dot2)*Vector.y);          
        } else {
          //Balls[i]->vx = dot1*Vector.x + dotp1*Vector2.x;
          //Balls[i]->vy = dot1*Vector.y + dotp1*Vector2.y;
          //Balls[j]->vx = dot2*Vector.x + dotp2*Vector2.x;
          //Balls[j]->vy = dot2*Vector.y + dotp2*Vector2.y; 
          OffsetRectR(&Balls[i]->rc,(r2+r-pit)*Vector.x/4.0,(r2+r-pit)*Vector.y/4.0);
          OffsetRectR(&Balls[j]->rc,-(r2+r-pit)*Vector.x/4.0,-(r2+r-pit)*Vector.y/4.0);
        }
                
      }      
    } 
    
    /*for (int j = 0;j < wn;j++){
      if (!Walls[j]->real) continue;
          
      //Simple Test 
      if ((((Balls[i]->rc.top >= Walls[j]->rc.top) && (Balls[i]->rc.top <= Walls[j]->rc.bottom)) || ((Balls[i]->rc.top <= Walls[j]->rc.top) && (Balls[i]->rc.bottom >= Walls[j]->rc.top)))
      && (((Balls[i]->rc.left >= Walls[j]->rc.left) && (Balls[i]->rc.left <= Walls[j]->rc.right)) || ((Balls[i]->rc.left <= Walls[j]->rc.left) && (Balls[i]->rc.right >= Walls[j]->rc.left)))){
        Walls[j]->alert = true;
        if (!(Walls[j]->a % 90)){
          Walls[j]->col = true;
          Balls[i]->vy = -Balls[i]->vy;
          Balls[i]->vx = -Balls[i]->vx;
        } else {  
      double fr,sr;
          fr = sqrt(1+sqr(Walls[j]->Bs[0].a));
          sr = sqrt(1+sqr(Walls[j]->Bs[2].a));
          if ((((Walls[j]->Bs[0].a*o.x-o.y+Walls[j]->Bs[0].b+fr*r*Walls[j]->Bs[0].side)*Walls[j]->Bs[0].side) >= 0) && (((Walls[j]->Bs[1].a*o.x-o.y+Walls[j]->Bs[1].b+fr*r*Walls[j]->Bs[1].side)*Walls[j]->Bs[1].side) >= 0)){
            if ((((Walls[j]->Bs[2].a*o.x-o.y+Walls[j]->Bs[2].b)*Walls[j]->Bs[2].side) >= 0) && (((Walls[j]->Bs[3].a*o.x-o.y+Walls[j]->Bs[3].b)*Walls[j]->Bs[3].side) >= 0)){
              if (((Walls[j]->Bs[0].a*o.x-o.y+Walls[j]->Bs[0].b)*Walls[j]->Bs[0].side) < 0){ //góra
                Walls[j]->col = true;
                TPointR Vector,Vector2;
                double ap = (-1)/Walls[j]->Bs[0].a;
                Vector.x = sqrt(1/(1+sqr(Walls[j]->Bs[0].a)));
                if (Walls[j]->Bs[0].a < 0) Vector.x = -Vector.x;
                Vector.y = Vector.x*Walls[j]->Bs[0].a;
                Vector2.x = sqrt(1/(1+sqr(ap)));
                if (ap < 0) Vector2.x = -Vector2.x;
                Vector2.y = Vector2.x*ap;
                double dot = Vector.x*Balls[i]->vx + Vector.y*Balls[i]->vy;
                double dot2 = Vector2.x*Balls[i]->vx + Vector2.y*Balls[i]->vy;
                dot *= (1-HITSIDEENERGYRED);
                dot2 *= (1-HITENERGYRED);
                Balls[i]->vx = dot*Vector.x-dot2*Vector2.x;
                Balls[i]->vy = dot*Vector.y-dot2*Vector2.y;
                double dot3 = Vector2.y*(Walls[j]->Bs[0].a*o.x-o.y+Walls[j]->Bs[0].b+fr*r*Walls[j]->Bs[0].side);
                OffsetRectR(&Balls[i]->rc,dot3*Vector2.x*(2-HITENERGYRED),dot3*Vector2.y*(2-HITENERGYRED));                                                               
              } else if (((Walls[j]->Bs[1].a*o.x-o.y+Walls[j]->Bs[1].b)*Walls[j]->Bs[1].side) < 0){ //dó³
                Walls[j]->col = true;
                TPointR Vector,Vector2;
                double ap = (-1)/Walls[j]->Bs[1].a;
                Vector.x = sqrt(1/(1+sqr(Walls[j]->Bs[1].a)));
                if (Walls[j]->Bs[1].a < 0) Vector.x = -Vector.x;
                Vector.y = Vector.x*Walls[j]->Bs[1].a;
                Vector2.x = sqrt(1/(1+sqr(ap)));
                if (ap < 0) Vector2.x = -Vector2.x;
                Vector2.y = Vector2.x*ap;
                double dot = Vector.x*Balls[i]->vx + Vector.y*Balls[i]->vy;
                double dot2 = Vector2.x*Balls[i]->vx + Vector2.y*Balls[i]->vy;
                dot *= (1-HITSIDEENERGYRED);
                dot2 *= (1-HITENERGYRED);
                Balls[i]->vx = dot*Vector.x-dot2*Vector2.x;
                Balls[i]->vy = dot*Vector.y-dot2*Vector2.y;
                double dot3 = Vector2.y*(Walls[j]->Bs[1].a*o.x-o.y+Walls[j]->Bs[1].b+fr*r*Walls[j]->Bs[1].side);
                OffsetRectR(&Balls[i]->rc,dot3*Vector2.x*(2-HITENERGYRED),dot3*Vector2.y*(2-HITENERGYRED));         
              } else { //Œrodek kuli znajduje siê w œrodku œciany
                /*Walls[j]->col = true;
                TPointR Vector;
                Vector.x = sqrt(1/(1+sqr(Walls[j]->Bs[0].a)));
                Vector.y = Vector.x*Walls[j]->Bs[0].a;
                double dot = Vector.x*Balls[i]->vx + Vector.y*Balls[i]->vy;
                dot *= 1-HITENERGYRED;
                Balls[i]->vx = -dot*Vector.x;
                Balls[i]->vy = -dot*Vector.y;
          }                                                                              
            } else {
              if ((((Walls[j]->Bs[2].a*o.x-o.y+Walls[j]->Bs[2].b+sr*r*Walls[j]->Bs[2].side)*Walls[j]->Bs[2].side) >= 0) && (((Walls[j]->Bs[3].a*o.x-o.y+Walls[j]->Bs[3].b+sr*r*Walls[j]->Bs[3].side)*Walls[j]->Bs[3].side) >= 0)){
                if ((((Walls[j]->Bs[0].a*o.x-o.y+Walls[j]->Bs[0].b)*Walls[j]->Bs[0].side) >= 0) && (((Walls[j]->Bs[1].a*o.x-o.y+Walls[j]->Bs[1].b)*Walls[j]->Bs[1].side) >= 0)){
                  if (((Walls[j]->Bs[2].a*o.x-o.y+Walls[j]->Bs[2].b)*Walls[j]->Bs[2].side) < 0){ //Prawy bok
                    Walls[j]->col = true;
                  TPointR Vector,Vector2;
                  double ap = (-1)/Walls[j]->Bs[2].a;
                  Vector.x = sqrt(1/(1+sqr(Walls[j]->Bs[2].a)));
                  if (Walls[j]->Bs[2].a < 0) Vector.x = -Vector.x;
                  Vector.y = Vector.x*Walls[j]->Bs[2].a;
                  Vector2.x = sqrt(1/(1+sqr(ap)));
                  if (ap < 0) Vector2.x = -Vector2.x;
                  Vector2.y = Vector2.x*ap;
                  double dot = Vector.x*Balls[i]->vx + Vector.y*Balls[i]->vy;
                  double dot2 = Vector2.x*Balls[i]->vx + Vector2.y*Balls[i]->vy;
                  dot *= (1-HITSIDEENERGYRED);
                dot2 *= (1-HITENERGYRED);
                  Balls[i]->vx = dot*Vector.x-dot2*Vector2.x;
                  Balls[i]->vy = dot*Vector.y-dot2*Vector2.y;
                  double dot3 = Vector2.y*(Walls[j]->Bs[2].a*o.x-o.y+Walls[j]->Bs[2].b+sr*r*Walls[j]->Bs[2].side);
                OffsetRectR(&Balls[i]->rc,dot3*Vector2.x*(2-HITENERGYRED),dot3*Vector2.y*(2-HITENERGYRED));                                                         
                  } else if (((Walls[j]->Bs[3].a*o.x-o.y+Walls[j]->Bs[3].b)*Walls[j]->Bs[3].side) < 0){ //Lewy bok
                     Walls[j]->col = true;
                TPointR Vector,Vector2;
                double ap = (-1)/Walls[j]->Bs[3].a;
                Vector.x = sqrt(1/(1+sqr(Walls[j]->Bs[3].a)));
                if (Walls[j]->Bs[3].a < 0) Vector.x = -Vector.x;
                Vector.y = Vector.x*Walls[j]->Bs[3].a;
                Vector2.x = sqrt(1/(1+sqr(ap)));
                if (ap < 0) Vector2.x = -Vector2.x;
                Vector2.y = Vector2.x*ap;
                double dot = Vector.x*Balls[i]->vx + Vector.y*Balls[i]->vy;
                double dot2 = Vector2.x*Balls[i]->vx + Vector2.y*Balls[i]->vy;
                dot *= (1-HITSIDEENERGYRED);
                dot2 *= (1-HITENERGYRED);
                Balls[i]->vx = dot*Vector.x-dot2*Vector2.x;
                Balls[i]->vy = dot*Vector.y-dot2*Vector2.y;
                double dot3 = Vector2.y*(Walls[j]->Bs[3].a*o.x-o.y+Walls[j]->Bs[3].b+sr*r*Walls[j]->Bs[3].side);
                OffsetRectR(&Balls[i]->rc,dot3*Vector2.x*(2-HITENERGYRED),dot3*Vector2.y*(2-HITENERGYRED));            
                  } else Walls[j]->col = true; //Niezidentyfikowane miejsce                                                                                                                          
                } else if (r*r >= sqr(o.x-Walls[j]->lt.x) + sqr(o.y-Walls[j]->lt.y)){
                  Walls[j]->col = true;
                  TPointR Vector,Vector2;
                  double pit = sqrt(sqr(o.x-Walls[j]->lt.x)+sqr(o.y-Walls[j]->lt.y));
                  Vector2.x = (o.x-Walls[j]->lt.x)/pit;
                  Vector2.y = (o.y-Walls[j]->lt.y)/pit;
                  Vector.x = Vector2.y;
                  Vector.y = -Vector2.x;
                  double dot = Vector.x*Balls[i]->vx + Vector.y*Balls[i]->vy;
                  double dot2 = Vector2.x*Balls[i]->vx + Vector2.y*Balls[i]->vy;
                  dot *= (1-HITSIDEENERGYRED);
                dot2 *= (1-HITENERGYRED);
                  Balls[i]->vx = dot*Vector.x-dot2*Vector2.x;
                  Balls[i]->vy = dot*Vector.y-dot2*Vector2.y;
                  OffsetRectR(&Balls[i]->rc,(r/pit-1)*(2-HITENERGYRED)*(o.x-Walls[j]->lt.x),(r/pit-1)*(2-HITENERGYRED)*(o.y-Walls[j]->lt.y)); 
                } else if (r*r >= sqr(o.x-Walls[j]->lb.x) + sqr(o.y-Walls[j]->lb.y)){
                  Walls[j]->col = true;
                  TPointR Vector,Vector2;
                  double pit = sqrt(sqr(o.x-Walls[j]->lb.x)+sqr(o.y-Walls[j]->lb.y));
                  Vector2.x = (o.x-Walls[j]->lb.x)/pit;
                  Vector2.y = (o.y-Walls[j]->lb.y)/pit;
                  Vector.x = Vector2.y;
                  Vector.y = -Vector2.x;
                  double dot = Vector.x*Balls[i]->vx + Vector.y*Balls[i]->vy;
                  double dot2 = Vector2.x*Balls[i]->vx + Vector2.y*Balls[i]->vy;
                  dot *= (1-HITSIDEENERGYRED);
                dot2 *= (1-HITENERGYRED);
                  Balls[i]->vx = dot*Vector.x-dot2*Vector2.x;
                  Balls[i]->vy = dot*Vector.y-dot2*Vector2.y;
                  OffsetRectR(&Balls[i]->rc,(r/pit-1)*(2-HITENERGYRED)*(o.x-Walls[j]->lb.x),(r/pit-1)*(2-HITENERGYRED)*(o.y-Walls[j]->lb.y));        
                } else if (r*r >= sqr(o.x-Walls[j]->rt.x) + sqr(o.y-Walls[j]->rt.y)){
                  Walls[j]->col = true;
                  TPointR Vector,Vector2;
                  double pit = sqrt(sqr(o.x-Walls[j]->rt.x)+sqr(o.y-Walls[j]->rt.y));
                  Vector2.x = (o.x-Walls[j]->rt.x)/pit;
                  Vector2.y = (o.y-Walls[j]->rt.y)/pit;
                  Vector.x = Vector2.y;
                  Vector.y = -Vector2.x;
                  double dot = Vector.x*Balls[i]->vx + Vector.y*Balls[i]->vy;
                  double dot2 = Vector2.x*Balls[i]->vx + Vector2.y*Balls[i]->vy;
                  dot *= (1-HITSIDEENERGYRED);
                dot2 *= (1-HITENERGYRED);
                  Balls[i]->vx = dot*Vector.x-dot2*Vector2.x;
                  Balls[i]->vy = dot*Vector.y-dot2*Vector2.y;  
                  OffsetRectR(&Balls[i]->rc,(r/pit-1)*(2-HITENERGYRED)*(o.x-Walls[j]->rt.x),(r/pit-1)*(2-HITENERGYRED)*(o.y-Walls[j]->rt.y));
                } else if (r*r >= sqr(o.x-Walls[j]->rb.x) + sqr(o.y-Walls[j]->rb.y)){
                  Walls[j]->col = true;
                  TPointR Vector,Vector2;
                  double pit = sqrt(sqr(o.x-Walls[j]->rb.x)+sqr(o.y-Walls[j]->rb.y));
                  Vector2.x = (o.x-Walls[j]->rb.x)/pit;
                  Vector2.y = (o.y-Walls[j]->rb.y)/pit;
                  Vector.x = Vector2.y;
                  Vector.y = -Vector2.x;
                  double dot = Vector.x*Balls[i]->vx + Vector.y*Balls[i]->vy;
                  double dot2 = Vector2.x*Balls[i]->vx + Vector2.y*Balls[i]->vy;
                  dot *= (1-HITSIDEENERGYRED);
                dot2 *= (1-HITENERGYRED);
                  Balls[i]->vx = dot*Vector.x-dot2*Vector2.x;
                  Balls[i]->vy = dot*Vector.y-dot2*Vector2.y;
                  OffsetRectR(&Balls[i]->rc,(r/pit-1)*(2-HITENERGYRED)*(o.x-Walls[j]->rb.x),(r/pit-1)*(2-HITENERGYRED)*(o.y-Walls[j]->rb.y));
                } //niewazne                                                                             
              }       
            }                                                                                
           
      }  
        }
        
      }
    }*/  
  }
     
}

void Draw(HDC hdc,RECT rc){
     
  rc.right -= rc.left;
  rc.bottom -= rc.top;
  rc.left = 0;
  rc.top = 0; 
  
  HDC BufferDC = CreateCompatibleDC(hdc);
  HBITMAP OldHbm = (HBITMAP)SelectObject(BufferDC,BufferHbm);
  
  HDC ColorDC = CreateCompatibleDC(hdc);
  HBITMAP OldColHbm = (HBITMAP)SelectObject(ColorDC,ColorHbm);
  
  HDC BufferDC2 = CreateCompatibleDC(hdc);                     
  HBITMAP OldHbm2 = (HBITMAP)SelectObject(BufferDC2,StarcraftHbms[0]);
                             
  HBRUSH BackgroundBrush = CreateSolidBrush(RGB(236,233,216));
  HBRUSH YellowBrush = CreateSolidBrush(0x00FFFF);
  HBRUSH RedBrush = CreateSolidBrush(0x0000FF);
                                      
  FillRect(BufferDC,&rc,BackgroundBrush);  
                    
  BITMAP bm;
  
  for (int i = 0;i < bn;i++){
    SelectObject(BufferDC2,BallMaskHbms[Balls[i]->size]);
    BitBlt(BufferDC,int(Balls[i]->rc.left+0.5),int(Balls[i]->rc.top+0.5),BallSizes[Balls[i]->size],BallSizes[Balls[i]->size],BufferDC2,0,0,SRCAND);
    SelectObject(BufferDC2,BallHbms[Balls[i]->size]);
    if (Balls[i]->col){
      RECT rc;
      rc.left = 0;
      rc.top = 0;
      rc.right = BallSizes[Balls[i]->size];
      rc.bottom = BallSizes[Balls[i]->size];
      FillRect(ColorDC,&rc,RedBrush);
      BitBlt(ColorDC,0,0,BallSizes[Balls[i]->size],BallSizes[Balls[i]->size],BufferDC2,0,0,SRCAND);
      BitBlt(BufferDC,int(Balls[i]->rc.left+0.5),int(Balls[i]->rc.top+0.5),BallSizes[Balls[i]->size],BallSizes[Balls[i]->size],ColorDC,0,0,SRCPAINT);  
    } else BitBlt(BufferDC,int(Balls[i]->rc.left+0.5),int(Balls[i]->rc.top+0.5),BallSizes[Balls[i]->size],BallSizes[Balls[i]->size],BufferDC2,0,0,SRCPAINT);
    //Add changing color when not real
  }
  
  for (int i = 0;i < wn;i++){
    if ((Walls[i]->a >359) || (Walls[i]->a < 0)) continue;              
    if (!Walls[i]->type){
      GetObject(WallHbms[Walls[i]->a],sizeof(BITMAP),&bm);
      RECT rc;
      rc.left = 0;
      rc.top = 0;
      rc.right = bm.bmWidth;
      rc.bottom = bm.bmHeight;
      if ((Walls[i]->a > 0) && (!(Walls[i]->a%90))){
        SelectObject(BufferDC2,WallHbms[Walls[i]->a]);
        if (Walls[i]->col){
          FillRect(ColorDC,&rc,RedBrush);
          BitBlt(ColorDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
          BitBlt(BufferDC,int(Walls[i]->rc.left+0.5),int(Walls[i]->rc.top+0.5),bm.bmWidth,bm.bmHeight,ColorDC,0,0,SRCCOPY);               
        } else if (Walls[i]->alert){
          FillRect(ColorDC,&rc,YellowBrush);
          BitBlt(ColorDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
          BitBlt(BufferDC,int(Walls[i]->rc.left+0.5),int(Walls[i]->rc.top+0.5),bm.bmWidth,bm.bmHeight,ColorDC,0,0,SRCCOPY);       
        } else BitBlt(BufferDC,int(Walls[i]->rc.left+0.5),int(Walls[i]->rc.top+0.5),bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCCOPY);                
      } else {
        SelectObject(BufferDC2,WallMaskHbms[Walls[i]->a]);
        BitBlt(BufferDC,int(Walls[i]->rc.left+0.5),int(Walls[i]->rc.top+0.5),bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
        SelectObject(BufferDC2,WallHbms[Walls[i]->a]);
        if (Walls[i]->col){
          FillRect(ColorDC,&rc,RedBrush);
          BitBlt(ColorDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
          BitBlt(BufferDC,int(Walls[i]->rc.left+0.5),int(Walls[i]->rc.top+0.5),bm.bmWidth,bm.bmHeight,ColorDC,0,0,SRCPAINT);               
        } else if (Walls[i]->alert){
          FillRect(ColorDC,&rc,YellowBrush);
          BitBlt(ColorDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
          BitBlt(BufferDC,int(Walls[i]->rc.left+0.5),int(Walls[i]->rc.top+0.5),bm.bmWidth,bm.bmHeight,ColorDC,0,0,SRCPAINT);       
        } else BitBlt(BufferDC,int(Walls[i]->rc.left+0.5),int(Walls[i]->rc.top+0.5),bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCPAINT);    
      }
    } else {
      GetObject(StarcraftHbms[Walls[i]->a],sizeof(BITMAP),&bm);
      RECT rc;
      rc.left = 0;
      rc.top = 0;
      rc.right = bm.bmWidth;
      rc.bottom = bm.bmHeight;
      if ((Walls[i]->a) && (!(Walls[i]->a%90))){
        SelectObject(BufferDC2,StarcraftHbms[Walls[i]->a]);
        if (Walls[i]->col){
          FillRect(ColorDC,&rc,RedBrush);
          BitBlt(ColorDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
          BitBlt(BufferDC,int(Walls[i]->rc.left+0.5),int(Walls[i]->rc.top+0.5),bm.bmWidth,bm.bmHeight,ColorDC,0,0,SRCCOPY);               
        } else if (Walls[i]->alert){
          FillRect(ColorDC,&rc,YellowBrush);
          BitBlt(ColorDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
          BitBlt(BufferDC,int(Walls[i]->rc.left+0.5),int(Walls[i]->rc.top+0.5),bm.bmWidth,bm.bmHeight,ColorDC,0,0,SRCCOPY);       
        } else BitBlt(BufferDC,int(Walls[i]->rc.left+0.5),int(Walls[i]->rc.top+0.5),bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCCOPY);                
      } else {
        SelectObject(BufferDC2,StarcraftMaskHbms[Walls[i]->a]);
        BitBlt(BufferDC,int(Walls[i]->rc.left+0.5),int(Walls[i]->rc.top+0.5),bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
        SelectObject(BufferDC2,StarcraftHbms[Walls[i]->a]);
        if (Walls[i]->col){
          FillRect(ColorDC,&rc,RedBrush);
          BitBlt(ColorDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
          BitBlt(BufferDC,int(Walls[i]->rc.left+0.5),int(Walls[i]->rc.top+0.5),bm.bmWidth,bm.bmHeight,ColorDC,0,0,SRCPAINT);               
        } else if (Walls[i]->alert){
          FillRect(ColorDC,&rc,YellowBrush);
          BitBlt(ColorDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
          BitBlt(BufferDC,int(Walls[i]->rc.left+0.5),int(Walls[i]->rc.top+0.5),bm.bmWidth,bm.bmHeight,ColorDC,0,0,SRCPAINT);       
        } else BitBlt(BufferDC,int(Walls[i]->rc.left+0.5),int(Walls[i]->rc.top+0.5),bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCPAINT);    
      }       
    }
    /*for (int j = 0;j <= 2;j++){
      SetPixel(BufferDC,int(Walls[i]->lt.x),int(Walls[i]->lt.y)+j,0x0000FF);
      SetPixel(BufferDC,int(Walls[i]->lt.x)+1,int(Walls[i]->lt.y)+j,0x0000FF);
      SetPixel(BufferDC,int(Walls[i]->lb.x),int(Walls[i]->lb.y)+j,0x0000FF);
      SetPixel(BufferDC,int(Walls[i]->lb.x)+1,int(Walls[i]->lb.y)+j,0x0000FF);
      SetPixel(BufferDC,int(Walls[i]->rt.x),int(Walls[i]->rt.y)+j,0x0000FF);
      SetPixel(BufferDC,int(Walls[i]->rt.x)+1,int(Walls[i]->rt.y)+j,0x0000FF);
      SetPixel(BufferDC,int(Walls[i]->rb.x),int(Walls[i]->rb.y)+j,0x0000FF);
      SetPixel(BufferDC,int(Walls[i]->rb.x)+1,int(Walls[i]->rb.y)+j,0x0000FF);
    }
    if (Walls[i]->a % 90){
      POINT lol;
      int r = 20;
      MoveToEx(BufferDC,0,int(Walls[i]->Bs[0].b+sqrt(1+sqr(Walls[i]->Bs[0].a))*r*Walls[i]->Bs[0].side),&lol);
      LineTo(BufferDC,1000,int(Walls[i]->Bs[0].b+sqrt(1+sqr(Walls[i]->Bs[0].a))*r*Walls[i]->Bs[0].side+Walls[i]->Bs[0].a*1000));
      MoveToEx(BufferDC,0,int(Walls[i]->Bs[1].b+sqrt(1+sqr(Walls[i]->Bs[1].a))*r*Walls[i]->Bs[1].side),&lol);
      LineTo(BufferDC,1000,int(Walls[i]->Bs[1].b+sqrt(1+sqr(Walls[i]->Bs[1].a))*r*Walls[i]->Bs[1].side+Walls[i]->Bs[1].a*1000));
      MoveToEx(BufferDC,0,int(Walls[i]->Bs[2].b+sqrt(1+sqr(Walls[i]->Bs[2].a))*r*Walls[i]->Bs[2].side),&lol);
      LineTo(BufferDC,1000,int(Walls[i]->Bs[2].b+sqrt(1+sqr(Walls[i]->Bs[2].a))*r*Walls[i]->Bs[2].side+Walls[i]->Bs[2].a*1000));
      MoveToEx(BufferDC,0,int(Walls[i]->Bs[3].b+sqrt(1+sqr(Walls[i]->Bs[3].a))*r*Walls[i]->Bs[3].side),&lol);
      LineTo(BufferDC,1000,int(Walls[i]->Bs[3].b+sqrt(1+sqr(Walls[i]->Bs[3].a))*r*Walls[i]->Bs[3].side+Walls[i]->Bs[3].a*1000));  
    }*/
  }
    
  BitBlt(hdc,0,0,rc.right,rc.bottom,BufferDC,0,0,SRCCOPY);
  
  SelectObject(BufferDC,OldHbm);
  SelectObject(BufferDC2,OldHbm2);          
  DeleteObject(BackgroundBrush);
  DeleteObject(RedBrush);
  DeleteObject(YellowBrush);
  DeleteDC(BufferDC); 
  DeleteDC(BufferDC2);
  SelectObject(ColorDC,OldColHbm);
  DeleteDC(ColorDC);
  
}

TBall* AddBall(int x,int y,int size,double vx,double vy,bool real){
  
  if (bn < MAXBALLS){
    Balls[bn] = new TBall;
    Balls[bn]->rc.left = x-BallSizes[size]/2;
    Balls[bn]->rc.right = Balls[bn]->rc.left + BallSizes[size];
    Balls[bn]->rc.top = y-BallSizes[size]/2;
    Balls[bn]->rc.bottom = Balls[bn]->rc.top + BallSizes[size];
    Balls[bn]->size = size;
    Balls[bn]->vx = vx;
    Balls[bn]->vy = vy;
    Balls[bn]->real = real;
    Balls[bn]->n = bn;
    return Balls[bn++];     
  } else return NULL;
       
}

TWall* AddWall(int x,int y,bool type,int a,bool real){
  
  if (wn < MAXWALLS){
    BITMAP bm;
    Walls[wn] = new TWall;
    Walls[wn]->type = type;
    Walls[wn]->a = a;
    Walls[wn]->real = real;
    POINT Mid,Mid2;
    if (type){
      GetObject(StarcraftHbms[a],sizeof(BITMAP),&bm);
      Walls[wn]->rc.left = x-StarcraftMids[a].x;
      Walls[wn]->rc.right = Walls[wn]->rc.left + bm.bmWidth;
      Walls[wn]->rc.top = y-StarcraftMids[a].y;
      Walls[wn]->rc.bottom = Walls[wn]->rc.top + bm.bmHeight;
      Mid.x = StarcraftMids[0].x;
      Mid.y = StarcraftMids[0].y;
      Mid2.x = StarcraftMids[a].x;
      Mid2.y = StarcraftMids[a].y;
      GetObject(StarcraftHbms[0],sizeof(BITMAP),&bm);          
    } else {
      GetObject(WallHbms[a],sizeof(BITMAP),&bm);
      Walls[wn]->rc.left = x-WallMids[a].x;
      Walls[wn]->rc.right = Walls[wn]->rc.left + bm.bmWidth;
      Walls[wn]->rc.top = y-WallMids[a].y;
      Walls[wn]->rc.bottom = Walls[wn]->rc.top + bm.bmHeight;
      Mid.x = WallMids[0].x;
      Mid.y = WallMids[0].y;
      Mid2.x = WallMids[a].x;
      Mid2.y = WallMids[a].y;
      GetObject(WallHbms[0],sizeof(BITMAP),&bm);          
    }
    GetPoints(bm.bmWidth,bm.bmHeight,int(Walls[wn]->rc.left),int(Walls[wn]->rc.top),a,Mid,Mid2,Walls[wn]->lt,Walls[wn]->rt,Walls[wn]->lb,Walls[wn]->rb);
    if (a % 90){
      if (Walls[wn]->rt.x > Walls[wn]->lt.x){
        Walls[wn]->Bs[0].a = (Walls[wn]->rt.y-Walls[wn]->lt.y)/(Walls[wn]->rt.x-Walls[wn]->lt.x);
      } else Walls[wn]->Bs[0].a = (Walls[wn]->lt.y-Walls[wn]->rt.y)/(Walls[wn]->lt.x-Walls[wn]->rt.x);
      Walls[wn]->Bs[0].b = Walls[wn]->lt.y - Walls[wn]->lt.x*Walls[wn]->Bs[0].a;
      Walls[wn]->Bs[0].side = int((Walls[wn]->Bs[0].a*Walls[wn]->rb.x-Walls[wn]->rb.y+Walls[wn]->Bs[0].b)/Abs(Walls[wn]->Bs[0].a*Walls[wn]->rb.x-Walls[wn]->rb.y+Walls[wn]->Bs[0].b));
      
      if (Walls[wn]->rb.x > Walls[wn]->lb.x){
        Walls[wn]->Bs[1].a = (Walls[wn]->rb.y-Walls[wn]->lb.y)/(Walls[wn]->rb.x-Walls[wn]->lb.x);
      } else Walls[wn]->Bs[1].a = (Walls[wn]->lb.y-Walls[wn]->rb.y)/(Walls[wn]->lb.x-Walls[wn]->rb.x);
      Walls[wn]->Bs[1].b = Walls[wn]->lb.y - Walls[wn]->lb.x*Walls[wn]->Bs[1].a;
      Walls[wn]->Bs[1].side = int((Walls[wn]->Bs[1].a*Walls[wn]->lt.x-Walls[wn]->lt.y+Walls[wn]->Bs[1].b)/Abs(Walls[wn]->Bs[1].a*Walls[wn]->lt.x-Walls[wn]->lt.y+Walls[wn]->Bs[1].b));
      
      
      if (Walls[wn]->rt.x > Walls[wn]->rb.x){
        Walls[wn]->Bs[2].a = (Walls[wn]->rt.y-Walls[wn]->rb.y)/(Walls[wn]->rt.x-Walls[wn]->rb.x);
      } else Walls[wn]->Bs[2].a = (Walls[wn]->rb.y-Walls[wn]->rt.y)/(Walls[wn]->rb.x-Walls[wn]->rt.x);
      Walls[wn]->Bs[2].b = Walls[wn]->rb.y - Walls[wn]->rb.x*Walls[wn]->Bs[2].a;
      Walls[wn]->Bs[2].side = int((Walls[wn]->Bs[2].a*Walls[wn]->lt.x-Walls[wn]->lt.y+Walls[wn]->Bs[2].b)/Abs(Walls[wn]->Bs[2].a*Walls[wn]->lt.x-Walls[wn]->lt.y+Walls[wn]->Bs[2].b));
      
      if (Walls[wn]->lb.x > Walls[wn]->lt.x){
        Walls[wn]->Bs[3].a = (Walls[wn]->lb.y-Walls[wn]->lt.y)/(Walls[wn]->lb.x-Walls[wn]->lt.x);
      } else Walls[wn]->Bs[3].a = (Walls[wn]->lt.y-Walls[wn]->lb.y)/(Walls[wn]->lt.x-Walls[wn]->lb.x);
      Walls[wn]->Bs[3].b = Walls[wn]->lt.y - Walls[wn]->lt.x*Walls[wn]->Bs[3].a;
      Walls[wn]->Bs[3].side = int((Walls[wn]->Bs[3].a*Walls[wn]->rb.x-Walls[wn]->rb.y+Walls[wn]->Bs[3].b)/Abs(Walls[wn]->Bs[3].a*Walls[wn]->rb.x-Walls[wn]->rb.y+Walls[wn]->Bs[3].b));
    } 
    Walls[wn]->n = wn;
    return Walls[wn++]; 
  } else return NULL;                                                        
}

double GetTime(){
  LARGE_INTEGER Ticks;
  QueryPerformanceCounter (&Ticks);
  return (double)(Ticks.QuadPart / (double)Freq.QuadPart);
}
 
void GetPoints(int w,int h,int x,int y,int a,POINT p,POINT p2,TPointR &lt,TPointR &rt,TPointR &lb,TPointR &rb){

  double Cos = cos(DegToRad(a)); 
  double Sin = sin(DegToRad(a));

  rb.x = (w-p.x)*Cos + (-p.y)*Sin+x+p2.x;
  rb.y = (-p.y)*Cos - (w-p.x)*Sin+y+p2.y;
  
  rt.x = (w-p.x)*Cos + (h-p.y)*Sin+x+p2.x;
  rt.y = (h-p.y)*Cos - (w-p.x)*Sin+y+p2.y;            
                                      
  lt.x = (-p.x)*Cos + (h-p.y)*Sin+x+p2.x;
  lt.y = (h-p.y)*Cos - (-p.x)*Sin+y+p2.y;
  
  lb.x = (-p.x)*Cos + (-p.y)*Sin+x+p2.x;
  lb.y = (-p.y)*Cos - (-p.x)*Sin+y+p2.y;
           
}

bool DeleteBall(int x){
  if ((x >= 0) && (x < bn)){
    delete Balls[x];
    Balls[x] = Balls[--bn];
    Balls[x]->n = x;
    return true;     
  } else return false;  
}

bool DeleteWall(int x){
  if ((x >= 0) && (x < wn)){
    delete Walls[x];
    Walls[x] = Walls[--wn];
    Walls[x]->n = x;
    return true;       
  } else return false;  
}

bool DeleteBall(TBall* x){
  if (x != NULL){
    int p = x->n;
    delete x;
    Balls[p] = Balls[--bn];
    Balls[p]->n = p;
    return true;    
  } else return false;   
}

bool DeleteWall(TWall* x){
  if (x != NULL){
    int p = x->n;
    delete x;
    Walls[p] = Walls[--wn];
    Walls[p]->n = p;
    return true;    
  } else return false;   
}

bool PrepareWall(TWall* x){
  BITMAP bm;
    POINT Mid,Mid2;
    if (x->type){
      Mid.x = StarcraftMids[0].x;
      Mid.y = StarcraftMids[0].y;
      Mid2.x = StarcraftMids[x->a].x;
      Mid2.y = StarcraftMids[x->a].y;
      GetObject(StarcraftHbms[0],sizeof(BITMAP),&bm);          
    } else {
      Mid.x = WallMids[0].x;
      Mid.y = WallMids[0].y;
      Mid2.x = WallMids[x->a].x;
      Mid2.y = WallMids[x->a].y;
      GetObject(WallHbms[0],sizeof(BITMAP),&bm);          
    }
  GetPoints(bm.bmWidth,bm.bmHeight,int(x->rc.left),int(x->rc.top),x->a,Mid,Mid2,x->lt,x->rt,x->lb,x->rb);
    if (x->a % 90){
      if (x->rt.x > x->lt.x){
        x->Bs[0].a = (x->rt.y-x->lt.y)/(x->rt.x-x->lt.x);
      } else x->Bs[0].a = (x->lt.y-x->rt.y)/(x->lt.x-x->rt.x);
      x->Bs[0].b = x->lt.y - x->lt.x*x->Bs[0].a;
      x->Bs[0].side = int((x->Bs[0].a*x->rb.x-x->rb.y+x->Bs[0].b)/Abs(x->Bs[0].a*x->rb.x-x->rb.y+x->Bs[0].b));
      
      if (x->rb.x > x->lb.x){
        x->Bs[1].a = (x->rb.y-x->lb.y)/(x->rb.x-x->lb.x);
      } else x->Bs[1].a = (x->lb.y-x->rb.y)/(x->lb.x-x->rb.x);
      x->Bs[1].b = x->lb.y - x->lb.x*x->Bs[1].a;
      x->Bs[1].side = int((x->Bs[1].a*x->lt.x-x->lt.y+x->Bs[1].b)/Abs(x->Bs[1].a*x->lt.x-x->lt.y+x->Bs[1].b));
      
      
      if (x->rt.x > x->rb.x){
        x->Bs[2].a = (x->rt.y-x->rb.y)/(x->rt.x-x->rb.x);
      } else x->Bs[2].a = (x->rb.y-x->rt.y)/(x->rb.x-x->rt.x);
      x->Bs[2].b = x->rb.y - x->rb.x*x->Bs[2].a;
      x->Bs[2].side = int((x->Bs[2].a*x->lt.x-x->lt.y+x->Bs[2].b)/Abs(x->Bs[2].a*x->lt.x-x->lt.y+x->Bs[2].b));
      
      if (x->lb.x > x->lt.x){
        x->Bs[3].a = (x->lb.y-x->lt.y)/(x->lb.x-x->lt.x);
      } else x->Bs[3].a = (x->lt.y-x->lb.y)/(x->lt.x-x->lb.x);
      x->Bs[3].b = x->lt.y - x->lt.x*x->Bs[3].a;
      x->Bs[3].side = int((x->Bs[3].a*x->rb.x-x->rb.y+x->Bs[3].b)/Abs(x->Bs[3].a*x->rb.x-x->rb.y+x->Bs[3].b));
    }    
} 
