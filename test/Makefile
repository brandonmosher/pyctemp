tests: $(wildcard test/*)
	echo $^

test/%: FORCE
	echo $*
	mkdir -p build include
	pyctemp --template-dirpath template --source-dirpath test/$* -q
	gcc -I include include/*.c test/$*/* -o build/$*
	chmod +x build/$*
	./build/$*

clean:
	rm -rf build
	rm -rf include

FORCE: