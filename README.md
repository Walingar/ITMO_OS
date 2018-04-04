# Курс «Операционные системы» ИТМО
Tests and problems were taken from [ITMO OS site's](http://neerc.ifmo.ru/~os/hw.html)

Текущие баллы: [баллы](https://docs.google.com/spreadsheets/d/1KqgTg7TLDLA2WZOVgANORlfEUPdYo-076Pb6dIe-Bo4/edit#gid=0&range=A15)

## Домашнее задание 2. Find

Условие:
  1. Необходимо написать подмножество утилиты find.
  2. Программа должна:
     * Первым аргументом принимать путь, в котором будет производиться поиск файлов
     * По-умолчанию выводить в стандартный поток вывода все найденные файлы по этому пути
     * Поддерживать аргумент `-inum num`. Аргумент задает номер инода
     * Поддерживать аргумент `-name name`. Аргумент задает имя файла
     * Поддерживать аргумент `-size [-=+]size`. Аргумент задает фильтр файлов по размеру(меньше, равен, больше)
     * Поддерживать аргумент `-nlinks num`. Аргумент задает количество *hardlink*'ов у файлов
     * Поддерживать аргумент `-exec path`. Аргумент задает путь до исполняемого файла, которому в качестве единственного аргумент нужно передать найденный в иерархии файл
     * Поддерживать комбинацию аргументов. 
     Например, хочется найти все файлы с размером больше *1GB* и скормить их утилите `/usr/bin/sha1sum`.
  3. Сильные духом призываются к выполнению задания с использованием системного вызова `getdents`(2). 
     Остальные могут использовать `readdir` и `opendir` для чтения содержимого директории.

Решение:
* [Find](src/find)


## Домашнее задание 1. Shell

Условие:
  1. Необходимо написать упрощенный командный интерпретатор(`shell`).
  2. `shell` должен:
     * В бесконечном цикле, как настоящий `shell`
     * Считывать со стандартного ввода программу, которую необходимо
        запустить, вместе с ее аргументами. Программа задается в виде полного пути до исполняемого файла.
     * Выполнять заданную программу с заданными аргументами
     * Дожидаться завершения программы и выводить в стандартный поток вывода код ее завершения
     * Необходимо обрабатывать ошибки всех используемых системных вызовов
     * Необходимо обрабатывать конец вводу со стандартного потока

Пример использования программы:
  * Input: `/bin/ls`
  * Output: `CMakeLists.txt	README.md  cmake-build-debug  shell  src`
  
Решение:
  * [Shell](src/shell)
