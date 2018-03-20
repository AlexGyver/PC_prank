/*
   Мощный компьютерный розыгрыщ на основе платформы BS Micro
   (SS Micro, Arduino Leonardo, Arduino Pro Micro, Atmega32U4)
   Плата вставляется в USB порт и по случайному таймеру активирует случайно одно из действий:
   - Случайный сдвиг курсора мыши
   - Нажатие случайной клавиши (буква, цифра, символ)
   - Пишет слово
   - Меняет активное окно (Alt+TAB)
   - Закрывает активное окно (Alt+F4)
   - Перемещает окно в случайное место (WIN+стрелка случайно)
   - Кликает PageDown
   - Кликает Insert
   - Сворачивает все окна (Win+D)
   - Меняет раскладку клавиатуры (Alt+Shift)
   - Включает/выключает Caps Lock

   **************************************
   Разработал: AlexGyver
   http://alexgyver.ru/pc-prank/
   https://github.com/AlexGyver/pc-prank/
*/

#include "Keyboard.h"
#include "Mouse.h"

#define MIN_TIME 300     // секунд, минимальное время между действиями
#define MAX_TIME 600     // секунд, максимальное время между действиями
#define PRANK_AMOUNT 11  // количество "пранков"

unsigned long myTimer, nextTime;
byte nextPrank;
byte safePrank[PRANK_AMOUNT];
byte prankTicks[PRANK_AMOUNT];
boolean flag = true;

void setup() {
  Mouse.begin();
  Keyboard.begin();
  delay(500);
  Serial.begin(9600);

  // установка количества вызовов, через которые пранк активируется
  // нужно для уменьшения частоты срабатывания "злых" пранков в стиле insert и alt+F4
  safePrank[2] = 10;  // вывод слова (пакость 2) будет вызываться каждые 10 раз
  safePrank[4] = 25;  // ALT+F4 (пакость 4) будет вызываться каждые 25 раз
  safePrank[8] = 15;  // свернуть все окна (пакость 4) будет вызываться каждые 15 раз

  // пинаем генератор случайных чисел
  randomSeed(analogRead(0) + analogRead(1));
}

void loop() {
  if (millis() - myTimer > nextTime) {
    // выбираем следующий случайный пранк
    nextPrank = random(0, PRANK_AMOUNT);

    if (safePrank[nextPrank] > 0) {                         // если выбранный пранк имеет ограничения
      if (prankTicks[nextPrank] < safePrank[nextPrank]) {   // если счётчик вызовов меньше разрешённого числа
        prankTicks[nextPrank] += 1;                         // увеличить счётчик вызовов
      } else {                        // если уже больше
        prankTicks[nextPrank] = 0;    // сбросить счётчик вызовов
        superPRANK();                 // активировать пранк
      }
    } else {                          // если ограничений нет
      superPRANK();                   // активировать пранк
    }

    // пинаем генератор случайных чисел
    randomSeed(myTimer);
    // сбрасываем таймер
    myTimer = millis();
    // получаем случайное время с учётом перевода в миллисекунды
    nextTime = random(MIN_TIME * 1000, MAX_TIME * 1000);
  }
}

void superPRANK() {
  switch (nextPrank) {
    case 0:
      // случайно сдвинуть курсор на -200...200px вверх и вниз
      Mouse.move(random(400) - 200, random(400) - 200);
      break;
    case 1:
      // написать случайную букву или символ
      char randKey;
      randKey = (char)random(20, 126);  // 20-126 по таблице ASCII - все символы и латинские буквы
      Keyboard.write(randKey);          // соотвевтственно отправить случайную букву/символ
      break;
    case 2:
      // написать слово
      Keyboard.write('d');
      Keyboard.write('i');
      Keyboard.write('c');
      Keyboard.write('k');
      break;
    case 3:
      // нажать сочетание ALT+TAB (смена активного окна программы)
      Keyboard.press(KEY_LEFT_ALT);
      delay(10);
      Keyboard.press(KEY_TAB);
      delay(10);
      Keyboard.releaseAll();
      break;
    case 4:
      // нажать сочетание ALT+F4 (закрыть текущее окно)
      Keyboard.press(KEY_LEFT_ALT);   // жмём альт
      delay(10);                      // ждём
      Keyboard.press(KEY_F4);         // жмём F4
      delay(10);                      // ждём
      Keyboard.releaseAll();          // отпускаем все
      break;
    case 5:
      // нажать сочетание WIN+стрелка (расположить активное окно слева, справа, во весь экран или свернуть)
      byte randArrow;
      randArrow = random(4);        // случайное число от 0 до 3
      Keyboard.press(KEY_LEFT_GUI); // жмём windows
      delay(10);
      switch (randArrow) {
        case 0: Keyboard.press(KEY_UP_ARROW);
          break;
        case 1: Keyboard.press(KEY_DOWN_ARROW);
          break;
        case 2: Keyboard.press(KEY_LEFT_ARROW);
          break;
        case 3: Keyboard.press(KEY_RIGHT_ARROW);
          break;
      }
      delay(10);
      Keyboard.releaseAll();          // отпускаем все
      break;
    case 6:
      // нажать Page Down, будет люто бесить в браузере
      Keyboard.press(KEY_PAGE_DOWN);
      delay(10);
      Keyboard.release(KEY_PAGE_DOWN);
      break;
    case 7:
      // нажать Insert. Если жертва о ней не знает, это будет полный трэш!
      Keyboard.press(KEY_INSERT);
      delay(10);
      Keyboard.release(KEY_INSERT);
      break;
    case 8:
      // свернуть все окна, сочетание Win+D
      Keyboard.press(KEY_LEFT_GUI);
      delay(10);
      Keyboard.write('D');
      delay(10);
      Keyboard.release(KEY_LEFT_GUI);
      break;
    case 9:
      // сменить раскладку клавиатуры (alt+shift)
      Keyboard.press(KEY_LEFT_ALT);
      delay(10);
      Keyboard.press(KEY_LEFT_SHIFT);
      delay(10);
      Keyboard.releaseAll();          // отпускаем все
      break;
    case 10:
      // caps lock
      Keyboard.press(KEY_CAPS_LOCK);
      delay(10);
      Keyboard.releaseAll();
      break;
    case 11:
      // придумай свой пранк!!! Не забудь указать правильное количество
      // пранков в самом верху настроек
      break;
    case 12:
      // придумай свой пранк!!! Не забудь указать правильное количество
      // пранков в самом верху настроек
      break;
  }
}

/* МЫШЬ
    Функции
  Mouse.click()
  Mouse.move()
  Mouse.press()
  Mouse.release()

  Кнопки мыши для функций click, press и release:
  MOUSE_LEFT
  MOUSE_RIGHT
  MOUSE_MIDDLE

   КЛАВИАТУРА
   Для отправки "единичного" нажатия испольузем Keyboard.write
   Для эмуляции одновременного нажатия нескольких клавиш используйте команду Keyboard.press().
   Отпустить можно командой Keyboard.release(), также Keyboard.releaseAll() отпускает все нажатые
   Список клавиш-модификаторов
  KEY_LEFT_CTRL
  KEY_LEFT_SHIFT
  KEY_LEFT_ALT
  KEY_LEFT_GUI
  KEY_RIGHT_CTRL
  KEY_RIGHT_SHIFT
  KEY_RIGHT_ALT
  KEY_RIGHT_GUI

  KEY_UP_ARROW
  KEY_DOWN_ARROW
  KEY_LEFT_ARROW
  KEY_RIGHT_ARROW
  KEY_BACKSPACE
  KEY_TAB
  KEY_RETURN
  KEY_ESC
  KEY_INSERT
  KEY_DELETE
  KEY_PAGE_UP
  KEY_PAGE_DOWN
  KEY_HOME
  KEY_END
  KEY_CAPS_LOCK
  KEY_F1
  KEY_F2
  KEY_F3
  KEY_F4
  KEY_F5
  KEY_F6
  KEY_F7
  KEY_F8
  KEY_F9
  KEY_F10
  KEY_F11
  KEY_F12
*/
