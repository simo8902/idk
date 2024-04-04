//
// Created by Simeon-PC on 3/27/2024.
//

#ifndef UNTITLED_TRIANGLE_H
#define UNTITLED_TRIANGLE_H


class Triangle{
public:
    Triangle();
    ~Triangle();

    void Draw();

private:
    unsigned int VBO, VAO;

    void SetupMesh();
};

#endif //UNTITLED_TRIANGLE_H
