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

#define MAXOBJECTS 1000
                                         
#define HITSIDEENERGYRED 0.3
#define HITENERGYRED 0.7

#define E 0.1

struct TPointR{
  double x,y;       
};

struct TRectR{
  double left,right;
  double top,bottom;       
};

struct TObject{
  int type; //0 - ball,1 - wall,2 - top,3 - bot,4 - left,5 - right
  int n;
  bool real;
  bool col;          
};

struct TBall : TObject{
  TRectR rc;
  int size;
  double vx,vy;    
};

struct TMapBorder : TObject{
  int pos;       
};

struct TBorder{
  double a,b;
  int side;       
};

struct TWall : TObject{
  bool wtype; //0-wall,1-starcraft
  TRectR rc;
  TPointR lt,lb,rt,rb;
  TBorder Bs[4];
  int a;   
};

double G,SPEED;

double lt;

Heap CH;
//int ColTable[MAXOBJECTS][MAXOBJECTS];

bool FULLSCREEN;
DEVMODE FS;
int WNDH,WNDW;

RECT WndRect;

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

TObject* Objects[MAXOBJECTS];
int n;

TObject* CObject;
int CMode;

int mx,my;

int BallSizes[6];                

void MakeGraphics();

void OffsetRectR(TRectR* rc,double dx,double dy);


void UpdateGame(double dt);
void UpdateGameWithout(double dt,int x,int y);
void CheckCollisions();
void EvaluateCollision(int x,int y,double e);
void SolveCollision(int x,int y);
void CheckUnrealCollisions();
void Draw(HDC hdc,RECT rc);

bool Collides(int x,int y);
bool CollidesBorB(int x,int y);
bool CollidesBorW(int x,int y);
bool CollidesBB(int x,int y);
bool CollidesBW(int x,int y);
bool CollidesWW(int x,int y);

void EvColBorB(int x,int y,double e);
void EvColBorW(int x,int y,double e);
void EvColBB(int x,int y,double e);
void EvColBW(int x,int y,double e);
void EvColWW(int x,int y,double e);

void SolveColBorB(int x,int y);
void SolveColBorW(int x,int y);
void SolveColBB(int x,int y);
void SolveColBW(int x,int y);
void SolveColWW(int x,int y);

TObject* AddBall(int x,int y,int size,double vx,double vy,bool real);
TObject* AddWall(int x,int y,bool type,int a,bool real);
bool AddBorder(RECT rc);

bool DeleteObject(int x);
bool DeleteObject(TObject* x);

bool PrepareWall(TWall* x); 

void GetPoints(int w,int h,int x,int y,int a,POINT p,POINT p2,TPointR &lt,TPointR &rt,TPointR &lb,TPointR &rb);

double GetTime();

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain (HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszArgument,int nFunsterStil){
    
    AllocConsole();
     
    FULLSCREEN = false;
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
    
    HWND HColL = CreateWindow("STATIC","Collision",SS_LEFT|WS_VISIBLE|WS_CHILD,WNDW-PANELWIDTH+10,360,100,15,hwnd,NULL,hInstance,NULL);
    
    MakeGraphics();
    
    CheckRadioButton(hwnd,ID_BTNSELECT,ID_BTNBUBBLE6,ID_BTNBUBBLE3);
    
    G = 1000;
    SPEED = 1;
    
    CMode = ID_BTNBUBBLE3;
    CObject = AddBall(WNDW,0,2,0,0,false);
    mx = WNDW;
    my = 0; 
    
    GetClientRect(hwnd,&WndRect);
    WndRect.right -= WndRect.left;
    WndRect.bottom -= WndRect.top;
    WndRect.left = 0;
    WndRect.top = 0;
    WndRect.right -= PANELWIDTH;
    AddBorder(WndRect);
    
    ShowWindow (hwnd, nFunsterStil);
    
    if (!QueryPerformanceFrequency(&Freq)){
      return 0;                                       
      
    };                                                                                      
    
    MSG msg;
    
    double ts,t;
    double lrt,rt;
    
    ts = GetTime();
    lt = ts;                  
    lrt = lt;
    
    CheckCollisions();

    while (msg.message != WM_QUIT){  
    
      if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
        TranslateMessage (&msg);
        DispatchMessage (&msg);
      } else {
        rt = GetTime();
        t = (rt-lrt)*SPEED+lt;
        lrt = rt;
        SetWindowText(HColL,inttostr(CH.GiveSize()));
        //if (CH.GiveSize()) SPEED = 0;
        while ((CH.GiveSize()) && (CH.Top() <= t)){
          Col c = CH.Take();
          cout << "took collision between " << inttostr(c.n1) << " and " << inttostr(c.n2) << "\n";           
          if (Collides(c.n1,c.n2)){
            cout << "They collide\n";
            SolveCollision(c.n1,c.n2);
            UpdateGameWithout((c.t-lt)*SPEED,c.n1,c.n2);
            lt = c.t;
            for (int i = 0;i < n;i++){
              if (i == c.n1) EvaluateCollision(c.n1,c.n2,E);
              else if (i != c.n2){
                EvaluateCollision(c.n1,i,E);
                EvaluateCollision(c.n2,i,E);     
              }   
            }
          } else {
            cout << "They dont collide\n";
            EvaluateCollision(c.n1,c.n2,c.e/10);  
          }   
        } 
        if (lt < t) UpdateGame(t-lt);
        CheckUnrealCollisions();    
        lt = t;
        HDC hdc = GetDC(hwnd);
        Draw(hdc,WndRect);
        ReleaseDC(hwnd,hdc);
      }
      
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
  switch (message){
    case WM_DESTROY:{
      PostQuitMessage(0);   
    } break;
    case WM_COMMAND:{
      if ((wParam >= ID_BTNSELECT) && (wParam <= ID_BTNBUBBLE6) && (wParam != CMode)){
        SetFocus(hwnd);
        CheckRadioButton(hwnd,ID_BTNSELECT,ID_BTNBUBBLE6,wParam);
        DeleteObject(CObject);    
        if ((wParam == ID_BTNWALL) || (wParam == ID_BTNSTARCRAFT)){                           
          CObject = AddWall(WNDW,0,wParam-ID_BTNWALL,63,false);
          mx = WNDW;
          my = 0;             
        } else if (wParam > ID_BTNSTARCRAFT){
          CObject = AddBall(WNDW,0,wParam-ID_BTNBUBBLE1,0,0,false);
          mx = WNDW;
          my = 0;       
        }
        CMode = wParam;            
      }
    } break;
    case WM_MOUSEMOVE:{
      if (CMode){
        int MX = LOWORD(lParam);
        int MY = HIWORD(lParam);
        MX -= WndRect.left;
        MY -= WndRect.top;
        if ((CMode == ID_BTNWALL) || (CMode == ID_BTNSTARCRAFT)){                           
          OffsetRectR(&(((TWall*)CObject)->rc),MX-mx,MY-my);
          CObject->col = true;            
        } else if (CMode > ID_BTNSTARCRAFT){
          OffsetRectR(&(((TBall*)CObject)->rc),MX-mx,MY-my);
          CObject->col = true;      
        }  
        mx = MX;
        my = MY;
      }   
    } break;
    case WM_LBUTTONDOWN:{
      if (CObject->col) break;
      int MX = LOWORD(lParam);
      int MY = HIWORD(lParam);
      if ((CMode == ID_BTNWALL) || (CMode == ID_BTNSTARCRAFT)){
        ((TWall*)CObject)->real = true;
        PrepareWall((TWall*)CObject);
        CObject = AddWall(MX,MY,CMode-ID_BTNWALL,((TWall*)CObject)->a,false);
          mx = MX;
          my = MY;         
      } else if (CMode > ID_BTNSTARCRAFT){
        ((TBall*)CObject)->real = true;
        for (int i = 0;i < n;i++) if (i != CObject->n) EvaluateCollision(i,CObject->n,E);
        CObject = AddBall(MX,MY,CMode-ID_BTNBUBBLE1,0,0,false);
        mx = MX;
        my = MY;        
      }   
    } break;
    case WM_KEYDOWN:{
      /*switch (wParam){
        case VK_LEFT:{
          if ((CMode == ID_BTNWALL) || (CMode == ID_BTNSTARCRAFT)){
            int z = ((TWall*)citem)->a;
            ((TWall*)citem)->a = (z+361)%360;
            if (CMode == ID_BTNWALL){
              OffsetRectR(&((TWall*)citem)->rc,WallMids[z].x-WallMids[((TWall*)citem)->a].x,WallMids[z].y-WallMids[((TWall*)citem)->a].y);             
            } else OffsetRectR(&((TWall*)citem)->rc,StarcraftMids[z].x-StarcraftMids[((TWall*)citem)->a].x,StarcraftMids[z].y-StarcraftMids[((TWall*)citem)->a].y);
          
          }   
        } break;
        case VK_RIGHT:{
          int z = ((TWall*)citem)->a;
          ((TWall*)citem)->a = (z+359)%360;
          if (CMode == ID_BTNWALL){
              OffsetRectR(&((TWall*)citem)->rc,WallMids[z].x-WallMids[((TWall*)citem)->a].x,WallMids[z].y-WallMids[((TWall*)citem)->a].y);             
            } else OffsetRectR(&((TWall*)citem)->rc,StarcraftMids[z].x-StarcraftMids[((TWall*)citem)->a].x,StarcraftMids[z].y-StarcraftMids[((TWall*)citem)->a].y);   
        } break;       
      }*/   
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

void UpdateGame(double dt){
               
  for (int i = 0;i < n;i++){
    
    if (!Objects[i]->real) continue;
    
    if (Objects[i]->type == 0){
      ((TBall*)Objects[i])->vy = ((TBall*)Objects[i])->vy + G*dt;
      OffsetRectR(&((TBall*)Objects[i])->rc,((TBall*)Objects[i])->vx*dt,(((TBall*)Objects[i])->vy-G*dt/2.0)*dt);               
    }                                
      
    /*Balls[i]->col = false;
    if (!Balls[i]->real){
      if ((Balls[i]->rc.bottom > rc.bottom) || (Balls[i]->rc.top < rc.top)
        || (Balls[i]->rc.left < rc.left) || (Balls[i]->rc.right > rc.right)) Balls[i]->col = true;
      continue;  
    }  
    Balls[i]->vy = Balls[i]->vy + SPEED*G*dt;
    
    OffsetRectR(&Balls[i]->rc,Balls[i]->vx*SPEED*dt,(Balls[i]->vy-SPEED*G*dt/2)*SPEED*dt);  
    /*
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
    }*/
  }
     
}

void UpdateGameWithout(double dt,int a,int b){
  for (int i = 0;i < n;i++){
    
    if (!Objects[i]->real) continue;
    
    if (Objects[i]->type == 0){
      ((TBall*)Objects[i])->vy = ((TBall*)Objects[i])->vy + G*dt;
      OffsetRectR(&((TBall*)Objects[i])->rc,((TBall*)Objects[i])->vx*dt,(((TBall*)Objects[i])->vy-G*dt/2.0)*dt);               
    }
  }   
}

void CheckCollisions(){
     
  for (int i = 0;i < n;i++){
    if (!Objects[i]->real) continue;
    for (int j = i+1;j < n;j++){
      if (!Objects[j]->real) continue;
      cout << "ec " << inttostr(i) << " " << inttostr(j) << "\n";
      EvaluateCollision(i,j,E);    
    }    
  }
     
  /*for (int j = 0;j < wn;j++){ 
    Walls[j]->alert = false;
    Walls[j]->col = false;
  }*/
  /*for (int i = 0;i < bn;i++){
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
    
    /*for (int j = i+1;j < bn;j++){
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
  //}
     
}

void EvaluateCollision(int x,int y,double e){
  if ((Objects[x]->type > 1) && (Objects[y]->type > 1)) return;
  else if (Objects[x]->type > 1){
    if (Objects[y]->type == 0){
      cout << "ec borb\n";
      EvColBorB(x,y,e);                
    } else {  
      EvColBorW(x,y,e);     
    }     
  } else if (Objects[y]->type > 1){
    if (Objects[x]->type == 0){
      cout << "ec borb\n";
      EvColBorB(y,x,e);                
    } else {
      EvColBorW(y,x,e);     
    }       
  } else if (Objects[x]->type == 0){
    if (Objects[y]->type == 0){
      EvColBB(x,y,e);                   
    } else {
      EvColBW(x,y,e);     
    }        
  } else {
    if (Objects[y]->type == 0){
      EvColBW(y,x,e);                   
    } else {
      EvColWW(x,y,e);     
    }      
  }
  cout << "CH.size = " << inttostr(CH.GiveSize()) << "\n";   
}

void SolveCollision(int x,int y){
  if ((Objects[x]->type > 1) && (Objects[y]->type > 1)) return;
  else if (Objects[x]->type > 1){
    if (Objects[y]->type == 0){
      SolveColBorB(x,y);                   
    } else {  
      SolveColBorW(x,y);     
    }     
  } else if (Objects[y]->type > 1){
    if (Objects[x]->type == 0){
      SolveColBorB(y,x);                   
    } else {
      SolveColBorW(y,x);     
    }       
  } else if (Objects[x]->type == 0){
    if (Objects[y]->type == 0){
      SolveColBB(x,y);                   
    } else {
      SolveColBW(x,y);     
    }        
  } else {
    if (Objects[y]->type == 0){
      SolveColBW(y,x);                   
    } else {
      SolveColWW(x,y);     
    }      
  }  
}

void CheckUnrealCollisions(){
  for (int i = 0;i < n;i++){
    if (!Objects[i]->real){
      Objects[i]->col = false;
      for (int j = 0;j < n;j++){
        if (i != j) if (Collides(i,j)){
          Objects[i]->col = true;
          break;
        }      
      }
    }   
  }   
}

bool Collides(int x,int y){

  if ((Objects[x]->type > 1) && (Objects[y]->type > 1)) return false;
  else if (Objects[x]->type > 1){
    if (Objects[y]->type == 0){
      return CollidesBorB(x,y);                   
    } else {  
      return CollidesBorW(x,y);     
    }     
  } else if (Objects[y]->type > 1){
    if (Objects[x]->type == 0){
      return CollidesBorB(y,x);                   
    } else {
      return CollidesBorW(y,x);     
    }       
  } else if (Objects[x]->type == 0){
    if (Objects[y]->type == 0){
      return CollidesBB(x,y);                   
    } else {
      return CollidesBW(x,y);     
    }        
  } else {
    if (Objects[y]->type == 0){
      return CollidesBW(y,x);                   
    } else {
      return CollidesWW(x,y);     
    }      
  }
  return false;     
}

bool CollidesBorB(int x,int y){
  if (((TBall*)Objects[y])->real){
    cout << "Checking collision BorB\n";
    cout << "ball.bottom = " << inttostr(((TBall*)Objects[y])->rc.bottom*100) << "\n";
    cout << "border.pos = " << inttostr(((TMapBorder*)Objects[x])->pos*100) << "\n";
}
  if (Objects[x]->type == 2) return (((TBall*)Objects[y])->rc.top < ((TMapBorder*)Objects[x])->pos+E); 
  if (Objects[x]->type == 3) return (((TBall*)Objects[y])->rc.bottom > ((TMapBorder*)Objects[x])->pos-E); 
  if (Objects[x]->type == 4) return (((TBall*)Objects[y])->rc.left < ((TMapBorder*)Objects[x])->pos+E); 
  if (Objects[x]->type == 5) return (((TBall*)Objects[y])->rc.right > ((TMapBorder*)Objects[x])->pos-E);   
}

bool CollidesBorW(int x,int y){
  return false;   
}

bool CollidesBB(int x,int y){
  TPointR o1,o2;
  o1.x = (((TBall*)Objects[x])->rc.left + ((TBall*)Objects[x])->rc.right)/2.0;
  o1.y = (((TBall*)Objects[x])->rc.top + ((TBall*)Objects[x])->rc.bottom)/2.0;
  o2.x = (((TBall*)Objects[y])->rc.left + ((TBall*)Objects[y])->rc.right)/2.0;
  o2.y = (((TBall*)Objects[y])->rc.top + ((TBall*)Objects[y])->rc.bottom)/2.0;
  double r1,r2;
  r1 = BallSizes[((TBall*)Objects[x])->size]/2.0;
  r2 = BallSizes[((TBall*)Objects[y])->size]/2.0;
  return (sqr(o1.x-o2.x) + sqr(o1.y-o2.y) < sqr(r1+r2));   
}

bool CollidesBW(int x,int y){
  return false;   
}

bool CollidesWW(int x,int y){
  return false;   
}

void EvColBorB(int x,int y,double e){
  if (Objects[x]->type == 2){
    cout << "top /start\n";
    double s = (((TMapBorder*)Objects[x])->pos - ((TBall*)Objects[y])->rc.top);
    double d = 2*s*G + sqr(((TBall*)Objects[y])->vy);
    if (d >= 0){
      Col c;                 
      c.n1 = x;
      c.n2 = y;
      c.t = min(max(0,(-((TBall*)Objects[y])->vy)- sqrt(d))/G,max(0,(-((TBall*)Objects[y])->vy + sqrt(d))/G)) + lt;
      CH.Add(c);
      cout << "top\n"; 
    }
    return;                
  }
  if (Objects[x]->type == 3){
    cout << "bottom /start\n";
    double s = ((TMapBorder*)Objects[x])->pos - ((TBall*)Objects[y])->rc.bottom;
    double d = 2*s*G + sqr(((TBall*)Objects[y])->vy);
    cout << "s = " << inttostr(int(s)) << "." << inttostr(int(s*100)-int(s)*100) << "\n";
    cout << "d = " << inttostr(int(d)) << "." << inttostr(int(d*100)-int(d)*100) << "\n";
    if (d > 0){
      Col c;                 
      c.n1 = x;
      c.n2 = y;
      double t1 = (-((TBall*)Objects[y])->vy- sqrt(d))/G;
      double t2 = (-((TBall*)Objects[y])->vy + sqrt(d))/G;
      if ((t1 > 0) && (t2 > 0)) c.t = max(t1,t2) + lt;
      else if (t1 > 0) c.t = t1 + lt;
      else if (t2 > 0) c.t = t2 + lt;
      else return;
      cout << "t1 = " << inttostr(t1*10);
      cout << "t2 = " << inttostr(t2*10);
      cout << "time to collision - " << inttostr(int((c.t-lt)*1000)) << "\n";
      CH.Add(c);
      cout << "bottom\n"; 
    }
    return;                       
  } 
  /*if (Objects[x]->type == 4){
    if (((TBall*)Objects[y])->vx < 0){
      Col c;                 
      c.n1 = x;
      c.n2 = y;
      CH.Add(c);
      cout << "left\n";   
    }
    return;                       
  }
  if (Objects[x]->type == 5){
    if (((TBall*)Objects[y])->vx > 0){
      Col c;                 
      c.n1 = x;
      c.n2 = y;
      CH.Add(c);
      cout << "right\n";  
    }
    return;                      
  }*/
}

void EvColBorW(int x,int y,double e){
     
}

void EvColBB(int x,int y,double e){
     
}

void EvColBW(int x,int y,double e){
     
}

void EvColWW(int x,int y,double e){
     
}


void SolveColBorB(int x,int y){
  if (Objects[x]->type == 2){
    ((TBall*)Objects[y])->vy = -((TBall*)Objects[y])->vy;           
  }
  if (Objects[x]->type == 3){
    ((TBall*)Objects[y])->vy = -((TBall*)Objects[y])->vy;                  
  } 
  if (Objects[x]->type == 4){
    ((TBall*)Objects[y])->vx = -((TBall*)Objects[y])->vx;                 
  }
  if (Objects[x]->type == 5){
    ((TBall*)Objects[y])->vx = -((TBall*)Objects[y])->vx;                
  }   
}

void SolveColBorW(int x,int y){
     
}

void SolveColBB(int x,int y){
     
}

void SolveColBW(int x,int y){
     
}

void SolveColWW(int x,int y){
     
}


void Draw(HDC hdc,RECT rc){
  int l = rc.left;
  int t = rc.top;   
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
  HBRUSH GreenBrush = CreateSolidBrush(0x00FF00);
                                      
  FillRect(BufferDC,&rc,BackgroundBrush);  
                    
  BITMAP bm;
  
  for (int i = 0;i < n;i++){
    if (Objects[i]->type == 0){
                         
      SelectObject(BufferDC2,BallMaskHbms[((TBall*)Objects[i])->size]);
      BitBlt(BufferDC,int(((TBall*)Objects[i])->rc.left+0.5),int(((TBall*)Objects[i])->rc.top+0.5),BallSizes[((TBall*)Objects[i])->size],BallSizes[((TBall*)Objects[i])->size],BufferDC2,0,0,SRCAND);
      SelectObject(BufferDC2,BallHbms[((TBall*)Objects[i])->size]);
      if (!((TBall*)Objects[i])->real){
        RECT rc;
        rc.left = 0;
        rc.top = 0;
        rc.right = BallSizes[((TBall*)Objects[i])->size];
        rc.bottom = BallSizes[((TBall*)Objects[i])->size];
        if (((TBall*)Objects[i])->col) FillRect(ColorDC,&rc,RedBrush);
        else FillRect(ColorDC,&rc,GreenBrush);
        BitBlt(ColorDC,0,0,BallSizes[((TBall*)Objects[i])->size],BallSizes[((TBall*)Objects[i])->size],BufferDC2,0,0,SRCAND);
        BitBlt(BufferDC,int(((TBall*)Objects[i])->rc.left+0.5),int(((TBall*)Objects[i])->rc.top+0.5),BallSizes[((TBall*)Objects[i])->size],BallSizes[((TBall*)Objects[i])->size],ColorDC,0,0,SRCPAINT);  
      } else BitBlt(BufferDC,int(((TBall*)Objects[i])->rc.left+0.5),int(((TBall*)Objects[i])->rc.top+0.5),BallSizes[((TBall*)Objects[i])->size],BallSizes[((TBall*)Objects[i])->size],BufferDC2,0,0,SRCPAINT);                  
    
    } else if (Objects[i]->type == 1){
           
      if ((((TWall*)Objects[i])->a >359) || (((TWall*)Objects[i])->a < 0)) continue;              
    if (!((TWall*)Objects[i])->wtype){
      GetObject(WallHbms[((TWall*)Objects[i])->a],sizeof(BITMAP),&bm);
      RECT rc;
      rc.left = 0;
      rc.top = 0;
      rc.right = bm.bmWidth;
      rc.bottom = bm.bmHeight;
      if ((((TWall*)Objects[i])->a > 0) && (!(((TWall*)Objects[i])->a%90))){
        SelectObject(BufferDC2,WallHbms[((TWall*)Objects[i])->a]);
        if (((TWall*)Objects[i])->col){
          FillRect(ColorDC,&rc,RedBrush);
          BitBlt(ColorDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
          BitBlt(BufferDC,int(((TWall*)Objects[i])->rc.left+0.5),int(((TWall*)Objects[i])->rc.top+0.5),bm.bmWidth,bm.bmHeight,ColorDC,0,0,SRCCOPY);               
        } else BitBlt(BufferDC,int(((TWall*)Objects[i])->rc.left+0.5),int(((TWall*)Objects[i])->rc.top+0.5),bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCCOPY);                
      } else {
        SelectObject(BufferDC2,WallMaskHbms[((TWall*)Objects[i])->a]);
        BitBlt(BufferDC,int(((TWall*)Objects[i])->rc.left+0.5),int(((TWall*)Objects[i])->rc.top+0.5),bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
        SelectObject(BufferDC2,WallHbms[((TWall*)Objects[i])->a]);
        if (((TWall*)Objects[i])->col){
          FillRect(ColorDC,&rc,RedBrush);
          BitBlt(ColorDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
          BitBlt(BufferDC,int(((TWall*)Objects[i])->rc.left+0.5),int(((TWall*)Objects[i])->rc.top+0.5),bm.bmWidth,bm.bmHeight,ColorDC,0,0,SRCPAINT);               
        } else BitBlt(BufferDC,int(((TWall*)Objects[i])->rc.left+0.5),int(((TWall*)Objects[i])->rc.top+0.5),bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCPAINT);    
      }
    } else {
      GetObject(StarcraftHbms[((TWall*)Objects[i])->a],sizeof(BITMAP),&bm);
      RECT rc;
      rc.left = 0;
      rc.top = 0;
      rc.right = bm.bmWidth;
      rc.bottom = bm.bmHeight;
      if ((((TWall*)Objects[i])->a) && (!(((TWall*)Objects[i])->a%90))){
        SelectObject(BufferDC2,StarcraftHbms[((TWall*)Objects[i])->a]);
        if (((TWall*)Objects[i])->col){
          FillRect(ColorDC,&rc,RedBrush);
          BitBlt(ColorDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
          BitBlt(BufferDC,int(((TWall*)Objects[i])->rc.left+0.5),int(((TWall*)Objects[i])->rc.top+0.5),bm.bmWidth,bm.bmHeight,ColorDC,0,0,SRCCOPY);               
        } else BitBlt(BufferDC,int(((TWall*)Objects[i])->rc.left+0.5),int(((TWall*)Objects[i])->rc.top+0.5),bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCCOPY);                
      } else {
        SelectObject(BufferDC2,StarcraftMaskHbms[((TWall*)Objects[i])->a]);
        BitBlt(BufferDC,int(((TWall*)Objects[i])->rc.left+0.5),int(((TWall*)Objects[i])->rc.top+0.5),bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
        SelectObject(BufferDC2,StarcraftHbms[((TWall*)Objects[i])->a]);
        if (((TWall*)Objects[i])->col){
          FillRect(ColorDC,&rc,RedBrush);
          BitBlt(ColorDC,0,0,bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCAND);
          BitBlt(BufferDC,int(((TWall*)Objects[i])->rc.left+0.5),int(((TWall*)Objects[i])->rc.top+0.5),bm.bmWidth,bm.bmHeight,ColorDC,0,0,SRCPAINT);               
        } else BitBlt(BufferDC,int(((TWall*)Objects[i])->rc.left+0.5),int(((TWall*)Objects[i])->rc.top+0.5),bm.bmWidth,bm.bmHeight,BufferDC2,0,0,SRCPAINT);    
      }       
    }
    /*for (int j = 0;j <= 2;j++){
      SetPixel(BufferDC,int(((TWall*)Objects[i])->lt.x),int(((TWall*)Objects[i])->lt.y)+j,0x0000FF);
      SetPixel(BufferDC,int(((TWall*)Objects[i])->lt.x)+1,int(((TWall*)Objects[i])->lt.y)+j,0x0000FF);
      SetPixel(BufferDC,int(((TWall*)Objects[i])->lb.x),int(((TWall*)Objects[i])->lb.y)+j,0x0000FF);
      SetPixel(BufferDC,int(((TWall*)Objects[i])->lb.x)+1,int(((TWall*)Objects[i])->lb.y)+j,0x0000FF);
      SetPixel(BufferDC,int(((TWall*)Objects[i])->rt.x),int(((TWall*)Objects[i])->rt.y)+j,0x0000FF);
      SetPixel(BufferDC,int(((TWall*)Objects[i])->rt.x)+1,int(((TWall*)Objects[i])->rt.y)+j,0x0000FF);
      SetPixel(BufferDC,int(((TWall*)Objects[i])->rb.x),int(((TWall*)Objects[i])->rb.y)+j,0x0000FF);
      SetPixel(BufferDC,int(((TWall*)Objects[i])->rb.x)+1,int(((TWall*)Objects[i])->rb.y)+j,0x0000FF);
    }
    if (((TWall*)Objects[i])->a % 90){
      POINT lol;
      int r = 20;
      MoveToEx(BufferDC,0,int(((TWall*)Objects[i])->Bs[0].b+sqrt(1+sqr(((TWall*)Objects[i])->Bs[0].a))*r*((TWall*)Objects[i])->Bs[0].side),&lol);
      LineTo(BufferDC,1000,int(((TWall*)Objects[i])->Bs[0].b+sqrt(1+sqr(((TWall*)Objects[i])->Bs[0].a))*r*((TWall*)Objects[i])->Bs[0].side+((TWall*)Objects[i])->Bs[0].a*1000));
      MoveToEx(BufferDC,0,int(((TWall*)Objects[i])->Bs[1].b+sqrt(1+sqr(((TWall*)Objects[i])->Bs[1].a))*r*((TWall*)Objects[i])->Bs[1].side),&lol);
      LineTo(BufferDC,1000,int(((TWall*)Objects[i])->Bs[1].b+sqrt(1+sqr(((TWall*)Objects[i])->Bs[1].a))*r*((TWall*)Objects[i])->Bs[1].side+((TWall*)Objects[i])->Bs[1].a*1000));
      MoveToEx(BufferDC,0,int(((TWall*)Objects[i])->Bs[2].b+sqrt(1+sqr(((TWall*)Objects[i])->Bs[2].a))*r*((TWall*)Objects[i])->Bs[2].side),&lol);
      LineTo(BufferDC,1000,int(((TWall*)Objects[i])->Bs[2].b+sqrt(1+sqr(((TWall*)Objects[i])->Bs[2].a))*r*((TWall*)Objects[i])->Bs[2].side+((TWall*)Objects[i])->Bs[2].a*1000));
      MoveToEx(BufferDC,0,int(((TWall*)Objects[i])->Bs[3].b+sqrt(1+sqr(((TWall*)Objects[i])->Bs[3].a))*r*((TWall*)Objects[i])->Bs[3].side),&lol);
      LineTo(BufferDC,1000,int(((TWall*)Objects[i])->Bs[3].b+sqrt(1+sqr(((TWall*)Objects[i])->Bs[3].a))*r*((TWall*)Objects[i])->Bs[3].side+((TWall*)Objects[i])->Bs[3].a*1000));  
    }*/
          
    }    
  }
    
  BitBlt(hdc,l,t,rc.right,rc.bottom,BufferDC,0,0,SRCCOPY);
  
  SelectObject(BufferDC,OldHbm);
  SelectObject(BufferDC2,OldHbm2);          
  DeleteObject(BackgroundBrush);
  DeleteObject(RedBrush);
  DeleteObject(YellowBrush);
  DeleteObject(GreenBrush);
  DeleteDC(BufferDC); 
  DeleteDC(BufferDC2);
  SelectObject(ColorDC,OldColHbm);
  DeleteDC(ColorDC);
  
}

TObject* AddBall(int x,int y,int size,double vx,double vy,bool real){
  
  if (n < MAXOBJECTS){
    Objects[n] = new TBall;
    ((TBall*)Objects[n])->rc.left = x-BallSizes[size]/2;
    ((TBall*)Objects[n])->rc.right = ((TBall*)Objects[n])->rc.left + BallSizes[size];
    ((TBall*)Objects[n])->rc.top = y-BallSizes[size]/2;
    ((TBall*)Objects[n])->rc.bottom = ((TBall*)Objects[n])->rc.top + BallSizes[size];
    ((TBall*)Objects[n])->size = size;
    ((TBall*)Objects[n])->vx = vx;
    ((TBall*)Objects[n])->vy = vy;
    Objects[n]->real = real;
    Objects[n]->type = 0;
    Objects[n]->n = n;
    return Objects[n++];     
  } else return NULL;
       
}

TObject* AddWall(int x,int y,bool type,int a,bool real){
  
  if (n < MAXOBJECTS){
    BITMAP bm;
    Objects[n] = new TWall;
    ((TWall*)Objects[n])->wtype = type;
    ((TWall*)Objects[n])->a = a;
    Objects[n]->type = 1;
    Objects[n]->real = real;
    POINT Mid,Mid2;
    if (type){
      GetObject(StarcraftHbms[a],sizeof(BITMAP),&bm);
      ((TWall*)Objects[n])->rc.left = x-StarcraftMids[a].x;
      ((TWall*)Objects[n])->rc.right = ((TWall*)Objects[n])->rc.left + bm.bmWidth;
      ((TWall*)Objects[n])->rc.top = y-StarcraftMids[a].y;
      ((TWall*)Objects[n])->rc.bottom = ((TWall*)Objects[n])->rc.top + bm.bmHeight;
      Mid.x = StarcraftMids[0].x;
      Mid.y = StarcraftMids[0].y;
      Mid2.x = StarcraftMids[a].x;
      Mid2.y = StarcraftMids[a].y;
      GetObject(StarcraftHbms[0],sizeof(BITMAP),&bm);          
    } else {
      GetObject(WallHbms[a],sizeof(BITMAP),&bm);
      ((TWall*)Objects[n])->rc.left = x-WallMids[a].x;
      ((TWall*)Objects[n])->rc.right = ((TWall*)Objects[n])->rc.left + bm.bmWidth;
      ((TWall*)Objects[n])->rc.top = y-WallMids[a].y;
      ((TWall*)Objects[n])->rc.bottom = ((TWall*)Objects[n])->rc.top + bm.bmHeight;
      Mid.x = WallMids[0].x;
      Mid.y = WallMids[0].y;
      Mid2.x = WallMids[a].x;
      Mid2.y = WallMids[a].y;
      GetObject(WallHbms[0],sizeof(BITMAP),&bm);          
    }
    GetPoints(bm.bmWidth,bm.bmHeight,int(((TWall*)Objects[n])->rc.left),int(((TWall*)Objects[n])->rc.top),a,Mid,Mid2,((TWall*)Objects[n])->lt,((TWall*)Objects[n])->rt,((TWall*)Objects[n])->lb,((TWall*)Objects[n])->rb);
    if (a % 90){
      if (((TWall*)Objects[n])->rt.x > ((TWall*)Objects[n])->lt.x){
        ((TWall*)Objects[n])->Bs[0].a = (((TWall*)Objects[n])->rt.y-((TWall*)Objects[n])->lt.y)/(((TWall*)Objects[n])->rt.x-((TWall*)Objects[n])->lt.x);
      } else ((TWall*)Objects[n])->Bs[0].a = (((TWall*)Objects[n])->lt.y-((TWall*)Objects[n])->rt.y)/(((TWall*)Objects[n])->lt.x-((TWall*)Objects[n])->rt.x);
      ((TWall*)Objects[n])->Bs[0].b = ((TWall*)Objects[n])->lt.y - ((TWall*)Objects[n])->lt.x*((TWall*)Objects[n])->Bs[0].a;
      ((TWall*)Objects[n])->Bs[0].side = int((((TWall*)Objects[n])->Bs[0].a*((TWall*)Objects[n])->rb.x-((TWall*)Objects[n])->rb.y+((TWall*)Objects[n])->Bs[0].b)/Abs(((TWall*)Objects[n])->Bs[0].a*((TWall*)Objects[n])->rb.x-((TWall*)Objects[n])->rb.y+((TWall*)Objects[n])->Bs[0].b));
      
      if (((TWall*)Objects[n])->rb.x > ((TWall*)Objects[n])->lb.x){
        ((TWall*)Objects[n])->Bs[1].a = (((TWall*)Objects[n])->rb.y-((TWall*)Objects[n])->lb.y)/(((TWall*)Objects[n])->rb.x-((TWall*)Objects[n])->lb.x);
      } else ((TWall*)Objects[n])->Bs[1].a = (((TWall*)Objects[n])->lb.y-((TWall*)Objects[n])->rb.y)/(((TWall*)Objects[n])->lb.x-((TWall*)Objects[n])->rb.x);
      ((TWall*)Objects[n])->Bs[1].b = ((TWall*)Objects[n])->lb.y - ((TWall*)Objects[n])->lb.x*((TWall*)Objects[n])->Bs[1].a;
      ((TWall*)Objects[n])->Bs[1].side = int((((TWall*)Objects[n])->Bs[1].a*((TWall*)Objects[n])->lt.x-((TWall*)Objects[n])->lt.y+((TWall*)Objects[n])->Bs[1].b)/Abs(((TWall*)Objects[n])->Bs[1].a*((TWall*)Objects[n])->lt.x-((TWall*)Objects[n])->lt.y+((TWall*)Objects[n])->Bs[1].b));
      
      
      if (((TWall*)Objects[n])->rt.x > ((TWall*)Objects[n])->rb.x){
        ((TWall*)Objects[n])->Bs[2].a = (((TWall*)Objects[n])->rt.y-((TWall*)Objects[n])->rb.y)/(((TWall*)Objects[n])->rt.x-((TWall*)Objects[n])->rb.x);
      } else ((TWall*)Objects[n])->Bs[2].a = (((TWall*)Objects[n])->rb.y-((TWall*)Objects[n])->rt.y)/(((TWall*)Objects[n])->rb.x-((TWall*)Objects[n])->rt.x);
      ((TWall*)Objects[n])->Bs[2].b = ((TWall*)Objects[n])->rb.y - ((TWall*)Objects[n])->rb.x*((TWall*)Objects[n])->Bs[2].a;
      ((TWall*)Objects[n])->Bs[2].side = int((((TWall*)Objects[n])->Bs[2].a*((TWall*)Objects[n])->lt.x-((TWall*)Objects[n])->lt.y+((TWall*)Objects[n])->Bs[2].b)/Abs(((TWall*)Objects[n])->Bs[2].a*((TWall*)Objects[n])->lt.x-((TWall*)Objects[n])->lt.y+((TWall*)Objects[n])->Bs[2].b));
      
      if (((TWall*)Objects[n])->lb.x > ((TWall*)Objects[n])->lt.x){
        ((TWall*)Objects[n])->Bs[3].a = (((TWall*)Objects[n])->lb.y-((TWall*)Objects[n])->lt.y)/(((TWall*)Objects[n])->lb.x-((TWall*)Objects[n])->lt.x);
      } else ((TWall*)Objects[n])->Bs[3].a = (((TWall*)Objects[n])->lt.y-((TWall*)Objects[n])->lb.y)/(((TWall*)Objects[n])->lt.x-((TWall*)Objects[n])->lb.x);
      ((TWall*)Objects[n])->Bs[3].b = ((TWall*)Objects[n])->lt.y - ((TWall*)Objects[n])->lt.x*((TWall*)Objects[n])->Bs[3].a;
      ((TWall*)Objects[n])->Bs[3].side = int((((TWall*)Objects[n])->Bs[3].a*((TWall*)Objects[n])->rb.x-((TWall*)Objects[n])->rb.y+((TWall*)Objects[n])->Bs[3].b)/Abs(((TWall*)Objects[n])->Bs[3].a*((TWall*)Objects[n])->rb.x-((TWall*)Objects[n])->rb.y+((TWall*)Objects[n])->Bs[3].b));
    } 
    ((TWall*)Objects[n])->n = n;
    return Objects[n++];
  } else return NULL;                                                        
}

bool AddBorder(RECT rc){
  if (n+3 < MAXOBJECTS){
    
    Objects[n] = new TMapBorder;
    ((TMapBorder*)Objects[n])->pos = rc.top;
    Objects[n]->real = true;
    Objects[n]->type = 2;
    Objects[n++]->n = n;
    Objects[n] = new TMapBorder;
    ((TMapBorder*)Objects[n])->pos = rc.bottom;
    Objects[n]->real = true;
    Objects[n]->type = 3;
    Objects[n++]->n = n;
    Objects[n] = new TMapBorder;
    ((TMapBorder*)Objects[n])->pos = rc.left;
    Objects[n]->real = true;
    Objects[n]->type = 4;
    Objects[n++]->n = n;
    Objects[n] = new TMapBorder;
    ((TMapBorder*)Objects[n])->pos = rc.right;
    Objects[n]->real = true;
    Objects[n]->type = 5;
    Objects[n++]->n = n;
      
  } else return false;      
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

bool DeleteObject(int x){
  if ((x >= 0) && (x < n)){
    delete Objects[x];
    Objects[x] = Objects[--n];
    Objects[x]->n = x;
    return true;     
  } else return false;  
}

bool DeleteObject(TObject* x){
  if (x != NULL){
    int p = x->n;
    delete x;
    Objects[p] = Objects[--n];
    Objects[p]->n = p;
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
