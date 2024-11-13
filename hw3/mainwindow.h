// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <vector>
#include <cmath>

using namespace std;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    struct Point {
        double x;
        double y;
        int cluster;
        Point(double x, double y) : x(x), y(y), cluster(-1) {}
    };

    struct Centroid {
        double x;
        double y;
        Centroid(double x, double y) : x(x), y(y) {}
    };

    void loadAndPlotData();
    double getDistance(const Point& p1, const Centroid& p2);
    void performKMeansClustering(const std::vector<std::vector<double>>& points);
    void plotClusters(const std::vector<Point>& points, const std::vector<Centroid>& centroids);

private:
    QGraphicsScene *scene;
    QGraphicsView *view;
    const int K = 3;  // 클러스터 개수
    const int MAX_ITERATIONS = 100;
};

#endif // MAINWINDOW_H
