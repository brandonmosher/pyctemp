tests: $(wildcard test/*.c)
	@echo $^

test/%: FORCE
	@echo $*
	@mkdir -p bin
	pyctemp --template-dirpath template --source-dirpath test/$*
	gcc -ggdb -I include include/*.c test/$* -o bin/$*
	@chmod +x bin/$*
	@./bin/$*

clean:
	rm -rf bin
	rm -rf include

FORCE: