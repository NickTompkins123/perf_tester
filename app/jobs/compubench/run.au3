Run("bin/CompuBench.exe")

WinWaitActive("CompuBench")

WinSetState("CompuBench", "", @SW_MAXIMIZE)

# For debugging
#MouseClick("left", 134, 1135)
#MouseClick("left", 227, 248)

MouseClick("left", 1190, 705)


While NOT WinExists('BenchmarkPage')
    Sleep(1000)
WEnd

WinWaitActive("CompuBench")

WinClose("CompuBench")
