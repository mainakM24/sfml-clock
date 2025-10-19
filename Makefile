default:
	g++ -o ./bin/main  src/main.cpp -I"C:\SFML-3.0.2\include" -L"C:\SFML-3.0.2\lib" -lsfml-graphics -lsfml-window -lsfml-system
	./bin/main.exe
