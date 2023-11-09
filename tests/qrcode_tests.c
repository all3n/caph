#include "qrcodegen.h"
#include <stdio.h>
static void printQr(const uint8_t qrcode[]) {
	int size = qrcodegen_getSize(qrcode);
	int border = 4;
	for (int y = -border; y < size + border; y++) {
		for (int x = -border; x < size + border; x++) {
			fputs((qrcodegen_getModule(qrcode, x, y) ? "##" : "  "), stdout);
		}
		fputs("\n", stdout);
	}
	fputs("\n", stdout);
}
int main(int argc, char *argv[]) {
  uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
  uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
  bool ok = qrcodegen_encodeText("https://www.nayuki.io/", tempBuffer, qrcode,
                                 qrcodegen_Ecc_HIGH, qrcodegen_VERSION_MIN,
                                 qrcodegen_VERSION_MAX, qrcodegen_Mask_3, true);
  if (ok) {
    printQr(qrcode);
  }
}
