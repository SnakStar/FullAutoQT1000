#ifndef CPACKCUSTOMPLOT_H
#define CPACKCUSTOMPLOT_H
#include "qcustomplot.h"

#define LineWidth 1
#define DotWidth 2

class CPackCustomPlot
{
public:
    CPackCustomPlot(QCustomPlot* plot);
public:
    void InitPlotParam();
    void InitPlotParam(int yAxisHight);
    void InitPlotParam(int xAxisLow, int xAxisHight, int yAxisLow, int yAxisHight);
    //画曲线图
    void DrawGraph(QVector<double> vecData);
    //
    void SetGraphData(QVector<double> vecData);
    //
    void ClearGraph();
    //
    void SetOverlay(bool bOverlay);
private:

private:
    //custom对象
    QCustomPlot* m_plot;
    //图形数据
    QVector<double> m_vecRawData;
    //
    QVector<double> m_PointX;
    //
    QVector<double> m_PointY;
    //是否叠加图形
    bool m_bOverlay;
};

#endif // CPACKCUSTOMPLOT_H
