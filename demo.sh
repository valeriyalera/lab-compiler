#!/bin/bash
# Використання: ./demo.sh examples/test2.lang
set -e
F=${1:-examples/test2.lang}
cd "$(dirname "$0")/src"
make -s
echo "=== 1. Виконання (інтерпретатор) ==="
./lang "../$F"
echo
echo "=== 2. Дерево (текст) ==="
./lang --tree "../$F"
echo
echo "=== 3. Генерація коду на C ==="
./lang --c "../$F" > /tmp/out.c
cat /tmp/out.c
gcc /tmp/out.c -o /tmp/out && echo "=== Результат скомпільованої програми ===" && /tmp/out
echo
echo "=== 4. Візуалізація AST у браузері ==="
./lang --web "../$F" > tree.js
open viewer.html
