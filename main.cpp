#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <commdlg.h>
#include "Polyalphabet.h"
#include "resources.h"

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("Шифровальщик");
PAINTSTRUCT ps={0,0,700,500};
HWND log,inarea,outarea,static1,static2,static3;
TCHAR text[1001]="",textout[1001]="",name[260]="";
int key[10]={8,9,11,23,3,7,5,4,6,10},reg=0,err;

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    HACCEL haccel;
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = (HICON) LoadIcon(hThisInstance,MAKEINTRESOURCE(M_ICON32));
    wincl.hIconSm = (HICON) LoadIcon(hThisInstance,MAKEINTRESOURCE(M_ICON16));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = MAKEINTRESOURCE(ID_MENU);                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Шифровальщик"),       /* Title Text */
           WS_OVERLAPPED|WS_SYSMENU, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           700,                 /* The programs width */
           500,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );


    haccel=LoadAccelerators(hThisInstance,MAKEINTRESOURCE(ID_ACCEL));
    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        if(!TranslateAccelerator(hwnd,haccel,&messages)){
            /* Translate virtual-key messages into character messages */
            TranslateMessage(&messages);
            /* Send message to WindowProcedure */
            DispatchMessage(&messages);
        }
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            log=CreateWindow("STATIC","",WS_VISIBLE|WS_CHILD|SS_CENTER,0,415,685,35,hwnd,NULL,GetModuleHandle(0),NULL);
            static1=CreateWindow("STATIC","Исходник",WS_VISIBLE|WS_CHILD|SS_CENTER,0,0,340,30,hwnd,NULL,GetModuleHandle(0),NULL);
            static2=CreateWindow("STATIC","Зашифрованный",WS_VISIBLE|WS_CHILD|SS_CENTER,345,0,340,30,hwnd,NULL,GetModuleHandle(0),NULL);
            static3=CreateWindow("STATIC","Ключ : {8,9,11,23,3,7,5,4,6,10}",WS_VISIBLE|WS_CHILD|SS_CENTER,0,380,685,35,hwnd,NULL,GetModuleHandle(0),NULL);
            inarea=CreateWindow("EDIT","",WS_VISIBLE|WS_CHILD|ES_MULTILINE|WS_VSCROLL,0,30,340,350,hwnd,(HMENU)INAREA,GetModuleHandle(0),NULL);
            outarea=CreateWindow("EDIT","",WS_VISIBLE|WS_CHILD|ES_MULTILINE|WS_VSCROLL,345,30,340,350,hwnd,NULL,GetModuleHandle(0),NULL);
            break;
        case WM_PAINT:
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            UpdateWindow(hwnd);
            break;
        case WM_COMMAND:
            switch(LOWORD(wParam)){
               case ID_OPEN:
                   SetWindowText(log,"");
                   SetWindowText(inarea,"");
                   SetWindowText(outarea,"");
                   text[0]='\0';
                   textout[0]='\0';
                   OPENFILENAME file;
                   FILE *fd;
                   file={0};
                   file.lStructSize = sizeof(file);
                   file.hwndOwner = hwnd;
                   file.lpstrFile = NULL;
                   file.lpstrFilter = "Текстовые\0*.txt\0\0";
                   file.nFilterIndex = 1;
                   file.lpstrFileTitle = name;
                   file.nMaxFileTitle = sizeof(name);
                   file.lpstrInitialDir = "C:\\Users\\я\\Desktop\\Данные\\Учёба\\3 курс\\ИБ САПР";
                   file.Flags = OFN_EXPLORER;
                   GetOpenFileName(&file);
                   text[0]='\0';
                   fd=fopen(name,"rb");
                   if(fread(text,sizeof(char),1001,fd)==1001) SetWindowText(log,"Текст слишком большой и был обрезан!");
                   fclose(fd);
                   SetWindowText(inarea,text);
               case ID_ENCRYPTION:
                   int i,error;
                   GetWindowText(inarea,text,1001);
                   for(i=0,error=0;i<strlen(text);i++){
                      printf("%d-",text[i]);
                      error+=Encrypt(text,textout,&key[0],i,reg,1);
                      printf("%d ",textout[i]);
                   }
                   if(error) SetWindowText(log,"Некорректный символ!");
                   SetWindowText(outarea,textout);
               break;
               case ID_CHANGE:
                   if(reg){
                      SetWindowText(static1,"Исходник");
                      SetWindowText(static2,"Зашифрованный");
                   }else{
                      SetWindowText(static1,"Зашифрованный");
                      SetWindowText(static2,"Расшифрованный");
                   }
                   reg=!reg;
                   GetWindowText(outarea,text,1001);
                   SetWindowText(inarea,text);
                   SetWindowText(outarea,"");
               break;
               case ID_HOTHELP:
                   MessageBox(hwnd," Открыть\t\tctrl+O\n Сохранить\tctrl+S\n Выйти\t\tctrl+Q\n За/Дешифровать\tctrl+E\n Смена режима\tctrl+W\n Горячие клавиши\tctrl+H\n О методе\t\tctrl+M\n О программе\tctrl+P","Горячие клавиши",MB_OK);
               break;
               case ID_ABOUTM:
                   MessageBox(hwnd," Многоалфавитное шифрование заключается в том,\n что для последовательных символов шифруемого текста\n используются одноалфавитные методы с различными ключами.\n Например, первый символ заменяется по методу Цезаря\n со смещением 14, второй - со смещением 10.","О методе",MB_OK);
               break;
               case ID_ABOUTP:
                   MessageBox(hwnd,"       Программа предназначена для \n     шифрации и дешифрации текста","О программе",MB_OK);
               break;
               case ID_EXIT:
                   SendMessage(hwnd,WM_CLOSE,0,0);
               break;
               case INAREA:
                   int last;
                   if(HIWORD(wParam)==EN_CHANGE){
                       last=GetWindowText(inarea,text,1001);
                       if(last==1000){ SetWindowText(log,"Текст слишком большой и был обрезан!"); break;}
                       if(last!=0){
                           error=Encrypt(text,textout,&key[0],last-1,reg,0);
                           if(error){
                               SetWindowText(log,"Некорректный символ!");
                               text[last-1]='\0';
                               SetWindowText(inarea,text);
                               SendMessage(inarea,EM_SETSEL,last-1,last-1);
                               break;
                           }else SetWindowText(log,"");
                       }
                       textout[last]='\0';
                       SetWindowText(outarea,textout);
                   }
               break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
