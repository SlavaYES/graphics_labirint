#include <SFML/Graphics.hpp>
#include <unistd.h>

#define NUM 40
#define SIZE 20
#define COUNT 10 * 30
#define STEP 3
#define USPEED_SEARCH 10000
#define USPEED_PATH 100000
using namespace sf;

struct Queue {
    int x, y, color;
    Queue *next;
};

void fillMatrix(RectangleShape **&matrix);
void generateStartStopPoint(RectangleShape **matrix,
                            int &start_x, int &start_y,
                            int &stop_x, int &stop_y,
                            int start_color, int end_color);

void add(Queue *&tail, int x, int y, int color);
void remove(Queue *&head);
void wave(RectangleShape **matrix,
          int x, int y,
          int color, int end_color,
          Queue *&tail);
void create_path(RectangleShape **matrix,
                 int x, int y,
                 int end_color, int start_color,
                 Queue *&tail);

void draw(RenderWindow &win, RectangleShape **matrix);

void waveAlgoritm(RectangleShape **matrix,
                  int x, int y,
                  int color, int end_color,
                  RenderWindow &win);

void searchPath(RectangleShape **pShape,
                int x, int y,
                int end_color, int start_color,
                RenderWindow &win);

void waitWindow(RenderWindow &win);

int main() {
    RenderWindow win(VideoMode(840, 900), "SFML");
    win.setPosition(Vector2i(0, 0));
    srand(time(NULL));

    RectangleShape** matrix = nullptr;
    fillMatrix(matrix);

    int start_color = 1, end_color = 180;
    int start_x, start_y, end_x, end_y;
    generateStartStopPoint(matrix,
                           start_x, start_y,
                           end_x, end_y,
                           start_color, end_color);

    waveAlgoritm(matrix,
                 start_x, start_y,
                 start_color, end_color,
                 win);

    searchPath(matrix,
               end_x, end_y,
               end_color, start_color,
               win);

    waitWindow(win);

    return 0;
}

void waitWindow(RenderWindow &win) {
    while (win.isOpen()) {
        Event event;
        while (win.pollEvent(event)) {
            if (event.type == Event::Closed
                || event.type == Event::KeyPressed
                || event.type == Event::MouseButtonPressed) {
                win.close();
            }
        }
    }
}

void searchPath(RectangleShape **matrix, int end_x, int end_y, int end_color, int start_color, RenderWindow &win) {

    Queue *path = new Queue(), *tail_path = nullptr;
    path->x = path->y = path->color = -1; path->next = nullptr;
    tail_path = path;

    add(tail_path, end_x, end_y, end_color);

    Queue *point = path->next;
    while (point->color != start_color) {
        Event event;
        while (win.pollEvent(event)) {
            if (event.type == Event::Closed
                || event.type == Event::KeyPressed
                || event.type == Event::MouseButtonPressed) {
                win.close();
            }
        }
        create_path(matrix, point->x, point->y, end_color, start_color, tail_path);
        point = point->next;
        draw(win, matrix);
        usleep(USPEED_PATH);
    }
}

void waveAlgoritm(RectangleShape **matrix,
                  int start_x, int start_y,
                  int start_color, int end_color,
                  RenderWindow &win) {

    Queue *head = nullptr, *tail = nullptr;
    head = new Queue();
    head->x = -1; head->y = -1; head->color = -1; head->next = nullptr;
    tail = head;

    add(tail, start_x, start_y, start_color + 30);

    Queue *point = head->next;
    while (point && point->color != end_color) {
        Event event;
        while (win.pollEvent(event)) {
            if (event.type == Event::Closed
                || event.type == Event::KeyPressed
                || event.type == Event::MouseButtonPressed) {
                win.close();
            }
        }
        wave(matrix, point->x, point->y, point->color, end_color, tail);
        remove(head);
        point = head->next;

        draw(win, matrix);
        usleep(USPEED_SEARCH);
    }

}

void fillMatrix(RectangleShape **&matrix) {
    matrix = new RectangleShape*[NUM];
    for (int i = 0; i < NUM; i++) { matrix[i] = new RectangleShape[NUM]; }

    for (int i = 0; i < NUM; i++) {
        for (int j = 0; j < NUM; j++) {
            matrix[i][j].setPosition(i * (SIZE + 1), j * (SIZE + 1));
            matrix[i][j].setSize(Vector2f(SIZE, SIZE));
            matrix[i][j].setFillColor(Color::White);
        }
    }
    for (int i = 0; i < NUM; i++) {
        matrix[i][0].setFillColor(Color::Red);
        matrix[0][i].setFillColor(Color::Red);
        matrix[NUM - 1][i].setFillColor(Color::Red);
        matrix[i][NUM - 1].setFillColor(Color::Red);
    }

    for (int i = 0; i < COUNT; i++) {
        int x, y;
        do {
            x = rand() % NUM;
            y = rand() % NUM;
        } while (matrix[x][y].getFillColor() == Color::Red);
        matrix[x][y].setFillColor(Color::Red);
    }
}

void generateStartStopPoint(RectangleShape **matrix,
                                int &start_x, int &start_y,
                                int &end_x, int &end_y,
                                int start_color, int end_color) {
    do {
        start_x = rand() % NUM;
        start_y = rand() % NUM;
    } while (matrix[start_x][start_y].getFillColor() == Color::Red);
    matrix[start_x][start_y].setFillColor(Color(143, start_color, 253));
    do {
        end_x = rand() % NUM;
        end_y = rand() % NUM;
    } while (matrix[end_x][end_y].getFillColor() == Color::Red);
    matrix[end_x][end_y].setFillColor(Color(0, 0, end_color));
}

void draw(RenderWindow &win, RectangleShape **matrix) {
    win.clear();
    for (int i = 0; i < NUM; i++) {
        for (int j = 0; j < NUM; j++) {
            win.draw(matrix[i][j]);
        }
    }
    win.display();
}

void remove(Queue *&head) {
    Queue *point = head->next;
    Queue *tmp = point->next;
    head->next = tmp;
    delete point;
}

void create_path(RectangleShape **matrix, int current_x, int current_y, int end_color, int start_color, Queue *&tail) {
    int min = 256;
    int x = -1, y = -1;

    if (min > matrix[current_x - 1][current_y].getFillColor().g
            && matrix[current_x - 1][current_y].getFillColor() != Color::Red
            && matrix[current_x - 1][current_y].getFillColor().g != 0) {
        min = matrix[current_x - 1][current_y].getFillColor().g;
        x = current_x - 1; y = current_y ;
    }
    if (min > matrix[current_x][current_y - 1].getFillColor().g
            && matrix[current_x][current_y - 1].getFillColor() != Color::Red
            && matrix[current_x][current_y - 1].getFillColor().g != 0) {
        min = matrix[current_x][current_y - 1].getFillColor().g;
        x = current_x; y = current_y - 1;
    }
    if (min > matrix[current_x + 1][current_y].getFillColor().g
            && matrix[current_x + 1][current_y].getFillColor() != Color::Red
            && matrix[current_x + 1][current_y].getFillColor().g != 0) {
        min = matrix[current_x + 1][current_y].getFillColor().g;
        x = current_x + 1; y = current_y;
    }
    if (min > matrix[current_x][current_y + 1].getFillColor().g
            && matrix[current_x][current_y + 1].getFillColor() != Color::Red
            && matrix[current_x][current_y + 1].getFillColor().g != 0) {
        min = matrix[current_x][current_y + 1].getFillColor().g;
        x = current_x; y = current_y + 1;
    }
    if (matrix[x][y].getFillColor().g == start_color) {
        matrix[x][y].setFillColor(Color(140, start_color, 200));
    } else if (matrix[x][y].getFillColor().b == end_color) {
        matrix[x][y].setFillColor(Color(0, 0, end_color));
    } else {
        matrix[x][y].setFillColor(Color(0, 213, 255));
    }
    add(tail, x, y, min);
}

void add(Queue *&tail, int x, int y, int color) {
    Queue *point = new Queue();
    point->x = x;
    point->y = y;
    point->color = color;
    point->next = nullptr;
    tail->next = point;
    tail = point;
}

void wave(RectangleShape **matrix, int current_x, int current_y, int color, int end_color, Queue *&tail) {
    if (matrix[current_x - 1][current_y].getFillColor() == Color::White) {
        matrix[current_x - 1][current_y].setFillColor(Color(0, color + STEP, 0));
        add(tail, current_x - 1, current_y, color + STEP);
    }
    if (matrix[current_x][current_y - 1].getFillColor() == Color::White) {
        matrix[current_x][current_y - 1].setFillColor(Color(0, color + STEP, 0));
        add(tail, current_x, current_y - 1, color + STEP);
    }
    if (matrix[current_x + 1][current_y].getFillColor() == Color::White) {
        matrix[current_x + 1][current_y].setFillColor(Color(0, color + STEP, 0));
        add(tail, current_x + 1, current_y, color + STEP);
    }
    if (matrix[current_x][current_y + 1].getFillColor() == Color::White) {
        matrix[current_x][current_y + 1].setFillColor(Color(0, color + STEP, 0));
        add(tail, current_x, current_y + 1, color + STEP);
    }

    if (matrix[current_x - 1][current_y].getFillColor() == Color(0, 0, end_color)) {
        matrix[current_x - 1][current_y].setFillColor(Color(0, 0, end_color));
        add(tail, current_x - 1, current_y, end_color);
    }
    if (matrix[current_x][current_y - 1].getFillColor() == Color(0, 0, end_color)) {
        matrix[current_x][current_y - 1].setFillColor(Color(0, 0, end_color));
        add(tail, current_x, current_y - 1, end_color);
    }
    if (matrix[current_x + 1][current_y].getFillColor() == Color(0, 0, end_color)) {
        matrix[current_x + 1][current_y].setFillColor(Color(0, 0, end_color));
        add(tail, current_x + 1, current_y, end_color);
    }
    if (matrix[current_x][current_y + 1].getFillColor() == Color(0, 0, end_color)) {
        matrix[current_x][current_y + 1].setFillColor(Color(0, 0, end_color));
        add(tail, current_x, current_y + 1, end_color);
    }
}