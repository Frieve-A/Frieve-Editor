//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("fomain.cpp", Fo_Main);
USEFORM("foselectcard.cpp", Fo_Select);
USEFORM("fosearch.cpp", Fo_Search);
USEFORM("foabout.cpp", Fo_About);
USEFORM("foedittext.cpp", Fo_EditText);
USEFORM("folabeledit.cpp", Fo_LabelEdit);
USEFORM("foview.cpp", Fo_View);
USEFORM("forandomanimation.cpp", Fo_RandomAnimation);
USEFORM("fofullscreen.cpp", Fo_FullScreen);
USEFORM("foinformation.cpp", Fo_Information);
USEFORM("foshape.cpp", Fo_Shape);
USEFORM("foprogress.cpp", Fo_Progress);
//---------------------------------------------------------------------------
#include "document.h"
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->Title = AppTitle;
         Application->Title = "Frieve Editor";
        Application->CreateForm(__classid(TFo_Main), &Fo_Main);
         Application->CreateForm(__classid(TFo_Search), &Fo_Search);
         Application->CreateForm(__classid(TFo_View), &Fo_View);
         Application->CreateForm(__classid(TFo_RandomAnimation), &Fo_RandomAnimation);
         Application->CreateForm(__classid(TFo_FullScreen), &Fo_FullScreen);
         Application->CreateForm(__classid(TFo_Shape), &Fo_Shape);
         Application->CreateForm(__classid(TFo_Progress), &Fo_Progress);
         Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    catch (...)
    {
         try
         {
             throw Exception("");
         }
         catch (Exception &exception)
         {
             Application->ShowException(&exception);
         }
    }
    return 0;
}
//---------------------------------------------------------------------------
