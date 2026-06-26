# Небосвод

## Обзор

"Небосвод" - астрофизическая симуляция частиц с гравитацией (N-body) и аккрецией. Частицы притягиваются друг к другу, сталкиваются и объединяются в частицы побольше. Постепенно из облака частиц вокруг звезды формируются планетоиды. 

Целью проекта было симулировать формирование планетарной системы из мелких частиц. В целом, эта цель была достигнута, но полученная модель аккреции показывает лишь качественное сходство и пока не является физически достоверной.

![Скриншот](assets/nebosvod.png)

## Реализация

"Небосвод" написан на C и использует raylib для графики. Интерфейс реализован с помощью собственной [библиотеки "Бетон"](https://github.com/RedCat17/beton) от RedCat17.

Для расчета гравитации используется алгоритм Барнса-Хата (Barnes-Hut). В отличие от наивного подхода со сложностью порядка O(n^2), где каждая частица взаимодействует с каждой, алгоритм Барнса-Хата обеспечивает сложность O(n log n). Это позволяет масштабировать симуляцию на куда большее число частиц. Принцип алгоритма таков: дальние кластеры частиц обрабатываются как единая масса. Для этого строится дерево квадрантов (quadtree), где каждый узел обладает своей массой. Алгоритм состоит из двух этапов: построение дерева и расчёт сил.

Кроме того, расчёт сил гравитации параллелизирован, т.е. работает на нескольких ядрах процессора. Это даёт прибавку к производительности в несколько раз.

В свою очередь, столкновения с наивным подходом также имеют сложность порядка O(n^2). Поэтому, для оптимизации в "Небосводе" реализованы два алгоритма расчёта столкновений. Первый - простая оптимизация с помощью сетки. В нём мир делится на ячейки, и каждая частица находится в какой-то ячейке. Каждая частица проверяет столкновения не с каждой другой, а лишь с теми, что находятся в этой и соседних ячейках. Это снижает сложность алгоритма до линейной, т.е. O(n). Хотя, реальная производительность зависит и от количества частиц в ячейке (чем их больше, тем хуже). Однако, для этого алгоритма каждая клетка должна быть больше максимального размера частицы, чтобы частица не находилась в нескольких клетках. Когда частицы примерно одного размера, всё работает хорошо. Но когда в центре системы есть огромная "звезда", это вынуждает делать такие же огромные ячейки, а это подрывает производительность.

Был реализован второй алгоритм. В нём ячейки небольшие, но каждая частица занимает не одну, а все ячейки, которых касается. Затем, цикл проходится по каждой ячейке и обрабатывает пары частиц в ней. Этот алгоритм хорош при высокой плотности частиц, но когда мир большой, а частиц мало, проигрывает первому.

Было реализовано два интегратора: неявная интеграция по Эйлеру и Verlet. Первая не обеспечивала сохранения энергия, поэтому мы перешли на Verlet, который является симметричным интегратором и сохраняет энергию.

## Сборка и запуск
```bash
git clone https://github.com/RedCat17/nebosvod.git
cd nebosvod
make
```
```bash
./build/main
```

## Использование

Справа расположена панель со статистикой.

Внизу находится меню управления с паузой (ползунок ничего не делает).

`Колёсико мыши` - приближение камеры;
`Нажатие колёсиком` - движение камеры;
`Стрелки` - движение камеры;
`Пробел` - пауза;
`R` - включить/выключить отрисовку;
`V` - включить/выключить векторы скорости;
`F` - следовать за случайной частицей;
`C` - перейти в центр масс;


# Nebosvod

## Overview

Nebosvod is astrophysical N-body simulation with accretion. Particless attract each other, collide and merge into larger ones. Over time, planetoids emerge from particle cloud around the star. 

The goal of the project was to simulate planetary system formation from dust cloud. Overall, the goal was achieved. Though, produced model of accretion only shows qualitative similarity and isn't yet physically accurate.

## Implementation

Nebosvod is written in C using raylib for graphics. UI is implemented using [Beton IMGUI](https://github.com/RedCat17/beton).

Barnes-Hut algorithm is used for gravity. It scales much better than naive pairwise approach.

Force calculation is parallelised, boosting performance several times.

Spatial grid is used to optimise collisions.

Two intergators were implemented: implicit Euler and Verlet. Euler doesn't conserve energy, so it was replaced by Verlet which is a simmetrical integrator.

## Building
```bash
git clone https://github.com/RedCat17/nebosvod.git
cd nebosvod
make
```
```bash
./build/main
```

## Usage

To the right, there is info panel.

On the bottom, there is control menu with pause button.

`Scroll` - zoom;
`Middle mouse button` - move camera;
`Arrow keys` - move camera;
`Space` - pause;
`R` - toggle render;
`V` - toggle velocity vectors;
`F` - follow random particle;
`C` - move to CoM;