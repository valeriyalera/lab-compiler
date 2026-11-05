# Транслятор підмножини C на Lex/Yacc (flex/bison)

Лабораторна робота з системного програмування.

Синтаксичний аналізатор для C-подібної імперативної мови з побудовою AST,
інтерпретатором, генерацією коду на C та візуалізацією дерева у браузері (d3.js).

## Мова (підмножина C)

- типи `int`, `double`, `void`, оголошення змінних (`int x = 5;`)
- функції з параметрами, `return`, виклики, рекурсія; виконання починається з `main`
- `if / else`, `while`, `for`, блоки `{ }`
- оператори `+ - * / %`, `< > <= >= == !=`, `&& || !`, `++`, `--`
- вбудований `print(expr)`
- коментарі `//` та `/* */`

~~~c
int fact(int n) {
    if (n <= 1) { return 1; }
    return n * fact(n - 1);
}

int main() {
    int i;
    for (i = 1; i <= 5; i++) { print(fact(i)); }
    return 0;
}
~~~

Спрощення: `i++` розгортається в `i = i + 1` у граматиці; область видимості одна на функцію.

## Режими

| Режим | Команда |
|-------|---------|
| Виконання (інтерпретатор) | `./lang file.lang` |
| Текстове AST | `./lang --tree file.lang` |
| Генерація C | `./lang --c file.lang > out.c` |
| AST для браузера | `./lang --web file.lang > tree.js`, відкрити `viewer.html` |

## Збірка (macOS)

~~~
brew install flex bison
export PATH="$(brew --prefix bison)/bin:$(brew --prefix flex)/bin:$PATH"
cd src && make
~~~

Потрібен bison 3.x (системний 2.3 застарілий).

## Демонстрація

~~~
./demo.sh examples/test3.lang
~~~

## Структура

- `src/lang.l` лексер (flex)
- `src/lang.y` граматика і побудова AST (bison), без конфліктів
- `src/ast.c` вузли дерева
- `src/interp.c` інтерпретатор (обчислення, типи, виклики функцій)
- `src/codegen.c` генерація C
- `src/ast_json.c` експорт AST для d3.js
- `src/viewer.html` візуалізація
- `examples/` тестові програми
