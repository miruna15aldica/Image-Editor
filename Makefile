build: image_editor

image_editor:
	gcc main.c -g -lm -o image_editor -Wall -Wextra

clean:
	rm image_editor

.PHONY: clean