# Описание программы

genetic_alg_konsole - программа для исследования применимости генетического алгоритма для генерации решений задачи коммивояжёра.

Здесь приведено краткое описание исходного кодаи возможностей программы.

## Содержание
- [Описание модулей и файлов проекта](#описание-модулей-и-файлов-проекта)
- [Использование](#использование)
- [Вывод и результаты](#вывод-и-результаты)

## Описание модулей и файлов проекта

### Модули

- include/genetic - определяет шаблоны функций и классов для работы генетического алгоритма:
    - interface_genom - базовый класс генома;
    - search_parents - класс со статическими функциями отбора родителей;
    - crossingover_get_points - функция кроссинговера;
    - genetic - шаблонная функция генетического алгоритма.
- include/byte_code - определяет классы-обработчики кодов на байт-коде:
    - byte_code_basic - базовый класс-обработчик с определениями типов, констант, списком команд и статическими функциями, работающими с этими командами;
    - byte_code_interpreter - класс-процессор байт-кода;
    - byte_code_text_interpreter - класс-обработчик текстов на байт-коде (пример такого кода - в спецификации). Переводит текст из UTF-8 в формат, понятный для byte_code_interpreter, который уже интепретирует код;
- include/byte_code_genetic - класс genom (одна особь в генетическом алгоритме) и ряд функций и классов для работы генетического алгоритма;
- include/fitness - предоставляет функции для расчёта функции приспособленности (модуль необходим для модуля include/byte_code_genetic);
- include/tomatik_clah - самописный модуль обработчика аргументов командной строки;
- include/get_file - модуль с одной функцией переноса содержимого файла в буфер;
- include/array_digits - модуль с одноименным шаблонным классом array_digits, который представляет собой массив из структур digit. Структура digit - это число value, его максимально возможное значение max и минимально возможное значение min. Класс хранит массив таких структур, который абстрактно можно интерпретировать как число, у которого каждый разряд может иметь значение в разных рамках (однако если у всех элементов массива min и max одинаковые, то тогда можно сказать, что массив является числом в (max-min)-ичной системе счисления). Класс предоставляет возможность инкрементировать или декрементировать это число. В дальнейшем в коде объект этого класса используется для перебора множества различных вариантов;
- include/random - содержит функции для создания рандомных чисел и функцию probability, единственный аргумент которого определяет вероятность возврата true (чем ближе к 1 - тем вероятнее возврат true);
- include/global - глобальный файл проекта, определяющие макросы и функции вывода информации на stdout, глобальные переменные и классы:
    - global_info - структура, хранит информацию об аргументах и состоянии генетического алгоритма;
    - result_code_t - перечисление возможных результатов работы генетического алгоритма на отдельном шаге;
    - graph_info_t - информация о графах из файла settings.json;
    - save_and_log - глобальный объект этого класса занимается открытием файла setting.json в начале работы программы и считыванием всех значений, а также перебором вариантов;
- include/tomatik_degree_of_completion - самописный модуль для вывода степени завершенности работы программы;
- main_program - определяет класс main_program, предоставляющий методы для инициализации и запуска генетического алгоритма.

### Отдельные файлы

- main.cpp - файл с функций main и полной обработкой аргументов командной строки;

## Использование

Для компилирования необходима библиотека Qt.
После компилирования бинарный файл представляет собой консольную программу, для запуска которой необходимо передать аргументы командной строки. Также для работы программы обязательно наличие в директории файла settings.json. После окончания работы программа попрождает директорию log с результатами работы.
Результаты работы - файлы с полной эволюцией популяции и изменением функции приспособленности самого лучшего индивидуума в популяции (а также, если найден индивидуум, подходящий условию завершения генетического алгоритма - его программный код в текстовом виде).

### settings.json

Хранит данные о графах и настройки генетического алгоритма.

##### Настройки генетического алгоритма

- gen_size - массив из произвольного количества чисел в строковом формате, обозначающих размер начальной популяции;
- cb_selection - массив чисел. Числа в массиве могут принимать значения от 0 до 4 включительно и являются индексами в массиве строк (строки обозначают выбранный оператор отбора родителей):
    - 0 - "Panmixia";
    - 1 - "Inbreeding";
    - 2 - "Outbreeding";
    - 3 - "Tournament";
    - 4 - "Roulette";
- count_points_crossingover - массив чисел. Числа в массиве могут принимать значения от 0 до 4 включительно и являются индексами в массиве строк (строки обозначают количество точек в кроссинговере):
    - 0 - "5";
    - 1 - "4";
    - 2 - "3";
    - 3 - "2";
    - 4 - "1";
- probability_mutation - массив из произвольного количества чисел в строковом формате, обозначающих вероятность мутации в процентах.

При запуске программы с аргументом work значения из массивов последовательно перебираются во время работы программы, иначе - игнорируются (хотя и происходит проверка массивов). Ни один массив не может быть пустым.

- graphs - список графов, у каждого должно быть наименование, длина лучшего пути и матрица инцендентности. Нужно заметить, что матрица представлена массивом строк, каждая строка в массиве - строка в матрице.

##### Дополнительные настройки

- sleep_milliseconds_after_stdout_msg - задержка между каждой отправкой в stdout сообщений от программы;
- genetic_function_print - если установлен true, то при заходе внутрь любой из функций генетического алгоритма в stdout будет отправляться название данной функции (таким образом можно проследить, на каких этапах генетический алгоритм задерживается), если false - в stdout отправляется только результат функции print;

##### Пример оформления settings.json

```json
{
    "sleep_milliseconds_after_stdout_msg": 100,
    "genetic_function_print": true,
    "gen_size": ["50"],
    "cb_selection": [3],
    "count_points_crossingover": [0, 1, 2, 3, 4, 5],
    "probability_mutation": ["10", "20", "30"],
    "graphs": [
        {
            "name": "Graph",
            "good_path": 74,
            "graph": [
                "0, 28, 49, 16, 33, 7,",
                "15, 0, 52, 31, 19, 6,",
                "20, 38, 0, 44, 25, 23,",
                "7, 5, 3, 0, 35, 29,",
                "45, 25, 11, 10, 0, 39,",
                "49, 15, 38, 21, 51, 0;"
            ]
        },

        ... другие графы
    ]
}

```

### Команды

- --help - вывод всех команд;
- graphs - красивый вывод графов из settings.json;
- remove - удаление /log и всех файлов в нём, если они были созданы;
- work - запуск программы, используя настройки генетического алгоритма из settings.json (перебор всех вариантов);
- work_ones --help - вывод всех команд work_ones.
- work_ones format-values - запуск программы, используя отдельный список вариантов.

settings.json призван автоматизировать перебор вариантов для запуска программы. Если не нужен перебор вариантов, а есть отдельный список вариантов, которые программа должна дать на вход генетическому алгоритму, нужно воспользоваться work_ones format-values.

После format-values идут аргументы в формате "let size cros pm", где
- let - буква из набора p, i, o, t, r - первые маленькие буквы названия операторов выбора родителей;
- size - размер начальной популяции;
- cros - количество точек в кроссинговере;
- pm - вероятность мутации в процентах.

Интерпретация выполняется через пробелы. Все аргументы - строки в этом формате. Все аргументы обязательны.

Примеры аргументов:

- " t 100 3 10" - турнирный отбор, популяция в 100 особей, 2-точечный кроссинговер, вероятность мутации 10%;
- " i 200 3 50" - инбридинг, популяция в 200 особей, 2-точечный кроссинговер, вероятность мутации 50%;
- "o 50 1 8" - аутбридинг, популяция в 50 особей, 4-точечный кроссинговер, вероятность мутации 8%.

## Вывод и результаты

Пример вывода программы:

```
user@User:~/project/genetic_alg_konsole$ ./genetic_alg_konsole work_ones format-values "p 20 5 25"
[0/1]-[00:51:06]-[00:00:01]:  work_ones                       START WORK_ONES!!!
[0/1]-[00:51:06]-[00:00:01]:  work_ones                       selected 20 0 0 25 Graph
[0/1]-[00:51:06]-[00:00:01]:  init_genetic_alg_konsole        save to /home/user/project/genetic_alg_konsole/logs/20_Panmixia_5_25_Graph_r.log
[0/1]-[00:51:06]-[00:00:01]:  init_genetic_alg_konsole        begin
[0/1]-[00:51:06]-[00:00:01]:  panmixia                        
[0/1]-[00:51:06]-[00:00:01]:  crossingover                    
[0/1]-[00:51:06]-[00:00:01]:  mutation                        
[0/1]-[00:51:06]-[00:00:01]:  log_print                       
[0/1]-[00:51:06]-[00:00:01]:  print                           0, 20, {0 }
[0/1]-[00:51:06]-[00:00:01]:  panmixia                        
[0/1]-[00:51:06]-[00:00:01]:  crossingover                    
[0/1]-[00:51:06]-[00:00:01]:  mutation                        
[0/1]-[00:51:06]-[00:00:01]:  log_print                       
[0/1]-[00:51:06]-[00:00:01]:  print                           1, 21, {0 }
[0/1]-[00:51:06]-[00:00:01]:  panmixia                        
[0/1]-[00:51:06]-[00:00:01]:  crossingover                    
[0/1]-[00:51:06]-[00:00:01]:  mutation                        
[0/1]-[00:51:06]-[00:00:01]:  log_print                       
[0/1]-[00:51:07]-[00:00:02]:  print                           2, 25, {0 }
[0/1]-[00:51:07]-[00:00:02]:  panmixia                        
[0/1]-[00:51:07]-[00:00:02]:  crossingover                    
[0/1]-[00:51:07]-[00:00:02]:  mutation                        
[0/1]-[00:51:07]-[00:00:02]:  log_print                       
[0/1]-[00:51:07]-[00:00:02]:  print                           3, 24, {0 }
[0/1]-[00:51:07]-[00:00:02]:  panmixia                        
[0/1]-[00:51:07]-[00:00:02]:  crossingover                    
[0/1]-[00:51:07]-[00:00:02]:  mutation                        
[0/1]-[00:51:07]-[00:00:02]:  log_print                       
[0/1]-[00:51:07]-[00:00:02]:  print                           4, 21, {0 }
[0/1]-[00:51:07]-[00:00:02]:  panmixia                        
[0/1]-[00:51:07]-[00:00:02]:  crossingover                    
[0/1]-[00:51:07]-[00:00:02]:  mutation                        
[0/1]-[00:51:07]-[00:00:02]:  log_print                       
[0/1]-[00:51:07]-[00:00:02]:  print                           5, 24, {0 }
[0/1]-[00:51:07]-[00:00:02]:  panmixia                        
[0/1]-[00:51:07]-[00:00:02]:  crossingover                    
[0/1]-[00:51:07]-[00:00:02]:  mutation                        
[0/1]-[00:51:07]-[00:00:02]:  log_print                       
[0/1]-[00:51:08]-[00:00:03]:  print                           6, 27, {0 }
[0/1]-[00:51:08]-[00:00:03]:  panmixia                        
[0/1]-[00:51:08]-[00:00:03]:  crossingover                    
[0/1]-[00:51:08]-[00:00:03]:  mutation                        
[0/1]-[00:51:08]-[00:00:03]:  log_print                       
[0/1]-[00:51:08]-[00:00:03]:  print                           7, 29, {0 }
[0/1]-[00:51:08]-[00:00:03]:  panmixia                        
[0/1]-[00:51:08]-[00:00:03]:  crossingover                    
[0/1]-[00:51:08]-[00:00:03]:  mutation                        
[0/1]-[00:51:08]-[00:00:03]:  log_print                       
[0/1]-[00:51:08]-[00:00:03]:  print                           8, 33, {0 }
[0/1]-[00:51:08]-[00:00:03]:  panmixia                        
[0/1]-[00:51:08]-[00:00:03]:  crossingover                    
[0/1]-[00:51:08]-[00:00:03]:  mutation                        
```

Слева направо:
1. Количество уже исследованных вариантов относительно количества исследуемых вариантов;
2. Текущее время;
3. Время с начала старта программы;
4. Название функции в коде С++, на которой сейчас остановился алгоритм;
5. Дополнительные данные.

Функция work_ones пишет в stdout параметры рассматриваемого варианта аргументов генетического алгоритма.

Функция init_genetic_alg_konsole пишет в stdout полный адрес файла, куда будут помещены результаты.

Если не была создана, то создаётся директория /log. При этом если названия уже лежащих там файла и файла, созданного программой, совпадают, то старый файл затирается новым.

По остальным функциям, вызываемым в программе (panmixia, crossingover, mutation) можно понять, насколько алгоритм замедлился на каждой из них. Функция log_print пишет шаг, величину популяции и приспособленность лучшего члена популяции в файл, а потом вызывает print, которая уже пишет это в stdout.

Концом работы генетического алгоритма является одно из условий:
1. Найдена особь с рабочим кодом, который отработал на всех входных графах и вернул рабочий результат;
2. Количество особей в популяции превысило 500000.

В случае, если на каком-либо шаге генетического алгоритма найдена особь с рабочим кодом, ее код незамедлительно пишется в конец файла результатов.