#include "stdafx.h"
#include "LinkPython.h"

#pragma comments(lib,"python27.lib")

PyObject* PyModule = nullptr;
Py_ssize_t CommentsListLen = -1;
Py_ssize_t PicturesListLen = -1;
wchar_t** WeiboCommentslist = nullptr;
wchar_t** WeiboPictureslist = nullptr;

bool PythonEnvInit()
{
	Py_SetPythonHome("C:\\Python27");
	Py_Initialize();
	PyModule = PyImport_ImportModule((const char *)"PythonSpider");
	if (!PyModule || PyErr_Occurred())
	{
		MessageBoxA(NULL, "Python environment init failed.","ERROR",NULL);
		ExitProcess(0);
	}
	return true;
}

void ResumeEnv()
{
	if (WeiboCommentslist != nullptr && CommentsListLen > 0)
	{
		for (int index = 0; index < CommentsListLen; index++)
		{
			delete WeiboCommentslist[index];
		}
		delete[] WeiboCommentslist;
		wchar_t** WeiboCommentslist = nullptr;
	}
	if (WeiboPictureslist != nullptr && PicturesListLen > 0)
	{
		for (int index = 0; index < PicturesListLen; index++)
		{
			delete WeiboPictureslist[index];
		}
		delete[] WeiboPictureslist;
		wchar_t** WeiboPictureslist = nullptr;
	}
	
	if (CommentsListLen != -1)	CommentsListLen = -1;
	if (PicturesListLen != -1)	PicturesListLen = -1;
}

bool CatchWeiboContent(char * Url, CEdit &CEC)
{
	char* WeiboContent = new char[140];
	ZeroMemory(WeiboContent, 140);
	PyObject* PyCatchWeiboContent = PyObject_GetAttrString(PyModule, "CatchWeiboContent");
	if (PyCatchWeiboContent)
	{
		PyObject* PyParam = Py_BuildValue("(s)", Url);
		PyObject* PyContent = PyObject_CallObject(PyCatchWeiboContent, PyParam);
		if (PyContent)
		{
			PyArg_Parse(PyContent, "s", &WeiboContent);
		}
		USES_CONVERSION;
		wchar_t* WeiboContentWide = A2W(WeiboContent);
		CEC.SetWindowTextW(WeiboContentWide);
	}
	return WeiboContent;
}

bool CatchComments(char* Url, CListBox &ListResult)
{
	PyObject* PyCatchComments = PyObject_GetAttrString(PyModule, "CatchComments");
	if (PyCatchComments)
	{
		PyObject* PyParam = Py_BuildValue("(s)", Url);
		PyObject* PyList = PyObject_CallObject(PyCatchComments, PyParam);
		if (PyList == nullptr || PyList->ob_type == nullptr)
			return false;
		CommentsListLen = PyList_Size(PyList);
		if (CommentsListLen < 0 || CommentsListLen%2 != 0)
		{
			MessageBoxA(NULL, "PyList Size", "ERROR", NULL);
			return false;
		}
		WeiboCommentslist = new wchar_t*[CommentsListLen];
		for (Py_ssize_t index = 0; index < CommentsListLen; index++)
		{
			char* TempComment;
			wchar_t* CommentItem = new wchar_t[512];
			PyObject* TempItem = PyList_GetItem(PyList, index);
			PyArg_Parse(TempItem, "s", &TempComment);
			USES_CONVERSION;
			wcscpy_s(CommentItem, 512,A2W(TempComment));
			WeiboCommentslist[index] = CommentItem;
			if(index%2 == 0)
				ListResult.InsertString(index/2,WeiboCommentslist[index]);
		}
	}
	return true;
}

bool CatchPictures(char* Url, CListBox &ListResult)
{
	PyObject* PyCatchComments = PyObject_GetAttrString(PyModule, "CatchPictures");
	if (PyCatchComments)
	{
		PyObject* PyParam = Py_BuildValue("(s)", Url);
		PyObject* PyList = PyObject_CallObject(PyCatchComments, PyParam);
		if (PyList == nullptr || PyList->ob_type == nullptr)
			return false;
		PicturesListLen = PyList_Size(PyList);
		if (PicturesListLen < 0)
		{
			MessageBoxA(NULL, "PyList Size", "ERROR", NULL);
			return false;
		}
		if (PicturesListLen > 0)
		{
			WeiboPictureslist = new wchar_t*[PicturesListLen];
			for (Py_ssize_t index = 0; index < PicturesListLen; index++)
			{
				char* TempPicture = new char[512];
				PyObject* TempItem = PyList_GetItem(PyList, index);
				PyArg_Parse(TempItem, "s", &TempPicture);
				USES_CONVERSION;
				WeiboPictureslist[index] = A2W(TempPicture);
				ListResult.InsertString(index, WeiboPictureslist[index]);
			}
		}
	}
	return true;
}

void ExchangeLikePoints(int index, CListBox &ListResult)
{
	wchar_t* CurrentTxt = new wchar_t[512];
	ListResult.GetText(index, CurrentTxt);
	if (0 == wcscmp(CurrentTxt, WeiboCommentslist[index * 2]))
	{
		ListResult.DeleteString(index);
		ListResult.InsertString(index, WeiboCommentslist[index * 2 + 1]);
	}
	else
	{
		ListResult.DeleteString(index);
		ListResult.InsertString(index, WeiboCommentslist[index * 2]);
	}
}
