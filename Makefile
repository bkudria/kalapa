all:
	platformio --force run --target upload

clean:
	platformio --force run --target clean
