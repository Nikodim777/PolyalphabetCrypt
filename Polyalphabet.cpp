#include <windows.h>
#include <stdio.h>

int Encrypt(TCHAR *text,TCHAR *textout,int *key,int num,int reg,int regim){//text,textout-содержание полей ввода и вывода;key-ключ;num-номер шифруемого символа;reg-режим работы;regim-определяет синхронный или асинхронный вывод
    if(text[num]==32){ textout[num]=text[num]; return 0; }
    if((text[num]>-65&&text[num]<0)||text[num]==-72){//Если русские буквы или 'ё'
        if(text[num]>-65&&text[num]<-32) text[num]+=32;//Если буква в верхнем регистре понижает регистр
        if(text[num]==-72) text[num]=-33;//Изменяет логический код 'ё' для удобства работы(сложения по модулю 33 с ключом).
        if(reg){//Если дешифрация
            textout[num]=text[num]-key[num%10];//Отнимает ключ
            if(textout[num]<-33) textout[num]=textout[num]%33;//Возвращает символ в диапазон приемлемых
        }else{
            textout[num]=text[num]+key[num%10];
            if(textout[num]>-1) textout[num]=textout[num]%33-33;
        }
        if(textout[num]==-33) textout[num]=-72;//Возвращяет символу 'ё' исходный код ASCII
    }else if(text[num]!=10&&text[num]!=13) return 1;//Если не нажата enter и введённый символ не буква русского языка возвращает ошибку
    else{
        if(regim) textout[num]=text[num];//Если шифрация асинхронная переписывает перенос строки посимвольно
        else{//Если шифрация синхронная (Была нажата Enter) переписывает перенос строки сразу весь.
           textout[num-1]='\r';
           textout[num]='\n';
        }
    }
    return 0;
}
