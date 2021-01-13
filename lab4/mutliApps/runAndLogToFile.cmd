echo "Running processes..."
start cmd /k "%CD%\out\build\x64-Debug\forward\forward.exe > %CD%\forward.txt"
start cmd /k "%CD%\out\build\x64-Debug\defender\defender.exe > %CD%\defender.txt"
start cmd /k "%CD%\out\build\x64-Debug\goalkeeper\goalkeeper.exe > %CD%\goalkeeper.txt"
start cmd /k "%CD%\out\build\x64-Debug\footballField\footballField.exe > %CD%\footballField.txt"
start cmd /k "%CD%\out\build\x64-Debug\coach\coach.exe > %CD%\coach.txt"
timeout 1
start cmd /k "%CD%\out\build\x64-Debug\doctor\doctor.exe > %CD%\doctor.txt"
pause