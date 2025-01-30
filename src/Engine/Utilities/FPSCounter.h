//
// Created by simeon on 22.01.25.
//

#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

class FPSCounter {
private:
    double previousTime;
    int frameCount;
    float fps;

public:
    FPSCounter();
    void update();
    float getFPS() const;
};

#endif //FPSCOUNTER_H
