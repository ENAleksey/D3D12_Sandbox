#include "stdafx.h"
#include "Engine.h"
#include "App.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    Engine engine(1280, 720);
    return App::Run(&engine, hInstance, nCmdShow);
}
