#pragma once
#include "Python.h"

bool PythonEnvInit();

void ResumeEnv();

bool CatchWeiboContent(char * Url, CEdit &CEC);

bool CatchComments(char* Url, CListBox &ListResult);

bool CatchPictures(char* Url, CListBox &ListResult);

void ExchangeLikePoints(int index, CListBox &ListResult);