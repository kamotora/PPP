echo "Running processes..."
start "defender" "%CD%\out\build\x64-Debug\defender\defender.exe"
start "coach" "%CD%\out\build\x64-Debug\coach\coach.exe"
start "forward" "%CD%\out\build\x64-Debug\forward\forward.exe"
start "goalkeeper" "%CD%\out\build\x64-Debug\goalkeeper\goalkeeper.exe"
start "footballField" "%CD%\out\build\x64-Debug\footballField\footballField.exe"
start "doctor" "%CD%\out\build\x64-Debug\doctor\doctor.exe"
pause