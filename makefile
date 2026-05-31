main:
	g++ --std=c++20 -Ofast ./egzaminas/*.cpp -o main

dev:
	g++ --std=c++20 -Og ./egzaminas/*.cpp -o main

clean:
	rm -f main