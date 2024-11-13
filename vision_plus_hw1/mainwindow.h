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
    struct LineModel
    {
        double slope;
        double intercept;
    };

    void loadAndPlotData();
    LineModel fitLine(const vector<double>& x, const vector<double>& y);
    double getDistance(double x, double y, const LineModel& model);
    void drawRansacLine(const std::vector<std::vector<double>>& points);

private:
    QGraphicsScene *scene;
    QGraphicsView *view;
};

#endif // MAINWINDOW_H
