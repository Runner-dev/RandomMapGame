#include <iostream>
float bilinear_interpolation(int x, int y, int points[4][3])
{
    return (points[0][2] * (points[2][0] - x) * (points[1][1] - y) +
            points[2][2] * (x - points[0][0]) * (points[1][1] - y) +
            points[1][2] * (points[2][0] - x) * (y - points[0][1]) +
            points[3][2] * (x - points[0][0]) * (y - points[0][1])) /
           ((points[2][0] - points[0][0]) * (points[1][1] - points[0][1]) + 0.0);
}

int main()
{
    int points[4][3] = {
        {10, 4, 100},
        {10, 6, 100},
        {20, 4, 200},
        {20, 6, 200}};
    std::cout << bilinear_interpolation(15, 5, points);
    return 0;
}
/*
print(bilinear_interpolation(15, 5,[
    (10, 4, 100),
    (10, 6, 100),
    (20, 4, 200),
    (20, 6, 200)]))*/