#!/bin/bash
# Использование: ./demo.sh examples/test2.lang
set -e
F=${1:-examples/test2.lang}
cd "$(dirname "$0")/src"
make -s
echo "=== 1. Выполнение (интерпретатор) ==="
./lang "../$F"
echo
echo "=== 2. Дерево (текст) ==="
./lang --tree "../$F"
echo
echo "=== 3. Генерация кода на C ==="
./lang --c "../$F" > /tmp/out.c
cat /tmp/out.c
gcc /tmp/out.c -o /tmp/out && echo "=== Результат скомпилированной программы ===" && /tmp/out
echo
echo "=== 4. Визуализация AST в браузере ==="
./lang --web "../$F" > tree.js
open viewer.html
