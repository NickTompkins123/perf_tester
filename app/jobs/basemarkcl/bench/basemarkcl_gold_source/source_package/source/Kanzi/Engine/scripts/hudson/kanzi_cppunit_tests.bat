@ECHO OFF
SET SOLUTION=Kanzi\Engine\configs\platforms\win32\Engine.sln
SET BUILDER="C:\Windows\Microsoft.NET\Framework\v3.5\msbuild"
SET CONFIG="ES2_IMG_Debug"
IF EXIST %SOLUTION%.proj DEL %SOLUTION%.proj
IF EXIST %SOLUTION%.cache DEL %SOLUTION%.cache
IF EXIST %WORKSPACE%/msbuild.log DEL %WORKSPACE%\msbuild.log
call %BUILDER% %SOLUTION% /t:"Tests\cppunit_tests:Rebuild" /p:Configuration=%CONFIG%;Platform=Win32 /fl /verbosity:quiet
copy %WORKSPACE%\Kanzi\Engine\tests\cppunit_tests\bin\testImage.png %WORKSPACE%\Kanzi\Engine\output\Win32\%CONFIG%
copy %WORKSPACE%\Kanzi\Engine\tests\cppunit_tests\bin\*.dll %WORKSPACE%\Kanzi\Engine\output\Win32\%CONFIG%
copy %WORKSPACE%\Kanzi\Engine\tests\cppunit_tests\bin\*.cfg %WORKSPACE%\Kanzi\Engine\output\Win32\%CONFIG%

cd %WORKSPACE%\Kanzi\Engine\output\Win32\%CONFIG%
call cppunit_tests.exe