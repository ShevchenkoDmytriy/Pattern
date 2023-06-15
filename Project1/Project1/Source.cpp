#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

// Прототип фігури
class Shape {
public:
    virtual ~Shape() {}
    virtual Shape* clone() const = 0;
    virtual void draw() const = 0;
};

// Конкретний прототип - Коло
class Circle : public Shape {
public:
    Circle* clone() const override {
        return new Circle(*this);
    }

    void draw() const override {
        cout << " - -" << endl;
        cout << "-   -" << endl;
        cout << " - -" << endl;
    }
};

// Конкретний прототип - Квадрат
class Square : public Shape {
public:
    Square* clone() const override {
        return new Square(*this);
    }

    void draw() const override {
        cout << "------" << endl;
        cout << "|    |" << endl;
        cout << "|    |" << endl;
        cout << "------" << endl;
    }
};

// Композит - група фігур
class CompositeShape : public Shape {
public:
    CompositeShape* clone() const override {
        CompositeShape* clone = new CompositeShape();
        for (const auto& shape : shapes_) {
            clone->addShape(shape->clone());
        }
        return clone;
    }

    void draw() const override {
        for (const auto& shape : shapes_) {
            shape->draw();
        }
    }

    void addShape(Shape* shape) {
        shapes_.push_back(shape);
    }

    void removeShape(Shape* shape) {
        auto it = find_if(shapes_.begin(), shapes_.end(),
            [shape](const Shape* s) { return s == shape; });
        if (it != shapes_.end()) {
            delete* it;
            shapes_.erase(it);
        }
    }

    vector<Shape*> getShapes() const {
        return shapes_;
    }

private:
    vector<Shape*> shapes_;
};

// Знімок - зберігання стану сцени
class Memento {
public:
    Memento(const vector<Shape*>& shapes) : shapes_(shapes) {}

    vector<Shape*> getShapes() const {
        return shapes_;
    }

private:
    vector<Shape*> shapes_;
};

// Зберігання та відновлення стану сцени
class Caretaker {
public:
    void saveState(const vector<Shape*>& shapes) {
        Memento* memento = new Memento(shapes);
        mementos_.push_back(memento);
    }

    void restoreState(vector<Shape*>& shapes) {
        if (!mementos_.empty()) {
            Memento* memento = mementos_.back();
            shapes = memento->getShapes();
            mementos_.pop_back();
            delete memento;
        }
    }

private:
    vector<Memento*> mementos_;
};

// Контролер сцени
class SceneController {
public:
    static SceneController& getInstance() {
        static SceneController instance;
        return instance;
    }

    void addShape(Shape* shape) {
        shapes_.push_back(shape);
    }

    void removeShape(Shape* shape) {
        auto it = find_if(shapes_.begin(), shapes_.end(),
            [shape](const Shape* s) { return s == shape; });
        if (it != shapes_.end()) {
            delete* it;
            shapes_.erase(it);
        }
    }

    void drawScene() const {
        for (const auto& shape : shapes_) {
            shape->draw();
        }
    }

    void saveScene(const string& filename) {
        ofstream file(filename, ios::binary | ios::trunc);
        if (file) {
            vector<Shape*> shapes;
            for (const auto& shape : shapes_) {
                shapes.push_back(shape->clone());
            }
            Caretaker caretaker;
            caretaker.saveState(shapes);

            file.write(reinterpret_cast<const char*>(&caretaker), sizeof(caretaker));
            for (const auto& shape : shapes) {
                file.write(reinterpret_cast<const char*>(shape), sizeof(*shape));
            }

            for (const auto& shape : shapes) {
                delete shape;
            }

            cout << "Scene saved to file: " << filename << endl;
        }
        else {
            cout << "Failed to save scene to file: " << filename << endl;
        }
    }

    void loadScene(const string& filename) {
        ifstream file(filename, ios::binary);
        if (file) {
            Caretaker caretaker;
            file.read(reinterpret_cast<char*>(&caretaker), sizeof(caretaker));

            vector<Shape*> shapes;
            Shape* shape;
            while (file.read(reinterpret_cast<char*>(&shape), sizeof(shape))) {
                shapes.push_back(shape->clone());
            }
            caretaker.restoreState(shapes);

            for (const auto& shape : shapes_) {
                delete shape;
            }
            shapes_ = shapes;

            cout << "Scene loaded from file: " << filename << endl;
        }
        else {
            cout << "Failed to load scene from file: " << filename << endl;
        }
    }

    void showMenu() {
        int choice;
        while (true) {
            cout << "Menu:" << endl;
            cout << "1. Add a Circle" << endl;
            cout << "2. Add a Square" << endl;
            cout << "3. Add a Composite Shape" << endl;
            cout << "4. Remove a Shape" << endl;
            cout << "5. Draw Scene" << endl;
            cout << "6. Save Scene" << endl;
            cout << "7. Load Scene" << endl;
            cout << "0. Exit" << endl;
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
            case 1:
                addCircle();
                break;
            case 2:
                addSquare();
                break;
            case 3:
                addCompositeShape();
                break;
            case 4:
                removeShape();
                break;
            case 5:
                drawScene();
                break;
            case 6:
                saveScene();
                break;
            case 7:
                loadScene();
                break;
            case 0:
                return;
            default:
                cout << "Invalid choice. Please try again." << endl;
            }
        }
    }
private:
    vector<Shape*> shapes_;

    SceneController() {}
    ~SceneController() {
        for (const auto& shape : shapes_) {
            delete shape;
        }
    }

    void addCircle() {
        Circle* circle = new Circle();
        addShape(circle);
        cout << "Circle added." << endl;
    }

    void addSquare() {
        Square* square = new Square();
        addShape(square);
        cout << "Square added." << endl;
    }

    void addCompositeShape() {
        CompositeShape* composite = new CompositeShape();
        addShape(composite);
        cout << "Composite Shape created." << endl;
    }

    void removeShape() {
        int index;
        cout << "Enter the index of the shape to delete: ";
        cin >> index;

        if (index >= 0 && index < shapes_.size()) {
            Shape* shape = shapes_[index];
            removeShape(shape);
            cout << "Shape deleted." << endl;
        }
        else {
            cout << "Invalid index. Please try again." << endl;
        }
    }

    void saveScene() {
        string filename;
        cout << "Enter the filename to save the scene: ";
        cin >> filename;
        saveScene(filename);
    }

    void loadScene() {
        string filename;
        cout << "Enter the filename to load the scene from: ";
        cin >> filename;
        loadScene(filename);
    }

    SceneController(const SceneController&) = delete;
    SceneController& operator=(const SceneController&) = delete;
};

int main() {
    SceneController& controller = SceneController::getInstance();
    controller.showMenu();
    return 0;
}
