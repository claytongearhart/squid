main:
	as -o main.o main.asm
	ld -macosx_version_min 11.0.0 -o main main.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64
	rm main.o