//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("forms\foview.cpp", Fo_View);
USEFORM("forms\fosearch.cpp", Fo_Search);
USEFORM("forms\foshape.cpp", Fo_Shape);
USEFORM("forms\foselectcard.cpp", Fo_Select);
USEFORM("forms\foinformation.cpp", Fo_Information);
USEFORM("forms\fofullscreen.cpp", Fo_FullScreen);
USEFORM("forms\folabeledit.cpp", Fo_LabelEdit);
USEFORM("forms\foabout.cpp", Fo_About);
USEFORM("forms\foedittext.cpp", Fo_EditText);
USEFORM("forms\foprogress.cpp", Fo_Progress);
USEFORM("forms\forandomcard.cpp", Fo_RandomCard);
USEFORM("forms\forandomanimation.cpp", Fo_RandomAnimation);
USEFORM("forms\fomain.cpp", Fo_Main);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TFo_Main), &Fo_Main);
		Application->CreateForm(__classid(TFo_FullScreen), &Fo_FullScreen);
		Application->CreateForm(__classid(TFo_Progress), &Fo_Progress);
		Application->CreateForm(__classid(TFo_RandomAnimation), &Fo_RandomAnimation);
		Application->CreateForm(__classid(TFo_Search), &Fo_Search);
		Application->CreateForm(__classid(TFo_Shape), &Fo_Shape);
		Application->CreateForm(__classid(TFo_View), &Fo_View);
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
