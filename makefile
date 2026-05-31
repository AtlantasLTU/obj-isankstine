main:
	g++ --std=c++20 -licuuc -licui18n -Ofast ./egzaminas/*.cpp -o main

dev:
	g++ --std=c++20 -licuuc -licui18n -Og ./egzaminas/*.cpp -o main

clean:
	rm -f main