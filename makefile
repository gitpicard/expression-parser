expression: expression.o main.o
	gcc bin/expression.o bin/main.o -lm -o bin/expression
expression.o: src/expression.c
	gcc -c -std=c89 -Iinc src/expression.c -o bin/expression.o
main.o: src/main.c
	gcc -c -std=c89 -Iinc src/main.c -o bin/main.o
run:
	./bin/expression
clean:
	rm -f bin/expression.o bin/main.o bin/expression
