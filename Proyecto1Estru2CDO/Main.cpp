#include "Myform.h"
using namespace System;
using namespace System::Windows::Forms; 
[STAThreadAttribute]
int main(array<String^>^ args)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    Proyecto1Estru2CDO::MyForm form; 
    Application::Run(% form);
    return 0;
}
