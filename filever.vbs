Option Explicit
On Error Resume Next

Const DEFINE = "#define"

Dim FSO, TextStream
Set FSO = CreateObject("Scripting.FileSystemObject")

Wscript.Echo "Creating filever.h..."

Set TextStream = FSO.CreateTextFile("filever.h")

TextStream.WriteLine(DEFINE & " rmj " + "1")
TextStream.WriteLine(DEFINE & " rmn " & Cstr(Year(Date())))
TextStream.WriteLine(DEFINE & " rmm " & Cstr(Month(Date())))
TextStream.WriteLine(DEFINE & " rup " & Cstr(Day(Date())))


TextStream.Close

Wscript.Echo "Success!"