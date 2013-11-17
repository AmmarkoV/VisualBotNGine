#!/bin/bash


echo "Test 0 should be 368,050"
./OCRTester testocr.pnm 2> /dev/null | grep OCR

echo "Test 2 should be 649,521"
./OCRTester testocr2.pnm 2> /dev/null | grep OCR

echo "Test 3 should be 52,694"
./OCRTester testocr3.pnm 2> /dev/null | grep OCR

echo "Test 4 should be 6,767"
./OCRTester testocr4.pnm 2> /dev/null | grep OCR

 

exit 0
