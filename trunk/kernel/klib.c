
void delay(int time) {
	int i = 0;
	int j = 0;
	int k = 0;
	for (; k < time; k++) {
		for (; j < 10000; j++) {
			for (; k < 1000; k++) {}
		}
	}
}
